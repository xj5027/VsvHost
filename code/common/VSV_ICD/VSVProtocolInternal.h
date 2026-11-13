#pragma once
//////////////////////////////////////////////////////////////////////////
// \copyright 2019 VSV.Inc
// \brief VSV视景外部通信协议
// \brief 部分借鉴CIGI协议设计，但更为简单易用，并增加自身内部功能。方便集成和扩展。
// \author gaimeng
//////////////////////////////////////////////////////////////////////////

#include <stdint.h>
#include "VSVProtocolCmdType.h"

#pragma pack(push)
#pragma pack(1)

namespace VSVLink {

enum INTERNAL_CMD_TYPE {
    //////////////////////////////////////////////////////////////////////////
    //内部命令类型 主控到视景 Host->IG
    INTERNAL_CMDS_START = 200,  //标记内部命令的起始位置
    LAUNCH,                     //启动视景
    DISCOVER,                   //发现视景
    ASSIGN_VIEW_ID,             //分配视景ID
    SWAP_BUFFER,                // SwapBuffer
    CAMERA_MOVE,                //直接操纵相机移动
    SCREEN_GRID,                //屏幕网格
    STATISTICS_REQUEST,         //统计信息请求

    ROTOR_ANGLE,    //旋翼角度
    INTERPOLATION,  //插值信息（时钟插值在channel端完成）
    ENTITY_SLERP,   //带时间戳的entity信息

    // 仿真时间
    SIMTIMER_START,
    SIMTIMER_TICK = SIMTIMER_START,  // 时钟校准
    SIMTIMER_AUTHO,                  // 时钟授权
    SIMTIMER_AUTHO_SETTING,          // 时钟授权,CHANNEL端临时构造使用
    SIMTIMER_CHECK,                  // 请求时钟
    SIMTIMER_END = SIMTIMER_CHECK,

    CLOUD_LAYER_REQUEST,  // 云层状态请求
    CLOUD_LAYER_SYNC,     // 云层同步

    HOST_UNIFIED_FRAME_ID,  // 唯一ID

    //////////////////////////////////////////////////////////////////////////
    //内部反馈命令类型 视景到主控 IG -> Host
    INTERNAL_RPLY_START = 240,  //内部反馈命令起始位置
    DISCOVER_RPLY,              //发现视景响应命令
    SYNC_FRAME,                 //帧同步
    STATISTICS_REPLY,           //统计信息反馈
    INTERPOLATION_ECHO,         //插值信息设置回馈，启动时可能会设置不成功，需要多设置几次

    // 仿真时间
    SIMTIMER_REPLY_START,
    SIMTIMER_TICK_REPLY = SIMTIMER_REPLY_START,  // 时钟校准反馈
    SIMTIMER_AUTHO_REPLY,                        // 时钟授权反馈
    SIMTIMER_CHECK_REPLY,                        // 请求时钟反馈
    SIMTIMER_REPLAY_END = SIMTIMER_CHECK_REPLY,

    CLOUD_LAYER_REPLY,  // 云层态请求反馈
    END_FRAME,          //结束帧，每帧反馈

