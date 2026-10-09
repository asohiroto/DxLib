#include "core/Paths.h"
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <shlobj.h>
#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "ole32.lib")
#endif

namespace {
std::string g_dir = "save\\";

std::string narrow(const std::wstring& w) {
#ifdef _WIN32
    if (w.empty()) return {};
    int n = WideCharToMultiByte(CP_UTF8, 0, w.c_str(), (int)w.size(), nullptr, 0, nullptr, nullptr);
    std::string s(n, '\0');
    WideCharToMultiByte(CP_UTF8, 0, w.c_str(), (int)w.size(), s.data(), n, nullptr, nullptr);
    return s;
#else
    return std::string(w.begin(), w.end());
#endif
}
}  // namespace

namespace Paths {
std::wstring widen(const std::string& s) {
#ifdef _WIN32
    if (s.empty()) return {};
    int n = MultiByteToWideChar(CP_UTF8, 0, s.c_str(), (int)s.size(), nullptr, 0);
    std::wstring w(n, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, s.c_str(), (int)s.size(), w.data(), n);
    return w;
#else
    return std::wstring(s.begin(), s.end());
#endif
}

void makeDir(const std::string& path) {
#ifdef _WIN32
    CreateDirectoryW(widen(path).c_str(), nullptr);
#endif
}

bool exists(const std::string& path) {
#ifdef _WIN32
    return GetFileAttributesW(widen(path).c_str()) != INVALID_FILE_ATTRIBUTES;
#else
    FILE* f = std::fopen(path.c_str(), "rb");
    if (f) std::fclose(f);
    return f != nullptr;
#endif
}

FILE* open(const std::string& path, const char* mode) {
#ifdef _WIN32
    return _wfopen(widen(path).c_str(), widen(mode).c_str());
#else
    return std::fopen(path.c_str(), mode);
#endif
}

bool rename(const std::string& from, const std::string& to) {
#ifdef _WIN32
    return MoveFileExW(widen(from).c_str(), widen(to).c_str(), MOVEFILE_REPLACE_EXISTING) != 0;
#else
    return std::rename(from.c_str(), to.c_str()) == 0;
#endif
}

bool copy(const std::string& from, const std::string& to) {
#ifdef _WIN32
    return CopyFileW(widen(from).c_str(), widen(to).c_str(), FALSE) != 0;
#else
    return false;
#endif
}

bool remove(const std::string& path) {
#ifdef _WIN32
    return DeleteFileW(widen(path).c_str()) != 0;
#else
    return std::remove(path.c_str()) == 0;
#endif
}

void init(bool portable) {
#ifdef _WIN32
    if (!portable) {
        PWSTR app = nullptr;
        if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, nullptr, &app)) && app) {
            std::wstring base = std::wstring(app) + L"\\ArcanaForge";
            CoTaskMemFree(app);
            CreateDirectoryW(base.c_str(), nullptr);
            std::wstring dir = base + L"\\save";
            CreateDirectoryW(dir.c_str(), nullptr);
            if (GetFileAttributesW(dir.c_str()) != INVALID_FILE_ATTRIBUTES) g_dir = narrow(dir) + "\\";
        }
    }
    if (g_dir != "save\\") {
        // migrate saves written next to the exe by earlier builds (never overwrite newer data)
        for (const char* f : {"profile.txt", "run.txt"}) {
            std::string old = std::string("save\\") + f, now = g_dir + f;
            if (exists(old) && !exists(now)) CopyFileW(widen(old).c_str(), widen(now).c_str(), TRUE);
        }
    } else {
        CreateDirectoryW(L"save", nullptr);
    }
#endif
}

const std::string& saveDir() { return g_dir; }
std::string save(const char* name) { return g_dir + name; }
}  // namespace Paths
