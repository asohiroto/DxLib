#include "app/Sprites.h"
#include "DxLib.h"
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <algorithm>

namespace {
std::unordered_map<std::string, SpriteAnim> g_anims;
bool g_loaded = false;

// Build a white silhouette of a region of a soft image.
int makeWhite(int soft, int x, int y, int w, int h) {
    int dst = MakeARGB8ColorSoftImage(w, h);
    for (int j = 0; j < h; ++j)
        for (int i = 0; i < w; ++i) {
            int r, g, b, a;
            GetPixelSoftImage(soft, x + i, y + j, &r, &g, &b, &a);
            DrawPixelSoftImage(dst, i, j, 255, 255, 255, a > 0 ? 255 : 0);
        }
    int h2 = CreateGraphFromSoftImage(dst);
    DeleteSoftImage(dst);
    return h2;
}
}  // namespace

namespace Sprites {
bool load(const std::string& png, const std::string& list) {
    int sheet = LoadGraph(png.c_str());
    int soft = LoadSoftImage(png.c_str());
    std::ifstream f(list);
    if (sheet < 0 || soft < 0 || !f) return false;
    struct Entry { std::string name; int x, y, w, h; };
    std::vector<Entry> entries;
    std::string line;
    while (std::getline(f, line)) {
        std::istringstream ss(line);
        Entry e;
        if (ss >> e.name >> e.x >> e.y >> e.w >> e.h) entries.push_back(e);
    }
    std::sort(entries.begin(), entries.end(), [](const Entry& a, const Entry& b) { return a.name < b.name; });
    for (auto& e : entries) {
        std::string base = e.name;
        size_t p = base.rfind("_f");
        if (p != std::string::npos && p + 2 < base.size() && isdigit((unsigned char)base[p + 2]) && base.find("_anim") != std::string::npos)
            base = base.substr(0, p);
        SpriteAnim& a = g_anims[base];
        a.w = e.w;
        a.h = e.h;
        a.frames.push_back(DerivationGraph(e.x, e.y, e.w, e.h, sheet));
        a.white.push_back(makeWhite(soft, e.x, e.y, e.w, e.h));
    }
    DeleteSoftImage(soft);
    g_loaded = !g_anims.empty();
    return g_loaded;
}

bool loaded() { return g_loaded; }

const SpriteAnim* get(const std::string& name) {
    auto it = g_anims.find(name);
    return it == g_anims.end() ? nullptr : &it->second;
}

const SpriteAnim* character(const std::string& base, bool moving) {
    if (moving) if (auto a = get(base + "_run_anim")) return a;
    if (auto a = get(base + "_idle_anim")) return a;
    if (auto a = get(base + "_anim")) return a;
    return get(base);
}

void drawFeet(const SpriteAnim* a, int frame, float x, float y, bool flip, float scale, bool white) {
    if (!a || a->frames.empty()) return;
    int i = ((frame % (int)a->frames.size()) + (int)a->frames.size()) % (int)a->frames.size();
    int h = white ? a->white[i] : a->frames[i];
    DrawRotaGraphF(x, y - a->h * scale * 0.5f, scale, 0.0, h, TRUE, flip ? TRUE : FALSE);
}

void drawCenter(const SpriteAnim* a, int frame, float x, float y, bool flip, float scale, float angle) {
    if (!a || a->frames.empty()) return;
    int i = ((frame % (int)a->frames.size()) + (int)a->frames.size()) % (int)a->frames.size();
    DrawRotaGraphF(x, y, scale, angle, a->frames[i], TRUE, flip ? TRUE : FALSE);
}

int handle(const std::string& name) {
    auto a = get(name);
    return a && !a->frames.empty() ? a->frames[0] : -1;
}
}  // namespace Sprites