    GSYNC_STATUS,  // Gsync硬同步卡状态
};
//////////////////////////////////////////////////////////////////////////
// 内部命令 主控到视景 Host -> IG

enum IGComputerQuality {
    LOW,
    HIGH
};

//启动
struct LaunchCmd {
    CmdHeader header{LAUNCH, sizeof(LaunchCmd)};  //命令头
    char master_multicast_ip[16];                 //master组播地址
    uint16_t master_multicast_port;               //master组播端口
    uint16_t master_port;                         //master收取worker反馈的端口
    uint8_t frame_sync;                           //是否开启帧同步
    uint8_t close_explorer;                       //是否关闭视景主机上的explorer
    uint8_t ig_computer_quality;                  // IG电脑性能 IGComputerQuality
    char app_name[16];                            //要启动的进程名
};

//发现视景
struct DiscoverCmd {
    CmdHeader header{DISCOVER, sizeof(DiscoverCmd)};  //命令头
};

//分配视景ID
struct AssignViewIDCmd {
    CmdHeader header{ASSIGN_VIEW_ID, sizeof(AssignViewIDCmd)};  //命令头
    uint8_t view_id;                                            //分配的ID
    char ip[16];                                                //对应的IP
};

// swap buffer
struct SwapBufferCmd {
    CmdHeader header{SWAP_BUFFER, sizeof(SwapBufferCmd)};  //命令头
    uint32_t frame_id;                                     //帧号
    uint32_t last_host_frame_id;                           //上一帧host帧号
    uint64_t timestamp;                                    //时间戳
    uint16_t version = 1;                                  //版本
};

//直接操纵相机移动
struct CameraMoveCmd {
    CmdHeader header{CAMERA_MOVE, sizeof(CameraMoveCmd)};  //命令头
    double pos_x, pos_y, pos_z;
    double dir_x, dir_y, dir_z;
    double up_x, up_y, up_z;
};

//屏幕网格
struct ScreenGridCmd {
    CmdHeader header{SCREEN_GRID, sizeof(ScreenGridCmd)};  //命令头
    uint8_t enable = false;
    uint8_t view_id = -1;
    uint8_t current_point = 0;         //当前选中的控制点
    float controlpoints[50] = {0.0f};  //控制点信息
};

//统计信息请求
struct StatisticsRequestCmd {
    CmdHeader header{STATISTICS_REQUEST, sizeof(StatisticsRequestCmd)};  //命令头
};

//旋翼角度
struct RotorAngleCmd {
    CmdHeader header{ROTOR_ANGLE, sizeof(RotorAngleCmd)};  //命令头
    uint16_t entity_id = -1;                               //实体ID
    uint16_t part_id = -1;                                 //子部件id（可选）
    float angle = 0.0f;                                    //旋转角度
};

// 插值信息（时钟插值在channel端完成）
struct InterpolationCmd {
    InterpolationCmd()
        : header(INTERPOLATION, sizeof(InterpolationCmd)), interpolation_method(0), time_span(0) {}
    CmdHeader header;              //命令头
    uint8_t interpolation_method;  // 插值方式，见VSVLink::InterpolationMethod
    uint16_t time_span;            // 时间缓冲
};

//带时间戳的entity信息
struct EntitySlerpCmd {
    EntitySlerpCmd()
        : header(ENTITY_SLERP, sizeof(EntitySlerpCmd)), entity_id(0), type_id(-1), yaw(FLT_MAX), pitch(FLT_MAX), roll(FLT_MAX), lon(DBL_MAX), lat(DBL_MAX), alt(DBL_MAX), sim_time(0.0) {}
    CmdHeader header;    //命令头
    uint16_t entity_id;  //实体ID。默认情况下0为主相机
    uint16_t type_id;    //类型id，由配置文件配置
    float yaw;           //偏航，-180 ~ 180，以正北为0，向东为正
    float pitch;         //俯仰，-90 ~ 90，向上为正
    float roll;          //滚转，-180 ~ 180，向右为正
    double lon;          //经度
    double lat;          //纬度
    double alt;          //海拔高度//旋转角度
    double sim_time;
};

// 时钟校准
struct SimTimerTickCmd {
    CmdHeader header{SIMTIMER_TICK, sizeof(SimTimerTickCmd)};  //命令头
    uint16_t tick_id = 0;                                      //校准时间戳消息ID
    double sim_time = 0.0;                                     //校准时间
};

//时钟授权
struct SimTimerAuthoCmd {
    CmdHeader header{SIMTIMER_AUTHO, sizeof(SimTimerAuthoCmd)};  //命令头
    uint16_t tick_id = 0;                                        //校准时间戳消息ID
    double sim_time_correct_time = 0.0;                          // 时间校准纠正时间偏移
};

//时钟授权
struct SimTimerAuthoSettingCmd {
    CmdHeader header{SIMTIMER_AUTHO_SETTING, sizeof(SimTimerAuthoSettingCmd)};  //命令头
    double sim_time = 0.0;                                                      // 授权的仿真时间
    double recv_time = 0.0;                                                     // 接收到时钟授权时的本地精确时间
    double time_offset = 0.0;                                                   // 时钟偏移量
};

//请求时钟
struct SimTimerCheckCmd {
    CmdHeader header{SIMTIMER_CHECK, sizeof(SimTimerCheckCmd)};  //命令头
};

enum CloudTechniqueType {
    PARTICLE = 0,  //粒子云
    SLICE,         //薄片云
    VOLUME         //体积云
};

enum CloudSyncType {
    CLOUD_NOSYNC = 0,           // 不同步
    CLOUD_SYNC = 1,             // 同步
    CLOUD_SYNC_IMMEDIATELY = 2  // 收到消息后立刻同步一次
};

// 云层状态请求
struct CloudLayerRequestCmd {
    CmdHeader header{CLOUD_LAYER_REQUEST, sizeof(CloudLayerRequestCmd)};  //命令头
    char ip[16];                                                          //本机网络IP
    uint8_t layer_id = 0;                                                 //云层id   //暂时无用
    uint8_t cloud_technique = PARTICLE;                                   //云类型   //暂时无用
    uint8_t sync = CLOUD_NOSYNC;                                          //是否同步 //暂时无用
    uint8_t sync_interval = 60;                                           //同步间隔 //暂时无用
};

// 云层同步
struct CloudLayerSyncCmd {
    CmdHeader header{CLOUD_LAYER_SYNC, sizeof(CloudLayerSyncCmd)};  //命令头
    uint32_t frame_id;                                              //帧号
    uint8_t layer_id = 0;                                           //云层id
    uint8_t cloud_technique = PARTICLE;                             //云类型
    double cloud_center[3] = {0, 0, 0};                             //云层中心
};

// 唯一ID
struct UnifiedFrameIDCmd {
    CmdHeader header{HOST_UNIFIED_FRAME_ID, sizeof(UnifiedFrameIDCmd)};  //命令头
    uint32_t unified_frame_id = 0;
};

//////////////////////////////////////////////////////////////////////////
// 内部反馈命令 视景到主控 IG -> Host

//发现视景响应命令
struct DiscoverReplyCmd {
    CmdHeader header{DISCOVER_RPLY, sizeof(DiscoverReplyCmd)};  //命令头
    char hostname[18];                                          //本机主机名
    uint16_t port;                                              //视景接收数据的端口
};

// sync frame
struct SyncFrameCmd {
    CmdHeader header{SYNC_FRAME, sizeof(SyncFrameCmd)};  //命令头
    uint32_t frame_id;                                   //帧号
    uint32_t last_host_frame_id;                         //上一帧host帧号
    uint64_t timestamp;                                  //时间戳
};

// end frame
struct EndFrameCmd {
    CmdHeader header{END_FRAME, sizeof(EndFrameCmd)};  //命令头
    uint32_t last_unified_frame_id;                    //上一帧帧号
};

struct StatisticsReplyCmd {
    CmdHeader header{STATISTICS_REPLY, sizeof(StatisticsReplyCmd)};  //命令头
    double MeanFrame;
    int64_t TransmissionDelay;
};

// 插值信息（时钟插值在channel端完成）
struct InterpolationEchoCmd {
    InterpolationEchoCmd()
        : header(INTERPOLATION_ECHO, sizeof(InterpolationEchoCmd)), interpolation_method(0), time_span(0) {}
    CmdHeader header;              //命令头
    uint8_t interpolation_method;  // 插值方式，见VSVLink::InterpolationMethod
    uint16_t time_span;            // 时间缓冲
};

// 时钟校准反馈
struct SimTimerTickReplyCmd {
    CmdHeader header{SIMTIMER_TICK_REPLY, sizeof(SimTimerTickReplyCmd)};  //命令头
    uint16_t tick_id = 0;                                                 //校准时间戳消息ID
};

// 时钟授权反馈
struct SimTimerAuthoReplyCmd {
    CmdHeader header{SIMTIMER_AUTHO_REPLY, sizeof(SimTimerAuthoReplyCmd)};  //命令头
    uint16_t tick_id = 0;                                                   //校准时间戳消息ID
    uint16_t autho_res = 0;                                                 //是否授权成功
    double frame_start_time = 0;                                            //一帧开始对应的时刻
};

// 请求时钟反馈
struct SimTimerCheckReplyCmd {
    CmdHeader header{SIMTIMER_CHECK_REPLY, sizeof(SimTimerCheckReplyCmd)};  //命令头
    double sim_time = 0.0;                                                  //仿真时间
};

// 云层状态请求反馈
struct CloudLayerReplyCmd {
    CmdHeader header{CLOUD_LAYER_REPLY, sizeof(CloudLayerReplyCmd)};  //命令头
    char ip[16];                                                      //通道对应的IP
    uint32_t frame_id;                                                //帧号
    uint8_t layer_id = 0;                                             //云层id
    uint8_t cloud_technique = PARTICLE;                               //云类型
    double cloud_center[3] = {0, 0, 0};                               //云层中心
};

// IG_Gsync硬同步卡状态枚举
enum IGGsyncStatus {
    IG_GSYNC_INSTALLED_NOTSYNC = 0,  // 同步卡已安装,未同步
    IG_GSYNC_SYNCED_SLAVE = 1,       // 同步卡已同步,作为客户端
    IG_GSYNC_SYNCED_MASTER = 2,      // 同步卡已同步,作为服务端
    IG_GSYNC_NOTINSTALLED = 3,       // 同步卡未安装
};
// Gsync硬同步卡状态
struct GsyncStatusCmd : public BaseCmd {
    GsyncStatusCmd()
        : BaseCmd(GSYNC_STATUS, sizeof(GsyncStatusCmd)), gsync_display_id(-1), gsync_status(IG_GSYNC_NOTINSTALLED), gsync_refresh_rate(60.0f) {}
    uint16_t gsync_display_id;  // IG_Gsync硬同步卡显示器ID
    uint8_t gsync_status;       // IG_Gsync硬同步卡状态,见IGGsyncStatus
    float gsync_refresh_rate;   // 刷新脉冲频率（精确的刷新率）
};

}  // namespace VSVLink
#pragma pack(pop)