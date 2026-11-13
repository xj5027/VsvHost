//////////////////////////////////////////////////////
// 文件：Log.cpp;
// 功能：输出日志;
// 作者：dongll;
// 日期：20140523;
//////////////////////////////////////////////////////
#include "Log.h"
#include <stack>
#include <string>
#include <sys/stat.h>
#ifdef _WIN32
#include <direct.h>
#include <io.h>
#endif
#include "FileNameUtils.h"
#include <cstring>
#include <filesystem>
#include <iostream>
#include <stdarg.h>
#include <stdio.h>
#include <time.h>

namespace Log {
#define LOG_BUF_LEN 4096 /// 记录日志缓存大小;

FILE *g_pFile;

int g_nBufIdx;
char g_szFileBuf[LOG_BUF_LEN];
char g_szFilePath[256];

bool g_bInited = false;

bool LogStart(const char *pszLogPath) {
  std::strcpy(g_szFilePath, pszLogPath);
  /// 创建log文件夹;
  std::string strDir = GetFilePath(pszLogPath);
  try {
    std::filesystem::create_directories(strDir);
  } catch (const std::filesystem::filesystem_error &e) {
    std::cerr << "Filesystem error: " << e.what() << std::endl;
    return false;
  }

  FILE *fp = fopen(pszLogPath, "w");
  if (fp == nullptr) {
    std::cout << "Open file failed: " << pszLogPath << std::endl;
    return false;
  }
  g_pFile = fp;

  time_t curTime;
  time(&curTime);
  struct tm timeInfo;
#ifdef _WIN32
  localtime_s(&timeInfo, &curTime);
#else
  localtime_r(&curTime, &timeInfo);
#endif
  char szFileHeader[256];
  std::sprintf(szFileHeader,
               "---------Log start(%d/%02d/%02d %02d:%02d:%02d)--------\n",
               timeInfo.tm_year + 1900, timeInfo.tm_mon + 1, timeInfo.tm_mday,
               timeInfo.tm_hour, timeInfo.tm_min, timeInfo.tm_sec);

  fwrite(szFileHeader, sizeof(char), strlen(szFileHeader), g_pFile);
  fclose(g_pFile);

  g_nBufIdx = 0;

  g_bInited = true;

  return true;
}

bool LogEnd() { return FlushToFile(true); }

void logMessage(E_LOG_TYPE eType, const char *format, va_list &argptr) {
  char szBuf0[1024];
  std::vsnprintf(szBuf0, sizeof(szBuf0), format, argptr);

  /// 追加时间;
  time_t curTime;
  time(&curTime);
  struct tm timeInfo;
#ifdef _WIN32
  localtime_s(&timeInfo, &curTime);
#else
  localtime_r(&curTime, &timeInfo);
#endif

  const char *pstrMsgType[3] = {"Message:", "Warning:", "Error  :"};

  char szBuf1[1200];
  std::snprintf(szBuf1, sizeof(szBuf1), "%s%s[%d/%02d/%02d %02d:%02d:%02d]\n",
               pstrMsgType[eType], szBuf0, timeInfo.tm_year + 1900,
               timeInfo.tm_mon + 1, timeInfo.tm_mday, timeInfo.tm_hour,
               timeInfo.tm_min, timeInfo.tm_sec);

  // 终端日志始终可用，并立即刷新，避免重定向或 IDE 捕获时延迟显示。
  std::printf("%s", szBuf1);
  std::fflush(stdout);

  // 只有显式启用文件日志后才缓存和写盘；默认不创建本地文件。
  if (!g_bInited)
    return;

  int nLen = (int)strlen(szBuf1);
  if (nLen + g_nBufIdx > LOG_BUF_LEN) {
    /// 超过限制，保存到文件;
    if (!FlushToFile())
      return;
  }
  std::memcpy(g_szFileBuf + g_nBufIdx, szBuf1, nLen);
  g_nBufIdx += nLen;
}

void Message(const char *format, ...) {
  va_list argptr;
  va_start(argptr, format);
  logMessage(LOG_MESSAGE, format, argptr);
  va_end(argptr);
}

void Warning(const char *format, ...) {
  va_list argptr;
  va_start(argptr, format);
  logMessage(LOG_WARNING, format, argptr);
  va_end(argptr);
}

void Error(const char *format, ...) {
  va_list argptr;
  va_start(argptr, format);
  logMessage(LOG_ERROR, format, argptr);
  va_end(argptr);
}

bool FlushToFile(bool log_end) {
  if (!g_bInited || g_nBufIdx == 0)
    return true;

  FILE *fp = fopen(g_szFilePath, "a+");
  if (fp == nullptr)
    return false;
  g_pFile = fp;

  fwrite(g_szFileBuf, sizeof(char), g_nBufIdx, g_pFile);
  if (log_end) {
    time_t curTime;
    time(&curTime);
    struct tm timeInfo;
#ifdef _WIN32
    localtime_s(&timeInfo, &curTime);
#else
    localtime_r(&curTime, &timeInfo);
#endif
    char szFileHeader[256];
    std::sprintf(szFileHeader,
                 "---------Log end(%d/%02d/%02d %02d:%02d:%02d)--------\n",
                 timeInfo.tm_year + 1900, timeInfo.tm_mon + 1, timeInfo.tm_mday,
                 timeInfo.tm_hour, timeInfo.tm_min, timeInfo.tm_sec);

    fwrite(szFileHeader, sizeof(char), strlen(szFileHeader), g_pFile);
  }
  fclose(g_pFile);
  g_nBufIdx = 0;
  return true;
}

LogAutoEnd::~LogAutoEnd() { LogEnd(); }
}; // namespace Log
