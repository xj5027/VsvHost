
#include "NetWorkWeatherRadar.h"
#include "SJ2414Struct.h"
#include "SJ2414ProtocolCmdTypeEx.h"
#include "SJ2414ProtocolExternal.h"
#include "NetWorkCenter.h"
NetWorkWeatherRadar::~NetWorkWeatherRadar()
{
}

void NetWorkWeatherRadar::send_alt_data(float alt)
{
    
}

void NetWorkWeatherRadar::Handler(unsigned char *buffer, std::size_t size)
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
    int get_siez = sizeof(SJ2414Protocol::weather_radar_cmd);
    if (size == sizeof(SJ2414Protocol::weather_radar_cmd))
    {
        SJ2414Protocol::weather_radar_cmd cmd;
        memcpy(&cmd,buffer,size);
        // 处理业务;
        
        if (cmd.gain_vaild)
        {
            std::string cmd_str = "sensor_wr_gain = ";
            cmd_str += std::to_string(cmd.gain_set/3);
            NetWorkCenter::GetInstance().SendConsoleCmd(cmd_str);
        }
        if (cmd.pitch_vaild)
        {
            std::string cmd_str = "sensor_wr_dir_elevation = ";
            cmd_str += std::to_string(cmd.pitch_set*0.5);
            NetWorkCenter::GetInstance().SendConsoleCmd(cmd_str);
        }
        if (cmd.range_set !=0)
        {
            std::string cmd_str = "sensor_wr_scan_max_range = ";
            int range = 0;
            switch(cmd.range_set)
            {
                case 1:
                    range = 10*1000;
                break;
                case 2:
                    range = 20*1000;
                break;
                case 3:
                    range = 40*1000;
                break;
                case 4:
                    range = 80*1000;
                break;
                case 5:
                    range = 160*1000;
                break;
                case 6:
                    range = 240*1000;
                break;
                default:
                break;
            }
            //cmd_str += std::to_string(range);
            cmd_str += std::to_string(range*1.85);
            NetWorkCenter::GetInstance().SendConsoleCmd(cmd_str);
            
        }
       
    }
    
}
