#include "app/Render.h"
#include "app/Probe.h"
#include "game/Profile.h"
std::string bindName(int bind);
#include "app/Sound.h"
#include "app/Probe.h"
#include "app/Fx.h"
#include "app/Probe.h"
#include "app/Sprites.h"
#include "app/Probe.h"
#include "DxLib.h"
#include <algorithm>
#include <cmath>
#include <cstdio>

// ---------------------------------------------------------------- colors
Rgb elemColor(Elem e) {
    switch (e) {
    case Elem::Fire: return {255, 120, 40};
    case Elem::Ice: return {110, 200, 255};
    case Elem::Thunder: return {255, 225, 70};
    case Elem::Earth: return {200, 145, 80};
    default: return {200, 170, 255};
    }
}
Rgb reactionColor(Reaction r) {
    switch (r) {
    case Reaction::Steam: return {225, 225, 235};
    case Reaction::Blast: return {255, 170, 60};
    case Reaction::Lava: return {255, 80, 30};
    case Reaction::Supercond: return {150, 230, 255};
    case Reaction::Permafrost: return {175, 235, 255};
    case Reaction::Magnet: return {210, 170, 100};
    case Reaction::Purify: return {255, 255, 255};
    case Reaction::Resonance: return {210, 185, 255};
    default: return {160, 160, 160};
    }
}
unsigned toColor(Rgb c) { return GetColor(c.r, c.g, c.b); }
static Rgb mix(Rgb a, Rgb b, float t) {
    return {(int)(a.r + (b.r - a.r) * t), (int)(a.g + (b.g - a.g) * t), (int)(a.b + (b.b - a.b) * t)};
}
static Rgb scale(Rgb a, float s) { return {std::min(255, (int)(a.r * s)), std::min(255, (int)(a.g * s)), std::min(255, (int)(a.b * s))}; }

// ---------------------------------------------------------------- init
bool Renderer::init() {
    screen_ = MakeScreen(640, 360, FALSE);
    Sprites::load("data/gfx/dungeon.png", "data/gfx/dungeon.txt");
    // bundled 12px pixel font, used only at integer multiples so every glyph stays crisp
    if (AddFontFile("data/font/Probly12-CJK.ttf") != 0) {
        fontT_ = CreateFontToHandle("Probly12", 12, -1, DX_FONTTYPE_EDGE, -1, 1);
        fontS_ = CreateFontToHandle("Probly12", 24, -1, DX_FONTTYPE_EDGE, -1, 2);
        fontM_ = CreateFontToHandle("Probly12", 24, -1, DX_FONTTYPE_EDGE, -1, 2);
        fontL_ = CreateFontToHandle("Probly12", 36, -1, DX_FONTTYPE_EDGE, -1, 3);
        fontXL_ = CreateFontToHandle("Probly12", 48, -1, DX_FONTTYPE_EDGE, -1, 4);
    }
    if (fontS_ < 0) {
        const char* face = "Yu Gothic UI";
        fontT_ = CreateFontToHandle(face, 12, 4, DX_FONTTYPE_ANTIALIASING_EDGE_4X4, -1, 1);
        fontS_ = CreateFontToHandle(face, 15, 4, DX_FONTTYPE_ANTIALIASING_EDGE_4X4, -1, 1);
        fontM_ = CreateFontToHandle(face, 19, 5, DX_FONTTYPE_ANTIALIASING_EDGE_4X4, -1, 2);
        fontL_ = CreateFontToHandle(face, 28, 7, DX_FONTTYPE_ANTIALIASING_EDGE_4X4, -1, 2);
        fontXL_ = CreateFontToHandle(face, 52, 9, DX_FONTTYPE_ANTIALIASING_EDGE_4X4, -1, 3);
    }
    return screen_ != -1;
}

void Renderer::drawText(int x, int y, const char* s, unsigned col, int size, bool centered) {
    int f = size < 0 ? fontT_ : (size == 0 ? fontS_ : (size == 1 ? fontM_ : (size == 2 ? fontL_ : fontXL_)));
    if (centered) x -= PStrWidth(s, (int)strlen(s), f) / 2;
    PDrawString(x, y, s, col, f, GetColor(10, 10, 20));
}

// ---------------------------------------------------------------- fx
void Renderer::spawnBurst(Vec2 p, int n, Rgb c, float speed, float life, float size, int shape) {
    for (int i = 0; i < n; ++i) {
        Particle q;
        q.pos = p;
        float a = fx_.range(0, 2 * PI);
        float s = fx_.range(0.3f, 1.0f) * speed;
        q.vel = Vec2::fromAngle(a) * s;
        q.life = q.maxLife = fx_.range(0.6f, 1.0f) * life;
        q.size = size * fx_.range(0.6f, 1.2f);
        q.color = toColor(c);
        q.shape = shape;
        parts_.push_back(q);
    }
}

void Renderer::addText(Vec2 p, const std::string& s, unsigned col, int big) {
    texts_.push_back({p + Vec2{fx_.range(-4, 4), -6}, s, big ? 1.0f : 0.7f, col, big});
}

void Renderer::onEvents(const World& w) {
    bool hitFx[(int)Elem::Count] = {};   // at most one Effekseer hit per element per frame
    for (const GameEvent& e : w.events) {
        switch (e.type) {
        case Ev::Damage: {
            char buf[32];
            std::snprintf(buf, sizeof buf, "%d", (int)std::lround(e.value));
            unsigned col = e.b == 1 ? GetColor(140, 230, 255) : (e.b == 2 || e.b == 5 ? GetColor(255, 220, 90) : GetColor(255, 255, 255));
            if (e.b == 3) col = GetColor(255, 150, 70);
            if (e.b == 4) col = GetColor(150, 150, 170);
            if ((e.b == 5 || e.b == 4) && affinityTextT_ <= 0) {   // tell the player about weaknesses / resistances
                addText(e.pos + Vec2{0, -26}, e.b == 5 ? "弱点！" : "耐性", e.b == 5 ? GetColor(255, 200, 60) : GetColor(160, 160, 190), 0);
                affinityTextT_ = 0.6f;
            }
            if (damageNumbers_) addText(e.pos, buf, col, e.b == 1 || e.value >= 25 ? 1 : 0);
            spawnBurst(e.pos, e.b == 3 ? 2 : 5, elemColor((Elem)e.a), 70, 0.25f, 1.5f, 1);
            if (e.b != 3) Sound::play(Sfx::Hit, 0.6f);
            if (e.b != 3 && e.a > 0 && e.a < (int)Elem::Count && !hitFx[e.a]) {
                hitFx[e.a] = true;
                static const char* names[] = {"", "hit_fire", "hit_ice", "hit_thunder", "hit_earth"};
                Fx::play(names[e.a], e.pos, 2.5f + std::min(3.0f, e.value / 15.0f));
            }
            break;
        }
        case Ev::Kill:
            spawnBurst(e.pos, 18, {255, 240, 220}, 120, 0.45f, 2.0f, 1);
            spawnBurst(e.pos, 1, {255, 255, 255}, 0, 0.25f, 14, 2);
            Fx::play("kill", e.pos, 6.0f);
            Sound::play(Sfx::Kill, 0.7f);
            break;
        case Ev::PlayerHurt:
            spawnBurst(e.pos, 14, {255, 60, 60}, 110, 0.4f, 2.0f, 1);
            Sound::play(Sfx::Hurt, 0.9f);
            break;
        case Ev::CardUsed:
            spawnBurst(e.pos, 10, elemColor((Elem)e.a), 90, 0.35f, 1.8f);
            Sound::play(Sfx::Card, 0.7f);
            break;
        case Ev::Fused: {
            Rgb c = reactionColor((Reaction)e.a);
            spawnBurst(e.pos, 24, c, 140, 0.6f, 2.0f);
            spawnBurst(e.pos, 1, c, 0, 0.4f, 26, 2);
            Fx::play("fuse", e.pos, 8.0f);
            Sound::play(Sfx::Fuse);
            break;
        }
        case Ev::Evolved:
            spawnBurst(e.pos, 50, {255, 220, 120}, 200, 0.9f, 2.4f);
            spawnBurst(e.pos, 1, {255, 230, 150}, 0, 0.6f, 40, 2);
            Fx::play("evolve", e.pos, 10.0f);
            Sound::play(Sfx::Evolve);
            lastEvolveDef_ = e.a;
            evolveCutT_ = 1.4f;
            break;
        case Ev::Explosion: {
            Rgb c = e.b == 2 ? Rgb{255, 170, 60} : elemColor((Elem)e.a);
            spawnBurst(e.pos, 6 + (int)(e.value * 0.4f), c, e.value * 3.0f, 0.35f, 2.0f);
            Particle ring;
            ring.pos = e.pos; ring.vel = {}; ring.life = ring.maxLife = 0.25f; ring.size = e.value;
            ring.color = toColor(c); ring.shape = 2;
            parts_.push_back(ring);
            if (e.b == 2 || e.value >= 30) { Fx::play("explosion", e.pos, e.value / 5.0f); Sound::play(Sfx::Explode, 0.6f); }
            break;
        }
        case Ev::Reaction:
            if (e.b == 0) {
                Rgb c = reactionColor((Reaction)e.a);
                addText(e.pos + Vec2{0, -10}, reactionName((Reaction)e.a), toColor(c), 1);
                spawnBurst(e.pos, 12, c, 80, 0.5f, 2.0f);
                Fx::play("fuse", e.pos, 3.5f);
            } else {
                Particle s;  // chain spark
                s.pos = e.pos; s.life = s.maxLife = 0.2f; s.size = 10; s.color = GetColor(255, 240, 120); s.shape = 2;
                parts_.push_back(s);
            }
            break;
        case Ev::Shatter:
            addText(e.pos + Vec2{0, -12}, "粉砕!", GetColor(160, 240, 255), 1);
            spawnBurst(e.pos, 20, {180, 240, 255}, 150, 0.5f, 2.2f, 1);
            Sound::play(Sfx::Shatter);
            break;
        case Ev::Freeze:
            spawnBurst(e.pos, 10, {170, 230, 255}, 50, 0.5f, 1.6f);
            Sound::play(Sfx::Freeze, 0.6f);
            break;
        case Ev::Dash:
            spawnBurst(e.pos, 6, {200, 230, 255}, 40, 0.3f, 2.0f);
            Sound::play(Sfx::Dash, 0.5f);
            break;
        case Ev::BasicShot:
            Sound::play(Sfx::Shot, 0.35f);
            break;
        case Ev::Heal:
            addText(e.pos + Vec2{0, -12}, "+" + std::to_string((int)e.value), GetColor(120, 255, 140), 1);
            spawnBurst(e.pos, 16, {120, 255, 140}, 60, 0.6f, 2.0f);
            Sound::play(Sfx::Heal, 0.8f);
            break;
        case Ev::Shield:
            if (e.a == 0) { spawnBurst(e.pos, 16, {210, 170, 110}, 60, 0.5f, 2.0f); Sound::play(Sfx::Shield, 0.7f); }
            else addText(e.pos + Vec2{0, -12}, "ガード", GetColor(230, 200, 140), 0);
            break;
        case Ev::Spawn:
            Sound::play(Sfx::Spawn, 0.25f);
            break;
        case Ev::Telegraph:
            Sound::play(Sfx::Warn, 0.45f);
            break;
        case Ev::BossPhase:
            banner_ = "怒り状態！"; bannerT_ = 1.6f; bannerCol_ = GetColor(255, 110, 110);
            spawnBurst(e.pos, 40, {255, 80, 80}, 180, 0.8f, 2.4f);
            Fx::play("evolve", e.pos, 9.0f);
            Sound::play(Sfx::BossPhase, 0.9f);
            break;
        case Ev::WaveStart: {
            char buf[32];
            std::snprintf(buf, sizeof buf, "WAVE %d / %d", e.a + 1, w.waveCount);
            banner_ = buf; bannerT_ = 1.5f; bannerCol_ = GetColor(255, 240, 200);
            Sound::play(Sfx::Select);
            break;
        }
        case Ev::RoomClear:
            banner_ = "CLEAR!"; bannerT_ = 99; bannerCol_ = GetColor(255, 230, 120);
            Sound::play(Sfx::Clear);
            break;
        case Ev::PlayerDead:
            banner_ = "DEFEATED"; bannerT_ = 99; bannerCol_ = GetColor(255, 90, 90);
            Sound::play(Sfx::Death);
            break;
        case Ev::NoMana:
            addText(w.player.pos + Vec2{0, -16}, "マナ不足", GetColor(150, 170, 255), 0);
            Sound::play(Sfx::NoMana, 0.6f);
            break;
        }
    }
}

