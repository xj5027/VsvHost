#pragma once
#include <string.h>
#include <stdio.h>
#include "SJ2414ProtocolCmdTypeEx.h"
#include <VSV_ICD/VSVProtocolCmdType.h>
#include <VSV_ICD/VSVProtocolExternal.h>
using namespace VSVLink;

#pragma pack(push)
#pragma pack(1)

namespace SJ2414Protocol
{

	struct UAV_TimeElement_DataCmd : public BaseCmd
	{
		UAV_TimeElement_DataCmd()
			: BaseCmd(CmdTypeEx::TimeElement_CmdTypeEx, sizeof(UAV_TimeElement_DataCmd)), U1_Month(0), U1_Day(0), U1_Hour(0), U1_Minute(0), U1_TimeSpeed(0)
		{
		}
		unsigned char U1_Month;		// 时间-月，[1,12]
		unsigned char U1_Day;		// 时间-天，[1,31]
		unsigned char U1_Hour;		// 时间-时，[0,23]
		unsigned char U1_Minute;	// 时间-分，[0,59]
		unsigned char U1_TimeSpeed; // 时间-时间倍速，[0,100]
	};

	struct UAV_CustomAirport_DataCmd : public BaseCmd
	{
		UAV_CustomAirport_DataCmd()
			: BaseCmd(CmdTypeEx::CustomAirport_CmdTypeEx, sizeof(UAV_CustomAirport_DataCmd)), Runway_Center_Lon(0.0), Runway_Center_Lat(0), Runway_Alt(0), F4_RunwayCourse_deg(0), F4_RunwayWidth_m(0), F4_RunwayLength_m(0)
		{
		}
		char name[4]; // 三字码，四字码；
		double Runway_Center_Lon;
		double Runway_Center_Lat;
		float Runway_Alt;
		float F4_RunwayCourse_deg; // 机场跑道航向
		unsigned short F4_RunwayWidth_m;
		unsigned short F4_RunwayLength_m;
	};

	struct UAV_NatureElement_DataCmd : public BaseCmd
	{
		UAV_NatureElement_DataCmd()
			: BaseCmd(CmdTypeEx::NatureElement_CmdTypeEx, sizeof(UAV_NatureElement_DataCmd)), U4_Visibility_m(0), U4_CloudTopHeight_m(0), U4_CloudBaseHeight_m(0), U1_CloudLevel(0), U1_CloudType(0), U1_WeahterType(0), U1_WeahterInstensity(0), U1_SeaCondition(0), U1_Season(0), U1_StartAmount(0), U1_StartBright(0), U1_RunwayCondition(0), U1_RunwayLight(0)
		{
		}
		unsigned long U4_Visibility_m;		 // 能见度
		unsigned short U4_CloudTopHeight_m;	 // 云顶高
		unsigned short U4_CloudBaseHeight_m; // 云低高
		unsigned char U1_CloudLevel;		 // 云量
		unsigned char U1_CloudType;			 // 云类型
		unsigned char U1_WeahterType;		 // 天气类型
		unsigned char U1_WeahterInstensity;	 // 天气强度
		unsigned char U1_SeaCondition;		 // 海况
		unsigned char U1_Season;			 // 季节
		unsigned char U1_StartAmount;		 // 星星数量
		unsigned char U1_StartBright;		 // 星星亮度
		unsigned char U1_RunwayCondition;	 // 跑道状态
		unsigned char U1_RunwayLight;		 // 跑道灯光亮度

		// 20261104 新增
		float  wind_dir;					 // 风向;
		float  wind_spend;					 // 风速;
	};

