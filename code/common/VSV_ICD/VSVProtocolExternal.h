#pragma once
//////////////////////////////////////////////////////////////////////////
// \copyright 2023 VSV.Inc
// \brief 视景外部通信数据协议 VIGI(VSV Image Generator Interface)
// \author gaimeng
//////////////////////////////////////////////////////////////////////////

#include "VSVProtocolCmdType.h"
#include <float.h>
#include <string.h>
#include <string>

#pragma pack(push)
#pragma pack(1)

namespace VSVLink {
//////////////////////////////////////////////////////////////////////////
//外部命令类型 主控到视景 Host->IG
//////////////////////////////////////////////////////////////////////////

//开始帧
struct BeginFrameCmd : public BaseCmd {
    BeginFrameCmd()
        : BaseCmd(BEGIN_FRAME, sizeof(BeginFrameCmd))
        , frame_id(0)
        , last_ig_frame_id(0)
        , timestamp(0)
        , version(20400) {}
    uint32_t frame_id;          //帧号
    uint32_t last_ig_frame_id;  //上一帧视景帧号
    uint64_t timestamp;         //时间戳(单位：微秒)
    uint16_t version;           //协议版本xyyzz，x: major, yy: minor, zz: patch
                                //例：10203表示1.2.3
};

// IG控制类型
enum IGCtrlType {
    RESUME_IG,     //恢复IG
    PAUSE_IG,      //暂停IG
    DEBUG_IG,      //调试IG
    CLOSE_IG,      //关闭IG
    SHUTDOWN_IG,   //IG关机
    CLOSE_Daemon,  //关闭Daemon
    RESTART_IG,    // 重启IG电脑;
    START_IG,      //启动IG
};
// IG控制
struct IGCtrlCmd : public BaseCmd {
    IGCtrlCmd()
        : BaseCmd(IG_CTRL, sizeof(IGCtrlCmd))
        , ctrl_type(RESUME_IG) {}
    uint8_t ctrl_type;  //控制类型 IGCtrlType
};

//视口定义
struct ViewDefineCmd : public BaseCmd {
    ViewDefineCmd()
        : BaseCmd(VIEW_DEF, sizeof(ViewDefineCmd))
        , id(0)
        , left(0.0)
        , right(0.0)
        , top(0.0)
        , bottom(0.0)
        , near_value(-1.0)
        , far_value(-1.0) {}
    uint8_t id;        // View ID
    float left;        //角度值
    float right;       //角度值
    float top;         //角度值
    float bottom;      //角度值
    float near_value;  //近裁面
    float far_value;   //远裁面
};

//视口控制
struct ViewCtrlCmd : public BaseCmd {
    ViewCtrlCmd()
        : BaseCmd(VIEW_CTRL, sizeof(ViewCtrlCmd))
        , id(0)
        , azimuth(0.0)
        , elevation(0.0)
        , roll(0.0)
        , offset_x(0.0)
        , offset_y(0.0)
        , offset_z(0.0) {}
    uint8_t id;       // View ID
    float azimuth;    //偏转角度
    float elevation;  //俯仰角度
    float roll;       //滚转角度
    float offset_x;   // x偏移
    float offset_y;   // y偏移
    float offset_z;   // z偏移
};

// 视口大小
struct ViewSizeCmd : public BaseCmd {
    ViewSizeCmd()
        : BaseCmd(VIEW_SIZE, sizeof(ViewSizeCmd))
        , id(0)
        , x(0)
        , y(0)
        , width(-1)
        , height(-1) {}
    uint8_t id;  // View ID
    int x;
    int y;
    int width;
    int height;
};

//////////////////////////////////////////////////////////////////////////
// 实体相关命令

//实体
struct EntityCmd : public BaseCmd {
    EntityCmd()
        : BaseCmd(ENTITY, sizeof(EntityCmd))
        , entity_id(0)
        , type_id(-1)
        , yaw(FLT_MAX)
        , pitch(FLT_MAX)
        , roll(FLT_MAX)
        , lon(DBL_MAX)
        , lat(DBL_MAX)
        , alt(DBL_MAX) {}
    uint16_t entity_id;  //实体ID。默认情况下0为主相机
    uint16_t type_id;    //类型id，由配置文件配置
    float yaw;           //偏航，-180 ~ 180，以正北为0，向东为正
    float pitch;         //俯仰，-90 ~ 90，向上为正
    float roll;          //滚转，-180 ~ 180，向右为正
    double lon;          //经度
    double lat;          //纬度
    double alt;          //海拔高度
};

//构建实体父子关系。
//将parent_id的实体设为entity_id实体的父节点，并可指定其作为子部件的ID为part_id
struct AttachEntityCmd : public BaseCmd {
    AttachEntityCmd()
        : BaseCmd(ATTACH_ENTITY, sizeof(AttachEntityCmd))
        , entity_id(-1)
        , parent_id(-1)
        , part_id(-1)
        , local_x(DBL_MAX)
        , local_y(DBL_MAX)
        , local_z(DBL_MAX)
        , euler_x(FLT_MAX)
        , euler_y(FLT_MAX)
        , euler_z(FLT_MAX)
        , parent_part_id(-1) {}
    uint16_t entity_id;       //子部件实体id
    uint16_t parent_id;       //父节点实体id。-1时解开父子部件
    uint16_t part_id;         //指定作为子部件的ID（可选）
    double local_x;           //相对父节点x偏移
    double local_y;           //相对父节点y偏移
    double local_z;           //相对父节点z偏移
    float euler_x;            //欧拉角 x
    float euler_y;            //欧拉角 y
    float euler_z;            //欧拉角 z
    uint16_t parent_part_id;  //父节点的子节点，支持绑定到模型的子节点
};

//子部件设置
struct PartCmd : public BaseCmd {
    PartCmd()
        : BaseCmd(PART, sizeof(PartCmd))
        , entity_id(-1)
        , part_id(-1)
        , offset_x(DBL_MAX)
        , offset_y(DBL_MAX)
        , offset_z(DBL_MAX)
        , euler_x(FLT_MAX)
        , euler_y(FLT_MAX)
        , euler_z(FLT_MAX) {}
    uint16_t entity_id;  //实体ID
    uint16_t part_id;    //子部件id（可选）
    double offset_x;     //相对父节点x偏移
    double offset_y;     //相对父节点y偏移
    double offset_z;     //相对父节点z偏移
    float euler_x;       //相对父节点俯仰 x
    float euler_y;       //相对父节点偏航 y
    float euler_z;       //相对父节点滚转 z
};

//相机类型实体
//将当前相机设为指定的entity_id
struct CameraCmd : public BaseCmd {
    CameraCmd()
        : BaseCmd(CAMERA, sizeof(CameraCmd))
        , entity_id(0)
        , part_id(-1) {}
    uint16_t entity_id;
    uint16_t part_id;  //子部件id（可选）
};

//灯光控制
struct LightCmd : public BaseCmd {
    LightCmd()
        : BaseCmd(LIGHT, sizeof(LightCmd))
        , entity_id(-1)
        , part_id(-1)
        , enable(1)
        , intensity(-1.0f)
        , r(0)
        , g(0)
        , b(0)
        , range(-1.0f)
        , outer_angle(-1.0f)
        , glow_intensity(-1.0f) {}
    uint16_t entity_id;    //实体ID
    uint16_t part_id;      //子部件id（可选）
    uint8_t enable;        //灯光开关
    float intensity;       //光照强度
    uint8_t r, g, b;       //灯光颜色
    float range;           //灯光范围
    float outer_angle;     //光锥锥角（角度）
    float glow_intensity;  //光晕强度
};

//实体颜色
enum ColorChannel {
    COLOR_NONE,
    COLOR_DIFFUSE,       //漫反射
    COLOR_SPECULAR,      //高光
    COLOR_ILLUMINATION,  //自发光
    //TODO: other channels
};
//注：当color、color_occupy无效时，恢复指定channel的颜色参数；
//   当channel值也同时为COLOR_NONE时，完全恢复原材质
struct EntityColorCmd {
    EntityColorCmd()
        : header(ENTITY_COLOR, sizeof(EntityColorCmd))
        , entity_id(-1)
        , part_id(-1)
        , color_occupy(-1)
        , color_channel(COLOR_NONE) {}
    CmdHeader header;       //命令头
    uint16_t entity_id;     //实体id
    uint16_t part_id;       //子部件id（可选）
    Color4c color;          //颜色
    uint8_t color_occupy;   //颜色占比0-100
    uint8_t color_channel;  //颜色通道ColorChannel
};

//旋翼
enum RotorType {
    AXIS_Y = 0,
    AXIS_X,
    AXIS_Z
};
struct RotorCmd : public BaseCmd {
    RotorCmd()
        : BaseCmd(ROTOR, sizeof(RotorCmd))
        , entity_id(-1)
        , part_id(-1)
        , rpm(0.0f)
        , type(AXIS_Y) {}
    uint16_t entity_id;  //实体ID
    uint16_t part_id;    //子部件id（可选）
    float rpm;           //转/分，
    uint8_t type;        //旋转类型RotorType
};

//实体状态属性位标记
enum EntityFlag {
    ETT_SHOW = 0x01,           //是否显示
    ETT_DELETE = 0x02,         //删除
    ETT_COLLIDE = 0x04,        //是否开启碰撞
    ETT_ON_GROUND = 0x08,      //是否贴地
    ETT_EXTRAPOLATION = 0x10,  //是否需要插值
    ETT_ACTIVE = 0x20,         //0: 非激活状态 1: 激活状态（渲染、更新位置、参与碰撞检测）2:特效重新播放特效;
    ETT_PLAY = 0x40,           //0: 停止播放，1: 开始播放
    ETT_PAUSE = 0x80           //0: 暂停播放，1: 继续播放
};
//实体状态命令，可利用flag_mask同时设置多个状态标记
struct EntityStatusCmd : public BaseCmd {
    EntityStatusCmd()
        : BaseCmd(ENTITY_STATUS, sizeof(EntityStatusCmd))
        , entity_id(-1)
        , part_id(-1)
        , entity_flag(0)
        , flag_mask(0) {}
    void SetStatus(EntityFlag flag, bool on = true) {
        flag_mask |= flag;
        if (on)
            entity_flag |= flag;
        else
            entity_flag &= ~flag;
    }
    uint16_t entity_id;    //实体ID
    uint16_t part_id;      //子部件id（可选）
    uint16_t entity_flag;  //实体状态属性位标记EntityFlag
    uint16_t flag_mask;    //标记entity_flag中哪些位是有效位
};

//实体缩放命令
struct EntityScaleCmd : public BaseCmd {
    EntityScaleCmd()
        : BaseCmd(ENTITY_SCALE, sizeof(EntityScaleCmd)), entity_id(-1), part_id(-1), scale_x(1.0f), scale_y(1.0f), scale_z(1.0f) {}
    uint16_t entity_id;  //实体ID
    uint16_t part_id;    //子部件id（可选）
    float scale_x;       //x方向缩放
    float scale_y;       //y方向缩放
    float scale_z;       //z方向缩放
};

//扩展实体数据命令，用于携带自定义实体数据
struct ExtEntityCmd : public BaseCmd {
    ExtEntityCmd()
        : BaseCmd(EXT_ENTITY, sizeof(VSVLink::ExtEntityCmd))
        , entity_id(-1)
        , part_id(-1) {}
    //注意需设置payload数据长度以更新 header.cmd_size ！
    void SetPayloadSize(uint16_t payload_len) {
        header.cmd_size = sizeof(VSVLink::ExtEntityCmd) + payload_len;
    }
    uint16_t GetPlayoadSize() const {
        return header.cmd_size - sizeof(VSVLink::ExtEntityCmd);
    }
    uint16_t entity_id;  //实体ID
    uint16_t part_id;    //子部件ID（可选）
#pragma warning(push)
#pragma warning(disable : 4200)
    char payload[0];  //柔性数组，携带自定义数据
#pragma warning(pop)
};

//实体开关切换命令
struct SwitchCmd : public BaseCmd {
    SwitchCmd()
        : BaseCmd(SWITCH, sizeof(SwitchCmd))
        , entity_id(-1)
        , part_id(-1)
        , level(-1) {}
    uint16_t entity_id;  //实体ID
    uint16_t part_id;    //子部件id（可选）
    uint8_t level;       //层级
};

//实体云
enum CloudType {
    //=========低云族============
    Cu = 0,  //大类：积云
    //积云子类：
    Cu_hum,   //淡积云
    Cu_me,    //中积云
    Fc,       //碎积云
    Cu_cong,  //浓积云

