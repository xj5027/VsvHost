
#pragma once
#include "NetWorkBase.h"
#include "CommandList.h"
#include "VSVCmdBuffer.h"
#include "VSV_ICD/VSVProtocolCmdType.h"
#include "VSV_ICD/VSVProtocolExternal.h"
class NetWorkEnviroment:public NetWorkBase {
public:

  ~NetWorkEnviroment();
  static NetWorkEnviroment& GetInstance() {static NetWorkEnviroment instance; return instance;}
public:
  virtual void Handler(unsigned char* buffer, std::size_t size);
private:
  const int GetCloudeRealType(int clound_type);
  NetWorkEnviroment()  = default;
private:
  VSVLink::CommandList command_list_;
};
