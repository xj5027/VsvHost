#pragma once

#include <cstdint>
#include "VSV_ICD/VSVProtocolCmdType.h"

#pragma pack(push)
#pragma pack(1)

namespace VSVLink {
    enum SJ2414CMD_TYPE {
        SJ2414_SENSOR_CHANGE = 1, // 传感器类型切换命令
        SJ2414_SENSOR_SWITCH = 2  // 传感器开关切换命令
    };

    struct SJ2414BaseCmd :public BaseCmd {
        SJ2414BaseCmd(uint8_t cmd_type, uint16_t cmd_size)
            :BaseCmd(EXT_COMMAND, cmd_size)
            , SJ2414_cmd_type(cmd_type)
        {}
        uint8_t SJ2414_cmd_type;
    };

    struct SJ2414SensorChangeCmd : public SJ2414BaseCmd {
        SJ2414SensorChangeCmd()
            :SJ2414BaseCmd(SJ2414_SENSOR_CHANGE, sizeof(SJ2414SensorChangeCmd))
            , sensor_type(0)
        {}
       uint8_t view_type = 0;    // 1:前视 2：左后视 3：右后视;
       uint8_t sensor_type = 0;  // 0:无传感器，1：电视，2：红外；
    };

    struct SJ2414SensorSwitchCmd : public SJ2414BaseCmd {
        SJ2414SensorSwitchCmd()
            :SJ2414BaseCmd(SJ2414_SENSOR_SWITCH, sizeof(SJ2414SensorSwitchCmd))
            , sensor_switch(0)
        {}
       uint8_t view_type = 0;  /// 1:前视 2：左后视 3：右后视;
       uint8_t view_order = 0; ///  0:不区分 1:视口1 ； 2：视口2；
       uint8_t sensor_switch = 0;      // 视频输出开关：0 关闭，1 开启；
    };
}

#pragma pack(pop)