    Cb = 10,  //大类：积雨云
    //积雨云子类：
    Cb_calv,  //秃积雨云
    Cb_cap,   //鬃状积雨云
    Cb_in,    //砧状积雨云

    Sc = 20,  //大类：层积云
    //层积云子类：
    Sc_tra,   //透光层积云
    Sc_op,    //蔽光层积云
    Sc_cug,   //积云性层积云
    Sc_cast,  //堡状层积云
    Sc_lent,  //荚状层积云

    St = 30,  //大类：层云
    //层云子类：
    ST,  //层云
    Fs,  //碎层云

    Ns = 40,  //大类：雨层云

    //=========中云族============
    As = 50,  //大类：高层云
    //高层云子类：
    As_tra,  //透光高层云
    As_op,   //蔽光高层云

    Ac = 60,  //大类：高积云
    //高积云子类:
    Ac_tra,   //透光高积云
    Ac_op,    //蔽光高积云
    Ac_lent,  //荚状高积云
    Ac_cug,   //积云性高积云
    Ac_flo,   //絮状高积云
    Ac_cast,  //堡状高积云

    //=========高云族============
    Ci = 70,  //大类：卷云
    //卷云子类：
    Ci_fil,   //毛卷云
    Ci_dens,  //密卷云
    Ci_not,   //伪卷云
    Ci_unc,   //钩卷云

    Cs = 80,  //大类：卷层云
    //卷层云子类：
    Cs_fil,   //毛卷层云
    Cs_nebu,  //薄幕卷层云

    Cc = 90,  //大类：卷积云

