#pragma once
#include <string>
#include <vector>

// Sprite sheet (data/gfx/dungeon.png + dungeon.txt: "name x y w h" per line).
// Frames named foo_anim_f0..f3 become the animation "foo_anim".
struct SpriteAnim {
    std::vector<int> frames;    // DxLib graph handles
    std::vector<int> white;     // white silhouettes (outlines / hit flash)
    int w = 16, h = 16;
};

namespace Sprites {
bool load(const std::string& png, const std::string& list);
bool loaded();
const SpriteAnim* get(const std::string& name);              // exact name ("swampy_anim", "column")
// Character lookup: "<base>_run_anim" when moving, "<base>_idle_anim", then "<base>_anim", then "<base>"
const SpriteAnim* character(const std::string& base, bool moving);
// Draw a frame centred horizontally on x with its feet at y (logical pixels, current draw target).
void drawFeet(const SpriteAnim* a, int frame, float x, float y, bool flip, float scale = 1.0f, bool white = false);
void drawCenter(const SpriteAnim* a, int frame, float x, float y, bool flip, float scale = 1.0f, float angle = 0.0f);
int handle(const std::string& name);   // first frame handle, -1 if missing
}
