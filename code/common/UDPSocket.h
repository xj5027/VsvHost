#pragma once

#include <atomic>
#include <string>
#include <thread>

#include "asio/ip/udp.hpp"

constexpr int kRecvCacheSize = 1024 * 1024;
constexpr int kNetCacheSize = 1024 * 1024 * 10;

class MessageCallBack;
class UDPSocket {
public:
  UDPSocket();
  ~UDPSocket();

  bool InitNetwork(const std::string &ip, std::uint16_t port);
  void JoinMulticastGroup(const std::string &multicast_ip);
  void ReleaseNetwork();

  void SetRemoteAddr(const std::string &ip, std::uint16_t port);
  void SendData(const unsigned char *buf, std::size_t len);
  void SendDataTo(const unsigned char *buf, std::size_t len,
                  const std::string &ip, std::uint16_t port);
  void Start();
  void SetCallBack(MessageCallBack *call_back) { call_back_ = call_back; }

protected:
  void recvHandler(const asio::error_code &errcode, std::size_t len);
  void setSyncFunc();
  void threadFunc();

private:
  bool inited_ = false;
  asio::io_service *io_ervice_ = nullptr;
  asio::ip::udp::socket *socket_ = nullptr;
  asio::ip::udp::endpoint recv_endpoint_;

  // 接收终端网络地址
  bool remote_setted_ = false;
  asio::ip::udp::endpoint remote_endpoint_;

  asio::ip::udp::endpoint sender_endpoint_;
  unsigned char recv_buf_[65536];
  std::size_t recv_size_;

  MessageCallBack *call_back_ = nullptr;
  std::thread *thread_ = nullptr;

  std::string currect_ip = "null";
};