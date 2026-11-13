#include <cstring>
#include <iostream>
#include "NetWorkUav.h"
#include "SJ2414Struct.h"
#include "SJ2414ProtocolCmdTypeEx.h"
#include "SJ2414ProtocolExternal.h"
#include "NetWorkCenter.h"
NetWorkUav::~NetWorkUav()
{
}



void NetWorkUav::Handler(unsigned char *buffer, std::size_t size)
{
    int my_size = sizeof(SJ2414Protocol::UAVehicle_ICD_DataCmd);
    if (buffer == nullptr || size < sizeof(VSVLink::CmdHeader))
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
        if (cmd_type == VSVLink::UAVehicleElement_CmdTypeEx)
        {
            int my_size = sizeof(SJ2414Protocol::UAVehicle_ICD_DataCmd);
            if (constbuffer.size < sizeof(SJ2414Protocol::UAVehicle_ICD_DataCmd))
            {
                continue;
            }

            const SJ2414Protocol::UAVehicle_ICD_DataCmd *uav_element_data = reinterpret_cast<const SJ2414Protocol::UAVehicle_ICD_DataCmd *>(constbuffer.data);
            int  get_size = uav_element_data->header.cmd_size;
            if (uav_element_data && uav_element_data->header.cmd_type)
            {
                // 无人机实体；
                VSVLink::CommandList command_list;
                VSVLink::EntityCmd entity_cmd;
                float M_UAVehicle_Alt = uav_element_data->M_UAVehicle_Alt + 67.8;
                entity_cmd.entity_id = 1;
                //entity_cmd.type_id = (uint16_t)(uav_element_data->M_UAVehicle_Type);

                entity_cmd.lon = uav_element_data->M_UAVehicle_Lon;
                //	简易无人机1纬度：±90（无人机物理中心）
                entity_cmd.lat = uav_element_data->M_UAVehicle_Lat;
                entity_cmd.alt = M_UAVehicle_Alt;                   //	简易无人机1高度：0-100 (m)（无人机物理中心）
                entity_cmd.yaw = uav_element_data->M_UAVehicle_H;   //	简易无人机1偏航角： 0-360 （deg）（无人机物理中心）
                entity_cmd.pitch = uav_element_data->M_UAVehicle_P; //	简易无人机1俯仰角： 0-360 （deg）（无人机物理中心）
                entity_cmd.roll = uav_element_data->M_UAVehicle_R;  //	简易无人机1滚转角：deg 0-360（无人机物理中心）

                command_list.Append(&entity_cmd, entity_cmd.header.cmd_size);
                // 请求地面高度;
                //NetWorkCenter::GetInstance().request_uav_alt(1,entity_cmd.lon,entity_cmd.lat);
                // 加油管设置;
                for (int i = 0; i <12; i++)
                {
                    VSVLink::PipeNodeCmd pip_cmd;
                    pip_cmd.entity_id = entity_cmd.entity_id;
                    pip_cmd.part_id = 10;
                    pip_cmd.node_id = i+1;
                    pip_cmd.offset_x = uav_element_data->F4_TubePosY_m_BODY[i];  // 左右;
                    pip_cmd.offset_y = -uav_element_data->F4_TubePosZ_m_BODY[i]; // 上下;
                    pip_cmd.offset_z = -uav_element_data->F4_TubePosX_m_BODY[i]; // 前后;
                    command_list.Append(&pip_cmd, pip_cmd.header.cmd_size);
                }

                //              // 613项目发送加油管数据;
                // for (int i = 0; i < 21; ++i) {
                //     VSVLink::PipeNodeCmd pipe_cmd;
                //             pipe_cmd.entity_id = refuel_sim_entity_id; // 加油机实体ID
                //     pipe_cmd.part_id = 10; // 可以根据需要设置不同的part_id
                //     pipe_cmd.node_id = i; // 质点索引作为node_id
                //     pipe_cmd.offset_x = dPosYRep[i];  //-17.11
                //     pipe_cmd.offset_y = dPosZRep[i]; // 13.1256
                //     pipe_cmd.offset_z = dPosXRep[i]; //-23.989
                //     NetworkManager::GetInstance().PushBackCommand(pipe_cmd);
                // }
                NetWorkCenter::GetInstance().PushBackCommandList(command_list);
                
            }
        }
    }
    // 处理接收channel消息;
}
