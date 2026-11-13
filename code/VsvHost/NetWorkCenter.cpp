
#include "NetWorkCenter.h"
#include "Log.h"
#include "SJ2414Struct.h"
#include "SJ2414ProtocolCmdTypeEx.h"
#include "SJ2414ProtocolExternal.h"
#include "NetWorkWeatherRadar.h"
NetWorkCenter::~NetWorkCenter()
{
    update_loop_flag_ = false;
    thread_flag_ = false;
    if (thread_update_loop_ && thread_update_loop_->joinable())
    {
        thread_update_loop_->join();
    }
    delete thread_update_loop_;
}

void NetWorkCenter::Handler(unsigned char *buffer, std::size_t size)
{
    // 处理接收channel消息;
    if (size < 2)
    {
        return;
    }
    if (ShouldSkipDuplicatePacket(buffer, size))
    {
        return;
    }
    VSVLink::VSVCmdBuffer cmdbuffer;
    cmdbuffer.SetBuffer({(const char *)buffer, size});
    VSVLink::CmdType cmd_type = VSVLink::INVALID;
    VSVLink::ConstBuffer constbuffer;
    while (std::tie(cmd_type, constbuffer) = cmdbuffer.NextCommand(),
           cmd_type != VSVLink::INVALID && constbuffer.size != 0)
    {
        std::string data((char *)constbuffer.data, constbuffer.size);

        if (cmd_type == VSVLink::HOT_REPLY)
        {
            const VSVLink::HOTReplyCmd *cmd_data = reinterpret_cast<const VSVLink::HOTReplyCmd *>(constbuffer.data);
            float alt = cmd_data->hot;
            NetWorkWeatherRadar::GetInstance().send_alt_data(alt);
        }
    }
}
void NetWorkCenter::Init()
{
    thread_flag_ = true;
    thread_update_loop_ = new std::thread(&NetWorkCenter::Loop, this);
    update_loop_flag_ = true;
}

void NetWorkCenter::Loop()
{
    while (thread_flag_)
    {
        if (update_loop_flag_)
        {
            using clock = std::chrono::steady_clock;
            auto next_flush_time = clock::now();
            auto next_sync_time = next_flush_time;

            next_flush_time += std::chrono::milliseconds(10);
            FlushPendingCommands();
            std::this_thread::sleep_until(next_flush_time);
        }
    }
}

void NetWorkCenter::PushBackCommandBuffer(const VSVLink::ConstBuffer &buffer)
{
    if (buffer.data == nullptr || buffer.size == 0)
    {
        return;
    }

    std::lock_guard<std::mutex> lock(send_mutex_);
    active_command_list_->Append(buffer);
}

void NetWorkCenter::PushBackCommandList(VSVLink::CommandList &command_list)
{
    if (command_list.Empty())
    {
        return;
    }

    std::lock_guard<std::mutex> lock(send_mutex_);
    active_command_list_->Append(command_list);
}

void NetWorkCenter::FlushPendingCommands()
{
    VSVLink::CommandList *send_list = nullptr;
    {
        std::lock_guard<std::mutex> lock(send_mutex_);
        send_list = active_command_list_;
        active_command_list_ = (active_command_list_ == &command_list_front_)
                                   ? &command_list_back_
                                   : &command_list_front_;
    }

    if (send_list != nullptr && !send_list->Empty())
    {
        SendBufferInternal(send_list->GetBuffer());
        send_list->Reset();
    }
}

void NetWorkCenter::SendBufferInternal(const VSVLink::ConstBuffer &buffer)
{
    if (buffer.data && buffer.size > 0)
    {
        SendData((u_int8_t *)buffer.data, static_cast<uint32_t>(buffer.size));
    }
}

void NetWorkCenter::SendConsoleCmd(const std::string &cmd)
{
    if (cmd.empty())
    {
        return;
    }

    VSVLink::CommandList command_list;
    VSVLink::ConsoleCmd cmd_data;
    std::memset(cmd_data.cmd, 0, sizeof(cmd_data.cmd));
    std::memcpy(cmd_data.cmd, cmd.data(), std::min(cmd.size(), sizeof(cmd_data.cmd) - 1));
    command_list.Append(cmd_data);
    PushBackCommandList(command_list);
}

void NetWorkCenter::SendConsoleCmd(const std::vector<std::string> &cmds)
{
    if (cmds.empty())
    {
        return;
    }

    VSVLink::CommandList command_list;
    for (const auto &cmd : cmds)
    {
        VSVLink::ConsoleCmd cmd_data;
        std::memset(cmd_data.cmd, 0, sizeof(cmd_data.cmd));
        std::memcpy(cmd_data.cmd, cmd.data(), std::min(cmd.size(), sizeof(cmd_data.cmd) - 1));
        command_list.Append(cmd_data);
    }

    PushBackCommandList(command_list);
}

void NetWorkCenter::change_ir_or_tv(int view_type, int flag)
{
    if (view_type < 1 || view_type > 3 || (flag != 1 && flag != 2))
    {
        return;
    }

    // sj2414_sensor_change <视口类型1~3 视口类型1~3 1:前视 2:左后视 3:右后视> <传感器1电视/2红外>，由插件执行切换。
    SendConsoleCmd("sj2414_sensor_change " + std::to_string(view_type) + " " + std::to_string(flag));
}

