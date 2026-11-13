
#pragma once
#include "NetWorkBase.h"
#include "CommandList.h"
#include "VSVCmdBuffer.h"
#include "VSV_ICD/VSVProtocolCmdType.h"
#include "VSV_ICD/VSVProtocolExternal.h"
#include <mutex>
#include <thread>
class NetWorkWeatherRadar : public NetWorkBase
{
public:
  ~NetWorkWeatherRadar();
  static NetWorkWeatherRadar &GetInstance()
  {
    static NetWorkWeatherRadar instance;
    return instance;
  }
  void send_alt_data(float alt);
private:
  NetWorkWeatherRadar() = default;
  

public:
  virtual void Handler(unsigned char *buffer, std::size_t size);
};