void Renderer::update() {
    time_ += DT;
    for (auto& p : parts_) {
        p.pos += p.vel * DT;
        p.vel *= p.drag;
        p.vel.y += p.grav * DT;
        p.life -= DT;
    }
    parts_.erase(std::remove_if(parts_.begin(), parts_.end(), [](const Particle& p) { return p.life <= 0; }), parts_.end());
    for (auto& t : texts_) { t.pos.y -= 18 * DT; t.life -= DT; }
    texts_.erase(std::remove_if(texts_.begin(), texts_.end(), [](const FloatText& t) { return t.life <= 0; }), texts_.end());
    if (bannerT_ > 0 && bannerT_ < 90) bannerT_ -= DT;
    if (evolveCutT_ > 0) evolveCutT_ -= DT;
    if (affinityTextT_ > 0) affinityTextT_ -= DT;
}

// ---------------------------------------------------------------- world
// Sprite version of an enemy. Returns false when the enemy has no sprite (shape fallback is used).
static bool drawEnemySprite(const Enemy& e, float time, Vec2 playerPos) {
    const EnemyDef& D = e.d();
    if (D.sprite.empty() || !Sprites::loaded()) return false;
    bool moving = e.st.frozenT <= 0 && D.speed > 0 && !(D.ai == AiKind::Sniper && e.state == 1);
    const SpriteAnim* a = Sprites::character(D.sprite, moving);
    if (!a) return false;
    float sc = D.sprScale * (e.elite ? ELITE_SIZE : 1.0f);
    float x = e.pos.x, fy = e.pos.y + e.r * 0.7f;
    if (D.ai == AiKind::BossSlime && e.state == 1) fy -= std::sin(std::min(1.0f, e.t) * PI) * 40;   // airborne
    bool flip = playerPos.x < x;
    int fr = (int)(time * (moving ? 10 : 6) + e.id * 3);
    if (e.elite) {   // gold silhouette outline
        int g = 180 + (int)(60 * std::sin(time * 6 + e.id));
        SetDrawBright(255, g, 60);
        for (Vec2 o : {Vec2{1, 0}, Vec2{-1, 0}, Vec2{0, 1}, Vec2{0, -1}}) Sprites::drawFeet(a, fr, x + o.x, fy + o.y, flip, sc, true);
    }
    if (!e.elite) {   // thin dark rim so sprites read against the floor
        SetDrawBright(110, 100, 140);   // light rim: dark sprites must stand out from the dark floor
        for (Vec2 o : {Vec2{1, 0}, Vec2{-1, 0}, Vec2{0, 1}, Vec2{0, -1}}) Sprites::drawFeet(a, fr, x + o.x, fy + o.y, flip, sc, true);
    }
    int tr = D.tr, tg = D.tg, tb = D.tb;
    if (e.st.frozenT > 0) { tr = 150; tg = 210; tb = 255; }
    SetDrawBright(tr, tg, tb);
    Sprites::drawFeet(a, fr, x, fy, flip, sc);
    SetDrawBright(255, 255, 255);
    if (e.hitFlash > 0) {
        SetDrawBlendMode(DX_BLENDMODE_ALPHA, (int)((D.boss ? 70 : 200) * std::min(1.0f, e.hitFlash / 0.08f)));
        Sprites::drawFeet(a, fr, x, fy, flip, sc, true);
        SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
    }
    // held weapons
    if (D.ai == AiKind::Archer || D.ai == AiKind::Sniper) {
        Vec2 d = (playerPos - e.pos).norm();
        if (const SpriteAnim* bow = Sprites::get("weapon_bow"))
            Sprites::drawCenter(bow, 0, x + d.x * 6, e.pos.y + d.y * 6, false, 0.55f, std::atan2(d.y, d.x));
    }
    if (D.ai == AiKind::BossSlime) {   // crown
        float cy = fy - a->h * sc + 2;
        DrawTriangleAA(x - 8, cy, x - 4, cy - 9, x, cy, GetColor(255, 210, 60), TRUE);
        DrawTriangleAA(x - 2, cy, x + 2, cy - 11, x + 6, cy, GetColor(255, 210, 60), TRUE);
        DrawTriangleAA(x + 4, cy, x + 8, cy - 9, x + 12, cy, GetColor(255, 210, 60), TRUE);
    }
    if (D.ai == AiKind::Shield) {
        if (e.guardBreakT <= 0) {
            Vec2 f = e.dir.len2() > 0.01f ? e.dir : Vec2{0, 1};
            Vec2 p = e.pos + f * (e.r + 3);
            Vec2 side{-f.y, f.x};
            float g = e.guard / 40.0f;
            DrawLineAA(p.x - side.x * 9, p.y - side.y * 9, p.x + side.x * 9, p.y + side.y * 9, GetColor(20, 20, 30), 6.0f);
            DrawLineAA(p.x - side.x * 8, p.y - side.y * 8, p.x + side.x * 8, p.y + side.y * 8, GetColor(200, 210, 240), 4.0f);
            DrawLineAA(p.x - side.x * 8 * g, p.y - side.y * 8 * g, p.x + side.x * 8 * g, p.y + side.y * 8 * g, GetColor(120, 160, 255), 2.0f);
        } else if (std::fmod(time * 8, 2.0f) < 1.0f) {
            DrawCircleAA(x, e.pos.y - e.r - 10, 2, 8, GetColor(255, 255, 140), TRUE);
        }
    }
    if (D.ai == AiKind::Bomber && std::fmod(time * (e.state ? 16 : 4), 2.0f) < 1.0f)
        DrawCircleAA(x + 4, e.pos.y - 10, 2, 8, GetColor(255, 240, 120), TRUE);
    if (D.ai == AiKind::BossGolem) {
        Rgb core = e.adaptResist != Elem::None ? elemColor(e.adaptResist) : Rgb{255, 240, 200};
        SetDrawBlendMode(DX_BLENDMODE_ADD, 170);
        DrawCircleAA(x, e.pos.y - 6, 9 + std::sin(time * 6) * 2, 16, toColor(core), FALSE, 2.0f);
        SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
    }
    // windup telegraphs (same visual language as hazards: red)
    if ((D.ai == AiKind::Charger || D.ai == AiKind::BossGolem) && e.state == 1) {
        float t = std::min(1.0f, e.t / (D.ai == AiKind::Charger ? 0.7f : 0.8f));
        SetDrawBlendMode(DX_BLENDMODE_ALPHA, (int)(80 + 140 * t));
        Vec2 end = e.pos + e.dir * (D.ai == AiKind::Charger ? 150.0f : 200.0f);
        DrawLineAA(x, e.pos.y, end.x, end.y, GetColor(255, 60, 60), 1.0f + 7 * t);
        SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
    }
    if (D.ai == AiKind::Sniper && e.state == 1) {
        float t = std::min(1.0f, e.t / 1.0f);
        Vec2 end = e.pos + e.dir * 700;
        SetDrawBlendMode(DX_BLENDMODE_ALPHA, (int)(60 + 180 * t));
        DrawLineAA(x, e.pos.y, end.x, end.y, t > 0.7f ? GetColor(255, 40, 40) : GetColor(255, 200, 80), t > 0.7f ? 2.0f : 1.0f);
        SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
    }
    if ((D.ai == AiKind::Caster || D.ai == AiKind::BossLich) && e.cd < 0.6f) {
        float k = 1 - std::max(0.0f, e.cd) / 0.6f;
        SetDrawBlendMode(DX_BLENDMODE_ADD, (int)(255 * k));
        DrawCircleAA(x, e.pos.y, e.r + 6 * k, 20, GetColor(255, 80, 200), FALSE, 1.5f);
        SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
    }
    return true;
}

