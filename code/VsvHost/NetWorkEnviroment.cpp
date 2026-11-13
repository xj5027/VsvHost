#include "NetWorkEnviroment.h"
#include "NetWorkCenter.h"
#include "NetWorkUav.h"
#include "SJ2414Struct.h"
#include "SJ2414ProtocolCmdTypeEx.h"

// 大气默认参数：能见度和高度单位为米，密度及 PM2.5 比例范围为 0～100。
constexpr float kDefaultAtmosphereVisibilityMeters = 500000.0f; // 非沙尘天气的大气能见度：500 km。
constexpr float kDefaultWeatherVisibilityMeters = 150000.0f;    // 发送端未设置能见度时使用：150 km。
constexpr float kAtmosphereDefaultHeight = 3000.0f;             // 大颗粒气溶胶分布的高度范围：3000 m。
constexpr uint8_t kAtmosphereDefaultAerosolDensity = 5;         // 大颗粒物质密度，非实际质量浓度。
constexpr uint8_t kAtmosphereDefaultMoleculeDensity = 8;        // 空气分子密度参数。
constexpr uint8_t kSandstormPm25Ratio = 80;                     // 沙尘天气中大颗粒物质的 PM2.5 占比。
NetWorkEnviroment::~NetWorkEnviroment()
{
}

