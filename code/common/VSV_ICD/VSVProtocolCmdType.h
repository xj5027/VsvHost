#pragma once
//////////////////////////////////////////////////////////////////////////
// \copyright 2023 VSV.Inc
// \brief 视景外部通信数据协议 ViCMD
// \author gaimeng
//////////////////////////////////////////////////////////////////////////

#pragma pack(push)
#pragma pack(1)
namespace VSVLink {
//命令类型枚举
enum ViCMD_TYPE {
    INVALID = 0,  //无效命令

    //////////////////////////////////////////////////////////////////////////
    //外部命令类型 主控到视景 Host->IG
    //////////////////////////////////////////////////////////////////////////
    EXTERNAL_CMDS_START = 1,  //标记外部命令起始位置
    BEGIN_FRAME,              //开始帧，每帧发送
    IG_CTRL,                  //视景状态控制（暂停、恢复、调试等）

    //视口相关
    VIEW_DEF,   //视口定义
    VIEW_CTRL,  //视口控制
    VIEW_SIZE,  //视口大小

    //实体相关命令
    ENTITY_CMDS_START = 10,  //标记实体相关命令起始位置
    ENTITY,                  //实体
    ATTACH_ENTITY,           //构建实体父子关系
    PART,                    //子部件设置
    CAMERA,                  //相机
    LIGHT,                   //灯光
    ENTITY_COLOR,            //实体颜色
    ROTOR,                   //旋翼转子
    ENTITY_STATUS,           //实体状态设置
    ENTITY_SCALE,            //实体缩放
    EXT_ENTITY,              //扩展实体
    SWITCH,                  //开关切换
    ENTITY_CLOUD,            //实体云
    ANIMATION,               //动画
    ANTI_PERSPECTIVE,        //反透视
    VELOCITY,                //速度
    ACCELERATION,            //加速度
    FRESNEL,                 //菲涅尔灯
    ARRESTINGROPE,           //阻拦索
    PIPENODE,                //管节点
    OILPIPE,                 //加油管连接

    //环境相关命令
    ENV_CMDS_START = 31,           //标记环境相关命令起始位置
    PRECIPITATE = ENV_CMDS_START,  //降水
    FOG,                           //雾
    WIND,                          //风
    CLOUD,                         //云
    TIME,                          //时间（时分）
    HAZE,                          //地平线薄雾
    SUNLIGHT,                      //太阳光芒
    MOON,                          //月亮
    STAR,                          //星星
    ATMOSPHERE,                    //大气
    OCEAN,                         //海洋
    SURFACE_CONDITION,             //表面状态
    DATE,                          //日期（年月日）
    LIGHTNING,                     //闪电
    CLOUD_VIS,                     //云层能见度
    CLOUD_OFFSET,                  //云层偏移（模拟地平线倾斜）
    STORM,                         //风暴云
    WEATHER,                       //气象控制 //环境相关命令ID满了，后面请勿追加

    //粒子相关
    TAILSPRAY = 50,  //尾喷

    //图元相关
    SYMBOL_CMDS_START = 60,    //标记图元相关命令起始位置
    SCREEN_COLOR,              //屏幕颜色
    POINT_STYLE,               //点样式
    LINE_STYLE,                //描边样式
    FILL_STYLE,                //填充样式
    TEXT_STYLE,                //文本样式
    CANVAS,                    //画布
    FEATURE_2D_CTRL,           //二维图元控制
    FEATURE_3D_CTRL,           //三维图元控制
    FEATURE_TEXT,              //文本
    FEATURE_2D,                //二维图元，预留
    FEATURE_CIRCLE,            //圆
    FEATURE_CIRCULAR_SECTOR,   //扇形
    FEATURE_3D,                //三维图元，预留
    FEATURE_SPHERE,            //球
    FEATURE_SPHERICAL_SECTOR,  //球扇
    REGION,                    //区域

    //场景相关命令
    SCENE_CMDS_START = 80,  //标记场景相关命令起始位置
    ADD_SCENE,              //添加场景
    LOAD_SCENE,             //加载场景
    DELETE_SCENE,           //卸载场景
    RESET_SCENE,            //重置场景

    //请求相关命令
    REQUEST_CMDS_START = 90,  //标记请求相关命令起始位置
    HOT_REQUEST,              //地形高度请求
    HAT_REQUEST,              //离地高度请求
    LOS_SEGMENT_REQUEST,      //视线线段查询
    LOS_VECTOR_REQUEST,       //视线向量查询
    SURFACE_TYPE_REQUEST,     //表面材质请求
    ENTITY_POS_REQUEST,       //实体位置查询
    HOT_EXT_REQUEST,          //地形高度请求扩展（相对实体）
    COLLISION_REQUEST,        //碰撞检测请求
    MOUSE_STATUS_REQUEST,     //鼠标状态请求
    ASTRO_STATUS_REQUEST,     //天体状态请求

    //////////////////////////////////////////////////////////////////////////
    //外部反馈命令类型 视景到主控 IG -> Host
    EXTERNAL_REPLY_START = 110,  //标记反馈相关命令的起始位置
    START_FRAME,                 //开始帧，每帧反馈
    HOT_REPLY,                   //地形高度反馈
    HAT_REPLY,                   //离地高度反馈
    SURFACE_TYPE_REPLY,          //表面类型反馈
    LOS_REPLY,                   //视线查询反馈
    ENTITY_POS_REPLY,            //实体位置反馈
    COLLISION_REPLY,             //碰撞检测反馈
    ARRESTINGHOOK_AUTOREPLY,     //阻拦索挂钩自动反馈
    MOUSE_STATUS_REPLY,          //鼠标状态反馈
    AGAD_STATUS_AUTOREPLY,       //飞机下滑状态辅助数据自动反馈 (Aircarft Glider Assist Data)
    ASTRO_STATUS_REPLY,          //天体状态反馈

    //通用机场相关
    AIRPORT = 130,  // 机场
    APRON,          // 停机坪
    HANGAR,         // 机库
    AIRTERMINAL,    // 候机楼
    RUNWAY,         // 跑道
    TAXIWAY,        // 滑行道
    RUNWAYLIGHT,    // 跑道灯光
    TOWER,          // 塔台

    CONSOLE_CMD = 254,  //控制台命令
    EXT_COMMAND = 255,  //扩展命令，用于携带自定义数据
};

using CmdType = uint8_t;

//命令头
struct CmdHeader {
    CmdHeader(CmdType type, uint16_t size) {
        cmd_type = type;
        cmd_size = size;
    }
    CmdType cmd_type;   //命令类型CmdType枚举
    uint16_t cmd_size;  //整个包的大小
};

//协议数据基类
struct BaseCmd {
    BaseCmd(CmdType type, uint16_t size)
        : header(type, size) {
    }
    CmdHeader header;
};

//颜色
struct Color4c {
    Color4c()
        : r(0), g(0), b(0), a(0) {
    }
    Color4c(uint8_t r_, uint8_t g_, uint8_t b_, uint8_t a_)
        : r(r_), g(g_), b(b_), a(a_) {
    }
    bool IsValid() const {
        return r != 0 || g != 0 || b != 0 || a != 0;
    }
    uint8_t r, g, b, a;
};

static bool IsEntityCmd(CmdType cmd_type) {
    return cmd_type > ENTITY_CMDS_START && cmd_type < ENV_CMDS_START;
}

static bool IsEnvironmentCmd(CmdType cmd_type) {
    return cmd_type > ENV_CMDS_START && cmd_type < SYMBOL_CMDS_START;
}

}  // namespace VSVLink
#pragma pack(pop)