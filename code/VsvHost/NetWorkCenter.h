
#pragma once
#include "NetWorkBase.h"
#include "CommandList.h"
#include "VSVCmdBuffer.h"
#include "VSV_ICD/VSVProtocolCmdType.h"
#include "VSV_ICD/VSVProtocolExternal.h"
#include <mutex>
#include <thread>
class NetWorkCenter : public NetWorkBase
{
public:
  ~NetWorkCenter();
  static NetWorkCenter &GetInstance()
  {
    static NetWorkCenter instance;
    return instance;
  }
  void Init();
  void PushBackCommandBuffer(const VSVLink::ConstBuffer &buffer);
  template <typename Command>
  void PushBackCommand(const Command &command)
  {
    std::lock_guard<std::mutex> lock(send_mutex_);
    active_command_list_->Append(command);
  }
  void PushBackCommandList(VSVLink::CommandList &command_list);
  void SendConsoleCmd(const std::string &cmd);
  void SendConsoleCmd(const std::vector<std::string> &cmds);
  // view_type  1: 前视  2：左后 3：右后;
  void change_ir_or_tv(int view_type,int flag);
  // view_order：前视为0，后视0表示整侧、1/2表示单视口；flag：0关、1开。
  void change_switch(int view_type, int view_order, int flag);
  void request_uav_alt(int id,double lon,double lat);
  

  // 通用机场相关;
  void create_custom_airport(int airport_id,double lon,double lat,double alt,float heading);
  void change_custom_airport(int airport_id,double lon,double lat,double alt,float heading);
private:
  void Loop();
  void FlushPendingCommands();
  void SendBufferInternal(const VSVLink::ConstBuffer &buffer);
  NetWorkCenter() = default;
  

public:
  virtual void Handler(unsigned char *buffer, std::size_t size);
  VSVLink::CommandList command_list_;
  std::thread *thread_update_loop_ = nullptr;
  bool update_loop_flag_ = false;
  bool thread_flag_ = false;

  VSVLink::CommandList command_list_front_;
  VSVLink::CommandList command_list_back_;
  VSVLink::CommandList *active_command_list_ = &command_list_front_;
  std::mutex send_mutex_;

  std::vector<int> airport_vec_; // 维护机场id，用于控制机场灯光;
};
