#include "Timer.h"
#include "Log.h"
#include <chrono>

ViTimer::ViTimer() {
  tick_time_scale_ =
      static_cast<double>(std::chrono::steady_clock::period::num) /
      static_cast<double>(std::chrono::steady_clock::period::den);
  start_tick_time_ =
      static_cast<double>(
          std::chrono::steady_clock::now().time_since_epoch().count()) *
      tick_time_scale_;
}

ViTimer::~ViTimer() {}

double ViTimer::GetNowTickTime() const {
  return static_cast<double>(
             std::chrono::steady_clock::now().time_since_epoch().count()) *
             tick_time_scale_ -
         start_tick_time_;
}

void ViTimer::SetNowTickTime(double tick_time) {
  double now_time =
      static_cast<double>(
          std::chrono::steady_clock::now().time_since_epoch().count()) *
      tick_time_scale_;
  start_tick_time_ = now_time - tick_time;
}

tm ViTimer::GetSystemTime() const {
  double seconds = 0.0;
  return GetSystemTime(seconds);
}

tm ViTimer::GetSystemTime(double &seconds) const {
  auto t = std::chrono::system_clock::now();
  seconds = double(std::chrono::duration_cast<std::chrono::microseconds>(
                       t.time_since_epoch())
                       .count()) /
            1e+6;
  //将获取的时间转换成time_t类型
  auto tNow = std::chrono::system_clock::to_time_t(t);
  tm tm_now;
#ifdef _WIN32
  localtime_s(&tm_now, &tNow);
#else
  localtime_r(&tNow, &tm_now);
#endif
  tm_now.tm_year += 1900;
  tm_now.tm_mon += 1;
  return tm_now;
}

double ViTimer::GetSystemTimeToSeconds() const {
  auto t = std::chrono::system_clock::now();
  return double(std::chrono::duration_cast<std::chrono::microseconds>(
                    t.time_since_epoch())
                    .count()) /
         1e+6;
}

tm ViTimer::GetSystemTimeFromSeconds(double seconds) const {
  auto as_duration =
      std::chrono::duration_cast<std::chrono::system_clock::duration>(
          std::chrono::duration<double>(seconds));
  auto as_time_point = std::chrono::system_clock::time_point(as_duration);
  auto tNow = std::chrono::system_clock::to_time_t(as_time_point);
  tm tm_now;
#ifdef _WIN32
  localtime_s(&tm_now, &tNow);
#else
  localtime_r(&tNow, &tm_now);
#endif
  tm_now.tm_year += 1900;
  tm_now.tm_mon += 1;
  return tm_now;
}

std::string ViTimer::FormatTimeToDate(const tm &t) const {
  char buf[128];
  std::snprintf(buf, sizeof(buf), "%d-%d-%d %02d:%02d:%02d", t.tm_year,
                t.tm_mon, t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec);
  return buf;
}

std::string ViTimer::FormatTimeToDate(double seconds) const {
  char buf[128];
  tm tm_data = ViTimer::GetInst()->GetSystemTimeFromSeconds(seconds);
  std::snprintf(buf, sizeof(buf), "%d-%d-%d %02d:%02d:%02d.%06d",
                tm_data.tm_year, tm_data.tm_mon, tm_data.tm_mday,
                tm_data.tm_hour, tm_data.tm_min, tm_data.tm_sec,
                int((seconds - uint64_t(seconds)) * 1000000));
  return buf;
}

std::string ViTimer::FormatTimeToHours(double seconds, bool precise) const {
  int hours = int(int64_t(seconds) / 3600);
  seconds -= hours * 3600;
  int minutes = int(int64_t(seconds) / 60);
  seconds -= minutes * 60.0;
  char buf[128];
  if (precise) {
    std::snprintf(buf, sizeof(buf), "%02d:%02d:%02d.%06d", hours, minutes,
                  (int)seconds, int((seconds - uint64_t(seconds)) * 1000000));
  } else {
    std::snprintf(buf, sizeof(buf), "%02d:%02d:%02d", hours, minutes,
                  (int)seconds);
  }

  return buf;
}