	struct UAVehicle_ICD_DataCmd : public BaseCmd
	{
		UAVehicle_ICD_DataCmd()
			: BaseCmd(CmdTypeEx::UAVehicleElement_CmdTypeEx, sizeof(UAVehicle_ICD_DataCmd)), M_UAVehicle_Id(0), M_UAVehicle_Type(0), M_UAVehicle_Lon(0.0), M_UAVehicle_Lat(0.0), M_UAVehicle_Alt(0.0f), M_UAVehicle_H(0.0f), M_UAVehicle_P(0.0f), M_UAVehicle_R(0.0f), U1_RefuelingProbe(0), U1_Luneburg(0), U1_DragChuteStatus(0), U1_Configuration(0), U1_LandingLightStatus(0), U1_TaxingLightStatus(0), U1_NavLightStatus(0), U1_FormLightStatus(0), U1_AntiColLightStatus(0), U1_AirRefuelLightSttus(0), U1_ExLightMode(0)
		// , H05_distance_oneHose_F4(0)
		{
			memset(ArrF4_Surface_deg, 0.0f, 20.0f);
			memset(ArrU1_EngineStatus, 0, 4);
			memset(ArrU1_EngineOnFire, 0, 4);
			memset(ArrU2_StoreType, 0, 40);
			memset(ArrU1_StoreNumber, 0, 40);
			memset(ArrU1_LghStatus, 0, 4);
			memset(ArrU1_LGLightStatus, 0, 4);
			// memset(F4_TubePosX_m_BODY,0,12);
			// memset(F4_TubePosY_m_BODY,0,12);
			// memset(F4_TubePosZ_m_BODY,0,12);
			// memset(F4_TubePsi_rad,0,12);
			// memset(F4_TubeTheta_rad,0,12);
		}
		short M_UAVehicle_Id;	//  主无人机模型ID
		short M_UAVehicle_Type; //  主无人机模型类型
		double M_UAVehicle_Lon; //  deg主无人机经度：±180（无人机物理中心）
		double M_UAVehicle_Lat; //  deg主无人机纬度：±90（无人机物理中心）
		float M_UAVehicle_Alt;	//  主无人机高度：0 - 100(m) （无人机物理中心）
		float M_UAVehicle_H;	//  deg主无人机偏航角：0 - 360（无人机物理中心）
		float M_UAVehicle_P;	//  deg主无人机俯仰角：0 - 360（无人机物理中心）
		float M_UAVehicle_R;	//  deg主无人机滚转角：0 - 360（无人机物理中心）

		float ArrF4_Surface_deg[20];
		unsigned char ArrU1_EngineStatus[4];
		unsigned char ArrU1_EngineOnFire[4];
		unsigned char U1_RefuelingProbe;
		float F4_Probe_X; // 受油探头机体系X轴上位置，向前为正，单位m
		float F4_Probe_Z; // 受油探头机体系Z轴上位置，向下为正，单位m
		float F4_Probe_Y; // 受油探头机体系Y轴上位置，向右为正，单位m

		unsigned char U1_Luneburg;			  // 龙伯球状态，0-NA,1-未使用，2-使用
		unsigned char U1_DragChuteStatus;	  // 机外阻力伞状态，0-NA,1-无伞；2-有伞
		unsigned char U1_Configuration;		  // 挂载构型，0-NA；1-非隐身构型；2-隐身构型；输出构型供目标特性计算使用
		unsigned short ArrU2_StoreType[40];	  // 挂载类型，数据各机型适配挂点可挂载武器型号，0-未挂载
		unsigned char ArrU1_StoreNumber[40];  // 挂载数量，数据各机型适配挂点可挂载武器数量
		unsigned char ArrU1_LghStatus[4];	  // 起落架状态，0-NA;1-收到位；2-放到位；3收放过程
		unsigned char U1_LandingLightStatus;  // 着陆灯状态，0-关闭;1-打开
		unsigned char U1_TaxingLightStatus;	  // 滑行灯状态，0-关闭;1-打开
		unsigned char U1_NavLightStatus;	  // 航行灯状态，0-关闭;1-打开
		unsigned char U1_FormLightStatus;	  // 编队灯状态，0-关闭;1-7打开，7为最大亮度
		unsigned char U1_AntiColLightStatus;  // 防撞灯状态，0-关闭;1-7打开，对应不同闪烁模式
		unsigned char U1_AirRefuelLightSttus; // 编队灯状态，0-关闭;1-99打开，99为最大亮度
		unsigned char ArrU1_LGLightStatus[4]; // 起落架信号灯状态，0-关闭;1-打开
		unsigned char U1_ExLightMode;		  // 机外照明模式，0-关闭;1-隐蔽模式 2-正常模式

