/*!
 * \file MessageCallBack.h
 * \date 2022/11/14 16:21
 *
 * \author Mr.Lu
 * Contact: user@company.com
 *
 * \brief 消息回调;
 *
 * \note
 */
#pragma once
#include <string>

class MessageCallBack {
public:
  virtual void Handler(unsigned char *buffer, std::size_t size) {}

protected:
private:
};