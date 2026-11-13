#pragma once
//////////////////////////////////////////////////////////////////////////
// \copyright 2025 VSV.Inc
// \brief 视景外部通信数据协议 VIGI(VSV Image Generator Interface)
// \author gaimeng
//////////////////////////////////////////////////////////////////////////
#include <stdint.h>

namespace VSVLink {
	//命令类型枚举
	enum CmdTypeEx {
		INVALID_EX = 0, //无效命令

		//////////////////////////////////////////////////////////////////////////
		//外部命令类型 主控到视景 Host->转发协议
		//////////////////////////////////////////////////////////////////////////
		TimeElement_CmdTypeEx = 1,							// 时间日期
		NatureElement_CmdTypeEx,							// 自然环境
		UAVehicleElement_CmdTypeEx,							// 无人机数据控制（位姿属性）
		CustomAirport_CmdTypeEx,							// 通用机场创建方式
		Y20VehicleElement_CmdTypeEx,						// Y20数据控制（位姿属性）
		SixUav_CmdTypeEx	,								// 六个无人机位姿信息		
		Weather_Radar_CmdTypeEx,							// 气象雷达;		
		Weather_Storm_CmdTypeEx,							// 风暴云;	
		Max_CmdTypeEx
	};
}