		// float rudder[12]					  // 舵面;
		/*
		// 新灯光
		uint8_t light_jwzm_mode = 0;     // 机外照明 0:关闭 1:隐蔽 2：正常; 控制航行灯和加油泛光灯;
		uint8_t  light_hxd = 0;			 // 航行灯 0:关闭 1：暗 2:亮;
		uint8_t light_jyfgd = 0;         // 加油泛光灯（0-100）；
		uint8_t light_fz_hhd = 0;		 // 防撞/会合灯 0:断, 1:白 2:红 3:会合 4:交替; 5:上部红;
		uint8_t light_fz_hhd_ybms = 0;	 // 隐蔽模式 0：不启用 1：启用 （控制防撞/会合灯）;
		uint8_t light_jygd = 0           // 加油管灯 0：不启用 1：启用;
		*/

		float F4_TubePosX_m_BODY[12];  // 软管第n节机体系x位置;
		float F4_TubePosY_m_BODY[12];  // 软管第n节机体系x位置;
		float F4_TubePosZ_m_BODY[12];  // 软管第n节机体系x位置;
		float F4_TubePsi_rad[12];	   // 软管第n节航向角;
		float F4_TubeTheta_rad[12];	   // 软管第n节俯仰角;
		float H05_distance_oneHose_F4; // 软管长度;
	};

	struct UAVehicle_ICD_RequestDataCmd : public BaseCmd
	{
		UAVehicle_ICD_RequestDataCmd()
			: BaseCmd(VSVLink::INVALID_EX, sizeof(UAVehicle_ICD_RequestDataCmd)), M_UAVehicle_Hot(0), M_UAVehicle_Hos(0), M_UAVehicle_Surface_Type(0)
		{
			memset(M_UAVehicle_Collision, 0, 10);
			memset(IG_State, 0, 10);
		}
		short M_UAVehicle_Hot;			 // 主无人机地形高度反馈：±100（数组1-无人机物理中心，数组2-无人机左前，数组3-无人机右前，数组4-无人机左后，数组5-无人机右后）
		short M_UAVehicle_Hos;			 // 主无人机海底地形高度反馈：±100（数组1-无人机物理中心，数组2-无人机左前，数组3-无人机右前，数组4-无人机左后，数组5-无人机右后）
		short M_UAVehicle_Surface_Type;	 // 主无人机地形材质反馈：0-4（1-水面，2-沙地，3-草地，4-水泥）
		short M_UAVehicle_Collision[10]; // 主无人机碰撞检测反馈：0-1(0-未碰撞，1-碰撞建筑，2-碰撞船只，碰撞树木)

		int IG_State[10]; // 图形生成子系统计算机工作状态：（0 - 正常，1 - 异常）
	};

	// 加油机Y20位置和加油管信息;
	struct Y20Vehicle_ICD_DataCmd : public BaseCmd
	{
		Y20Vehicle_ICD_DataCmd()
			: BaseCmd(CmdTypeEx::Y20VehicleElement_CmdTypeEx, sizeof(Y20Vehicle_ICD_DataCmd)), M_Vehicle_Id(0), M_Vehicle_Type(0), M_Vehicle_Lon(0.0), M_Vehicle_Lat(0.0), M_Vehicle_Alt(0.0f), M_Vehicle_H(0.0f), M_Vehicle_P(0.0f), M_Vehicle_R(0.0f), U1_RefuelingProbe(0)
		// , H05_distance_oneHose_F4(0)
		{
		}
		short M_Vehicle_Id = 0;		//  Y20模型ID
		short M_Vehicle_Type = 0;	//  Y20模型类型
		double M_Vehicle_Lon = 0.f; //  degY20经度：±180（Y20物理中心）
		double M_Vehicle_Lat = 0.f; //  degY20纬度：±90（Y20物理中心）
		float M_Vehicle_Alt = 0.f;	//  Y20高度：0 - 100(m) （Y20物理中心）
		float M_Vehicle_H = 0.f;	//  degY20偏航角：0 - 360（Y20物理中心）
		float M_Vehicle_P = 0.f;	//  degY20俯仰角：0 - 360（Y20物理中心）
		float M_Vehicle_R = 0.f;	//  degY20滚转角：0 - 360（Y20物理中心）

