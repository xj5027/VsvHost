#include "NetWorkVideoContrl.h"

#include "Log.h"
#include "NetWorkCenter.h"

#include <cstring>

namespace
{
constexpr std::size_t kVideoPacketSize = 66;
constexpr uint16_t kFrontControl = 0x62B2;
constexpr uint16_t kRearSwitchControl = 0x62B9;
constexpr uint16_t kRearLeftControl = 0x62B4;
constexpr uint16_t kRearRightControl = 0x62BA;

bool IsValidSwitch(uint8_t value)
{
  return value == 1 || value == 2;
}
} // namespace

NetWorkVideoContrl::NetWorkVideoContrl()
{
  // 前视默认开启；后视组件默认收起，四个视口均不输出。
  front_contrl_back_.bit_video_control.Bit.flse_work_state = 0;
  front_contrl_back_.bit_video_control.Bit.main_work_state = 1;
  front_contrl_back_.bit_video_control.Bit.video_chioce = 0;
  front_contrl_back_.bit_video_control.Bit.net_state = 1;
  front_contrl_back_.bit_video_control.Bit.commu429_state = 1;
  back_switch_back_.bit_back_work_state.Bit.main_work_state = 1;

  back_left_back_.bit_back_video_control.Bit.lise_work_state = 0;
  back_left_back_.bit_back_video_control.Bit.camera_control = 0;
  back_left_back_.bit_back_video_control.Bit.main_work_state = 1;
  back_left_back_.bit_back_video_control.Bit.video_chioce = 0;
  back_left_back_.bit_back_video_control.Bit.commu818_state = 1;

  back_right_back_.bit_back_video_control.Bit.lise_work_state = 0;
  back_right_back_.bit_back_video_control.Bit.camera_control = 0;
  back_right_back_.bit_back_video_control.Bit.main_work_state = 1;
  back_right_back_.bit_back_video_control.Bit.video_chioce = 0;
  back_right_back_.bit_back_video_control.Bit.commu818_state = 1;
  // 开关回报独立初始化，不根据组件状态计算。
  auto &reply = back_switch_back_.bit_back_video_control.Bit;
  reply.left1_video_out = 0;
  reply.left2_video_out = 0;
  reply.right1_video_out = 0;
  reply.right2_video_out = 0;
}

NetWorkVideoContrl::~NetWorkVideoContrl() = default;

void NetWorkVideoContrl::send_data(void *data, std::size_t size)
{
  SendData(static_cast<unsigned char *>(data), size);
}

void NetWorkVideoContrl::Handler(unsigned char *buffer, std::size_t size)
{
  if (buffer == nullptr || size < sizeof(uint16_t))
  {
    return;
  }

  uint16_t msg_type = 0;
  std::memcpy(&msg_type, buffer, sizeof(msg_type));
  if (size != kVideoPacketSize)
  {
    Log::Error("Video control packet size invalid, type: 0x%04X, size: %d",
               msg_type, static_cast<int>(size));
    return;
  }

  switch (msg_type)
  {
  case kFrontControl:
    std::memcpy(&front_contrl_, buffer, sizeof(front_contrl_));
    deal_front_contrl(front_contrl_);
    back_front_contrl(front_contrl_);
    break;
  case kRearSwitchControl:
    std::memcpy(&back_switch_, buffer, sizeof(back_switch_));
    deal_back_switch(back_switch_);
    back_back_switch(back_switch_);
    break;
  case kRearLeftControl:
    std::memcpy(&back_left_, buffer, sizeof(back_left_));
    deal_back_left(back_left_);
    back_back_left(back_left_);
    break;
  case kRearRightControl:
    std::memcpy(&back_right_, buffer, sizeof(back_right_));
    deal_back_right(back_right_);
    back_back_right(back_right_);
    break;
  default:
    Log::Error("Unknown video control packet type: 0x%04X", msg_type);
    break;
  }
}

void NetWorkVideoContrl::deal_front_contrl(const SJ2414Protocol::CI_026 &data)
{
  if (data.Equip_act != 7)
  {
    return;
  }

  const uint8_t video_choice = data.bit_video_control.Bit.video_chioce;
  if (IsValidSwitch(video_choice))
  {
    // 有效指令每次都转发，允许丢包或视频端重启后通过重复指令恢复状态。
    front_sensor_type_ = video_choice;
    NetWorkCenter::GetInstance().change_ir_or_tv(1, video_choice);
  }
  const uint8_t video_switch = data.bit_video_control.Bit.videl_switch;
  if (IsValidSwitch(video_switch))
  {
    // 前视报文1关2开，与后视报文相反；cmd统一使用0关1开。
    NetWorkCenter::GetInstance().change_switch(1, 0, video_switch == 1);
  }
}