void NetWorkCenter::change_switch(int view_type, int view_order, int flag)
{
    if (view_type < 1 || view_type > 3 || view_order < 0 || view_order > 2 ||
        (view_type == 1 && view_order != 0) || (flag != 0 && flag != 1))
    {
        return;
    }

    // sj2414_sensor_switch <视口类型1~3 1:前视 2:左后视 3:右后视> <视口0:代表全部 1:第一个 2:第二个 > <开关0关/1开>，由插件控制输出。
    // 与传感器切换共用控制台发送队列，每次有效调用均发送，支持重复指令恢复状态。
    SendConsoleCmd("sj2414_sensor_switch " + std::to_string(view_type) + " " +
                   std::to_string(view_order) + " " + std::to_string(flag));
}

void NetWorkCenter::request_uav_alt(int id, double lon, double lat)
{
    VSVLink::HOTRequestCmd cmd;
    cmd.id = id;
    cmd.lon = lon;
    cmd.lat = lat;
    VSVLink::CommandList command_list;
    command_list.Append(&cmd, cmd.header.cmd_size);
    PushBackCommandList(command_list);
    // SendData((unsigned char *)command_list_.GetBuffer().data, command_list_.GetBuffer().size);
}
void NetWorkCenter::create_custom_airport(int airport_id, double lon, double lat, double alt, float heading)
{
    VSVLink::CommandList command_list;
    // 先创建机场;
    VSVLink::AirportCmd airport_cmd;
    airport_cmd.entity_id = airport_id;
    std::string name = std::to_string(airport_id);
    memcpy(airport_cmd.name, name.data(), 4);
    airport_cmd.lon = lon;
    airport_cmd.lat = lat;
    airport_cmd.alt = alt;
    airport_cmd.operator_mode = 0;
    command_list.Append(&airport_cmd, airport_cmd.header.cmd_size);

    // 跑道;
    VSVLink::RunwayCmd runway_cmd;
    runway_cmd.airport_id = airport_id;
    runway_cmd.heading = heading;
    runway_cmd.part_id = 1;
    runway_cmd.is_light_always_on = 1;
    runway_cmd.length = 1000;
    runway_cmd.width = 50;
    command_list.Append(&runway_cmd, runway_cmd.header.cmd_size);

    // 滑行道;
    VSVLink::TaxiwayCmd taxiway_cmd;
    taxiway_cmd.airport_id = airport_id;
    taxiway_cmd.part_id = 2;
    taxiway_cmd.side = 0;
    command_list.Append(&taxiway_cmd, taxiway_cmd.header.cmd_size);
    VSVLink::TaxiwayCmd taxiway_cmd_2;
    taxiway_cmd_2.airport_id = airport_id;
    taxiway_cmd_2.part_id = 3;
    taxiway_cmd_2.side = 1;
    command_list.Append(&taxiway_cmd_2, taxiway_cmd_2.header.cmd_size);

    // 停机坪;
    VSVLink::ApronCmd apron_cmd;
    apron_cmd.airport_id = airport_id;
    apron_cmd.part_id = 4;
    apron_cmd.runway_part_id = 1;
    apron_cmd.type = 0;
    apron_cmd.side = 0;
    command_list.Append(&apron_cmd, apron_cmd.header.cmd_size);

    // 机库;
    VSVLink::HangarCmd hangar_cmd;
    hangar_cmd.airport_id = airport_id;
    hangar_cmd.part_id = 5;
    hangar_cmd.runway_part_id = 1;
    hangar_cmd.type = 0;
    hangar_cmd.side = 0;
    command_list.Append(&hangar_cmd, hangar_cmd.header.cmd_size);

    // 候机楼;
    VSVLink::AirTerminalCmd air_terminal_cmd;
    air_terminal_cmd.airport_id = airport_id;
    air_terminal_cmd.part_id = 6;
    air_terminal_cmd.side = 0;
    air_terminal_cmd.type = 0;
    air_terminal_cmd.runway_part_id = 1;
    command_list.Append(&air_terminal_cmd, air_terminal_cmd.header.cmd_size);

    // 塔台;
    VSVLink::TowerCmd tower_cmd;
    tower_cmd.airport_id = airport_id;
    tower_cmd.part_id = 7;
    tower_cmd.side = 1;
    tower_cmd.type = 0;
    tower_cmd.runway_part_id = 1;
    command_list.Append(&tower_cmd, tower_cmd.header.cmd_size);

    PushBackCommandList(command_list);
}
void NetWorkCenter::change_custom_airport(int airport_id, double lon, double lat, double alt, float heading)
{

    VSVLink::CommandList command_list;
    VSVLink::EntityStatusCmd status_cmd;
    status_cmd.SetStatus(ETT_ON_GROUND);
    status_cmd.entity_id = airport_id;
    command_list.Append(&status_cmd, status_cmd.header.cmd_size);

    VSVLink::EntityCmd cmd;
    cmd.entity_id = airport_id;
    cmd.lon = lon;
    cmd.lat = lat;
    cmd.alt = 0;
    cmd.yaw = heading;

    command_list.Append(&cmd, cmd.header.cmd_size);
    PushBackCommandList(command_list);
}