static void drawEnemy(const Enemy& e, float time, Vec2 playerPos) {
    float x = e.pos.x, y = e.pos.y;
    if (e.spawnT > 0) {
        float t = 1.0f - e.spawnT / SPAWN_TELEGRAPH;
        SetDrawBlendMode(DX_BLENDMODE_ALPHA, (int)(120 + 100 * t));
        DrawCircleAA(x, y, e.r * (2.2f - 1.2f * t), 24, GetColor(255, 80, 120), FALSE, 1.5f);
        DrawCircleAA(x, y, e.r * t, 16, GetColor(255, 80, 120), TRUE);
        SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
        return;
    }
    // shadow
    SetDrawBlendMode(DX_BLENDMODE_ALPHA, 90);
    DrawOvalAA(x, y + e.r * 0.8f, e.r * 0.9f, e.r * 0.35f, 16, GetColor(0, 0, 0), TRUE);
    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
    const EnemyDef& D = e.d();
    Rgb body{D.r, D.g, D.b};
    if (e.st.frozenT > 0) body = mix(body, {170, 230, 255}, 0.7f);
    if (e.hitFlash > 0) body = {255, 255, 255};
    unsigned c = toColor(body), dark = toColor(scale(body, 0.45f));
    bool sprite = drawEnemySprite(e, time, playerPos);
    if (e.elite && !sprite) {   // golden aura
        SetDrawBlendMode(DX_BLENDMODE_ADD, (int)(110 + 60 * std::sin(time * 6 + e.id)));
        DrawCircleAA(x, y, e.r + 4, 24, GetColor(255, 200, 80), FALSE, 2.0f);
        SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
    }
    if (!sprite) switch (D.ai) {
    case AiKind::Chase:
    case AiKind::Splitter:
    case AiKind::BossSlime: {
        float sq = 1.0f + 0.15f * std::sin(time * 10 + e.id);
        if (D.ai == AiKind::BossSlime && e.state == 1) {   // airborne: shadow at target, body lifted
            float t = std::min(1.0f, e.t);
            float lift = std::sin(t * PI) * 40;
            y -= lift;
            sq = 0.85f;
        }
        DrawOvalAA(x, y, e.r * sq, e.r / sq, 24, dark, TRUE);
        DrawOvalAA(x, y - 1, e.r * sq - 1.2f, e.r / sq - 1.2f, 24, c, TRUE);
        float eye = e.r * 0.3f;
        DrawCircleAA(x - eye, y - eye, std::max(1.2f, e.r * 0.15f), 8, GetColor(20, 30, 20), TRUE);
        DrawCircleAA(x + eye, y - eye, std::max(1.2f, e.r * 0.15f), 8, GetColor(20, 30, 20), TRUE);
        if (D.ai == AiKind::Splitter) DrawLineAA(x, y - e.r + 2, x, y + e.r - 2, dark, 1.5f);
        if (D.ai == AiKind::BossSlime) {   // crown
            DrawTriangleAA(x - 8, y - e.r + 2, x - 4, y - e.r - 8, x, y - e.r + 2, GetColor(255, 210, 60), TRUE);
            DrawTriangleAA(x - 2, y - e.r + 2, x + 2, y - e.r - 10, x + 6, y - e.r + 2, GetColor(255, 210, 60), TRUE);
            DrawTriangleAA(x + 4, y - e.r + 2, x + 8, y - e.r - 8, x + 12, y - e.r + 2, GetColor(255, 210, 60), TRUE);
        }
        break;
    }
    case AiKind::Archer:
    case AiKind::Sniper: {
        DrawCircleAA(x, y, e.r, 16, dark, TRUE);
        DrawCircleAA(x, y, e.r - 1.2f, 16, c, TRUE);
        DrawTriangleAA(x - 3, y - e.r - 1, x + 3, y - e.r - 1, x, y - e.r - 6, dark, TRUE);
        if (D.ai == AiKind::Sniper && e.state == 1) {   // aiming laser
            float t = std::min(1.0f, e.t / 1.0f);
            Vec2 end = e.pos + e.dir * 700;
            SetDrawBlendMode(DX_BLENDMODE_ALPHA, (int)(60 + 180 * t));
            DrawLineAA(x, y, end.x, end.y, t > 0.7f ? GetColor(255, 40, 40) : GetColor(255, 200, 80), t > 0.7f ? 2.0f : 1.0f);
            SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
        }
        break;
    }
    case AiKind::Bomber: {
        DrawCircleAA(x, y, e.r, 16, dark, TRUE);
        DrawCircleAA(x, y, e.r - 1.2f, 16, c, TRUE);
        DrawLineAA(x + 2, y - e.r, x + 5, y - e.r - 5, GetColor(120, 100, 80), 1.5f);
        if (std::fmod(time * (e.state ? 16 : 4), 2.0f) < 1.0f) DrawCircleAA(x + 5, y - e.r - 6, 2, 8, GetColor(255, 240, 120), TRUE);
        break;
    }
    case AiKind::Shield: {
        DrawCircleAA(x, y, e.r, 16, dark, TRUE);
        DrawCircleAA(x, y, e.r - 1.2f, 16, c, TRUE);
        if (e.guardBreakT <= 0) {
            Vec2 f = e.dir.len2() > 0.01f ? e.dir : Vec2{0, 1};
            Vec2 p = e.pos + f * (e.r + 3);
            Vec2 side{-f.y, f.x};
            float g = e.guard / 40.0f;
            DrawLineAA(p.x - side.x * 9, p.y - side.y * 9, p.x + side.x * 9, p.y + side.y * 9, GetColor(230, 230, 250), 4.0f);
            DrawLineAA(p.x - side.x * 9 * g, p.y - side.y * 9 * g, p.x + side.x * 9 * g, p.y + side.y * 9 * g, GetColor(140, 170, 255), 2.0f);
        } else if (std::fmod(time * 8, 2.0f) < 1.0f) {
            DrawCircleAA(x, y - e.r - 4, 2, 8, GetColor(255, 255, 140), TRUE);   // stunned
        }
        break;
    }
    case AiKind::Summoner: {
        DrawTriangleAA(x, y - e.r - 3, x - e.r, y + e.r, x + e.r, y + e.r, dark, TRUE);
        DrawTriangleAA(x, y - e.r - 1, x - e.r + 1.5f, y + e.r - 1, x + e.r - 1.5f, y + e.r - 1, c, TRUE);
        SetDrawBlendMode(DX_BLENDMODE_ADD, 120);
        DrawCircleAA(x, y, e.r + 5 + std::sin(time * 4) * 2, 20, GetColor(255, 100, 200), FALSE, 1.0f);
        SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
        break;
    }
    case AiKind::Turret: {
        DrawBoxAA(x - e.r, y - e.r, x + e.r, y + e.r, dark, TRUE);
        DrawBoxAA(x - e.r + 2, y - e.r + 2, x + e.r - 2, y + e.r - 2, c, TRUE);
        Vec2 g = Vec2::fromAngle(e.t * 2.3f) * (e.r + 4);
        DrawLineAA(x, y, x + g.x, y + g.y, GetColor(60, 60, 80), 3.0f);
        break;
    }
    case AiKind::BossLich: {
        DrawTriangleAA(x, y - e.r - 6, x - e.r, y + e.r, x + e.r, y + e.r, dark, TRUE);
        DrawTriangleAA(x, y - e.r - 4, x - e.r + 2, y + e.r - 1, x + e.r - 2, y + e.r - 1, c, TRUE);
        DrawCircleAA(x, y - e.r * 0.3f, e.r * 0.45f, 16, GetColor(230, 240, 255), TRUE);
        DrawCircleAA(x - 3, y - e.r * 0.35f, 1.6f, 8, GetColor(80, 200, 255), TRUE);
        DrawCircleAA(x + 3, y - e.r * 0.35f, 1.6f, 8, GetColor(80, 200, 255), TRUE);
        SetDrawBlendMode(DX_BLENDMODE_ADD, 110);
        for (int k = 0; k < 3; ++k) {
            Vec2 o = Vec2::fromAngle(time * 1.5f + k * 2.1f) * (e.r + 10);
            DrawCircleAA(x + o.x, y + o.y, 3, 10, GetColor(150, 220, 255), TRUE);
        }
        SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
        break;
    }
    case AiKind::BossGolem: {
        DrawBoxAA(x - e.r, y - e.r, x + e.r, y + e.r, dark, TRUE);
        DrawBoxAA(x - e.r + 2, y - e.r + 2, x + e.r - 2, y + e.r - 2, c, TRUE);
        Rgb core = e.adaptResist != Elem::None ? elemColor(e.adaptResist) : Rgb{255, 240, 200};
        SetDrawBlendMode(DX_BLENDMODE_ADD, 200);
        DrawCircleAA(x, y, 5 + std::sin(time * 6), 16, toColor(core), TRUE);
        SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
        if (e.state == 1) {
            float t = std::min(1.0f, e.t / 0.8f);
            SetDrawBlendMode(DX_BLENDMODE_ALPHA, (int)(80 + 140 * t));
            Vec2 end = e.pos + e.dir * 200;
            DrawLineAA(x, y, end.x, end.y, GetColor(255, 60, 60), 2.0f + 8 * t);
            SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
        }
        break;
    }
    case AiKind::Charger: {
        DrawBoxAA(x - e.r, y - e.r, x + e.r, y + e.r, dark, TRUE);
        DrawBoxAA(x - e.r + 1.5f, y - e.r + 1.5f, x + e.r - 1.5f, y + e.r - 1.5f, c, TRUE);
        DrawTriangleAA(x - e.r, y - e.r, x - e.r - 3, y - e.r - 5, x - e.r + 4, y - e.r, GetColor(240, 230, 210), TRUE);
        DrawTriangleAA(x + e.r, y - e.r, x + e.r + 3, y - e.r - 5, x + e.r - 4, y - e.r, GetColor(240, 230, 210), TRUE);
        if (e.state == 1) {  // windup telegraph
            float t = std::min(1.0f, e.t / 0.7f);
            SetDrawBlendMode(DX_BLENDMODE_ALPHA, (int)(80 + 140 * t));
            Vec2 end = e.pos + e.dir * 150;
            DrawLineAA(x, y, end.x, end.y, GetColor(255, 60, 60), 1.0f + 6 * t);
            SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
        }
        break;
    }
    case AiKind::Caster: {
        DrawTriangleAA(x, y - e.r - 3, x - e.r, y + e.r, x + e.r, y + e.r, dark, TRUE);
        DrawTriangleAA(x, y - e.r - 1, x - e.r + 1.5f, y + e.r - 1, x + e.r - 1.5f, y + e.r - 1, c, TRUE);
        float cd = std::max(0.0f, e.cd);
        if (cd < 0.6f) {  // casting glow
            SetDrawBlendMode(DX_BLENDMODE_ADD, (int)(255 * (1 - cd / 0.6f)));
            DrawCircleAA(x, y, e.r + 6 * (1 - cd / 0.6f), 20, GetColor(255, 80, 200), FALSE, 1.5f);
            SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
        }
        break;
    }
    default: break;
    }
    // status overlays
    if (e.st.burnT > 0) {
        SetDrawBlendMode(DX_BLENDMODE_ADD, 160);
        for (int i = 0; i < std::min(3, e.st.burnStacks); ++i) {
            float ox = std::sin(time * 9 + i * 2.1f) * e.r * 0.7f;
            DrawCircleAA(x + ox, y - e.r - 2 - std::fmod(time * 20 + i * 5, 8.0f), 1.6f, 8, GetColor(255, 130, 40), TRUE);
        }
        SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
    }
    if (e.st.shockT > 0 && std::fmod(time * 20, 2.0f) < 1.0f) {
        SetDrawBlendMode(DX_BLENDMODE_ADD, 200);
        DrawLineAA(x - e.r, y - 2, x + e.r, y + 1, GetColor(255, 240, 90), 1.0f);
        SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
    }
    if (e.st.chill > 0 && e.st.frozenT <= 0) {
        for (int i = 0; i < (int)std::min(3.0f, std::ceil(e.st.chill)); ++i)
            DrawCircleAA(x - 4 + i * 4, y + e.r + 4, 1.3f, 6, GetColor(150, 220, 255), TRUE);
    }
    if (e.st.frozenT > 0) {
        SetDrawBlendMode(DX_BLENDMODE_ALPHA, 140);
        DrawBoxAA(x - e.r - 2, y - e.r - 2, x + e.r + 2, y + e.r + 2, GetColor(190, 240, 255), TRUE);
        SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
        DrawBoxAA(x - e.r - 2, y - e.r - 2, x + e.r + 2, y + e.r + 2, GetColor(230, 250, 255), FALSE, 1.0f);
    }
    // hp bar (bosses use the HUD bar)
    if (e.hp < e.maxHp && !D.boss) {
        float w = e.r * 2 + 4, t = std::max(0.0f, e.hp / e.maxHp);
        DrawBox((int)(x - w / 2), (int)(y - e.r - 6), (int)(x + w / 2), (int)(y - e.r - 4), GetColor(30, 10, 10), TRUE);
        DrawBox((int)(x - w / 2), (int)(y - e.r - 6), (int)(x - w / 2 + w * t), (int)(y - e.r - 4), GetColor(240, 70, 60), TRUE);
    }
}