void NetWorkVideoContrl::back_front_contrl(const SJ2414Protocol::CI_026 &data)
{
  if (data.Equip_act == 7)
  {
    if (IsValidSwitch(data.bit_video_control.Bit.video_chioce))
    {
      front_contrl_back_.bit_video_control.Bit.video_chioce = front_sensor_type_;
    }
    const uint8_t demist = data.bit_video_control.Bit.visble_light_demlist;
    if (IsValidSwitch(demist))
    {
      front_contrl_back_.bit_video_control.Bit.visble_leight_demist = demist;
    }

    const uint8_t correction = data.bit_video_control.Bit.infrared_non_uinform_control;
    if(correction == 1 || correction == 2)
    {
        front_contrl_back_.bit_video_control.Bit.infrared_non_uniformstate = 2;
    }
    else
    {
        front_contrl_back_.bit_video_control.Bit.infrared_non_uniformstate = correction;
    }
    const uint8_t video_switch = data.bit_video_control.Bit.videl_switch;
    if (IsValidSwitch(video_switch))
    {
      if(video_switch == 1)  // 输出;
        front_contrl_back_.bit_video_control.Bit.flse_work_state = 2;
      else if(video_switch == 2)
        front_contrl_back_.bit_video_control.Bit.flse_work_state = 1;
    }
  }

  send_data(&front_contrl_back_, sizeof(front_contrl_back_));
}

void NetWorkVideoContrl::deal_back_switch(const SJ2414Protocol::CI_040 &data)
{
  if (data.Equip_act != 7)
  {
    return;
  }
  const auto &control = data.bit_back_switch_control.Bit;
  const uint8_t switches[2][2] = {
      {control.left_1_switch, control.left_2_switch},
      {control.right_1_switch, control.right_2_switch}};
  for (int side = 0; side < 2; ++side)
  {
    for (int viewport = 0; viewport < 2; ++viewport)
    {
      if (IsValidSwitch(switches[side][viewport]))
      {
        rear_switch_requested_[side][viewport] = switches[side][viewport] == 1;
        // 执行输出同时取决于开关请求和该侧组件状态。
        const bool enabled = rear_switch_requested_[side][viewport] && rear_camera_deployed_[side];
        NetWorkCenter::GetInstance().change_switch(side + 2, viewport + 1, enabled);
      }
    }
  }
}

void NetWorkVideoContrl::UpdateRearCamera(int side, uint8_t control)
{
  if (!IsValidSwitch(control))
  {
    return;
  }
  // 组件1收起、2放出；只有组件放出且视口已开启时才允许输出。
  rear_camera_deployed_[side] = control == 2;
  // 收起只停止输出，保留各视口请求；再次放出时重新判断。
  for (int viewport = 0; viewport < 2; ++viewport)
  {
    const bool enabled = rear_switch_requested_[side][viewport] && rear_camera_deployed_[side];
    NetWorkCenter::GetInstance().change_switch(side + 2, viewport + 1, enabled);
  }
}

void NetWorkVideoContrl::back_back_switch(const SJ2414Protocol::CI_040 &data)
{
  if (data.Equip_act == 7)
  {
    // 仅回显有效开关指令，无效值保留上次回报，与组件和执行结果无关。
    const auto &control = data.bit_back_switch_control.Bit;
    auto &reply = back_switch_back_.bit_back_video_control.Bit;
    if (IsValidSwitch(control.left_1_switch))
      reply.left1_video_out = control.left_1_switch;
    if (IsValidSwitch(control.left_2_switch))
      reply.left2_video_out = control.left_2_switch;
    if (IsValidSwitch(control.right_1_switch))
      reply.right1_video_out = control.right_1_switch;
    if (IsValidSwitch(control.right_2_switch))
      reply.right2_video_out = control.right_2_switch;

    const auto &input_work = data.bit_work_model.Bit;
    if (input_work.relo_type >= 1 && input_work.relo_type <= 5)
      back_switch_back_.bit_back_work_model.Bit.Relo_type = input_work.relo_type;
    if (input_work.wm >= 1 && input_work.wm <= 3)
      back_switch_back_.bit_back_work_model.Bit.vm = input_work.wm;

    const auto &input_mode = data.bit_dm_model.Bit;
    if (input_mode.dnm >= 1 && input_mode.dnm <= 3)
      back_switch_back_.bit_back_dm_model.Bit.dnm = input_mode.dnm;
    if (input_mode.djm == 1 || input_mode.djm == 2)
      back_switch_back_.bit_back_dm_model.Bit.djm = input_mode.djm;
  }

  send_data(&back_switch_back_, sizeof(back_switch_back_));
}


