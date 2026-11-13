/*!
 * \file Utils.h
 * \date 2023/12/04 13:03
 *
 * \author Mr.Lu
 *
 * TODO: 通用接口;
 *
 * \note
 */
#pragma once
#include <chrono>

//自Epoch时间的毫秒数
static uint64_t MilliSecSinceEpoch() {
  return std::chrono::duration_cast<std::chrono::milliseconds>(
             std::chrono::steady_clock::now().time_since_epoch())
      .count();
}