// tile a sprite over a rectangle (clipped)
static void tileRect(int handle, int x, int y, int w, int h) {
    if (handle < 0) return;
    PSetDrawArea(x, y, x + w, y + h);
    for (int ty = y; ty < y + h; ty += 16)
        for (int tx = x; tx < x + w; tx += 16) DrawGraph(tx, ty, handle, TRUE);
    PSetDrawArea(0, 0, 640, 360);
}

void Renderer::drawWorld(const World& w) {
    const uint32_t arenaSeed_ = w.pillars.empty() ? 7u : (uint32_t)w.pillars[0].x * 31u + (uint32_t)w.pillars.size();
    DrawBox(0, 0, 640, 360, GetColor(16, 14, 24), TRUE);
    if (Sprites::loaded()) {
        // floor: mostly plain tiles with sparse variation, dimmed so actors stand out
        // calm mid-tone base with the tile texture at low opacity: actors must pop
        DrawBox((int)ARENA_L, (int)ARENA_T, (int)ARENA_R, (int)ARENA_B, GetColor(58, 52, 68), TRUE);
        static const char* floors[] = {"floor_1", "floor_1", "floor_1", "floor_2", "floor_3", "floor_4", "floor_5", "floor_6", "floor_7", "floor_8"};
        PSetDrawArea((int)ARENA_L, (int)ARENA_T, (int)ARENA_R, (int)ARENA_B);
        SetDrawBlendMode(DX_BLENDMODE_ALPHA, 70);
        for (int ty = 0; ty < 19; ++ty)
            for (int tx = 0; tx < 40; ++tx) {
                uint32_t hsh = (uint32_t)(tx * 73856093u) ^ (uint32_t)(ty * 19349663u) ^ (uint32_t)(arenaSeed_ * 83492791u);
                hsh ^= hsh >> 13; hsh *= 0x5bd1e995u; hsh ^= hsh >> 15;
                if (hsh % 5 != 0) continue;   // sparse texture only
                DrawGraph(tx * 16, ty * 16, Sprites::handle(floors[(hsh >> 8) % 10]), TRUE);
            }
        SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
        // faint grid
        for (int tx = 0; tx < 40; ++tx) DrawLine(tx * 16, (int)ARENA_T, tx * 16, (int)ARENA_B, GetColor(52, 47, 62));
        for (int ty = 0; ty < 19; ++ty) DrawLine((int)ARENA_L, ty * 16, (int)ARENA_R, ty * 16, GetColor(52, 47, 62));
        PSetDrawArea(0, 0, 640, 360);
        // outer walls
        int top = Sprites::handle("wall_top_mid"), mid = Sprites::handle("wall_mid");
        SetDrawBright(120, 115, 140);
        tileRect(top, 0, 0, 640, 2);
        tileRect(top, 0, (int)ARENA_B, 640, 300 - (int)ARENA_B);
        tileRect(top, 0, 0, (int)ARENA_L, 300);
        tileRect(top, (int)ARENA_R, 0, 640 - (int)ARENA_R, 300);
        SetDrawBright(255, 255, 255);
        tileRect(mid, (int)ARENA_L, -2, (int)(ARENA_R - ARENA_L), (int)ARENA_T + 2);
        // pillars: brick top face plus a front face so they read as raised blocks
        for (auto& rc : w.pillars) {
            SetDrawBlendMode(DX_BLENDMODE_ALPHA, 110);
            DrawBox((int)rc.x + 2, (int)(rc.y + rc.h), (int)(rc.x + rc.w + 2), (int)(rc.y + rc.h + 5), GetColor(0, 0, 0), TRUE);
            SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
            SetDrawBright(150, 145, 170);
            tileRect(top, (int)rc.x, (int)rc.y - 8, (int)rc.w, (int)rc.h);
            SetDrawBright(255, 255, 255);
            tileRect(mid, (int)rc.x, (int)(rc.y + rc.h - 8), (int)rc.w, 10);
            DrawBox((int)rc.x, (int)rc.y - 8, (int)(rc.x + rc.w), (int)(rc.y + rc.h + 2), GetColor(20, 16, 28), FALSE);
        }
    } else {
        for (int ty = 0; ty < 19; ++ty)
            for (int tx = 0; tx < 40; ++tx) {
                int x = tx * 16, y = ty * 16;
                unsigned c = ((tx + ty) & 1) ? GetColor(42, 38, 54) : GetColor(38, 34, 50);
                DrawBox(x, y, x + 16, y + 16, c, TRUE);
            }
        unsigned wallTop = GetColor(70, 62, 90);
        DrawBox(0, 0, 640, (int)ARENA_T, wallTop, TRUE);
        DrawBox(0, (int)ARENA_B, 640, 300, wallTop, TRUE);
        DrawBox(0, 0, (int)ARENA_L, 300, wallTop, TRUE);
        DrawBox((int)ARENA_R, 0, 640, 300, wallTop, TRUE);
        for (auto& rc : w.pillars) DrawBox((int)rc.x, (int)rc.y, (int)(rc.x + rc.w), (int)(rc.y + rc.h), GetColor(84, 76, 108), TRUE);
    }

    // zones
    for (auto& z : w.zones) {
        float a = std::min(1.0f, z.life / 0.4f) * std::min(1.0f, (z.maxLife - z.life) / 0.15f + 0.3f);
        Rgb c;
        switch (z.kind) {
        case ZoneKind::Steam: c = {210, 210, 225}; break;
        case ZoneKind::Lava: c = {255, 90, 30}; break;
        case ZoneKind::Permafrost: c = {170, 230, 255}; break;
        default: c = elemColor(z.hit.elems[0]); break;
        }
        if (z.kind == ZoneKind::Steam) {
            // drifting soft puffs
            for (int i = 0; i < 7; ++i) {
                float ang = i * 0.9f + time_ * 0.6f;
                float rr = z.r * (0.25f + 0.12f * (i % 3));
                Vec2 q = z.pos + Vec2::fromAngle(ang) * (z.r * 0.55f);
                q.y -= std::fmod(time_ * 6 + i * 3, 8.0f);
                SetDrawBlendMode(DX_BLENDMODE_ALPHA, (int)(55 * a));
                DrawCircleAA(q.x, q.y, rr, 20, toColor(c), TRUE);
            }
            SetDrawBlendMode(DX_BLENDMODE_ALPHA, (int)(40 * a));
            DrawCircleAA(z.pos.x, z.pos.y, z.r * 0.6f, 24, toColor(c), TRUE);
            SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
            continue;
        }
        SetDrawBlendMode(DX_BLENDMODE_ADD, (int)(110 * a));
        DrawCircleAA(z.pos.x, z.pos.y, z.r, 32, toColor(c), TRUE);
        SetDrawBlendMode(DX_BLENDMODE_ADD, (int)(200 * a));
        DrawCircleAA(z.pos.x, z.pos.y, z.r, 32, toColor(c), FALSE, 1.5f);
        if (z.kind == ZoneKind::Damage) {
            float flick = 0.7f + 0.3f * std::sin(time_ * 30);
            DrawCircleAA(z.pos.x, z.pos.y, z.r * 0.5f * flick, 24, toColor(scale(c, 1.2f)), TRUE);
        }
        SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
    }
    // placed
    for (auto& pl : w.placed) {
        float a = std::min(1.0f, pl.life / 0.5f);
        if (pl.kind == PlacedKind::Wall) {
            for (auto& b : pl.blocks) {
                SetDrawBlendMode(DX_BLENDMODE_ALPHA, (int)(220 * a));
                DrawBoxAA(b.x - pl.blockR, b.y - pl.blockR, b.x + pl.blockR, b.y + pl.blockR, GetColor(120, 190, 240), TRUE);
                DrawBoxAA(b.x - pl.blockR + 2, b.y - pl.blockR + 2, b.x + pl.blockR - 2, b.y - 1, GetColor(210, 240, 255), TRUE);
            }
            SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
        } else {
            Rgb c = elemColor(pl.hit.elems[0]);
            DrawBoxAA(pl.pos.x - 5, pl.pos.y - 4, pl.pos.x + 5, pl.pos.y + 6, GetColor(70, 70, 90), TRUE);
            DrawTriangleAA(pl.pos.x - 4, pl.pos.y - 4, pl.pos.x + 4, pl.pos.y - 4, pl.pos.x, pl.pos.y - 14, toColor(c), TRUE);
            SetDrawBlendMode(DX_BLENDMODE_ADD, 90);
            DrawCircleAA(pl.pos.x, pl.pos.y, pl.range, 48, toColor(c), FALSE, 1.0f);
            DrawCircleAA(pl.pos.x, pl.pos.y - 12, 4 + std::sin(time_ * 12) * 1.5f, 12, toColor(c), TRUE);
            SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
        }
    }

    // enemies
    // telegraphed hazards (under enemies, above floor effects)
    for (auto& h : w.hazards) {
        float t = std::min(1.0f, h.t / h.delay);
        if (!h.fired) {
            if (h.kind == HazardKind::Circle) {
                SetDrawBlendMode(DX_BLENDMODE_ALPHA, (int)(50 + 70 * t));
                DrawCircleAA(h.pos.x, h.pos.y, h.r, 32, GetColor(255, 60, 60), TRUE);
                SetDrawBlendMode(DX_BLENDMODE_ALPHA, 200);
                DrawCircleAA(h.pos.x, h.pos.y, h.r, 32, GetColor(255, 90, 90), FALSE, 1.5f);
                DrawCircleAA(h.pos.x, h.pos.y, h.r * t, 32, GetColor(255, 160, 120), FALSE, 1.5f);
            } else {
                Vec2 e = h.pos + h.dir * h.len;
                SetDrawBlendMode(DX_BLENDMODE_ALPHA, (int)(40 + 100 * t));
                DrawLineAA(h.pos.x, h.pos.y, e.x, e.y, GetColor(255, 60, 60), h.r * 2 * (0.3f + 0.7f * t));
            }
        } else {
            float a = 1.0f - (h.t - h.delay) / std::max(0.01f, h.linger);
            SetDrawBlendMode(DX_BLENDMODE_ADD, (int)(255 * std::max(0.0f, a)));
            if (h.kind == HazardKind::Circle) DrawCircleAA(h.pos.x, h.pos.y, h.r, 32, GetColor(255, 200, 150), TRUE);
            else {   // fired beam: red body + thin hot core, never a screen-wide white wash
                Vec2 e = h.pos + h.dir * h.len;
                SetDrawBlendMode(DX_BLENDMODE_ADD, (int)(150 * std::max(0.0f, a)));
                DrawLineAA(h.pos.x, h.pos.y, e.x, e.y, GetColor(255, 90, 70), h.r * 2);
                SetDrawBlendMode(DX_BLENDMODE_ADD, (int)(230 * std::max(0.0f, a)));
                DrawLineAA(h.pos.x, h.pos.y, e.x, e.y, GetColor(255, 220, 190), std::max(1.5f, h.r * 0.5f));
            }
        }
        SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
    }
    // actors sorted by depth: enemies above the player first
    const Player& p = w.player;
    for (auto& e : w.enemies) if (e.alive && e.pos.y <= p.pos.y) drawEnemy(e, time_, p.pos);

    // player
    SetDrawBlendMode(DX_BLENDMODE_ALPHA, 100);
    DrawOvalAA(p.pos.x, p.pos.y + 5, 6, 2.5f, 16, GetColor(0, 0, 0), TRUE);
    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
    bool blink = p.invuln > 0 && p.dashT <= 0 && std::fmod(time_ * 20, 2.0f) < 1.0f;
    bool moving = dist2(p.pos, prevPlayerPos_) > 0.05f;
    prevPlayerPos_ = p.pos;
    const SpriteAnim* hero = Sprites::character("wizzard_m", moving);
    if (p.dashT > 0) {
        SetDrawBlendMode(DX_BLENDMODE_ADD, 120);
        for (int i = 1; i <= 3; ++i) {
            Vec2 q = p.pos - p.dashDir * (i * 7.0f);
            if (hero) { SetDrawBright(80, 160, 255); Sprites::drawFeet(hero, 0, q.x, q.y + 6, p.aimDir.x < 0, 1.0f, true); SetDrawBright(255, 255, 255); }
            else DrawCircleAA(q.x, q.y, p.r, 16, GetColor(80, 160, 255), TRUE);
        }
        SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
    }
    if (hero && !blink) {
        int fr = (int)(time_ * (moving ? 12 : 6));
        bool flip = p.aimDir.x < 0;
        float fy = p.pos.y + 6;
        // white outline keeps the hero readable on any background
        for (Vec2 o : {Vec2{1, 0}, Vec2{-1, 0}, Vec2{0, 1}, Vec2{0, -1}})
            Sprites::drawFeet(hero, fr, p.pos.x + o.x, fy + o.y, flip, 1.0f, true);
        if (p.hurtFlash > 0) SetDrawBright(255, 120, 120);
        Sprites::drawFeet(hero, fr, p.pos.x, fy, flip, 1.0f);
        SetDrawBright(255, 255, 255);
        // staff points at the aim
        if (const SpriteAnim* staff = Sprites::get("weapon_red_magic_staff")) {
            Vec2 hand = p.pos + Vec2{flip ? -5.0f : 5.0f, -2};
            Sprites::drawCenter(staff, 0, hand.x + p.aimDir.x * 4, hand.y + p.aimDir.y * 4, false, 0.8f, std::atan2(p.aimDir.y, p.aimDir.x) + PI / 2);
        }
    } else if (!hero && !blink) {
        unsigned body = p.hurtFlash > 0 ? GetColor(255, 120, 120) : GetColor(90, 200, 255);
        DrawCircleAA(p.pos.x, p.pos.y, p.r + 1, 20, GetColor(20, 40, 70), TRUE);
        DrawCircleAA(p.pos.x, p.pos.y, p.r, 20, body, TRUE);
        DrawCircleAA(p.pos.x - 1.5f, p.pos.y - 2, 2.2f, 12, GetColor(220, 245, 255), TRUE);
        Vec2 tip = p.pos + p.aimDir * 11;
        DrawLineAA(p.pos.x + p.aimDir.x * 6, p.pos.y + p.aimDir.y * 6, tip.x, tip.y, GetColor(255, 255, 255), 2.0f);
    }
    if (p.shield > 0) {
        SetDrawBlendMode(DX_BLENDMODE_ADD, 160);
        DrawCircleAA(p.pos.x, p.pos.y, p.r + 5 + std::sin(time_ * 8), 24, GetColor(220, 180, 110), FALSE, 2.0f);
        SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
    }
    for (auto& e : w.enemies) if (e.alive && e.pos.y > p.pos.y) drawEnemy(e, time_, p.pos);

    // player bullets (additive glow)
    for (auto& b : w.bullets) {
        Rgb c = b.hit.basic ? Rgb{170, 200, 255} : elemColor(b.hit.elems[0]);
        SetDrawBlendMode(DX_BLENDMODE_ADD, 110);
        DrawCircleAA(b.pos.x, b.pos.y, b.r * 2.2f, 16, toColor(c), TRUE);
        if (b.hit.elems[1] != Elem::None) DrawCircleAA(b.pos.x, b.pos.y, b.r * 1.6f, 16, toColor(elemColor(b.hit.elems[1])), TRUE);
        SetDrawBlendMode(DX_BLENDMODE_ADD, 255);
        DrawCircleAA(b.pos.x, b.pos.y, b.r, 12, toColor(mix(c, {255, 255, 255}, 0.6f)), TRUE);
        Vec2 tail = b.pos - b.vel.norm() * (b.r * 3);
        DrawLineAA(b.pos.x, b.pos.y, tail.x, tail.y, toColor(c), b.r);
        SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
    }

    // particles (below enemy bullets so threats stay readable)
    for (auto& q : parts_) {
        float t = q.life / q.maxLife;
        if (q.shape == 2) {
            SetDrawBlendMode(DX_BLENDMODE_ADD, (int)(220 * t));
            DrawCircleAA(q.pos.x, q.pos.y, q.size * (1.4f - t * 0.6f), 32, q.color, FALSE, 2.0f);
        } else if (q.shape == 1) {
            SetDrawBlendMode(DX_BLENDMODE_ADD, (int)(255 * t));
            Vec2 tail = q.pos - q.vel * 0.04f;
            DrawLineAA(q.pos.x, q.pos.y, tail.x, tail.y, q.color, q.size * t + 0.5f);
        } else {
            SetDrawBlendMode(DX_BLENDMODE_ADD, (int)(200 * t));
            DrawCircleAA(q.pos.x, q.pos.y, q.size * (0.4f + 0.6f * t), 10, q.color, TRUE);
        }
    }
    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

    // enemy bullets: dark outline + bright core, always on top
    for (auto& b : w.ebullets) {
        DrawCircleAA(b.pos.x, b.pos.y, b.r + 1.5f, 12, GetColor(40, 0, 20), TRUE);
        DrawCircleAA(b.pos.x, b.pos.y, b.r, 12, GetColor(255, 60, 140), TRUE);
        DrawCircleAA(b.pos.x, b.pos.y, b.r * 0.45f, 8, GetColor(255, 220, 240), TRUE);
    }
}