    VA = 200,  //火山灰
};
struct EntityCloudCmd : public BaseCmd {
    EntityCloudCmd()
        : BaseCmd(ENTITY_CLOUD, sizeof(EntityCloudCmd))
        , entity_id(-1)
        , layer_id(0)
        , cloud_type(CloudType::Cb)
        , enable(true)
        , thickness(-1)
        , brightness(-1)
        , rotate_speed(-1) {}
    uint16_t entity_id;  //实体ID
    uint8_t layer_id;    //云层id
    uint8_t cloud_type;  //云类型
    uint8_t enable;
    float thickness;
    float brightness;
    float rotate_speed;
};

//动画
enum AnimationState {
    Animation_Stop = 0,
    Animation_Play = 1,
    Animation_Pause = 2,
    Animation_Continue = 3,
    Animation_Manual_Time = 4,
    Animation_Manual_Progress = 5
};

struct AnimationCmd : public BaseCmd {
    AnimationCmd()
        : BaseCmd(ANIMATION, sizeof(AnimationCmd))
        , entity_id(-1)
        , part_id(-1)
        , clip_id(-1)
        , state_value(-1.0f)
        , loop_mode(0)
        , state(Animation_Play) {}
    uint16_t entity_id;  //实体ID
    uint16_t part_id;    //子部件id（可选）
    uint16_t clip_id;    //动画ID
    float state_value;   //动画速度/动画时间/动画进度
    uint8_t loop_mode;   //是否循环播放
    uint8_t state;       //播放状态控制
};

//反透视。超过一定距离实体不再按透视规则缩小
struct AntiPerspectiveCmd : public BaseCmd {
    AntiPerspectiveCmd()
        : BaseCmd(ANTI_PERSPECTIVE, sizeof(AntiPerspectiveCmd))
        , entity_id(-1)
        , distance(0.0f)
        , factor(-1.0f) {}
    uint16_t entity_id;  //实体id
    float distance;      //反透视计算的距离
    float factor;        //反透视因子，预留
};

//速度
struct VelocityCmd : public BaseCmd {
    VelocityCmd()
        : BaseCmd(VELOCITY, sizeof(VelocityCmd))
        , entity_id(-1)
        , part_id(-1)
        , velocity_x(FLT_MAX)
        , velocity_y(FLT_MAX)
        , velocity_z(FLT_MAX)
        , angular_velocity_x(FLT_MAX)
        , angular_velocity_y(FLT_MAX)
        , angular_velocity_z(FLT_MAX)
        , local_coord(false) {
    }
    uint16_t entity_id;  //实体ID
    uint16_t part_id;    //子部件id（可选）
    float velocity_x;
    float velocity_y;
    float velocity_z;
    float angular_velocity_x;
    float angular_velocity_y;
    float angular_velocity_z;
    uint8_t local_coord;  //是否在局部空间
};

//加速度
struct AccelerationCmd : public BaseCmd {
    AccelerationCmd()
        : BaseCmd(ACCELERATION, sizeof(AccelerationCmd))
        , entity_id(-1)
        , part_id(-1)
        , acceleration_x(FLT_MAX)
        , acceleration_y(FLT_MAX)
        , acceleration_z(FLT_MAX)
        , angular_acceleration_x(FLT_MAX)
        , angular_acceleration_y(FLT_MAX)
        , angular_acceleration_z(FLT_MAX)
        , local_coord(false) {}
    uint16_t entity_id;  //实体ID
    uint16_t part_id;    //子部件id（可选）
    float acceleration_x;
    float acceleration_y;
    float acceleration_z;
    float angular_acceleration_x;
    float angular_acceleration_y;
    float angular_acceleration_z;
    uint8_t local_coord;  //是否在局部空间
};

//菲涅尔灯
struct FresnelCmd : public BaseCmd {
    FresnelCmd()
        : BaseCmd(FRESNEL, sizeof(FresnelCmd))
        , entity_id(-1)
        , part_id(-1)
        , target_id(-1)
        , shut_down(false)
        , horizon(true)
        , refly(false)
        , angle(0.0f)
        , brightness(100) {}
    uint16_t entity_id;  //实体ID
    uint16_t part_id;    //子部件id（可选）
    uint16_t target_id;  //关联的实体ID
    bool shut_down;      // 是否关闭
    bool horizon;        // 是否打开水平基准灯
    bool refly;          // 是否要复飞
    float angle;         // 下滑角度
    uint8_t brightness;  // 亮度 0~100
};

//阻拦索属性位标记
enum ArrestingRopeFlag {
    ARRESTING_CONNECT = 0x01,           // 阻拦索手动挂钩
    ARRESTING_DISCONNECT = 0x02,        // 阻拦索解开挂钩
    ARRESTING_AUTOCONNECT_ON = 0x04,    // 阻拦索自动判断挂钩打开
    ARRESTING_AUTOCONNECT_OFF = 0x08,   // 阻拦索自动判断挂钩关闭
    ARRESTING_HOOKDIST_REQUEST = 0x10,  // 阻拦索请求
};
//阻拦索
struct ArrestingRopeCmd : public BaseCmd {
    ArrestingRopeCmd()
        : BaseCmd(ARRESTINGROPE, sizeof(ArrestingRopeCmd))
        , hook_entity_id(-1)
        , hook_part_id(-1)
        , arresting_entity_id(-1)
        , arresting_part_id(-1)
        , arresting_flag(0) {}
    void SetFlag(ArrestingRopeFlag flag) {
        arresting_flag |= flag;
    }
    uint16_t hook_entity_id;       // 挂点实体ID
    uint16_t hook_part_id;         // 挂点子部件ID(可选)
    uint16_t arresting_entity_id;  // 阻拦索ID
    uint16_t arresting_part_id;    // 阻拦索子部件ID(可选)
    uint16_t arresting_flag;       // 阻拦索属性位标记
};

//管节点
struct PipeNodeCmd : public BaseCmd {
    PipeNodeCmd()
        : BaseCmd(PIPENODE, sizeof(PipeNodeCmd))
        , entity_id(-1)
        , part_id(-1)
        , node_id(-1)
        , offset_x(FLT_MAX)
        , offset_y(FLT_MAX)
        , offset_z(FLT_MAX) {}
    uint16_t entity_id;  //实体ID
    uint16_t part_id;    //子部件id
    uint16_t node_id;    //节点id>=0
    float offset_x;      //相对父节点x偏移
    float offset_y;      //相对父节点y偏移
    float offset_z;      //相对父节点z偏移
};

//加油管连接
struct OilPipeCmd : public BaseCmd {
    OilPipeCmd()
        : BaseCmd(OILPIPE, sizeof(OilPipeCmd))
        , oil_entity_id(-1)
        , oil_part_id(-1)
        , oil_receive_entity_id(-1)
        , oil_receive_part_id(-1) {}
    uint16_t oil_entity_id;          //加油机ID
    uint16_t oil_part_id;            //加油机的加油管子部件id
    uint16_t oil_receive_entity_id;  //受油机ID
    uint16_t oil_receive_part_id;    //受油机 受油管头子部件id（-1时断开连接）
};

//尾喷
struct TailSprayCmd : public BaseCmd {
    TailSprayCmd()
        : BaseCmd(TAILSPRAY, sizeof(TailSprayCmd))
        , entity_id(-1)
        , part_id(-1)
        , scale(1) {}
    uint16_t entity_id;  //飞机ID
    uint16_t part_id;    //尾喷子部件id
    float scale;         //尾喷大小 0~1
};

//////////////////////////////////////////////////////////////////////////
// 环境相关命令

//降水
struct PrecipitateCmd : public BaseCmd {
    PrecipitateCmd()
        : BaseCmd(PRECIPITATE, sizeof(PrecipitateCmd))
        , enable(true)
        , rain_level(0)
        , snow_level(0)
        , hail_level(0)
        , height(-1.0f) {}
    uint8_t enable;      //降水开关
    uint8_t rain_level;  //雨等级 0-5
    uint8_t snow_level;  //雪等级 0-5
    uint8_t hail_level;  //冰雹等级 0-5
    float height;        //高度范围 (可选值）
};

//雾
struct FogCmd : public BaseCmd {
    FogCmd()
        : BaseCmd(FOG, sizeof(FogCmd))
        , enable(true)
        , visibility(-1.0f)
        , height(-1.0f)
        , transition_thickness(-1.0f)
        , coverage(-1)
        , turbidity(-1) {}
    uint8_t enable;              //雾开关
    float visibility;            //能见度
    float height;                //高度范围
    float transition_thickness;  //过渡区厚度
    uint8_t coverage;            //不均匀雾覆盖率
    uint8_t turbidity;           //雾浑浊度0~100
};

//风
struct WindCmd : public BaseCmd {
    WindCmd()
        : BaseCmd(WIND, sizeof(WindCmd))
        , enable(true)
        , bottom_height(-1)
        , direction(-1)
        , speed(0)
        , layer_id(0)
        , top_height(-1)
        , transition_thickness(0) {}

