#include "FileNameUtils.h"
static const char *const PATH_SEPARATORS = "/\\";

std::string GetFilePath(const std::string &fileName) {
  std::string::size_type slash = fileName.find_last_of(PATH_SEPARATORS);
  if (slash == std::string::npos)
    return std::string();
  else
    return std::string(fileName, 0, slash);
}

std::string GetNameLessExtension(const std::string &strName) {
  std::string::size_type dot = strName.find_last_of('.');
  std::string::size_type slash = strName.find_last_of(PATH_SEPARATORS);
  if (dot == std::string::npos || (slash != std::string::npos && dot < slash))
    return strName;
  return std::string(strName.begin(), strName.begin() + dot);
}