// ---------------------------------------------------------------- card UI (native coords)
int Renderer::relicIcon(const std::string& id) {
    auto it = relicIcons_.find(id);
    if (it != relicIcons_.end()) return it->second;
    std::string path = "data/gfx/relics/" + id + ".png";
    int h = FileRead_size(path.c_str()) > 0 ? LoadGraph(path.c_str()) : -1;
    relicIcons_[id] = h;
    return h;
}

void Renderer::drawIcon(int handle, int cx, int cy, int scale) {
    if (handle < 0) return;
    int s = 16 * scale;
    SetDrawMode(DX_DRAWMODE_NEAREST);
    DrawExtendGraph(cx - s, cy - s, cx + s, cy + s, handle, TRUE);
    SetDrawMode(DX_DRAWMODE_BILINEAR);
}

int Renderer::cardIcon(int def) {
    const auto& all = CardDB::all();
    if (def < 0 || def >= (int)all.size()) return -1;
    if (iconCache_.size() != all.size()) iconCache_.assign(all.size(), -2);
    if (iconCache_[def] != -2) return iconCache_[def];
    int base = def;
    if (all[def].isEvolved)
        for (int i = 0; i < (int)all.size(); ++i) if (all[i].evolveTo == def) base = i;
    std::string path = "data/gfx/cards/" + all[base].id + ".png";
    int h = FileRead_size(path.c_str()) > 0 ? LoadGraph(path.c_str()) : -1;
    iconCache_[def] = h;
    return h;
}

static void drawStar(float cx, float cy, float r, unsigned col) {
    float in = r * 0.45f;
    Vec2 pts[10];
    for (int i = 0; i < 10; ++i) pts[i] = Vec2{cx, cy} + Vec2::fromAngle(-PI / 2 + i * PI / 5) * (i % 2 ? in : r);
    for (int i = 0; i < 10; ++i) {
        Vec2 a = pts[i], b = pts[(i + 1) % 10];
        DrawTriangleAA(cx, cy, a.x, a.y, b.x, b.y, col, TRUE);
    }
}