    uint8_t enable;              //风开关
    float bottom_height;         //风场高度
    float top_height;            //风场高度
    float direction;             //风吹来的方向。以北为正， 0~360
    float speed;                 //米/秒
    uint8_t layer_id;            //风层ID
    float transition_thickness;  //过渡区厚度
};

//云
struct CloudCmd : public BaseCmd {
    CloudCmd()
        : BaseCmd(CLOUD, sizeof(CloudCmd))
        , enable(true)
        , layer_id(0)
        , cloud_type(Cu_hum)
        , coverage(-1)
        , bottom_height(1000)
        , top_height(-1) {}
    uint8_t enable;       //云层开关
    uint8_t layer_id;     //云层id
    uint8_t cloud_type;   //云类型
    uint8_t coverage;     //覆盖率 [0-100]
    float bottom_height;  //云底高
    float top_height;     //云顶高(可选值)
};

//时间
struct TimeCmd : public BaseCmd {
    TimeCmd()
        : BaseCmd(TIME, sizeof(TimeCmd))
        , enable(true)
        , hour(12)
        , minute(0) {}
    uint8_t enable;  //时间开关
    uint8_t hour;    //时
    uint8_t minute;  //分
};

//地平线薄雾
struct HazeCmd : public BaseCmd {
    HazeCmd()
        : BaseCmd(HAZE, sizeof(HazeCmd))
        , enable(true)
        , density(-1) {}
    uint8_t enable;   //地平线薄雾开关
    uint8_t density;  //地平线薄雾浓度(0~100)
};

//太阳光芒
struct SunLightCmd : public BaseCmd {
    SunLightCmd()
        : BaseCmd(SUNLIGHT, sizeof(SunLightCmd))
        , enable(true) {}
    uint8_t enable;  //太阳光芒模块开关
};

//月亮
struct MoonCmd : public BaseCmd {
    MoonCmd()
        : BaseCmd(MOON, sizeof(MoonCmd))
        , enable(true)
        , brightness(100) {}
    uint8_t enable;      //月亮开关
    uint8_t brightness;  //月亮亮度
};

//星星
struct StarCmd : public BaseCmd {
    StarCmd()
        : BaseCmd(STAR, sizeof(StarCmd))
        , enable(true)
        , brightness(100)
        , level(2) {}
    uint8_t enable;      //星星开关
    uint8_t level;       //星星等级
    uint8_t brightness;  //星星亮度
};

//大气
struct AtmosphereCmd : public BaseCmd {
    AtmosphereCmd()
        : BaseCmd(ATMOSPHERE, sizeof(AtmosphereCmd))
        , enable(true)
        , global_visibility(-1.0f)
        , aerosol_hight(-1.0f)
        , aerosol_density(-1)
        , pollution_ratio(-1)
        , molecule_density(-1)
        , enable_luminance(-1) {}
    uint8_t enable;            //大气模块开关
    float global_visibility;   //大气能见度
    float aerosol_hight;       //大颗粒分布的高度范围(米)
    uint8_t aerosol_density;   //大颗粒物质密度, [0-100]
    uint8_t pollution_ratio;   //大颗粒物质中PM2.5所占比例, [0-100]
    uint8_t molecule_density;  //空气分子密度, [0-100]
    uint8_t enable_luminance;  //天空亮度开关, 0关闭, 1开启
};

//海洋
struct OceanCmd : public BaseCmd {
    OceanCmd()
        : BaseCmd(OCEAN, sizeof(OceanCmd))
        , enable(true)
        , level(0) {}
    uint8_t enable;
    uint8_t level;  //蒲福风级，0~12
};

//表面状态类型枚举
enum SurfaceConditionType {
    DRY = 0,  //干燥（默认）
    WET,      //积水
    SNOWY,    //积雪
    ICY       //结冰
};
//表面状态
struct SurfaceConditionCmd : public BaseCmd {
    SurfaceConditionCmd()
        : BaseCmd(SURFACE_CONDITION, sizeof(SurfaceConditionCmd))
        , enable(true)
        , type(DRY)
        , coverage(0)
        , enable_light_reflection(-1) {}
    uint8_t enable;
    uint8_t type;                     //表面状态类型SurfaceConditionType
    uint8_t coverage;                 //覆盖率[0-100]
    uint8_t enable_light_reflection;  //开启灯光反射 ( 0:关闭，1:开启，其余为无效值）
};

//日期
struct DateCmd : public BaseCmd {
    DateCmd()
        : BaseCmd(DATE, sizeof(DateCmd))
        , month(0)
        , day(0)
        , year(0) {}
    uint8_t month;  //月
    uint8_t day;    //日
    uint16_t year;  //年
};

//闪电
struct LightningCmd : public BaseCmd {
    LightningCmd()
        : BaseCmd(LIGHTNING, sizeof(LightningCmd))
        , enable(true)
        , severity(-1)
        , cloud_layer(-1) {}
    uint8_t enable;       //开关
    uint8_t severity;     //剧烈度[0-100]
    uint8_t cloud_layer;  //云层id
};

//云层见度
struct CloudVisibilityCmd : public BaseCmd {
    CloudVisibilityCmd()
        : BaseCmd(CLOUD_VIS, sizeof(CloudVisibilityCmd))
        , enable(true)
        , layer_id(0)
        , visibility(-1.0f)
        , bottom_transition(-1.0f)
        , top_transition(-1.0f) {}
    uint8_t enable;           //云层能见度开关
    uint8_t layer_id;         //云层id
    float visibility;         //云中能见度
    float bottom_transition;  //底部过渡区厚度
    float top_transition;     //顶部过渡区厚度
};

//云层偏移
struct CloudOffsetCmd : public BaseCmd {
    CloudOffsetCmd()
        : BaseCmd(CLOUD_OFFSET, sizeof(CloudOffsetCmd))
        , layer_id(0)
        , lon(0.0)
        , lat(0.0)
        , rotate_axis_heading(0.0f)
        , rotate_degrees(0.0f) {}
    uint8_t layer_id;           //云层id
    double lon;                 //偏移中心点经度
    double lat;                 //偏移中心点纬度
    float rotate_axis_heading;  //旋转轴对应heading（北为0度，东为90度）
    float rotate_degrees;       //旋转角度
};

//风暴云
struct StormCmd : public BaseCmd {
    StormCmd()
        : BaseCmd(STORM, sizeof(StormCmd))
        , enable(true)
        , storm_id(1)
        , lon(0.0)
        , lat(0.0)
        , alt(1000)
        , visibility(500.0f)
        , lightning_level(3)
        , rain_level(3) {}
    uint8_t enable;           // 风暴云开关
    uint8_t storm_id;         // 风暴云ID [1-254]
    uint8_t storm_type;       // 风暴云类型 [0-9]
    double lon;               // 风暴云经度
    double lat;               // 风暴云纬度
    double alt;               // 风暴云高度
    float heading;            // 风暴云朝向(北为0度，东为90度）
    float visibility;         // 风暴云中能见度
    uint8_t lightning_level;  // 风暴云闪电级别 [0-5]
    uint8_t rain_level;       // 风暴云降水级别[0-5]
};

enum ProtocolWeatherType {
    PROTOCOL_WEATHER_CLOUD = 0,
    PROTOCOL_WEATHER_RAIN,
    PROTOCOL_WEATHER_SNOW,
    PROTOCOL_WEATHER_HAIL,
    PROTOCOL_WEATHER_FOG,
    PROTOCOL_WEATHER_WIND,
    PROTOCOL_WEATHER_SANDSTORM,
    PROTOCOL_WEATHER_OCEAN,
    PROTOCOL_WEATHER_STAR
};

union ProtocolWeatherParam {
    struct {
        int32_t layer_id;  //云层id
        int32_t padding;
    } cloud;
    struct {
        int32_t level;  //雨/雪/冰雹等级 0-5
        float height;   //高度范围 (可选值）
    } precipitate;
    struct {
        float visibility;  //能见度(雾和沙尘共用，后设的有效)
        float height;      //高度范围
    } fog;
    struct {
        int32_t layer_id;  //风层id
        int32_t padding;
    } wind;
    struct {
        int32_t level;  //海洋等级, 0~12
        int32_t padding;
    } ocean;
    struct {
        int32_t level;       // 星星等级, 0-5
        int32_t brightness;  // 星星亮度, 0-100
    } star;
};

// 气象控制
struct WeatherCmd : public BaseCmd {
    WeatherCmd()
        : BaseCmd(WEATHER, sizeof(WeatherCmd))
        , enable(true)
        , region_id(-1)
        , type(-1) {
        param.cloud.layer_id = param.cloud.padding = 0;
    }
    uint8_t enable;     // 开关
    uint8_t region_id;  // 区域ID
    uint8_t type;       // 气象类型
    ProtocolWeatherParam param;
};

//////////////////////////////////////////////////////////////////////////
// 通用机场相关命令

// 机场
struct AirportCmd : public BaseCmd {
    AirportCmd()
        : BaseCmd(AIRPORT, sizeof(AirportCmd))
        , entity_id(-1)
        , lon(DBL_MAX)
        , lat(DBL_MAX)
        , alt(DBL_MAX)
        , ground_type(0)
        , operator_mode(0) {
        memset(name, 0, 4);
    }
    char name[4];        // 三字码(IATA)或四字码(ICAO)
    uint16_t entity_id;  // 实体ID
    double lon;          // 经度
    double lat;          // 纬度
    double alt;          // 海拔高度
    int ground_type;     // 地面类型
    int operator_mode;   // 机场(0)创建,(1)销毁
};

enum SIDE {
    RUNWAY_LEFT = 0,
    RUNWAY_RIGHT
};

// 停机坪
struct ApronCmd : public BaseCmd {
    ApronCmd()
        : BaseCmd(APRON, sizeof(ApronCmd))
        , airport_id(-1)
        , runway_part_id(-1)
        , part_id(-1)
        , type(-1)
        , side(VSVLink::RUNWAY_LEFT) {}