void NetWorkVideoContrl::deal_back_left(const SJ2414Protocol::SC_024 &data)
{
  if (data.Equip_act == 7)
  {
    const uint8_t choice = data.bit_back_video_control.Bit.video_chioce;
    if (IsValidSwitch(choice))
    {
      left_rear_sensor_type_ = choice;
      NetWorkCenter::GetInstance().change_ir_or_tv(2, choice);
    }
    UpdateRearCamera(0, data.bit_back_video_control.Bit.camera_control);
  }
}

void NetWorkVideoContrl::back_back_left(const SJ2414Protocol::SC_024 &data)
{
  if (data.Equip_act == 7)
  {
    const auto &control = data.bit_back_video_control.Bit;
    if (IsValidSwitch(control.video_chioce))
    {
      back_left_back_.bit_back_video_control.Bit.video_chioce = left_rear_sensor_type_;
    }
    if (IsValidSwitch(control.camera_control))
    {
      back_left_back_.bit_back_video_control.Bit.camera_control = control.camera_control;
      back_left_back_.bit_back_video_control.Bit.lise_work_state = control.camera_control;
    }
    if (control.infrared_non_uinform_control >= 1 &&
        control.infrared_non_uinform_control <= 3)
    {
      if(control.infrared_non_uinform_control == 1 || control.infrared_non_uinform_control == 2)
      {
          back_left_back_.bit_back_video_control.Bit.infrared_non_uinform_control =
          2;
      }
      else
      {
        back_left_back_.bit_back_video_control.Bit.infrared_non_uinform_control =
          3;
      }
      
    }
    const uint8_t warning = data.bit_back_gwc.Bit.gj;
    if (IsValidSwitch(warning))
    {
      back_left_back_.bit_back_gwc.Bit.gj = warning;
    }
  }

  send_data(&back_left_back_, sizeof(back_left_back_));
}

void NetWorkVideoContrl::deal_back_right(const SJ2414Protocol::SC_024 &data)
{
  if (data.Equip_act == 7)
  {
    const uint8_t choice = data.bit_back_video_control.Bit.video_chioce;
    if (IsValidSwitch(choice))
    {
      right_rear_sensor_type_ = choice;
      NetWorkCenter::GetInstance().change_ir_or_tv(3, choice);
    }
    UpdateRearCamera(1, data.bit_back_video_control.Bit.camera_control);
  }
}

void NetWorkVideoContrl::back_back_right(const SJ2414Protocol::SC_024 &data)
{
  if (data.Equip_act == 7)
  {
    const auto &control = data.bit_back_video_control.Bit;
    if (IsValidSwitch(control.video_chioce))
    {
      back_right_back_.bit_back_video_control.Bit.video_chioce = right_rear_sensor_type_;
    }
    if (IsValidSwitch(control.camera_control))
    {
      back_right_back_.bit_back_video_control.Bit.camera_control = control.camera_control;
      back_right_back_.bit_back_video_control.Bit.lise_work_state = control.camera_control;
    }
    if (control.infrared_non_uinform_control >= 1 &&
        control.infrared_non_uinform_control <= 3)
    {
      if(control.infrared_non_uinform_control == 1 || control.infrared_non_uinform_control == 2)
      {
          back_right_back_.bit_back_video_control.Bit.infrared_non_uinform_control =
          2;
      }
      else
      {
        back_right_back_.bit_back_video_control.Bit.infrared_non_uinform_control =
          3;
      }
      


    }
    const uint8_t warning = data.bit_back_gwc.Bit.gj;
    if (IsValidSwitch(warning))
    {
      back_right_back_.bit_back_gwc.Bit.gj = warning;
    }
  }

  send_data(&back_right_back_, sizeof(back_right_back_));
}
