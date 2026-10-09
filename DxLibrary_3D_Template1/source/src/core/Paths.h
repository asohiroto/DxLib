#pragma once
// User-writable locations (saves, logs, screenshots). Everything lives under %APPDATA%\ArcanaForge so the
// game also works when installed to a read-only folder. Paths are UTF-8 strings (user names may be Japanese);
// file access goes through the wide-char helpers below.
#include <string>
#include <cstdio>

namespace Paths {
// Creates the save folder (and migrates ./save from older builds). Call once at startup.
void init(bool portable = false);
const std::string& saveDir();               // UTF-8, ends with '\\'
std::string save(const char* name);         // saveDir() + name
std::wstring widen(const std::string& utf8);
FILE* open(const std::string& utf8Path, const char* mode);
bool rename(const std::string& from, const std::string& to);
bool remove(const std::string& path);
bool copy(const std::string& from, const std::string& to);
bool exists(const std::string& path);
void makeDir(const std::string& path);
}  // namespace Paths