		float ArrF4_Surface_deg[20] = {0};
		unsigned char ArrU1_EngineStatus[4] = {0};
		unsigned char ArrU1_EngineOnFire[4] = {0};
		unsigned char U1_RefuelingProbe = 0;
		float F4_Probe_X = 0.f;					   // 受油探头机体系X轴上位置，向前为正，单位m
		float F4_Probe_Z = 0.f;					   // 受油探头机体系Z轴上位置，向下为正，单位m
		float F4_Probe_Y = 0.f;					   // 受油探头机体系Y轴上位置，向右为正，单位m
		float F4_LeftTubePosX_m_BODY[12] = {0.f};  // 左侧软管第n节机体系x位置;
		float F4_LeftTTubePosY_m_BODY[12] = {0.f}; // 左侧软管第n节机体系x位置;
		float F4_LeftTTubePosZ_m_BODY[12] = {0.f}; // 左侧软管第n节机体系x位置;

		float F4_CenterTubePosX_m_BODY[12] = {0.f}; // 中间软管第n节机体系x位置;
		float F4_CenterTubePosY_m_BODY[12] = {0.f}; // 中间软管第n节机体系x位置;
		float F4_CenterTubePosZ_m_BODY[12] = {0.f}; // 中间软管第n节机体系x位置;

		float F4_RightTubePosX_m_BODY[12] = {0.f}; // 右侧软管第n节机体系x位置;
		float F4_RightTubePosY_m_BODY[12] = {0.f}; // 右侧软管第n节机体系x位置;
		float F4_RightTubePosZ_m_BODY[12] = {0.f}; // 右侧软管第n节机体系x位置;

		float F4_TubePsi_rad[12] = {0.f};	   // 软管第n节航向角;
		float F4_TubeTheta_rad[12] = {0.f};	   // 软管第n节俯仰角;
		float H05_distance_oneHose_F4 = {0.f}; // 软管长度;
	};
	struct pos_ypr
	{
		short M_Vehicle_Id = 0;	  //  模型ID
		short M_Vehicle_Type = 0; //  模型类型
		double M_Vehicle_Lon = 0; //  deg经度：±180（物理中心）
		double M_Vehicle_Lat = 0; //  deg纬度：±90（物理中心）
		float M_Vehicle_Alt = 0;  //  高度：0 - 100(m) （物理中心）
		float M_Vehicle_H = 0;	  //  deg偏航角：0 - 360（物理中心）
		float M_Vehicle_P = 0;	  //  deg俯仰角：0 - 360（物理中心）
		float M_Vehicle_R = 0;	  //  deg滚转角：0 - 360（物理中心）
	};
	// 六个受油无人机;
	struct six_uav_cmd : public BaseCmd
	{
		six_uav_cmd()
			: BaseCmd(CmdTypeEx::SixUav_CmdTypeEx, sizeof(six_uav_cmd)) {}
		pos_ypr six_uav[5];
	};

	// 气象雷达;
	struct weather_radar_cmd
	{
		uint8_t fc631[72];
		uint8_t load_action = 0;		// 载荷动作: 0-保留 2-启动自检 7-工作及指令 8-软件版本查询;
		uint8_t work_mode : 3;			// 工作模式: 0-无效,1-待机;2-气象+湍流，3-风切变;
		uint8_t wx_radio_control : 3;	// 辐射模式: 0,1:静默，2:-衰减（3db） 3:全功率;
		uint8_t pitch_Control_type : 2; // 俯仰控制 0:无效 1:自动 2:手动;
		uint8_t pitch_vaild : 1;		// 俯仰角有效性 0-无效 1-有效;
		uint8_t gain_vaild : 1;			// 增益有效性 0-无效 1:有效;
		uint8_t power_sw : 2;			// 电源整体 0:无效 1-上电  2-下电;
		uint8_t record_sw : 2;			// 记录开关 0-无效 1-开 2-关;
		uint8_t ground_test_sw : 2;		// 地面测试开关: 0-无效 1:开  2:关;
		uint8_t range_set;				// 量程设置 0-无效，1-10海里 2-20 3-30 4-80 5-160 6-240;
		short pitch_set;				// 俯仰角设置 使用时*0.5;
		short gain_set;					// 增益设置: 0 -30 ;
	};