// draws text that fits `maxW`: big font if it fits, otherwise the 12px font (never fractional scaling)
static void fitText(int x, int y, int maxW, const std::string& s, unsigned col, int big, int tiny, bool centered) {
    int f = big;
    int tw = PStrWidth(s.c_str(), (int)s.size(), f);
    if (tw > maxW) { f = tiny; tw = PStrWidth(s.c_str(), (int)s.size(), f); }
    int dx = centered ? x + (maxW - tw) / 2 : x;
    if (tw > maxW) {
        // two lines: split after "・" (fused names are "reaction・card") or where the width runs out
        std::string a = s, b;
        size_t dot = s.find("・");
        if (dot != std::string::npos) { a = s.substr(0, dot + 3); b = s.substr(dot + 3); }
        while (PStrWidth(a.c_str(), 0, f) > maxW && a.size() > 3) {
            size_t k = a.size();
            while (k > 0 && ((unsigned char)a[k - 1] & 0xC0) == 0x80) --k;
            --k;
            b = a.substr(k) + b;
            a.erase(k);
        }
        int base = f == big ? y : y + 6;
        int lh = GetFontSizeToHandle(f) + 1;
        for (int i = 0; i < 2; ++i) {
            const std::string& ln = i == 0 ? a : b;
            int w = PStrWidth(ln.c_str(), 0, f);
            int lx = centered ? x + (maxW - w) / 2 : x;
            PSetDrawArea(x, base - 8 + i * lh, x + maxW, base - 7 + (i + 1) * lh);
            PDrawString(std::max(x, lx), base - 7 + i * lh, ln.c_str(), col, f, GetColor(8, 6, 16));
            PSetDrawArea(0, 0, 1280, 720);
        }
    } else {
        PDrawString(dx, f == big ? y : y + 6, s.c_str(), col, f, GetColor(8, 6, 16));
    }
}

void Renderer::drawCard(int x, int y, int w, int h, const CardInstance& c, bool selected, bool affordable, int keyLabel) {
    if (Probe::enabled) Probe::add(Probe::Kind::Card, x, y, w, h, c.def >= 0 ? c.displayName() : std::string("(empty)"));
    if (c.def < 0) {
        SetDrawBlendMode(DX_BLENDMODE_ALPHA, 120);
        DrawRoundRectAA((float)x, (float)y, (float)(x + w), (float)(y + h), 8, 8, 8, GetColor(40, 36, 56), TRUE);
        SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
        DrawRoundRectAA((float)x, (float)y, (float)(x + w), (float)(y + h), 8, 8, 8, GetColor(80, 74, 100), FALSE, 1.5f);
        return;
    }
    const CardDef& d = c.d();
    Rgb e0 = elemColor(c.elems[0]);
    Rgb e1 = c.elems[1] != Elem::None ? elemColor(c.elems[1]) : e0;
    // frame (two-tone for fusions)
    Rgb frame = c.canEvolve() ? Rgb{255, 215, 90} : scale(e0, 0.85f);
    DrawRoundRectAA((float)x - 2, (float)y - 2, (float)(x + w + 2), (float)(y + h + 2), 10, 10, 8, toColor(frame), TRUE);
    if (c.elems[1] != Elem::None) {
        PSetDrawArea(x + w / 2, y - 3, x + w + 3, y + h + 3);
        DrawRoundRectAA((float)x - 2, (float)y - 2, (float)(x + w + 2), (float)(y + h + 2), 10, 10, 8, toColor(scale(e1, 0.85f)), TRUE);
        PSetDrawArea(0, 0, 1280, 720);
    }
    DrawRoundRectAA((float)x, (float)y, (float)(x + w), (float)(y + h), 8, 8, 8, GetColor(26, 22, 36), TRUE);
    if (d.isEvolved) DrawRoundRectAA((float)x + 2, (float)y + 2, (float)(x + w - 2), (float)(y + h - 2), 7, 7, 8, GetColor(230, 180, 70), FALSE, 1.5f);
    // art box
    // a name that needs two lines gets a taller name strip (the art box gives up the space)
    std::string nm = c.displayName();
    bool twoLines = PStrWidth(nm.c_str(), 0, fontT_) > w - 8;
    const int top = 24, bottom = twoLines ? 42 : 30;
    int ax = x + 5, ay = y + top, aw = w - 10, ah = h - top - bottom;
    for (int i = 0; i < ah; ++i) {
        float t = (float)i / ah;
        Rgb col = mix(scale(e0, 0.42f), scale(e1, 0.25f), t);
        DrawLine(ax, ay + i, ax + aw, ay + i, toColor(col));
    }
    // the power number gets its own corner of the art box; the icon is placed in the space left over
    // (beside the number, or above it, whichever allows the larger icon) so the two never overlap
    char buf[64];
    std::snprintf(buf, sizeof buf, "%d", (int)std::lround(c.power()));
    int pw = PStrWidth(buf, (int)strlen(buf), fontS_);
    const int plateW = pw + 8, plateH = 25;
    int sSide = std::min(aw - plateW - 2, ah) / 32, sTop = std::min(aw, ah - plateH - 2) / 32;
    float cx = ax + aw / 2.0f, cy = ay + ah / 2.0f;
    int sFit = std::max(sSide, sTop);
    if (sFit >= 1) {
        if (sSide >= sTop) cx = ax + (aw - plateW) / 2.0f;
        else cy = ay + (ah - plateH) / 2.0f;
    }
    unsigned glyph = toColor(mix(e0, {255, 255, 255}, 0.55f));
    int icon = cardIcon(c.def);
    if (icon >= 0) {
        int s = std::max(1, sFit);
        int sz = 32 * s;
        if (d.isEvolved) {
            SetDrawBlendMode(DX_BLENDMODE_ADD, (int)(120 + 60 * std::sin(time_ * 4)));
            DrawCircleAA(cx, cy, sz * 0.55f, 32, GetColor(255, 200, 80), TRUE);
            SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
        }
        SetDrawMode(DX_DRAWMODE_NEAREST);
        DrawExtendGraph((int)cx - sz / 2, (int)cy - sz / 2, (int)cx + sz / 2, (int)cy + sz / 2, icon, TRUE);
        SetDrawMode(DX_DRAWMODE_BILINEAR);
    } else switch (d.form) {
    case Form::Bolt: DrawCircleAA(cx + 6, cy, 7, 16, glyph, TRUE); DrawTriangleAA(cx - 16, cy - 5, cx - 16, cy + 5, cx + 2, cy, glyph, TRUE); break;
    case Form::Area: DrawCircleAA(cx, cy, 12, 24, glyph, FALSE, 2.5f); DrawCircleAA(cx, cy, 5, 16, glyph, TRUE); break;
    case Form::Melee: DrawCircleAA(cx - 4, cy + 4, 14, 24, glyph, FALSE, 3.0f); break;
    case Form::Place: DrawBoxAA(cx - 9, cy - 4, cx + 9, cy + 12, glyph, TRUE); break;
    case Form::Guard: DrawTriangleAA(cx - 11, cy - 10, cx + 11, cy - 10, cx, cy + 13, glyph, TRUE); break;
    }
    // power (bottom-right corner of the art, on an opaque plate)
    DrawRoundRectAA((float)(ax + aw - plateW), (float)(ay + ah - plateH), (float)(ax + aw), (float)(ay + ah), 5, 5, 6, GetColor(14, 10, 24), TRUE);
    PDrawString(ax + aw - pw - 4, ay + ah - 24, buf, GetColor(255, 240, 210), fontS_, GetColor(8, 6, 16));
    // reaction tag (top centre)
    if (c.fused) {
        Rgb rc = reactionColor(c.reaction);
        const char* rn = reactionName(c.reaction);
        int rw = PStrWidth(rn, (int)strlen(rn), fontT_);
        DrawRoundRectAA((float)(x + w / 2 - rw / 2 - 4), (float)(y + 4), (float)(x + w / 2 + rw / 2 + 4), (float)(y + 20), 4, 4, 6, toColor(scale(rc, 0.3f)), TRUE);
        PDrawString(x + w / 2 - rw / 2, y + 6, rn, toColor(rc), fontT_, GetColor(0, 0, 0));
    }
    // cost orb
    unsigned costCol = affordable ? GetColor(80, 130, 255) : GetColor(80, 80, 100);
    // the orb is sized to the 24px digit (which used to poke out of it) and the digit is centred on its ink
    DrawCircleAA((float)x + 14, (float)y + 14, 15, 24, GetColor(8, 8, 24), TRUE);
    DrawCircleAA((float)x + 14, (float)y + 14, 13.5f, 24, costCol, TRUE);
    std::snprintf(buf, sizeof buf, "%d", c.cost());
    int icx = 0, icy = 0;
    PInkCenter(buf, fontS_, icx, icy);
    PDrawString(x + 14 - icx, y + 14 - icy, buf, GetColor(255, 255, 255), fontS_, GetColor(0, 0, 40));
    // rank stars (drawn shapes)
    for (int i = 0; i < c.rank; ++i) {
        drawStar((float)(x + w - 11 - i * 15), (float)(y + 12), 7.5f, GetColor(40, 26, 0));
        drawStar((float)(x + w - 11 - i * 15), (float)(y + 12), 6.0f, GetColor(255, 215, 70));
    }
    // name
    // one size for every card name so the hand reads evenly (12px is the font's native size)
    // fitText puts the first of two lines 7px above y: start 3px below the art box
    fitText(x + 4, y + h - bottom + (twoLines ? 10 : 9), w - 8, nm, d.isEvolved ? GetColor(255, 225, 140) : GetColor(250, 245, 255), fontT_, fontT_, true);
    // mastery bar
    if (d.evolveTo >= 0 && !c.anchored) {
        float m = std::min(1.0f, (float)c.mastery / MASTERY_CAP);
        DrawBox(x + 5, y + h - 5, x + w - 5, y + h - 2, GetColor(16, 14, 26), TRUE);
        DrawBox(x + 5, y + h - 5, x + 5 + (int)((w - 10) * m), y + h - 2, c.canEvolve() ? GetColor(255, 215, 90) : GetColor(150, 140, 210), TRUE);
    }
    if (!affordable) {   // dimmed, but name and numbers must stay readable
        SetDrawBlendMode(DX_BLENDMODE_ALPHA, 100);
        DrawRoundRectAA((float)x, (float)y, (float)(x + w), (float)(y + h), 8, 8, 8, GetColor(0, 0, 0), TRUE);
        SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
        // a grey frame over the element colour, so "cannot pick" reads as grey and not just darker
        DrawRoundRectAA((float)x, (float)y, (float)(x + w), (float)(y + h), 8, 8, 8, GetColor(96, 92, 106), FALSE, 2.5f);
    }
    if (c.canEvolve()) {
        float pulse = 0.5f + 0.5f * std::sin(time_ * 8);
        SetDrawBlendMode(DX_BLENDMODE_ADD, (int)(80 + 120 * pulse));
        DrawRoundRectAA((float)x - 3, (float)y - 3, (float)(x + w + 3), (float)(y + h + 3), 10, 10, 8, GetColor(255, 210, 80), FALSE, 3.0f);
        SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
        if (handCard_) {   // battle: the key to press, above the card so it never covers the art
            std::string evs = usingPad_ ? std::string("B:進化") : bindName(B_EVOLVE) + ":進化";
            const char* ev = evs.c_str();
            int ew = PStrWidth(ev, (int)strlen(ev), fontT_);
            DrawRoundRectAA((float)(x + w / 2 - ew / 2 - 6), (float)(y - 21), (float)(x + w / 2 + ew / 2 + 6), (float)(y - 4), 4, 4, 6, GetColor(255, 210, 80), TRUE);
            PDrawString(x + w / 2 - ew / 2, y - 19, ev, GetColor(40, 22, 0), fontT_, GetColor(255, 240, 190));
        }
    }
    if (selected) DrawRoundRectAA((float)x - 5, (float)y - 5, (float)(x + w + 5), (float)(y + h + 5), 12, 12, 8, GetColor(255, 255, 255), FALSE, 3.0f);
    if (keyLabel > 0 && keyLabel <= 4) {   // the key bound to this slot, under the card
        std::string k = bindName(B_CARD1 + keyLabel - 1);
        int kw = PStrWidth(k.c_str(), (int)k.size(), fontT_);
        PDrawString(x + w / 2 - kw / 2, y + h + 3, k.c_str(), GetColor(170, 160, 200), fontT_, GetColor(0, 0, 0));
    }
}

