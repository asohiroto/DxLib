#include "app/Fx.h"
#include "DxLib.h"
#include "EffekseerForDXLib.h"
#include <string>
#include <unordered_map>
#include <vector>

namespace {
std::unordered_map<std::string, int> g_res;   // name -> resource handle (-1 = missing)
std::vector<int> g_playing;                   // handles started since the last stopAll

int resource(const char* name) {
    auto it = g_res.find(name);
    if (it != g_res.end()) return it->second;
    std::string p = std::string("data/fx/") + name + ".efkefc";
    int h = LoadEffekseerEffect(p.c_str(), 1.0f);
    g_res[name] = h;
    return h;
}
}  // namespace

namespace Fx {
void init() { g_res.clear(); g_playing.clear(); }

void play(const char* name, Vec2 lp, float scale, float rotation) {
    int r = resource(name);
    if (r < 0) return;
    int h = PlayEffekseer2DEffect(r);
    if (h < 0) return;
    SetPosPlayingEffekseer2DEffect(h, lp.x * 2, lp.y * 2, 0);
    SetScalePlayingEffekseer2DEffect(h, scale, scale, scale);
    if (rotation != 0) SetRotationPlayingEffekseer2DEffect(h, 0, 0, rotation);
    // keep the list short: drop handles whose effect already ended
    if (g_playing.size() > 64) {
        std::vector<int> alive;
        for (int p : g_playing) if (IsEffekseer2DEffectPlaying(p) == 0) alive.push_back(p);
        g_playing.swap(alive);
    }
    g_playing.push_back(h);
}

void updateAndDraw(Vec2 shake) {
    (void)shake;
    UpdateEffekseer2D();
    DrawEffekseer2D();
}

void stopAll() {
    for (int h : g_playing) StopEffekseer2DEffect(h);
    g_playing.clear();
}
}  // namespace Fx