    uint16_t airport_id;      // 机场entity_id
    uint16_t runway_part_id;  // 跑道part_id
    uint16_t part_id;         // 子节点id
    int type;
    int side;
};

// 机库
struct HangarCmd : public BaseCmd {
    HangarCmd()
        : BaseCmd(HANGAR, sizeof(HangarCmd))
        , airport_id(-1)
        , runway_part_id(-1)
        , part_id(-1)
        , type(-1)
        , side(VSVLink::RUNWAY_LEFT) {}
    uint16_t airport_id;      // 机场entity_id
    uint16_t runway_part_id;  // 跑道part_id
    uint16_t part_id;         // 子节点id
    int type;
    int side;
};

// 候机楼
struct AirTerminalCmd : public BaseCmd {
    AirTerminalCmd()
        : BaseCmd(AIRTERMINAL, sizeof(AirTerminalCmd))
        , airport_id(-1)
        , runway_part_id(-1)
        , part_id(-1)
        , type(-1)
        , side(VSVLink::RUNWAY_LEFT) {}
    uint16_t airport_id;      // 机场entity_id
    uint16_t runway_part_id;  // 跑道part_id
    uint16_t part_id;         // 子节点id
    int type;
    int side;
};

// 候机楼
struct TowerCmd : public BaseCmd {
    TowerCmd()
        : BaseCmd(TOWER, sizeof(TowerCmd))
        , airport_id(-1)
        , runway_part_id(-1)
        , part_id(-1)
        , type(-1)
        , side(VSVLink::RUNWAY_LEFT) {}
    uint16_t airport_id;      // 机场entity_id
    uint16_t runway_part_id;  // 跑道part_id
    uint16_t part_id;         // 子节点id
    int type;
    int side;
};

// 跑道
struct RunwayCmd : public BaseCmd {
    RunwayCmd()
        : BaseCmd(RUNWAY, sizeof(RunwayCmd))
        , airport_id(-1)
        , part_id(-1)
        , lon(DBL_MAX)
        , lat(DBL_MAX)
        , alt(DBL_MAX)
        , length(0)
        , width(0)
        , heading(0)
        , slope_angle(0.0f)
        , is_light_always_on(false) {
        memset(num_0, 0, 3);
        memset(num_1, 0, 3);
    }
    uint16_t airport_id;      // 机场entity_id
    uint16_t part_id;         // 子节点id
    double lon;               // 跑道位置
    double lat;               // 跑道位置
    double alt;               // 跑道位置
    float length;             // 长度
    float width;              // 宽度
    float heading;            // 朝向
    float slope_angle;        // 坡度
    bool is_light_always_on;  // 是否灯光常量
    char num_0[3];            // 跑道编号
    char num_1[3];
};

// 滑行道
struct TaxiwayCmd : public BaseCmd {
    TaxiwayCmd()
        : BaseCmd(TAXIWAY, sizeof(TaxiwayCmd))
        , airport_id(-1)
        , runway_part_id(-1)
        , part_id(-1)
        , type(-1)
        , side(VSVLink::RUNWAY_LEFT) {}
    uint16_t airport_id;      // 机场entity_id
    uint16_t runway_part_id;  // 跑道part_id
    uint16_t part_id;         // 子节点id
    int type;
    int side;
    //uint8_t TCL;  // 滑行道中线灯 0/1
    //uint8_t TEL;  // 滑行道边灯 0/1
};

enum ALSType {
    ALSNONE,
    ALSF1,
    ALSF2,
    CALVERT,
    CALVERT2,
    MALS,
    MALSF,
    MALSR,
    ODALS,
    RAIL,
    SALS,
    SALSF,
    SSALF,
    SSALR,
    SSALS
};

enum VASIType {
    VASINONE,
    PAPI2,     // (2 light PAPI)
    PAPI4,     // (4 light PAPI)
    PVASI,     //(Pulsating VASI)
    TRICOLOR,  // Tri Color VASI
    TVASI,     // colored VASI in a shared-'T' shape
    VASI21,    // 2 rows, 1 box/row
    VASI22,    // 2 rows, 2 boxes/row
    VASI23,    // 2 rows, 3 boxes/row
    VASI31,    // 3 rows, 1 box/row
    VASI32,    // 3 rows, 2 boxes/row
    VASI33,    // 3 rows, 3 boxes/row (far row has only two boxes, 8 light system)
    BALL,
    APAP,  // panels
    PANELS
};

// 跑道灯光类型
enum RunwayLightType {
    RWL_RTHL = 0x01,  // 入口/末端灯
    RWL_REIL = 0x02,  // 跑道末端指示灯
    RWL_TDZL = 0x04,  // 接地带灯
};

struct RunwayLightCmd : public BaseCmd {
    RunwayLightCmd()
        : BaseCmd(RUNWAYLIGHT, sizeof(RunwayLightCmd))
        , airport_id(-1)
        , runway_part_id(-1)
        , part_id(-1)
        , RCL(0)
        , REL(0) {
        ALS_type[0] = VSVLink::ALSNONE;
        ALS_type[1] = VSVLink::ALSNONE;
        VASI_type[0] = VSVLink::VASINONE;
        VASI_type[1] = VSVLink::VASINONE;
    }
    uint16_t airport_id;
    uint16_t runway_part_id;
    uint16_t part_id;

