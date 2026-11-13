
#pragma once
#include "NetWorkBase.h"
#include "CommandList.h"
#include "VSVCmdBuffer.h"
#include "VSV_ICD/VSVProtocolCmdType.h"
#include "VSV_ICD/VSVProtocolExternal.h"
#include <vector>
const int k_airport_start_id = 30000;
class NetWorkUav:public NetWorkBase {
public:
   
  ~NetWorkUav();
  static NetWorkUav& GetInstance() {static NetWorkUav instance; return instance;}
private:
  NetWorkUav()  = default;
public:
  virtual void Handler(unsigned char* buffer, std::size_t size);
  const std::vector<int>& get_custom_airport_vec(){return custom_airport_vec_;}
private:
  std::vector<int> custom_airport_vec_; // 维护机场id，用于控制机场灯光;

};