	// 风暴云;
	struct storm_cmd : public BaseCmd
	{
		storm_cmd()
			: BaseCmd(CmdTypeEx::Weather_Storm_CmdTypeEx, sizeof(storm_cmd)) {}
		uint8_t enable = 0; // 开关 0:待机 1:工作;
		uint8_t id = 0;
		uint8_t type = 0; // [0-9];
		double lon = 0.f;
		double lat = 0.f;
		double alt = 0.f;
		float heading = 0;
	};

	// 视频控制
	// 前视视频控制  大小 66字节  0x62B2;
	struct CI_026
	{
		uint16_t msg_type = 0x62B2;
		uint8_t Equip_act = 0;	// 载荷动作 为7时下面的数有效;
		union
		{
			uint8_t All = 0;
			struct
			{
				uint8_t video_chioce : 2;				  // 视屏选择 0:无效,1:电视,2:红外;
				uint8_t visble_light_demlist : 2;		  // 可见光透雾 0:无效,1:开启,2:关闭;
				uint8_t infrared_non_uinform_control : 2; // 红外校正 0:无效,1:自动,2:执行校正,3:关闭校正;
				uint8_t videl_switch : 2;				  // 视屏开关 0:无效,1:开启，2:关闭;
			} Bit;
		} bit_video_control;
		uint8_t tail[62] = {0}; // 占位;
	};

	// 后视;
	// 后视视屏选择  大小 66字节 0x62B9 ;
	struct CI_040
	{
		uint16_t msg_type = 0x62B9;
		uint8_t Equip_act = 0; // 载荷动作 为7时下面的数有效;
		union
		{
			uint8_t All = 0;
			struct
			{
				uint8_t left_1_switch : 2;	// 左1开关 0:无效,1:开,2:关;
				uint8_t left_2_switch : 2;	// 左2开关 0:无效,1:开,2:关;
				uint8_t right_1_switch : 2; // 右1开关 0:无效,1:开,2:关;
				uint8_t right_2_switch : 2; // 右2开关 0:无效,1:开,2:关;
			} Bit;
		} bit_back_switch_control;
		union
		{
			uint8_t All = 0;
			struct
			{
				uint8_t relo_type : 3; // 相对位置结算方式 0:无效,1:自主,2:动动差分,3:I链定位,4:视觉定位,5:关闭;
				uint8_t wm : 3;		   // 工作模式 0:无效,1:任务自动,航路自动,2:任务指令 航路自动，3:任务指令、航路指令;
				uint8_t patc : 2;	   // 加油指令 0:无效,1:同步加油计划参数;
			} Bit;
		} bit_work_model;
		union
		{
			uint8_t All = 0;
			struct
			{
				uint8_t dnm : 2; // 日夜模式 0:无效,1:日,2:夜，3:夜视;
				uint8_t djm : 2; // 对接模式 0:无效,1:湿,2:干对接;
				uint8_t bk : 4;	 // 占位;
			} Bit;
		} bit_dm_model;
		uint8_t tail[60] = {0}; // 占位;
	};
	// 后视视屏控制  大小 66字节  0x62B4--左  0x62BA:右;
	struct SC_024 
	{
		uint16_t msg_type = 0;
		uint8_t Equip_act = 0; // 载荷动作 为7时下面的数有效;
		union
		{
			uint8_t All = 0;
			struct
			{
				uint8_t video_chioce : 2;				  // 视屏选择 0:无效,1:电视,2:红外;
				uint8_t camera_control : 2;				  // 摄像监视组件控制 0:无效,1:收起、不输出,2:放出，输出;
				uint8_t infrared_non_uinform_control : 2; // 红外校正 0:无效,1:自动,2:执行校正,3:关闭校正;
				uint8_t bk : 2;
			} Bit;
		} bit_back_video_control;
		union
		{
			uint8_t All = 0;
			struct
			{
				uint8_t gj : 2; // 导弹告警开启、关闭 0:无效,1:开,2:关;
				uint8_t bk : 6;
			} Bit;
		} bit_back_gwc;
		uint8_t tail[61] = {0}; // 占位
	};