// ---------------------------------------------------------------- HUD
void Renderer::drawHud(const World& w, bool usingPad) {
    DrawBox(0, 600, 1280, 720, GetColor(16, 14, 26), TRUE);
    DrawLine(0, 600, 1280, 600, GetColor(90, 80, 120));
    const Player& p = w.player;
    char buf[128];
    // HP
    float hpT = std::max(0.0f, p.hp / p.maxHp);
    DrawBox(14, 610, 294, 636, GetColor(50, 20, 24), TRUE);
    DrawBox(14, 610, 14 + (int)(280 * hpT), 636, hpT < 0.3f ? GetColor(255, 60, 60) : GetColor(220, 70, 70), TRUE);
    if (p.shield > 0) DrawBox(14, 610, 14 + (int)(280 * std::min(1.0f, p.shield / p.maxHp)), 616, GetColor(230, 200, 140), TRUE);
    DrawBox(14, 610, 294, 636, GetColor(240, 230, 255), FALSE);
    std::snprintf(buf, sizeof buf, "HP %d/%d", (int)std::ceil(std::max(0.0f, p.hp)), (int)p.maxHp);
    drawText(154, 611, buf, GetColor(255, 255, 255), 0, true);
    // mana pips
    const int pips = (int)w.mods.manaMax;
    // label on the left, pips after it; spacing shrinks so up to 8 pips stay left of the hand (x < 300)
    PDrawString(14, 656, "マナ", GetColor(170, 200, 255), fontT_, GetColor(0, 0, 0));
    const float gapPip = pips > 6 ? 29.0f : 34.0f;
    for (int i = 0; i < pips; ++i) {
        float fill = clampv(w.mana - i, 0.0f, 1.0f);
        float cx = 62.0f + i * gapPip, cy = 662;
        DrawCircleAA(cx, cy, 15, 24, GetColor(20, 24, 50), TRUE);
        if (fill >= 1.0f) {
            DrawCircleAA(cx, cy, 12, 24, GetColor(90, 150, 255), TRUE);
            DrawCircleAA(cx - 3, cy - 4, 4, 12, GetColor(200, 225, 255), TRUE);
        } else if (fill > 0) {
            PSetDrawArea((int)(cx - 13), (int)(cy + 12 - 24 * fill), (int)(cx + 13), (int)(cy + 13));
            DrawCircleAA(cx, cy, 12, 24, GetColor(50, 70, 130), TRUE);
            PSetDrawArea(0, 0, 1280, 720);
        }
        DrawCircleAA(cx, cy, 15, 24, GetColor(130, 150, 230), FALSE, 1.5f);
    }
    std::snprintf(buf, sizeof buf, "山札%d 捨札%d", (int)w.hand.draw.size(), (int)w.hand.discard.size());
    drawText(14, 686, buf, GetColor(180, 170, 210), 0);

    // hand
    const int cw = 148, ch = 100, gap = 26, hx = 310, hy = 600;   // key labels under the cards must stay on screen (y <= 720)
    int pairL = w.pairLeftForCursor();
    for (int i = 0; i < HAND_SIZE; ++i) {
        int x = hx + i * (cw + gap);
        const CardInstance& c = w.hand.slots[i];
        bool sel = (i == w.hand.cursor);
        int y = hy + (sel ? -8 : 0);
        handCard_ = true; usingPad_ = usingPad;
        drawCard(x, y, cw, ch, c, sel, c.def >= 0 && c.cost() <= w.mana + 1e-4f, 0);
        handCard_ = false;
        if (usingPad) {   // pad: X plays the selected card; LB/RB move the selection to the others
            // only the neighbours are labelled: one press of LB/RB moves the selection there
            const char* pk = sel ? (c.def >= 0 ? "X:使う" : "") : (i == w.hand.cursor - 1 ? "LB:選択" : (i == w.hand.cursor + 1 ? "RB:選択" : ""));
            int kw = PStrWidth(pk, 0, fontT_);
            PDrawString(x + cw / 2 - kw / 2, hy + ch + 3, pk, GetColor(190, 180, 220), fontT_, GetColor(0, 0, 0));
        }
        if (!usingPad) {
            std::string k = bindName(B_CARD1 + i);
            int kw = PStrWidth(k.c_str(), 0, fontT_);
            PDrawString(x + cw / 2 - kw / 2, hy + ch + 3, k.c_str(), GetColor(190, 180, 220), fontT_, GetColor(0, 0, 0));
        }
        if (c.def < 0) {
            float t = 1.0f - std::max(0.0f, w.hand.refillT[i]) / REFILL_DELAY;
            DrawBox(x + 14, y + 46, x + 14 + (int)((cw - 28) * t), y + 52, GetColor(120, 110, 160), TRUE);
        }
    }
    for (int l = 0; l + 1 < HAND_SIZE; ++l) {
        int x = hx + l * (cw + gap) + cw + gap / 2;
        int y = hy + 46;
        FusePreview pv = w.previewPair(l);
        bool active = (l == pairL);
        float r = active ? 13.0f : 10.0f;
        if (pv.kind == FuseKind::Invalid) {
            DrawCircleAA((float)x, (float)y, r, 20, GetColor(40, 36, 50), TRUE);
            DrawCircleAA((float)x, (float)y, r, 20, GetColor(80, 74, 100), FALSE, 1.5f);
        } else {
            Rgb c = pv.kind == FuseKind::RankUp ? Rgb{255, 220, 90} : reactionColor(pv.result.reaction);
            DrawCircleAA((float)x, (float)y, r + 2, 20, GetColor(10, 10, 20), TRUE);
            DrawCircleAA((float)x, (float)y, r, 20, toColor(scale(c, active ? 0.85f : 0.45f)), TRUE);
            if (pv.kind == FuseKind::RankUp) drawStar((float)x, (float)y, r * 0.6f, GetColor(255, 255, 255));
            else { DrawBox(x - 5, y - 1, x + 6, y + 2, GetColor(255, 255, 255), TRUE); DrawBox(x - 1, y - 5, x + 2, y + 6, GetColor(255, 255, 255), TRUE); }
            if (active) {
                float pulse = 0.5f + 0.5f * std::sin(time_ * 6);
                SetDrawBlendMode(DX_BLENDMODE_ADD, (int)(100 + 100 * pulse));
                DrawCircleAA((float)x, (float)y, r + 4, 24, toColor(c), FALSE, 2.0f);
                SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
            }
        }
        if (!usingPad) {
            std::string ks = bindName(l == 0 ? B_FUSE12 : (l == 1 ? B_FUSE23 : B_FUSE34));
            const char* k = ks.c_str();
            // on the card-number row under the gap, clear of the cards' names
            PDrawString(x - PStrWidth(k, 0, fontT_) / 2, hy + ch + 3, k, GetColor(255, 200, 110), fontT_, GetColor(0, 0, 0));   // fuse keys: the + colour
        } else if (active) {   // pad fuses the pair at the cursor
            PDrawString(x - PStrWidth("Y:合成", 0, fontT_) / 2, hy + ch + 3, "Y:合成", GetColor(255, 200, 110), fontT_, GetColor(0, 0, 0));
        }
    }
    // fusion preview panel for the cursor pair
    {
        int px = 1004, py = 606, pw = 268, ph = 108;
        DrawRoundRectAA((float)px, (float)py, (float)(px + pw), (float)(py + ph), 8, 8, 8, GetColor(30, 26, 44), TRUE);
        DrawRoundRectAA((float)px, (float)py, (float)(px + pw), (float)(py + ph), 8, 8, 8, GetColor(100, 90, 140), FALSE, 1.5f);
        FusePreview pv = w.previewPair(pairL);
        if (usingPad) std::snprintf(buf, sizeof buf, "Y:合成 %d+%d の結果", pairL + 1, pairL + 2);   // X/B labels sit on the cards
        else if (w.hand.empty(0) && w.hand.empty(1) && w.hand.empty(2) && w.hand.empty(3)) std::snprintf(buf, sizeof buf, "手札");   // nothing to fuse: no fusion heading
        else std::snprintf(buf, sizeof buf, "%s:合成 %d+%d の結果", bindName(pairL == 0 ? B_FUSE12 : (pairL == 1 ? B_FUSE23 : B_FUSE34)).c_str(), pairL + 1, pairL + 2);   // "Z/Q" read as two different fusions
        PDrawString(px + 10, py + 6, buf, GetColor(190, 180, 230), fontT_, GetColor(0, 0, 0));
        if (pv.kind == FuseKind::Invalid) {
            bool l0 = w.hand.empty(pairL), l1 = w.hand.empty(pairL + 1);
            const char* why = pv.reason && *pv.reason ? pv.reason : (w.hand.empty(0) && w.hand.empty(1) && w.hand.empty(2) && w.hand.empty(3) ? "手札を補充中（少し待つ）" : l0 && l1 ? "2枚とも補充中（少し待つ）" : (l0 ? "左のカードを補充中" : "右のカードを補充中"));
            fitText(px + 10, py + 30, pw - 20, why, GetColor(150, 140, 170), fontS_, fontT_, false);
        } else {
            const CardInstance& r = pv.result;
            Rgb c = pv.kind == FuseKind::RankUp ? Rgb{255, 220, 90} : reactionColor(r.reaction);
            fitText(px + 10, py + 22, pw - 20, r.displayName(), toColor(c), fontS_, fontT_, false);
            std::snprintf(buf, sizeof buf, "威力%d コスト%d", (int)std::lround(r.power()), r.cost());
            drawText(px + 10, py + 50, buf, GetColor(235, 235, 245), 0);
            const char* desc = pv.kind == FuseKind::RankUp ? "ランク+1:威力・範囲・数が増える" : reactionDesc(r.reaction);
            PDrawString(px + 10, py + 84, desc, GetColor(200, 195, 225), fontT_, GetColor(0, 0, 0));
        }
    }
    // wave / boss bar
    if (const Enemy* b = w.boss()) {
        float t = std::max(0.0f, b->hp / b->maxHp);
        SetDrawBlendMode(DX_BLENDMODE_ALPHA, 190);
        DrawRoundRectAA(320, 2, 960, 56, 8, 8, 8, GetColor(12, 8, 20), TRUE);   // band: the name must not sit on the wall texture
        SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
        DrawBox(340, 34, 940, 50, GetColor(30, 10, 20), TRUE);
        DrawBox(340, 34, 340 + (int)(600 * t), 50, b->phase ? GetColor(255, 80, 120) : GetColor(220, 60, 90), TRUE);
        DrawBox(340, 34, 940, 50, GetColor(255, 255, 255), FALSE);
        drawText(640, 6, b->d().name.c_str(), GetColor(255, 200, 220), 0, true);
        {
            if (b->d().weak != Elem::None) {
                std::string s = std::string("弱点:") + elemName(b->d().weak);
                drawText(40, 28, s.c_str(), toColor(elemColor(b->d().weak)), 0);
            }
            if (b->d().resist != Elem::None) {
                std::string s = std::string("耐性:") + elemName(b->d().resist);
                drawText(180, 28, s.c_str(), GetColor(170, 170, 200), 0);
            }
            if (b->d().weak == Elem::None && b->d().ai == AiKind::BossGolem) {   // under the bar, on its own plate
                const char* gn = "弱点なし：同じ属性で攻め続けると耐性がつく";
                int gw = PStrWidth(gn, 0, fontS_);
                SetDrawBlendMode(DX_BLENDMODE_ALPHA, 190);
                DrawRoundRectAA((float)(640 - gw / 2 - 12), 58, (float)(640 + gw / 2 + 12), 90, 8, 8, 8, GetColor(12, 8, 20), TRUE);
                SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
                drawText(640, 62, gn, GetColor(220, 190, 255), 0, true);
            }
        }
        if (b->adaptResist != Elem::None) {
            std::string s = std::string(elemName(b->adaptResist)) + "に耐性";
            drawText(956, 30, s.c_str(), toColor(elemColor(b->adaptResist)), 0);
        }
    } else if (w.waveCount > 0) {
        std::snprintf(buf, sizeof buf, "WAVE %d/%d", std::max(1, w.wave), w.waveCount);
        drawText(30, 28, buf, GetColor(230, 220, 255), 0);
        bool elite = false;
        for (auto& e : w.enemies) if (e.alive && e.elite) elite = true;
        if (elite) drawText(30 + PStrWidth(buf, 0, fontS_) + 20, 28, "強敵", GetColor(255, 200, 80), 0);   // gold, like the elite outline
    }
    std::snprintf(buf, sizeof buf, "%d:%02d", w.frame / 3600, (w.frame / 60) % 60);
    drawText(1248 - PStrWidth(buf, 0, fontS_), 28, buf, GetColor(200, 190, 230), 0);   // clear of the right wall
}

