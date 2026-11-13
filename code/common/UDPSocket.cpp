#include "UDPSocket.h"
#include "Log.h"
#include "MessageCallBack.h"
#include "asio/ip/multicast.hpp"
#include "macros.h"
#include <functional>

///////////////////UDPSocket///////////////////
UDPSocket::UDPSocket() {}

UDPSocket::~UDPSocket() { ReleaseNetwork(); }

bool UDPSocket::InitNetwork(const std::string &ip, std::uint16_t port) {
  if (inited_) {
    Log::Error("Net has initialized, InitNetwork failed!");
    return false;
  }

  try {
    io_ervice_ = new asio::io_service();
    socket_ = new asio::ip::udp::socket(*io_ervice_);

    socket_->open(asio::ip::udp::v4());
    socket_->bind(
        asio::ip::udp::endpoint(asio::ip::address::from_string(ip), port));
    asio::error_code errcode;
    socket_->set_option(asio::socket_base::reuse_address(true), errcode);
    socket_->set_option(asio::socket_base::receive_buffer_size(kRecvCacheSize),
                        errcode);
    if (errcode.value() != 0) {
      Log::Error(
          "UDP init big recv buffer (size: %d) failed, change to 4096KB.",
          kRecvCacheSize);
      socket_->set_option(asio::socket_base::receive_buffer_size(4096 * 1024),
                          errcode);
    }

    Log::Message("Init UDP success, ip: %s, port: %d", ip.c_str(), port);
  } catch (std::exception &e) {
    Log::Error("Init UDP failed, ip: %s, port: %d\nException: %s", ip.c_str(),
               port, e.what());
    SAFE_DELETE(socket_);
    SAFE_DELETE(io_ervice_);
    return false;
  }

  inited_ = true;
  return true;
}

void UDPSocket::JoinMulticastGroup(const std::string &multicast_ip) {
  if (multicast_ip.empty())
    return;
  const auto multicast_address = asio::ip::address::from_string(multicast_ip);
  bool is_multiCast = multicast_address.is_multicast();
  if (is_multiCast) {
    socket_->set_option(asio::ip::multicast::join_group(multicast_address));
  }
}

void UDPSocket::ReleaseNetwork() {
  if (!inited_ && !socket_ && !io_ervice_ && !thread_)
    return;

  inited_ = false;
  if (socket_) {
    asio::error_code errcode;
    socket_->cancel(errcode);
    socket_->close(errcode);
  }
  if (io_ervice_) {
    io_ervice_->stop();
  }
  if (thread_) {
    if (thread_->joinable()) {
      thread_->join();
    }
    SAFE_DELETE(thread_);
  }
  SAFE_DELETE(socket_);
  SAFE_DELETE(io_ervice_);
}

void UDPSocket::SetRemoteAddr(const std::string &ip, std::uint16_t port) {
  remote_endpoint_ =
      asio::ip::udp::endpoint(asio::ip::address_v4::from_string(ip), port);
  remote_setted_ = true;
}

void UDPSocket::SendData(const unsigned char *buf, std::size_t len) {
  if (!inited_) {
    Log::Error("Can't send data before initialize network!");
    return;
  }
  if (!remote_setted_) {
    Log::Error("Can't send data before set remote address!");
    return;
  }
  try {
    asio::error_code error_cd;
    int send_size = (int)socket_->send_to(asio::buffer(buf, len),
                                          remote_endpoint_, 0, error_cd);
    if (error_cd.value() != 0) {
      Log::Error("UDP send failed, destination: %s-%d, error: %s",
                 remote_endpoint_.address().to_string().c_str(),
                 remote_endpoint_.port(), error_cd.message().c_str());
      return;
    } else if (send_size != len) {
      Log::Error("UDP send failed, destination: %s-%d, package size: %d, send "
                 "size: %d",
                 remote_endpoint_.address().to_string().c_str(),
                 remote_endpoint_.port(), len, send_size);
      return;
    }
  } catch (std::exception &e) {
    Log::Error("UDP send failed, exception: %s", e.what());
    return;
  }
}

void UDPSocket::SendDataTo(const unsigned char *buf, std::size_t len,
                           const std::string &ip, std::uint16_t port) {
  currect_ip = ip;
  if (!inited_) {
    Log::Error("Can't send data before initialize network!");
    return;
  }
  try {

    asio::ip::udp::endpoint endpoint_(asio::ip::address_v4::from_string(ip),
                                      port);
    asio::error_code error_cd;
    int send_size =
        (int)socket_->send_to(asio::buffer(buf, len), endpoint_, 0, error_cd);
    if (error_cd.value() != 0) {
      Log::Error("UDP send failed, destination: %s-%d, error: %s", ip.c_str(),
                 port, error_cd.message().c_str());
      return;
    } else if (send_size != len) {
      Log::Error("UDP send failed, destination: %s-%d, package size: %d, send "
                 "size: %d",
                 ip.c_str(), port, len, send_size);
      return;
    }
  } catch (std::exception &e) {
    Log::Error("UDP send failed, exception: %s", e.what());
    return;
  }
}

void UDPSocket::Start() {
  if (thread_) {
    return;
  }

  setSyncFunc();
  thread_ = new std::thread(&UDPSocket::threadFunc, this);
}

void UDPSocket::recvHandler(const asio::error_code &errcode, std::size_t len) {
  if (errcode || !inited_) {
    return;
  }

  recv_size_ = len;
  if (recv_size_ > 0 && call_back_) {
    // 处理接收到的数据;
    call_back_->Handler(recv_buf_, recv_size_);
  }
  if (inited_) {
    setSyncFunc();
  }
}

void UDPSocket::setSyncFunc() {
  if (socket_ && socket_->is_open()) {
    socket_->async_receive_from(
        asio::buffer(recv_buf_, sizeof(recv_buf_)), sender_endpoint_,
        std::bind(&UDPSocket::recvHandler, this, std::placeholders::_1,
                  std::placeholders::_2));
  } else {
    Log::Error("setSyncFunc failed, exception: %s", currect_ip.c_str());
  }
}

void UDPSocket::threadFunc() {
  if (io_ervice_) {
    io_ervice_->run();
  } else {
    Log::Error("threadFunc failed, exception: %s", currect_ip.c_str());
  }
}