	// 视屏控制回报数据;
	// 前视视屏控制回报 66字节;
	struct SC_023 
	{
		uint16_t msg_type = 0x64B2;;
		union
		{
			uint32_t All = 0;
			struct
			{
				uint32_t flse_work_state : 2;			// FISE工作模式 0:无效,1:待机（关闭）,2:正常（打开）;
				uint32_t main_work_state : 2;			// 主工作模式 0:无效,1:正常,2:维护;
				uint32_t video_chioce : 2;				// 视频选择 0:无效,1:电视,2:红外;
				uint32_t visble_leight_demist : 2;		// 可见光透雾 0:无效 1:开启 2:关闭;
				uint32_t infrared_non_uniformstate : 2; // 红外校正状态 0：无效，1:自动校正,2:校正结束;
				uint32_t net_state : 1;					// 以太网通信状态 0:异常，1:正常;
				uint32_t commu429_state : 1;			   // 429通信状态 0:异常，1:正常;
				uint32_t bk : 4;
				uint32_t infared_magery_disable : 1;	   // 前视红外成像失效 0:无故障，1:有故障;
				uint32_t PFL_Level : 3;					   // PFL等级 0:无效 1=1级，2... 5=5级;
				uint32_t visble_light_imagery_disable : 1; // 可见光成像失效  0:无故障，1:有故障;
				uint32_t PFL_Level_2 : 3;				   // PFL等级2 0:无效 1=1级，2... 5=5级；
				uint32_t tail : 8;						   // 占位;

			} Bit;
		} bit_video_control;
		uint8_t tail[60] = {0}; // 占位;
	};
	// 后视开关控制回报  ; 
	struct RMC_001
	{
		uint16_t msg_type = 0x64B9;
		union
		{
			uint8_t All = 0;
			struct
			{
				uint8_t main_work_state : 2; // 工作主方式 0:无效,1:正常,2:维护;
				uint8_t ctp_vaild : 1;		 // 选定任务参数有效性 0:无效,1:有效;
				uint8_t bk : 5;
			} Bit;
		} bit_back_work_state;
		union
		{
			uint8_t All = 0;
			struct
			{
				uint8_t left1_video_out : 2;  // 左1开关 0:无效,1:开,2:关;
				uint8_t left2_video_out : 2;  // 左2开关 0:无效,1:开,2:关;
				uint8_t right1_video_out : 2; // 右1开关 0:无效,1:开,2:关;
				uint8_t right2_video_out : 2; // 右2开关 0:无效,1:开,2:关;
			} Bit;
		} bit_back_video_control;

		union
		{
			uint8_t All = 0;
			struct
			{
				uint8_t Relo_type : 3; // 相对位置结算方式;
				uint8_t vm : 3;		   // 工作模式;
				uint8_t bk : 2;
			} Bit;
		} bit_back_work_model;
		uint8_t rw = 0; // 任务;
		union
		{
			uint8_t All = 0;
			struct
			{
				uint8_t jyrin : 4; // 加油任务标识号;
				uint8_t dnm : 2;   // 日夜模式 0: 无效 1:日 2: 夜 3:夜视;
				uint8_t djm : 2;   // 对接 0:无效 1:湿对接 2:干对接;
			} Bit;
		} bit_back_dm_model;
		uint8_t fight_line_id = 0; // 航线id;
		uint8_t tail[58] = {0};	// 占位；
	};

