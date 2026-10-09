#pragma once
#include <cstdio>
#include <cstdarg>
#include <ctime>
#include "core/Paths.h"

// Append-only diagnostic log (%APPDATA%\ArcanaForge\save\log.txt). Kept tiny on purpose: players can attach it to bug reports.
namespace Log {
inline void write(const char* fmt, ...) {
    static bool first = true;
    FILE* f = Paths::open(Paths::save("log.txt"), first ? "w" : "a");
    if (!f) return;
    if (first) {
        std::time_t t = std::time(nullptr);
        char buf[64];
        std::strftime(buf, sizeof buf, "%Y-%m-%d %H:%M:%S", std::localtime(&t));
        std::fprintf(f, "Arcana Forge log %s\n", buf);
        first = false;
    }
    va_list ap;
    va_start(ap, fmt);
    std::vfprintf(f, fmt, ap);
    va_end(ap);
    std::fputc('\n', f);
    std::fclose(f);
}
}