// ---------------------------------------------------------------- frame
void Renderer::draw(const World& w, Vec2 mouseL, bool showDebug, bool usingPad) {
    SetDrawScreen(screen_);
    ClearDrawScreen();
    drawWorld(w);
    if (showDebug) {
        for (auto& e : w.enemies) DrawCircleAA(e.pos.x, e.pos.y, e.r, 16, GetColor(0, 255, 0), FALSE);
        DrawCircleAA(w.player.pos.x, w.player.pos.y, w.player.r, 16, GetColor(0, 255, 255), FALSE);
    }
    SetDrawScreen(DX_SCREEN_BACK);   // (DxLib resets the draw area here)
    if (Probe::enabled) Probe::setClip(0, 0, 1280, 720);
    ClearDrawScreen();
    int sx = 0, sy = 0;
    float shk = reduceFx_ ? w.shake * 0.3f : w.shake;
    if (shk > 0) { sx = (int)(fx_.range(-1, 1) * shk * 2); sy = (int)(fx_.range(-1, 1) * shk * 2); }
    SetDrawMode(DX_DRAWMODE_NEAREST);
    DrawExtendGraph(sx, sy, 1280 + sx, 720 + sy, screen_, FALSE);
    SetDrawMode(DX_DRAWMODE_BILINEAR);

    Fx::updateAndDraw({(float)sx, (float)sy});

    // player marker on top of all effects so the hero is never lost
    {
        const Player& p = w.player;
        float px = p.pos.x * 2 + sx, py = p.pos.y * 2 + sy;
        float pulse = 0.5f + 0.5f * std::sin(time_ * 5);
        DrawCircleAA(px, py + 10, 19, 32, GetColor(20, 30, 50), FALSE, 5.0f);
        DrawCircleAA(px, py + 10, 19, 32, GetColor(120, 230, 255), FALSE, 3.0f);
        DrawTriangleAA(px - 6, py - 30 - 3 * pulse, px + 6, py - 30 - 3 * pulse, px, py - 21 - 3 * pulse, GetColor(255, 255, 255), TRUE);
        DrawTriangleAA(px - 4, py - 29 - 3 * pulse, px + 4, py - 29 - 3 * pulse, px, py - 23 - 3 * pulse, GetColor(90, 210, 255), TRUE);
    }

    // floating texts (native)
    for (auto& t : texts_) {
        int a = (int)(255 * std::min(1.0f, t.life * 3));
        SetDrawBlendMode(DX_BLENDMODE_ALPHA, a);
        int f = t.big ? fontL_ : fontM_;
        int tw = PStrWidth(t.text.c_str(), (int)t.text.size(), f);
        // floating combat text moves every frame and is not layout: not probed
        DrawStringToHandle((int)(t.pos.x * 2) - tw / 2 + sx, (int)(t.pos.y * 2) - 16 + sy, toWide(t.text.c_str()).c_str(), t.color, f, GetColor(20, 10, 10));
    }
    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

    // evolution flash + cut-in
    if (w.evolveFlash > 0 && !reduceFx_) {
        float ft = w.evolveFlash / 0.6f;
        SetDrawBlendMode(DX_BLENDMODE_ADD, (int)(80 * ft * ft));
        DrawBox(0, 0, 1280, 600, GetColor(255, 220, 140), TRUE);
        SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
    }
    if (evolveCutT_ > 0 && lastEvolveDef_ >= 0) {
        float t = evolveCutT_;
        float slide = std::min(1.0f, (1.4f - t) / 0.15f);
        float a = std::min(1.0f, t / 0.3f);
        SetDrawBlendMode(DX_BLENDMODE_ALPHA, (int)(200 * a));
        int y0 = 70;
        DrawBox(320, y0, 960, y0 + 50, GetColor(40, 20, 0), TRUE);
        SetDrawBlendMode(DX_BLENDMODE_ADD, (int)(180 * a));
        DrawBox(320, y0, 960, y0 + 3, GetColor(255, 200, 80), TRUE);
        DrawBox(320, y0 + 47, 960, y0 + 50, GetColor(255, 200, 80), TRUE);
        SetDrawBlendMode(DX_BLENDMODE_ALPHA, (int)(255 * a));
        std::string s = std::string("進化！ ") + CardDB::get(lastEvolveDef_).name;
        int tw = PStrWidth(s.c_str(), (int)s.size(), fontL_);
        PDrawString((int)(640 - tw / 2 - 120 * (1 - slide)), y0 + 9, s.c_str(), GetColor(255, 230, 140), fontL_, GetColor(80, 30, 0));
        SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
    }

    drawHud(w, usingPad);

    // banner
    if (bannerT_ > 0) {
        float a = std::min(1.0f, bannerT_ / 0.3f);
        // a dark band behind the banner so spawn markers and effects never show through the letters
        int bw = PStrWidth(banner_.c_str(), 0, fontXL_);
        SetDrawBlendMode(DX_BLENDMODE_ALPHA, (int)(150 * a));
        DrawRoundRectAA((float)(640 - bw / 2 - 30), 220, (float)(640 + bw / 2 + 30), 292, 12, 12, 8, GetColor(8, 6, 16), TRUE);
        // spawn markers over the band (where a new enemy appears stays visible), the letters over the markers
        for (auto& e : w.enemies) {
            if (!e.alive || e.spawnT <= 0) continue;
            float t = 1.0f - e.spawnT / SPAWN_TELEGRAPH;
            float ex = e.pos.x * 2 + sx, ey = e.pos.y * 2 + sy;
            SetDrawBlendMode(DX_BLENDMODE_ALPHA, (int)(120 + 100 * t));
            DrawCircleAA(ex, ey, e.r * 2 * (2.2f - 1.2f * t), 32, GetColor(255, 80, 120), FALSE, 3.0f);
        }
        SetDrawBlendMode(DX_BLENDMODE_ALPHA, (int)(255 * a));
        drawText(640, 230, banner_.c_str(), bannerCol_, 3, true);
        SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
    }
    // reticle
    if (!usingPad) {
        float mx = mouseL.x * 2, my = mouseL.y * 2;
        DrawCircleAA(mx, my, 9, 20, GetColor(255, 255, 255), FALSE, 1.5f);
        DrawLineAA(mx - 14, my, mx - 5, my, GetColor(255, 255, 255), 1.5f);
        DrawLineAA(mx + 5, my, mx + 14, my, GetColor(255, 255, 255), 1.5f);
        DrawLineAA(mx, my - 14, mx, my - 5, GetColor(255, 255, 255), 1.5f);
        DrawLineAA(mx, my + 5, mx, my + 14, GetColor(255, 255, 255), 1.5f);
    }
    if (showDebug) {
        char buf[256];
        std::snprintf(buf, sizeof buf, "F%d hitstop %d enemies %d bullets %d eb %d zones %d parts %d | dmgTaken %.0f fus %d evo %d",
                      w.frame, w.hitstop, (int)w.enemies.size(), (int)w.bullets.size(), (int)w.ebullets.size(),
                      (int)w.zones.size(), (int)parts_.size(), w.stats.damageTaken, w.stats.fusions, w.stats.evolutions);
        drawText(10, 60, buf, GetColor(0, 255, 120), 0);
    }
}
