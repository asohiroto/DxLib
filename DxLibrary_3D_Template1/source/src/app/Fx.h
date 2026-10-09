#pragma once
#include "core/Vec2.h"

// Thin wrapper around EffekseerForDXLib. Effects live in data/fx/<name>.efkefc.
// Positions are logical (640x360); they are scaled to the 1280x720 back buffer.
namespace Fx {
void init();
void play(const char* name, Vec2 logicalPos, float scale = 1.0f, float rotation = 0.0f);
void updateAndDraw(Vec2 shakeOffsetNative);
void stopAll();
}
