
#pragma once
#include <map>
#include <string>
#include "tinyxml2.h"
#include <thread>

#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>
#include <cstring>
#include <cerrno>
class NetWorkManager {
struct NetConf
{
  std::string recv_ip;
  int recv_port;
  std::string send_ip;
  int send_port;
  bool enabled = true;
};
enum class ENetWorkType : uint8_t {
    ECT_UAV,                                      // 接收地面站无人机信息
    ECT_ENVIROMENT,                               // 接收地面站环境设置信息;
    ECT_ViCenter,                                 // 主控软件;
    ECT_WEATHERRADAR,                             // 气象雷达;
    ECT_VIDEOCONTRL                               // 视屏控制;
};

public:
   NetWorkManager();
  ~NetWorkManager();

public:
  bool ReadConf(std::string joson_file);

  bool InitNetWork();
private:
  void InitThread();
  void UnintThread();
  
  void InitKeyEventMonitor(std::string device_path);
  void UnintKeyEventMonitor();
  void HandleKeyEvent(const input_event& ev);
  std::string GetKeyName(int code);
private:
  void loop();

private:
  std::map<ENetWorkType,NetConf> map_net_conf_;
  std::thread* thread_ = nullptr;
  bool thread_flg_ = false;
  bool loop_flg_  = false;

  int fd_ = -1;

};
