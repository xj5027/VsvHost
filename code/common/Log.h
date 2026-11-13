//////////////////////////////////////////////////////
// 文件：Log.h;
// 功能：输出日志;
// 作者：dongll;
// 日期：20140523;
//////////////////////////////////////////////////////
#pragma once

namespace Log {
enum E_LOG_TYPE {
  LOG_MESSAGE, /// 消息;
  LOG_WARNING, /// 警告;
  LOG_ERROR,   /// 错误;
};

// 默认仅输出终端；显式调用此函数才启用文件日志。
bool LogStart(const char *pszLogPath);
bool LogEnd();

void Message(const char *format, ...);
void Warning(const char *format, ...);
void Error(const char *format, ...);
bool FlushToFile(bool log_end = false);

struct LogAutoEnd {
  ~LogAutoEnd();
};
}; // namespace Log
