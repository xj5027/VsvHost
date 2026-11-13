
#pragma once
#include "UDPSocket.h"
#include "MessageCallBack.h"
#include <vector>
class NetWorkBase : public MessageCallBack{
public:
   NetWorkBase()  = default;
  ~NetWorkBase();

public:
   bool InitNetWork(std::string recv_ip,int recv_port,std::string send_ip,int send_port);
   bool SendData(unsigned char* buffer, std::size_t size);
   UDPSocket& socket(){return socket_;}
   virtual void SetCallBack();
   virtual void Handler(unsigned char* buffer, std::size_t size);

protected:
   bool ShouldSkipDuplicatePacket(const unsigned char* buffer, std::size_t size);
   
private:
  UDPSocket socket_;
  std::vector<unsigned char> last_packet_;
};