void NetWorkEnviroment::Handler(unsigned char *buffer, std::size_t size)
{
    if (size < 2)
    {
        return;
    }
    if (ShouldSkipDuplicatePacket(buffer, size))
    {
        return;
    }
    printf("get_data\n");
    VSVLink::VSVCmdBuffer cmdbuffer;
    cmdbuffer.SetBuffer({(const char *)buffer, size});
    VSVLink::CmdType cmd_type = VSVLink::INVALID;
    VSVLink::ConstBuffer constbuffer;
    while (std::tie(cmd_type, constbuffer) = cmdbuffer.NextCommand(),
           cmd_type != VSVLink::INVALID && constbuffer.size != 0)
    {

        if (cmd_type == VSVLink::HAT_REPLY)
        {
        }
        else if (cmd_type == CmdTypeEx::TimeElement_CmdTypeEx)
        {
            std::string data((char *)constbuffer.data, constbuffer.size);
            const VSVLink::BaseCmd *cmd = (const VSVLink::BaseCmd *)data.data();
            if (cmd && cmd->header.cmd_type)
            {
                const SJ2414Protocol::UAV_TimeElement_DataCmd *time_element_data = reinterpret_cast<const SJ2414Protocol::UAV_TimeElement_DataCmd *>(cmd);
                if (time_element_data && time_element_data->U1_Month != 0)
                {
                    // 日期;
                    VSVLink::DateCmd date_cmd;
                    date_cmd.year = 2025;
                    date_cmd.month = time_element_data->U1_Month;
                    date_cmd.day = time_element_data->U1_Day;
                    command_list_.Reset();
                    command_list_.Append(&date_cmd, date_cmd.header.cmd_size);
                    // SendData((unsigned char *)command_list_.GetBuffer().data, command_list_.GetBuffer().size);

                    // 时间;
                    VSVLink::TimeCmd time_cmd;
                    time_cmd.enable = true;
                    time_cmd.hour = time_element_data->U1_Hour;
                    time_cmd.minute = time_element_data->U1_Minute;
                    command_list_.Reset();
                    command_list_.Append(&time_cmd, time_cmd.header.cmd_size);
                    NetWorkCenter::GetInstance().PushBackCommandList(command_list_);
                }
            }
        }
        else if (cmd_type == CmdTypeEx::NatureElement_CmdTypeEx)
        {
            std::string data((char *)constbuffer.data, constbuffer.size);
            VSVLink::BaseCmd *cmd = (VSVLink::BaseCmd *)data.data();
            if (cmd && cmd->header.cmd_type)
            {
                SJ2414Protocol::UAV_NatureElement_DataCmd *env_element_data = reinterpret_cast<SJ2414Protocol::UAV_NatureElement_DataCmd *>(cmd);
                if (env_element_data->U4_Visibility_m <= 0)
                {
                    env_element_data->U4_Visibility_m = static_cast<unsigned int>(kDefaultWeatherVisibilityMeters); // 如果发送端没有设置能见度，增加默认能见度
                }
                if (env_element_data && env_element_data->U4_Visibility_m != 0)
                {
                    // 云;
                    VSVLink::CloudCmd cloud_cmd;
                    cloud_cmd.enable = env_element_data->U1_CloudType > 0 ? true : false;
                    cloud_cmd.cloud_type = GetCloudeRealType(env_element_data->U1_CloudType); //  云类型：0 - 10（0 - 无云，1 - 淡积云，2 - 中积云，3 - 浓积云，4 - 积雨云，5 - 层积云，6- 雨层云，7 - 高层云，8 - 高积云，9 - 卷层云，10 - 卷积云）
                    cloud_cmd.layer_id = 0;
                    cloud_cmd.coverage = env_element_data->U1_CloudLevel * 14;    //  覆盖率：0 - 100
                    cloud_cmd.top_height = env_element_data->U4_CloudTopHeight_m; //  云顶高：0 - 100000
                    cloud_cmd.bottom_height = env_element_data->U4_CloudBaseHeight_m;

                    command_list_.Append(&cloud_cmd, cloud_cmd.header.cmd_size);

                    // 雾;
                    VSVLink::FogCmd fog_cmd;
                    float visiblility = (float)env_element_data->U4_Visibility_m;
                    fog_cmd.enable = 1;
                    fog_cmd.visibility = visiblility; //  能见度：0 - 500000 (m)
                    fog_cmd.height = 5000;
                    command_list_.Append(&fog_cmd, fog_cmd.header.cmd_size);

                    // 每次先恢复普通天气的大气参数，避免切出沙尘后保留污染比例。
                    VSVLink::AtmosphereCmd atmosphere_cmd;
                    atmosphere_cmd.enable = 1; // 开启大气模块。
                    atmosphere_cmd.global_visibility = kDefaultAtmosphereVisibilityMeters;
                    atmosphere_cmd.aerosol_hight = kAtmosphereDefaultHeight;
                    atmosphere_cmd.aerosol_density = kAtmosphereDefaultAerosolDensity;
                    atmosphere_cmd.pollution_ratio = 0; // 普通天气不附加 PM2.5 污染比例。
                    atmosphere_cmd.molecule_density = kAtmosphereDefaultMoleculeDensity;
                    atmosphere_cmd.enable_luminance = 1; // 开启天空亮度。
                    // 雨、雪、冰雹;
                    VSVLink::PrecipitateCmd precipitate_cmd;
                    precipitate_cmd.height = 5000;
                    switch (env_element_data->U1_WeahterType)
                    {
                    case 2:
                    {
                        precipitate_cmd.enable = env_element_data->U1_WeahterInstensity > 0 ? true : false;
                        precipitate_cmd.rain_level = env_element_data->U1_WeahterInstensity; //  雨等级设置：0 - 5
                    }
                    break;
                    case 3:
                    {

                        precipitate_cmd.enable = env_element_data->U1_WeahterInstensity > 0 ? true : false;
                        precipitate_cmd.snow_level = env_element_data->U1_WeahterInstensity; //  雪等级设置：0 - 5
                    }
                    break;
                    case 4: // 沙尘;
                        // 沙尘使用输入能见度，并提高 PM2.5 占比。
                        atmosphere_cmd.pollution_ratio = kSandstormPm25Ratio;
                        atmosphere_cmd.global_visibility = visiblility;
                        break;
                    case 5: // 雾;
                        break;
                    case 6: // 雷暴云效果待实现，不触发冰雹。
                        precipitate_cmd.enable = false;
                        break;
                    case 7: // 冰雹;
                    {

                        precipitate_cmd.enable = env_element_data->U1_WeahterInstensity > 0 ? true : false;
                        precipitate_cmd.hail_level = env_element_data->U1_WeahterInstensity; //  冰雹等级设置：0 - 5
                    }
                    break;
                    }

                    // 在天气分支完成后发送大气参数，确保沙尘设置和普通天气恢复都生效。
                    command_list_.Append(&atmosphere_cmd, atmosphere_cmd.header.cmd_size);
                    command_list_.Append(&precipitate_cmd, precipitate_cmd.header.cmd_size);

                    // 季节;
                    VSVLink::ConsoleCmd console_cmd;
                    int season_contrl_ = env_element_data->U1_Season;
                    if (season_contrl_ == 0 || season_contrl_ == 2)
                    {
                        std::string cmd = "effect.setSeason summer";
                        std::memcpy(console_cmd.cmd, cmd.c_str(), cmd.size());
                        command_list_.Append(&console_cmd, console_cmd.header.cmd_size);
                    }
                    else if (season_contrl_ == 1)
                    {

                        std::string cmd = "effect.setSeason spring";
                        std::memcpy(console_cmd.cmd, cmd.c_str(), cmd.size());
                        command_list_.Append(&console_cmd, console_cmd.header.cmd_size);
                    }
                    else if (season_contrl_ == 3)
                    {
                        std::string cmd = "effect.setSeason autumn";
                        std::memcpy(console_cmd.cmd, cmd.c_str(), cmd.size());
                        command_list_.Append(&console_cmd, console_cmd.header.cmd_size);
                    }
                    else if (season_contrl_ == 4)
                    {
                        std::string cmd = "effect.setSeason winter";
                        std::memcpy(console_cmd.cmd, cmd.c_str(), cmd.size());
                        command_list_.Append(&console_cmd, console_cmd.header.cmd_size);
                    }

                    // 表面状态组件;
                    VSVLink::SurfaceConditionCmd surface_cmd;
                    surface_cmd.enable = true;
                    surface_cmd.type = (std::uint8_t)env_element_data->U1_RunwayCondition;
                    surface_cmd.coverage = 100;
                    command_list_.Append(&surface_cmd, surface_cmd.header.cmd_size);

                    // 灯光;
                    VSVLink::LightCmd light_cmd;
                    light_cmd.entity_id = 14471;
                    light_cmd.glow_intensity = env_element_data->U1_RunwayLight;
                    command_list_.Append(&light_cmd, light_cmd.header.cmd_size);

                    // 同步设置通用机场灯光;
                    if (NetWorkUav::GetInstance().get_custom_airport_vec().size() > 0)
                    {
                        for (const auto &itr : NetWorkUav::GetInstance().get_custom_airport_vec())
                        {
                            VSVLink::LightCmd light_cmd;
                            light_cmd.entity_id = itr;
                            light_cmd.glow_intensity = env_element_data->U1_RunwayLight;
                            command_list_.Append(&light_cmd, light_cmd.header.cmd_size);
                        }
                    }

                    // 风;
                    VSVLink::WindCmd wind_cmd;
                    wind_cmd.enable = 1;
                    wind_cmd.speed = env_element_data->wind_spend;
                    wind_cmd.direction = env_element_data->wind_dir;
                    command_list_.Append(&wind_cmd, wind_cmd.header.cmd_size);

                    NetWorkCenter::GetInstance().PushBackCommandList(command_list_);
                }
            }
        }
    }
}

const int NetWorkEnviroment::GetCloudeRealType(int clound_type)
{
    switch (clound_type)
    {
    case 0:
        /* code */
        return clound_type;
        break;
    case 1:
        /* code */
        return 5; //- 层积云，
        break;
    case 2:
        return 1; // - 淡积云
        break;
    default:
        break;
    }
    return 0;
}