    uint8_t ALS_type[2];         // ALSType进近灯光类型
    uint8_t VASI_type[2];        // VASI/PAPI类型
    uint8_t light_type_flag[2];  // RunwayLightType
    uint8_t RCL;                 // 跑道中线灯 0/1
    uint8_t REL;                 // 跑道边灯 0/1
};

//////////////////////////////////////////////////////////////////////////
// 图元相关命令

//屏幕颜色
struct ScreenColorCmd : public BaseCmd {
    ScreenColorCmd()
        : BaseCmd(SCREEN_COLOR, sizeof(ScreenColorCmd))
        , enable(true)
        , r(0)
        , g(0)
        , b(0)
        , a(0) {}
    uint8_t enable;
    uint8_t r, g, b, a;  //颜色值
};

//点样式
struct PointStyleCmd : public BaseCmd {
    PointStyleCmd()
        : BaseCmd(POINT_STYLE, sizeof(PointStyleCmd))
        , feature_id(-1)
        , point_size(-1) {}
    uint16_t feature_id;  //图元ID
    uint16_t point_size;  //点大小
    Color4c color;        //点颜色
};

//描边样式
struct LineStyleCmd : public BaseCmd {
    LineStyleCmd()
        : BaseCmd(LINE_STYLE, sizeof(LineStyleCmd))
        , feature_id(-1)
        , line_width(-1)
        , stipple_pattern(-1)
        , stipple_factor(-1) {}
    uint16_t feature_id;       //图元ID
    uint16_t line_width;       //描边宽度
    Color4c color;             //描边颜色
    uint16_t stipple_pattern;  //虚线模式
    uint16_t stipple_factor;   //虚线模式倍数
};

//填充样式
struct FillStyleCmd : public BaseCmd {
    FillStyleCmd()
        : BaseCmd(FILL_STYLE, sizeof(FILL_STYLE))
        , feature_id(-1) {}
    uint16_t feature_id;
    Color4c color;  //填充颜色
};

//文字样式
enum TextAlignment {
    UNSPECIFIED = 0,  //未指定
    LEFT_BOTTOM,
    LEFT_BASELINE,
    LEFT_MIDDLE,
    LEFT_TOP,
    ORIGIN_BOTTOM,
    ORIGIN_BASELINE,
    ORIGIN_MIDDLE,
    ORIGIN_TOP,
    CENTER_BOTTOM,
    CENTER_BASELINE,
    CENTER_MIDDLE,
    CENTER_TOP,
    RIGHT_BOTTOM,
    RIGHT_BASELINE,
    RIGHT_MIDDLE,
    RIGHT_TOP
};
struct TextStyleCmd : public BaseCmd {
    TextStyleCmd()
        : BaseCmd(TEXT_STYLE, sizeof(TextStyleCmd))
        , feature_id(-1)
        , font_size(-1)
        , font_id(-1)
        , alignment(UNSPECIFIED) {}
    uint16_t feature_id;
    uint16_t font_size;
    Color4c color;
    uint8_t font_id;
    uint8_t alignment;  //文字对齐 TextAlignment
};

//坐标类型
enum CoordType {
    INVALID_COORD,  //无效
    GEODETIC,       //大地坐标系（经纬高）
    WORLD_COORD,    //地心坐标系（xyz）
    ENTITY_COORD,   //相对于实体的局部空间
    CANVAS_COORD,   //画布空间
    SCREEN_COORD,   //屏幕空间
};

//二维图元控制
struct Feature2DCtrlCmd : public BaseCmd {
    Feature2DCtrlCmd()
        : BaseCmd(FEATURE_2D_CTRL, sizeof(Feature2DCtrlCmd))
        , feature_id(-1)
        , parent_id(-1)
        , canvas_id(-1)
        , layer(-1)
        , coord_type(INVALID_COORD)
        , position_u(FLT_MAX)
        , position_v(FLT_MAX) {}
    uint16_t feature_id;  //图元ID
    uint16_t parent_id;   //父图元ID
    uint16_t canvas_id;   //画布ID，仅用于coord_type为ENTITY_COORD
    uint16_t layer;       //图层ID，影响图元绘制排序
    uint8_t coord_type;   //CoordType坐标类型，CANVAS_COORD或SCREEN_COORD
    float position_u;     //u坐标
    float position_v;     //v坐标
    float rotation;       //旋转角度
    float scale_u;        //u缩放
    float scale_v;        //v缩放
};

//三维图元控制
struct Feature3DCtrlCmd : public BaseCmd {
    Feature3DCtrlCmd()
        : BaseCmd(FEATURE_2D_CTRL, sizeof(Feature3DCtrlCmd))
        , feature_id(-1)
        , parent_id(-1)
        , entity_id(-1)
        , coord_type(INVALID_COORD)
        , lon(DBL_MAX)
        , lat(DBL_MAX)
        , alt(DBL_MAX)
        , yaw(FLT_MAX)
        , pitch(FLT_MAX)
        , roll(FLT_MAX) {}
    uint16_t feature_id;  //图元ID
    uint16_t parent_id;   //父图元ID
    uint16_t entity_id;   //实体ID，仅用于coord_type为ENTITY_COORD
    uint8_t coord_type;   //CoordType坐标类型，GEODETIC或ENTITY_COORD
    union {
        double lon;
        double offset_x;
    };
    union {
        double lat;
        double offset_y;
    };
    union {
        double alt;
        double offset_z;
    };
    union {
        float pitch;
        float euler_x;
    };
    union {
        float yaw;
        float euler_y;
    };
    union {
        float roll;
        float euler_z;
    };
};

//文本
struct TextCmd : public BaseCmd {
    TextCmd()
        : BaseCmd(FEATURE_TEXT, sizeof(BaseCmd) + sizeof(feature_id))
        , feature_id(-1) {}
    //提供接口设置tesxt内容以更新header.cmd_size
    void SetText(const std::string &txt) {
        text = txt;
        header.cmd_size = static_cast<std::uint16_t>(sizeof(BaseCmd) + sizeof(feature_id) + text.size() + 1);
    }
    std::string GetText() const {
        return text;
    }
    uint16_t feature_id;

private:
    std::string text;  //文本内容
};

//圆弧
struct CircleCmd : public BaseCmd {
    CircleCmd()
        : BaseCmd(FEATURE_CIRCLE, sizeof(CircleCmd))
        , feature_id(-1)
        , radius(FLT_MAX) {}
    uint16_t feature_id;
    float radius;
};

//扇形
struct CircularSectorCmd : public BaseCmd {
    CircularSectorCmd()
        : BaseCmd(FEATURE_CIRCULAR_SECTOR, sizeof(CircularSectorCmd))
        , feature_id(-1)
        , inner_radius(FLT_MAX)
        , angle_start(FLT_MAX)
        , angle_end(FLT_MAX) {}
    uint16_t feature_id;
    float radius;        //半径
    float inner_radius;  //内半径
    float angle_start;   //起始角度
    float angle_end;     //结束角度
};

//球
struct SphereCmd : public BaseCmd {
    SphereCmd()
        : BaseCmd(FEATURE_SPHERE, sizeof(SphereCmd))
        , feature_id(-1)
        , radius(FLT_MAX) {}
    uint16_t feature_id;
    float radius;
};

//球扇
struct SphericalSectorCmd : public BaseCmd {
    SphericalSectorCmd()
        : BaseCmd(FEATURE_CIRCULAR_SECTOR, sizeof(SphericalSectorCmd))
        , feature_id(-1)
        , radius(FLT_MAX)
        , inner_radius(FLT_MAX)
        , horiz_angle_start(FLT_MAX)
        , horiz_angle_end(FLT_MAX)
        , vert_angle_start(FLT_MAX)
        , vert_angle_end(FLT_MAX) {}
    uint16_t feature_id;
    float radius;             //半径
    float inner_radius;       //内半径
    float horiz_angle_start;  //水平张角开始（角度）
    float horiz_angle_end;    //水平张角结束（角度）
    float vert_angle_start;   //垂直张角起始（角度）
    float vert_angle_end;     //垂直张角结束（角度）
};

// 区域
struct RegionCmd : public BaseCmd {
    RegionCmd()
        : BaseCmd(REGION, sizeof(RegionCmd))
        , enable(true)
        , region_id(-1)
        , center_lon(0.0)
        , center_lat(0.0)
        , width(0.0f)
        , height(0.0f)
        , round_ratio(0.0f)
        , rot_angle(0.0f)
        , transition(0.1f) {}
    uint8_t enable;     //区域开关
    uint8_t region_id;  //区域ID
    double center_lon;  // 中心点经度
    double center_lat;  // 中心点纬度
    float width;        // 区域宽度（水平方向最大外边界，米）
    float height;       // 区域高度（垂直方向最大外边界，米）
    float round_ratio;  // 圆度系数0.0~1.0，0.0为矩形，1.0为椭圆或圆
    float rot_angle;    // 朝向，-180~180度，0为正北
    float transition;   // 过渡区距离，米
};

//////////////////////////////////////////////////////////////////////////
//场景相关命令

//添加场景
struct AddSceneCmd : public BaseCmd {
    AddSceneCmd()
        : BaseCmd(ADD_SCENE, sizeof(AddSceneCmd))
        , id(-1)
        , preload(false)
        , persist(true) {
        memset(name, 0, 256);
        memset(path, 0, 256);
    }
    uint8_t id;
    char name[256];
    char path[256];
    bool preload;
    bool persist;
};

//加载场景
struct LoadSceneCmd : public BaseCmd {
    LoadSceneCmd()
        : BaseCmd(LOAD_SCENE, sizeof(LoadSceneCmd)), id(-1) {}
    uint8_t id;  //场景id
};

//卸载场景
struct DeleteSceneCmd : public BaseCmd {
    DeleteSceneCmd()
        : BaseCmd(DELETE_SCENE, sizeof(DeleteSceneCmd)), id(-1) {}
    uint8_t id;  //场景id
};

//重置场景
struct ResetSceneCmd : public BaseCmd {
    ResetSceneCmd()
        : BaseCmd(RESET_SCENE, sizeof(ResetSceneCmd)), id(-1) {}
    uint8_t id;  //场景id
};

//////////////////////////////////////////////////////////////////////////
// 视景请求相关

//地形高度请求(Height of Terrain)
struct HOTRequestCmd : public BaseCmd {
    HOTRequestCmd()
        : BaseCmd(HOT_REQUEST, sizeof(HOTRequestCmd))
        , id(0)
        , lon(0)
        , lat(0) {}
    uint16_t id;  //请求ID，用于与反馈命令对应
    double lon;   //请求位置经度
    double lat;   //请求位置纬度
};

//离地高度请求(Height above Terrain)
struct HATRequestCmd : public BaseCmd {
    HATRequestCmd()
        : BaseCmd(HAT_REQUEST, sizeof(HATRequestCmd))
        , id(0)
        , lon(0)
        , lat(0)
        , alt(0) {}
    uint16_t id;  //请求ID，用于与反馈命令对应
    double lon;   //请求位置经度
    double lat;   //请求位置纬度
    double alt;   //请求位置海拔高度
};

//视线检测反馈目标类型
enum CollisionTargetType {
    TARGET_NONE = 0,    //无
    TARGET_OTHER = 1,   //其它
    TARGET_ENTITY = 2,  //实体
    TARGET_GROUND = 4,  //地面
    //...待补充
};

//视线线段查询，用于大地坐标系查询
struct LOSSegmentRequestCmd : public BaseCmd {
    LOSSegmentRequestCmd()
        : BaseCmd(LOS_SEGMENT_REQUEST, sizeof(LOSSegmentRequestCmd))
        , start_lon(0)
        , start_lat(0)
        , start_alt(0)
        , end_lon(0)
        , end_lat(0)
        , end_alt(0)
        , target_mask(-1) {}
    double start_lon;     //起点经度
    double start_lat;     //起点纬度
    double start_alt;     //起点高度
    double end_lon;       //终点经度
    double end_lat;       //终点纬度
    double end_alt;       //终点高度
    uint8_t target_mask;  //CollisionTargetType标记
    uint16_t id;          //请求ID，用于与反馈命令对应
};

//视线向量查询，用于相对entity查询
struct LOSVectorRequestCmd : public BaseCmd {
    LOSVectorRequestCmd()
        : BaseCmd(LOS_VECTOR_REQUEST, sizeof(LOSVectorRequestCmd))
        , entity_id(-1)
        , offset_x(0)
        , offset_y(0)
        , offset_z(0)
        , yaw(0)
        , pitch(0)
        , max_range(10000)
        , target_mask(-1) {}
    std::uint16_t entity_id;  //相对的实体
    float offset_x;           //相对x轴偏移
    float offset_y;           //相对y轴偏移
    float offset_z;           //相对z轴偏移
    float yaw;                //沿entity Y轴的旋转角度
    float pitch;              //沿entity X轴的旋转角度
    float max_range;          //查询最大范围
    uint8_t target_mask;      //CollisionTargetType标记
    uint16_t id;              //请求ID，用于与反馈命令对应
};

//表面类型请求
struct SurfaceTypeRequestCmd : public BaseCmd {
    SurfaceTypeRequestCmd()
        : BaseCmd(SURFACE_TYPE_REQUEST, sizeof(SurfaceTypeRequestCmd))
        , id(0)
        , lon(0)
        , lat(0) {}
    uint16_t id;
    double lon;  //请求位置经度
    double lat;  //请求位置纬度
};

//实体位置查询
struct EntityPosRequestCmd : public BaseCmd {
    EntityPosRequestCmd()
        : BaseCmd(ENTITY_POS_REQUEST, sizeof(EntityPosRequestCmd))
        , coord_type(INVALID_COORD)
        , entity_id(0)
        , part_id(-1) {}
    uint8_t coord_type;  //坐标类型
    uint16_t entity_id;  //实体ID
    uint16_t part_id;    //子部件ID（可选）
};

//地形高度请求扩展（相对实体）
struct HOTExtRequestCmd : public BaseCmd {
    HOTExtRequestCmd()
        : BaseCmd(HOT_EXT_REQUEST, sizeof(HOTExtRequestCmd))
        , id(0)
        , entity_id(-1)
        , part_id(-1)
        , offset_x(0.0f)
        , offset_y(0.0f)
        , offset_z(0.0f) {}
    uint16_t id;         //请求ID，用于与反馈命令对应
    uint16_t entity_id;  //相对的实体
    uint16_t part_id;    //子部件ID（可选）
    float offset_x;      //x方向偏移
    float offset_y;      //y方向偏移
    float offset_z;      //z方向偏移
};

//碰撞检测请求
enum class CollisionRequestType : uint8_t {
    ONE_SHOT_REPLY = 0,       //根据查询命令返回一次当前状态
    KEEP_REPLY,               //每帧持续返回状态
    ON_COLLISION_REPLY_ONCE,  //发生碰撞时反馈一次
    ON_COLLISION_KEEP_REPLY,  //发生碰撞时持续反馈
    STOP_REPLY,               //取消反馈
};

struct CollisionRequestCmd : public BaseCmd {
    CollisionRequestCmd()
        : BaseCmd(COLLISION_REQUEST, sizeof(CollisionRequestCmd))
        , entity_id(-1)
        , target_mask(-1)
        , request_type((uint8_t)CollisionRequestType::ON_COLLISION_REPLY_ONCE) {}
    uint16_t entity_id;
    uint8_t target_mask;   //CollisionTargetType碰撞目标
    uint8_t request_type;  //CollisionRequestType请求类型
};

//鼠标状态请求
enum class MouseStatusRequestType : uint8_t {
    ONE_SHOT_REPLY = 0,  //根据查询命令返回一次当前状态
    KEEP_REPLY,          //每帧持续返回状态
    STOP_REPLY,          //取消反馈
};

struct MouseStatusRequestCmd : public BaseCmd {
    MouseStatusRequestCmd()
        : BaseCmd(MOUSE_STATUS_REQUEST, sizeof(MouseStatusRequestCmd))
        , id(0)
        , request_type((uint8_t)MouseStatusRequestType::ONE_SHOT_REPLY)
        , coord_type(INVALID_COORD) {}
    uint16_t id;           //请求ID，用于与反馈命令对应
    uint8_t request_type;  //鼠标状态请求类型
    uint8_t coord_type;    //坐标类型当前支持 GEODETIC：大地坐标系（经纬高），WORLD_COORD：地心坐标系（xyz）， SCREEN_COORD：屏幕空间
};

enum class AstroType : uint8_t {
    SUN = 0,  // 太阳
    MOON,     // 月亮
    TypeMax
};

struct AstroStatusRequestCmd : public BaseCmd {
    AstroStatusRequestCmd()
        : BaseCmd(ASTRO_STATUS_REQUEST, sizeof(AstroStatusRequestCmd))
        , id(0)
        , astro_type(-1) {}
    uint16_t id;         //请求ID，用于与反馈命令对应
    uint8_t astro_type;  //AstroType 天体类型
};

//////////////////////////////////////////////////////////////////////////
// 外部反馈命令 视景到主控 IG -> Host

// IG状态枚举
enum IGStatus {
    IG_START = 0,  //已启动        daemon 收到开始命令  启动IG
    IG_LOADING,    //正在加载数据  IG 等待其他worker机 就绪
    IG_READY,      //正常就绪      所有worker机 已经就绪
    IG_ONLINE,     //主机在线      daemon  已经启动
    IG_OFFLINE     //主机下线      daemon  没启动或者没有开机
};
//开始帧，每帧反馈
struct StartFrameCmd : public BaseCmd {
    StartFrameCmd()
        : BaseCmd(START_FRAME, sizeof(StartFrameCmd)), frame_id(0), last_host_frame_id(0), timestamp(0), version(10100), status(0) {}
    uint32_t frame_id;            //帧号
    uint32_t last_host_frame_id;  //上一帧host帧号
    uint64_t timestamp;           //时间戳（微秒）
    uint16_t version;             //协议版本xyyzz，x: major, yy: minor, zz: patch
    uint8_t status;               // IG状态IGStatus
};

//地形高度反馈
struct HOTReplyCmd : public BaseCmd {
    HOTReplyCmd()
        : BaseCmd(HOT_REPLY, sizeof(HOTReplyCmd))
        , hot(0.0f) {}
    uint16_t id;  //Request命令中的id
    float hot;    //地形高度
};

//离地高度反馈
struct HATReplyCmd : public BaseCmd {
    HATReplyCmd()
        : BaseCmd(HAT_REPLY, sizeof(HATReplyCmd))
        , hat(0.0f) {}
    uint16_t id;  //Request命令中的id
    float hat;    //离地高度
};

//表面材质类型反馈
enum SurfaceType {
    UNKNOWN = 0,  //未知
    WATER,        //水面
    CONCRETE,     //水泥
    ASPHALT,      //沥青
    SNOW,         //雪
    ICE,          //冰
    GRASS,        //草地
    FOREST,       //树林
    DIRT,         //土
    SAND,         //沙地
    BRICK,        //砖
    METAL         //钢
};
struct SurfaceTypeReplyCmd : public BaseCmd {
    SurfaceTypeReplyCmd()
        : BaseCmd(SURFACE_TYPE_REPLY, sizeof(SurfaceTypeReplyCmd))
        , id(0)
        , type(UNKNOWN) {}
    uint16_t id;
    uint8_t type;
};

//视线检测反馈值
struct LOSReplyCmd : public BaseCmd {
    LOSReplyCmd()
        : BaseCmd(LOS_REPLY, sizeof(LOSReplyCmd))
        , target_type(-1)
        , entity_id(-1)
        , range(DBL_MAX)
        , lon(0.0)
        , lat(0.0)
        , alt(0.0f)
        , id(0) {}
    uint8_t target_type;  //检测到的目标类型
    uint16_t entity_id;   //检测到的entity id
    double range;         //检测到目标距离查询起点的距离
    double lon;
    double lat;
    double alt;
    uint16_t id;
};

//实体位置反馈
struct EntityPosReplyCmd : public BaseCmd {
    EntityPosReplyCmd()
        : BaseCmd(ENTITY_POS_REPLY, sizeof(EntityPosReplyCmd))
        , entity_id(-1)
        , part_id(-1)
        , coord_type(INVALID_COORD)
        , lon(0.0)
        , lat(0.0)
        , alt(0.0)
        , pitch(0.0)
        , yaw(0.0)
        , roll(0.0) {}
    uint16_t entity_id;
    uint16_t part_id;
    uint8_t coord_type;
    union {
        double lon;
        double x;
    };
    union {
        double lat;
        double y;
    };
    union {
        double alt;
        double z;
    };
    union {
        float pitch;
        float euler_x;
    };
    union {
        float yaw;
        float euler_y;
    };
    union {
        float roll;
        float euler_z;
    };
};

//碰撞检测反馈
struct CollisionReplyCmd : public BaseCmd {
    CollisionReplyCmd()
        : BaseCmd(COLLISION_REPLY, sizeof(CollisionReplyCmd))
        , entity_id(-1)
        , contact_entity_id(-1)
        , target_type(-1) {}
    uint16_t entity_id;
    uint16_t contact_entity_id;
    uint8_t target_type;  //CollisionTargetType碰撞类型
};

//阻拦索挂钩反馈
struct ArrestingHookAutoReplyCmd : public BaseCmd {
    ArrestingHookAutoReplyCmd()
        : BaseCmd(ARRESTINGHOOK_AUTOREPLY, sizeof(ArrestingHookAutoReplyCmd))
        , entity_id(-1)
        , part_id(-1)
        , hook_to_center(-1.0) {
    }
    uint16_t entity_id;    //阻拦索ID
    uint16_t part_id;      //阻拦索子部件ID（可选）
    float hook_to_center;  //挂钩位置距离阻拦索中心点距离
};

enum class MouseType : uint8_t {
    kLeftButtonDoubleClick = 0,
    kLeftButtonClick,
    kLeftButtonDown,
    kLeftButtonUp,
    kLeftButtonBeginDrag,
    kLeftButtonDrag,
    kLeftButtonEndDrag,
    kLeftButtonDragOver,

