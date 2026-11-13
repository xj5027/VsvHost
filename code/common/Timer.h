#pragma once
#include "Singleton.h"
#include <ctime>
#include <string>

class ViTimer {
  VIWO_SINGLETON(ViTimer)
public:
  // 获取当前时间，从程序启动开始算起
  double GetNowTickTime() const;
  // 设置当前时间
  void SetNowTickTime(double tick_time);
  // 获取当前系统时间
  // seconds: 返回系统时间对应的秒数
  tm GetSystemTime() const;
  tm GetSystemTime(double &seconds) const;
  double GetSystemTimeToSeconds() const;
  // 根据上面函数的秒数获得对应的系统时间
  tm GetSystemTimeFromSeconds(double seconds) const;

  // 将时间格式化成YYYY-MM-DD HH:MM:SS形式
  std::string FormatTimeToDate(const tm &t) const;
  // 将时间格式化成YYYY-MM-DD HH:MM:SS.SSSSSS形式
  std::string FormatTimeToDate(double seconds) const;
  // 将时间格式化成HH:MM:SS.SSSSSS形式
  // precise: 是否保留秒数之后的小数
  std::string FormatTimeToHours(double seconds, bool precise = true) const;

private:
  double tick_time_scale_ = 1.0;
  double start_tick_time_ = 0;
};