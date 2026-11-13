#include "Log.h"
#include "NetWorkMananger.h"
#include "NetWorkBase.h"
#include <cstring>

NetWorkBase::~NetWorkBase()
{
}

bool NetWorkBase::InitNetWork(std::string recv_ip, int recv_port, std::string send_ip, int send_port)
{
    bool ret = socket_.InitNetwork(recv_ip, recv_port);
    if (!ret)
    {
        Log::Error("InitNetwork failed!");
        return false;
    }

    socket_.SetRemoteAddr(send_ip,send_port);
    SetCallBack();
    return true;
}

bool NetWorkBase::SendData(unsigned char *buffer, std::size_t size)
{
     socket_.SendData(buffer,size);
     printf("send_data\n");
     return true;
}

void NetWorkBase::SetCallBack()
{
   socket_.SetCallBack(this);
   socket_.Start();
}

void NetWorkBase::Handler(unsigned char *buffer, std::size_t size)
{
    
}

bool NetWorkBase::ShouldSkipDuplicatePacket(const unsigned char *buffer, std::size_t size)
{
    if (buffer == nullptr || size == 0)
    {
        return false;
    }

    const bool same_as_last = last_packet_.size() == size &&
                              std::memcmp(last_packet_.data(), buffer, size) == 0;
    if (same_as_last)
    {
        return true;
    }

    last_packet_.assign(buffer, buffer + size);
    return false;
}