	// 左后视视屏控制回报 66字节;
	struct RMC_002
	{
		uint16_t msg_type = 0x64B4;
		union
		{
			uint32_t All = 0;
			struct
			{
				uint32_t lise_work_state : 2;
				uint32_t main_work_state : 2;
				uint32_t video_chioce : 2;					 // 视屏选择 0:无效,1:电视,2:红外;
				uint32_t camera_control : 2;				 // 摄像监视组件控制 0:无效,1:收起、不输出,2:放出，输出;
				uint32_t infrared_non_uinform_control : 2;	 // 红外校正 0:无效,1:自动,2:执行校正,3:关闭校正;
				uint32_t bk : 1;							 // 占位
				uint32_t commu818_state : 1;				 // 818 通信  0:异常，1:正常
				uint32_t infared_magery_disable : 1;		 // 红外1成像失效 0:无故障，1:有故障;
				uint32_t PFL_Level : 3;						 // PFL等级 0:无效 1=1级，2... 5=5级;
				uint32_t infared_magery_2_disable : 1;		 // 红外2成像失效  0:无故障，1:有故障;
				uint32_t PFL_Level_2 : 3;					 // PFL等级2 0:无效 1=1级，2... 5=5级；
				uint32_t visble_light_imagery_1_disable : 1; // 可见光1成像失效  0:无故障，1:有故障;
				uint32_t PFL_Level_3 : 3;					 // PFL等级2 0:无效 1=1级，2... 5=5级；
				uint32_t visble_light_imagery_2_disable : 1; // 可见光2成像失效  0:无故障，1:有故障;
				uint32_t PFL_Level_4 : 3;					 // PFL等级2 0:无效 1=1级，2... 5=5级；
				uint32_t lifting_mechan_disable : 1;		 // 升降机构失效  0:无故障，1:有故障;
				uint32_t PFL_Level_5 : 3;					 // PFL等级2 0:无效 1=1级，2... 5=5级；
			} Bit;
		} bit_back_video_control;
		union
		{
			uint8_t All = 0;
			struct
			{
				uint8_t gj : 2; // 导弹告警开启、关闭 0:无效,1:开,2:关;
				uint8_t bk : 6;
			} Bit;
		} bit_back_gwc;
		uint8_t tail[59] = {0}; // 占位
	};
	// 右后视回报数据；
	struct RMC_003
	{
		uint16_t msg_type = 0x64BA;
		union
		{
			uint32_t All = 0;
			struct
			{
				uint32_t lise_work_state : 2;
				uint32_t main_work_state : 2;
				uint32_t video_chioce : 2;					 // 视屏选择 0:无效,1:电视,2:红外;
				uint32_t camera_control : 2;				 // 摄像监视组件控制 0:无效,1:收起、不输出,2:放出，输出;
				uint32_t infrared_non_uinform_control : 2;	 // 红外校正 0:无效,1:自动,2:执行校正,3:关闭校正;
				uint32_t bk : 1;							 // 占位
				uint32_t commu818_state : 1;				 // 818 通信  0:异常，1:正常
				uint32_t infared_magery_disable : 1;		 // 红外1成像失效 0:无故障，1:有故障;
				uint32_t PFL_Level : 3;						 // PFL等级 0:无效 1=1级，2... 5=5级;
				uint32_t infared_magery_2_disable : 1;		 // 红外2成像失效  0:无故障，1:有故障;
				uint32_t PFL_Level_2 : 3;					 // PFL等级2 0:无效 1=1级，2... 5=5级；
				uint32_t visble_light_imagery_1_disable : 1; // 可见光1成像失效  0:无故障，1:有故障;
				uint32_t PFL_Level_3 : 3;					 // PFL等级2 0:无效 1=1级，2... 5=5级；
				uint32_t visble_light_imagery_2_disable : 1; // 可见光2成像失效  0:无故障，1:有故障;
				uint32_t PFL_Level_4 : 3;					 // PFL等级2 0:无效 1=1级，2... 5=5级；
				uint32_t lifting_mechan_disable : 1;		 // 升降机构失效  0:无故障，1:有故障;
				uint32_t PFL_Level_5 : 3;					 // PFL等级2 0:无效 1=1级，2... 5=5级；
			} Bit;
		} bit_back_video_control;
		union
		{
			uint8_t All = 0;
			struct
			{
				uint8_t gj : 2; // 导弹告警开启、关闭 0:无效,1:开,2:关;
				uint8_t bk : 6;
			} Bit;
		} bit_back_gwc;
		uint8_t tail[59] = {0}; // 占位
	};

	static_assert(sizeof(CI_026) == 66, "CI_026 must be 66 bytes");
	static_assert(sizeof(CI_040) == 66, "CI_040 must be 66 bytes");
	static_assert(sizeof(SC_024) == 66, "SC_024 must be 66 bytes");
	static_assert(sizeof(SC_023) == 66, "SC_023 must be 66 bytes");
	static_assert(sizeof(RMC_001) == 66, "RMC_001 must be 66 bytes");
	static_assert(sizeof(RMC_002) == 66, "RMC_002 must be 66 bytes");
	static_assert(sizeof(RMC_003) == 66, "RMC_003 must be 66 bytes");

} // namespace ViTrans_Protocol
#pragma pack(pop)
