#pragma once
#include <string>
#include "game/RunState.h"

constexpr int SAVE_VERSION = 1;

namespace Save {
std::string serializeRun(const RunState& r);
// Returns false on any parse error or version mismatch (the caller then starts fresh).
bool deserializeRun(const std::string& text, RunState& out);
bool writeFile(const std::string& path, const std::string& text);   // atomic (tmp + rename)
bool readFile(const std::string& path, std::string& out);
}