    kRightButtonDoubleClick,
    kRightButtonClick,
    kRightButtonDown,
    kRightButtonUp,
    kRightButtonBeginDrag,
    kRightButtonDrag,
    kRightButtonEndDrag,
    kRightButtonDragOver,

    kMiddleButtonDoubleClick,
    kMiddleButtonClick,
    kMiddleButtonDown,
    kMiddleButtonUp,
    kMiddleButtonBeginDrag,
    kMiddleButtonDrag,
    kMiddleButtonEndDrag,
    kMiddleButtonDragOver,

    kMouseLeave,
    kMouseWheel,
    kMouseMove,
    kMouseEnter
};

struct MouseStatusReplyCmd : public BaseCmd {
    MouseStatusReplyCmd()
        : BaseCmd(MOUSE_STATUS_REPLY, sizeof(MouseStatusReplyCmd))
        , id(-1)
        , coord_type(INVALID_COORD)
        , mouse_event_type(-1) {}
    uint16_t id;         //请求ID，用于与反馈命令对应
    uint8_t coord_type;  //坐标类型当前支持 GEODETIC：大地坐标系（经纬高），WORLD_COORD：地心坐标系（xyz）， SCREEN_COORD：屏幕空间下，x,y
    uint8_t mouse_event_type;
    union {
        double lon;
        double x;
    };
    union {
        double lat;
        double y;
    };
    union {
        double alt;
        double z;
    };
};

//飞机下滑辅助数据(AircarftGlider Assist Data)
struct AircarftGADAutoReplyCmd : public BaseCmd {
    AircarftGADAutoReplyCmd()
        : BaseCmd(AGAD_STATUS_AUTOREPLY, sizeof(AircarftGADAutoReplyCmd))
        , enity_id(1)
        , landgear_to_antenna_hori_distance(0.0)
        , landgear_to_antenna_vert_distance(0.0)
        , landgear_to_driver_eye_point_hori_distance(0.0)
        , landgear_to_driver_eye_point_vert_distance(0.0)
        , cockpit_static_dip_angle(0.0)
        , rw_threshold_to_extened_line_intercept_p_hori_distance(0.0)
        , glide_slope_angle(0.0)
        , pitch_angle(0.0)
        , runway_marking("") {}
    void Clear() {
        enity_id = 0;
        landgear_to_antenna_hori_distance = 0.0;
        landgear_to_antenna_vert_distance = 0.0;
        landgear_to_driver_eye_point_hori_distance = 0.0;
        landgear_to_driver_eye_point_vert_distance = 0.0;
        cockpit_static_dip_angle = 0.0;
        rw_threshold_to_extened_line_intercept_p_hori_distance = 0.0;
        glide_slope_angle = 0.0;
        pitch_angle = 0.0;

        for (size_t si = 0; si < 4; si++) {
            runway_marking[si] = '\0';
        }
    }
    uint16_t enity_id;                                             //  请求ID，用于与反馈命令对应
    float landgear_to_antenna_hori_distance;                       //  从主起落架到下滑道接收天线的水平距离
    float landgear_to_antenna_vert_distance;                       //  从主起落架到下滑道接收天线的垂直距离
    float landgear_to_driver_eye_point_hori_distance;              //  从主机起落到驾驶员眼点的水平和垂直距离
    float landgear_to_driver_eye_point_vert_distance;              //  从主机起落到驾驶员眼点的水平和垂直距离
    float cockpit_static_dip_angle;                                //  驾驶舱的静态倾角
    float rw_threshold_to_extened_line_intercept_p_hori_distance;  //  从跑道入口到跑道延长线上方下滑道截获点的水平距离
    float glide_slope_angle;                                       //  下滑到坡度角度
    float pitch_angle;                                             //  进近时的飞机俯仰角；
    char runway_marking[4] = {'\0'};                               //  跑道标识
};

struct AstroStatusReplyCmd : public BaseCmd {
    AstroStatusReplyCmd()
        : BaseCmd(ASTRO_STATUS_REPLY, sizeof(AstroStatusReplyCmd))
        , id(0)
        , lon(-400.0)
        , lat(-400.0)
        , distance(-1.0) {}
    uint16_t id;      //请求ID，用于与请求命令对应
    double lon;       // 经度
    double lat;       // 纬度
    double distance;  // 天体据地心距离
};

//控制台命令
struct ConsoleCmd {
    CmdHeader header{CONSOLE_CMD, sizeof(ConsoleCmd)};  //命令头
    char cmd[128] = {0};                                //命令
};

//扩展命令，用于携带传输其他数据
struct ExtCommandCmd {
    CmdHeader header{EXT_COMMAND, 0};  //命令头，注意header.cmd_size需手工赋值！
    char protocol[16] = {0};           //可选键，用于表示协议
#pragma warning(push)
#pragma warning(disable : 4200)
    char payload[0];  //命令数据
#pragma warning(pop)
};

}  // namespace VSVLink
#pragma pack(pop)