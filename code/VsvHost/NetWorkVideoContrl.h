#pragma once

#include "NetWorkBase.h"
#include "SJ2414Struct.h"
class NetWorkVideoContrl : public NetWorkBase
{
public:
  ~NetWorkVideoContrl();

  static NetWorkVideoContrl &GetInstance()
  {
    static NetWorkVideoContrl instance;
    return instance;
  }

  void Handler(unsigned char *buffer, std::size_t size) override;

private:
  NetWorkVideoContrl();


  // 回报数据;
  void send_data(void *data, std::size_t size);
  void back_front_contrl(const SJ2414Protocol::CI_026 &data);
  void back_back_switch(const SJ2414Protocol::CI_040 &data);
  void back_back_left(const SJ2414Protocol::SC_024 &data);
  void back_back_right(const SJ2414Protocol::SC_024 &data);

  // 处理数据;
  void deal_front_contrl(const SJ2414Protocol::CI_026 &data);
  void deal_back_switch(const SJ2414Protocol::CI_040 &data);
  void deal_back_left(const SJ2414Protocol::SC_024 &data);
  void deal_back_right(const SJ2414Protocol::SC_024 &data);

  SJ2414Protocol::CI_026 front_contrl_;
  SJ2414Protocol::CI_040 back_switch_;
  SJ2414Protocol::SC_024 back_left_;
  SJ2414Protocol::SC_024 back_right_;

  SJ2414Protocol::SC_023 front_contrl_back_;
  SJ2414Protocol::RMC_001 back_switch_back_;
  SJ2414Protocol::RMC_002 back_left_back_;
  SJ2414Protocol::RMC_003 back_right_back_;

  void UpdateRearCamera(int side, uint8_t control);

  // 开关意图独立保存；收起只停止输出，放出后可恢复原先开启的视口。
  bool rear_switch_requested_[2][2] = {};
  bool rear_camera_deployed_[2] = {};
  uint8_t front_sensor_type_ = 1;
  uint8_t left_rear_sensor_type_ = 1;
  uint8_t right_rear_sensor_type_ = 1;

};
