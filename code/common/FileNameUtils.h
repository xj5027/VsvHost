#pragma once
#include <string>
#include <vector>

// 得到文件路径，c:/a/b/c.exe->c:/a/b
std::string GetFilePath(const std::string &fileName);

// 去除文件后缀，c:/a/b/c.exe->c:/a/b/c;
std::string GetNameLessExtension(const std::string &strName);