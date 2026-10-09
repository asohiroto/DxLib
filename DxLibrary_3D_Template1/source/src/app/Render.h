#pragma once
#include <vector>
#include <string>
#include <unordered_map>
#include "core/Vec2.h"
#include "core/Rng.h"
#include "game/World.h"

struct Particle {
    Vec2 pos, vel;
    float life, maxLife;
    float size;
    unsigned color;
    float drag = 0.9f;
    float grav = 0;
    bool add = true;
    int shape = 0;   // 0 circle, 1 spark line, 2 ring
};

struct FloatText {
    Vec2 pos;
    std::string text;
    float life;
    unsigned color;
    int big;
};

struct Rgb { int r, g, b; };
Rgb elemColor(Elem e);
Rgb reactionColor(Reaction r);
unsigned toColor(Rgb c);

class Renderer {
public:
    bool init();
    void onEvents(const World& w);
    void update();
    void draw(const World& w, Vec2 mouseL, bool showDebug, bool usingPad);
    void clearFx() { parts_.clear(); texts_.clear(); bannerT_ = 0; evolveCutT_ = 0; }
    void setFxOptions(bool reduce, bool numbers) { reduceFx_ = reduce; damageNumbers_ = numbers; }

    // shared UI helpers (native 1280x720 coordinates)
    void drawCard(int x, int y, int w, int h, const CardInstance& c, bool selected, bool affordable, int keyLabel);
    void drawText(int x, int y, const char* s, unsigned col, int size = 1, bool centered = false);
    int fontT() const { return fontT_; }
    int fontS() const { return fontS_; }
    int fontM() const { return fontM_; }
    int fontL() const { return fontL_; }
    int fontXL() const { return fontXL_; }
    int cardIcon(int def);   // icon graph for a card (evolved forms use their base card), -1 if none
    int relicIcon(const std::string& id);   // data/gfx/relics/<id>.png, -1 if none
    void drawIcon(int handle, int cx, int cy, int scale);   // 32px icon centred, integer scale

private:
    void drawWorld(const World& w);
    void drawHud(const World& w, bool usingPad);
    void spawnBurst(Vec2 p, int n, Rgb c, float speed, float life, float size, int shape = 0);
    void addText(Vec2 p, const std::string& s, unsigned col, int big = 0);

    int screen_ = -1;
    int fontT_ = -1, fontS_ = -1, fontM_ = -1, fontL_ = -1, fontXL_ = -1;
    std::vector<Particle> parts_;
    std::vector<FloatText> texts_;
    Rng fx_{777};
    float time_ = 0;
    std::string banner_;
    float bannerT_ = 0;
    unsigned bannerCol_ = 0xFFFFFF;
    int lastEvolveDef_ = -1;
    Vec2 prevPlayerPos_;
    bool reduceFx_ = false, damageNumbers_ = true;
    bool handCard_ = false, usingPad_ = false;   // set while drawCard draws the battle hand
    float affinityTextT_ = 0;
    std::vector<int> iconCache_;   // per def: -2 unknown, -1 none
    std::unordered_map<std::string, int> relicIcons_;
    float evolveCutT_ = 0;
};
