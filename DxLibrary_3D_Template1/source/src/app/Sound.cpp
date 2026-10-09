#include "app/Sound.h"
#include "DxLib.h"
#include <cmath>
#include <functional>
#include <string>
#include <algorithm>
#include "core/Log.h"
#include "core/Vec2.h"

namespace {
int g_handles[(int)Sfx::Count];
float g_master = 0.7f;
int g_lastPlayFrame[(int)Sfx::Count];
int g_frameCounter = 0;
unsigned g_noise = 12345;

float noise() {
    g_noise = g_noise * 1664525u + 1013904223u;
    return ((g_noise >> 9) & 0x7FFF) / 16384.0f - 1.0f;
}

// Build a sound from a sample generator f(t, len) -> [-1,1]
int synth(float seconds, const std::function<float(float, float)>& f) {
    const int rate = 44100;
    int n = (int)(seconds * rate);
    int h = MakeSoftSound1Ch16Bit44KHz(n);
    for (int i = 0; i < n; ++i) {
        float t = (float)i / rate;
        float v = f(t, seconds);
        v = v < -1 ? -1 : (v > 1 ? 1 : v);
        WriteSoftSoundData(h, i, (int)(v * 32000), 0);
    }
    int snd = LoadSoundMemFromSoftSound(h);
    DeleteSoftSound(h);
    return snd;
}

float env(float t, float len, float attack = 0.005f) {
    float a = t < attack ? t / attack : 1.0f;
    float d = 1.0f - t / len;
    return a * d * d;
}
float sq(float ph) { return std::fmod(ph, 1.0f) < 0.5f ? 1.0f : -1.0f; }
float tri(float ph) { float x = std::fmod(ph, 1.0f); return x < 0.5f ? 4 * x - 1 : 3 - 4 * x; }

int loadOr(const char* name, const std::function<int()>& gen) {
    std::string p = std::string("data/sfx/") + name + ".wav";
    int h = LoadSoundMem(p.c_str());
    if (h != -1) return h;
    Log::write("sfx: %s missing, using generated tone", p.c_str());
    return gen();
}
}  // namespace

namespace Sound {
void init() {
    using F = std::function<int()>;
    auto& H = g_handles;
    H[(int)Sfx::Shot] = loadOr("shot", [] { return synth(0.07f, [](float t, float L) { return sq(t * (900 - t * 6000)) * env(t, L) * 0.25f; }); });
    H[(int)Sfx::Hit] = loadOr("hit", [] { return synth(0.09f, [](float t, float L) { return (noise() * 0.6f + sq(t * 180) * 0.4f) * env(t, L) * 0.45f; }); });
    H[(int)Sfx::Kill] = loadOr("kill", [] { return synth(0.25f, [](float t, float L) { return (noise() * 0.5f + tri(t * (300 - t * 800)) * 0.5f) * env(t, L) * 0.55f; }); });
    H[(int)Sfx::Card] = loadOr("card", [] { return synth(0.18f, [](float t, float L) { return (tri(t * (500 + t * 2500)) * 0.6f + noise() * 0.2f) * env(t, L) * 0.4f; }); });
    H[(int)Sfx::Fuse] = loadOr("fuse", [] { return synth(0.45f, [](float t, float L) {
        float f = t < 0.12f ? 523 : (t < 0.24f ? 659 : 784);
        return (tri(t * f) * 0.6f + tri(t * f * 2) * 0.2f) * env(t, L, 0.01f) * 0.5f; }); });
    H[(int)Sfx::Evolve] = loadOr("evolve", [] { return synth(1.0f, [](float t, float L) {
        float v = tri(t * 523) + tri(t * 659) + tri(t * 784) + tri(t * 1046) * 0.5f;
        return v * 0.18f * env(t, L, 0.05f) + noise() * 0.1f * env(t, 0.3f); }); });
    H[(int)Sfx::Hurt] = loadOr("hurt", [] { return synth(0.22f, [](float t, float L) { return (sq(t * (220 - t * 500)) * 0.5f + noise() * 0.4f) * env(t, L) * 0.5f; }); });
    H[(int)Sfx::Dash] = loadOr("dash", [] { return synth(0.15f, [](float t, float L) { return noise() * env(t, L, 0.03f) * 0.3f; }); });
    H[(int)Sfx::Explode] = loadOr("explode", [] { return synth(0.4f, [](float t, float L) {
        static float lp = 0; float n = noise(); lp = lp * 0.85f + n * 0.15f;
        return (lp * 1.5f + sq(t * (90 - t * 100)) * 0.3f) * env(t, L) * 0.7f; }); });
    H[(int)Sfx::Freeze] = loadOr("freeze", [] { return synth(0.3f, [](float t, float L) { return tri(t * 1800 + std::sin(t * 90) * 4) * env(t, L) * 0.3f; }); });
    H[(int)Sfx::Shatter] = loadOr("shatter", [] { return synth(0.35f, [](float t, float L) { return (noise() * 0.6f + tri(t * 2400) * 0.4f) * env(t, L) * 0.6f; }); });
    H[(int)Sfx::NoMana] = loadOr("nomana", [] { return synth(0.12f, [](float t, float L) { return sq(t * 140) * env(t, L) * 0.3f; }); });
    H[(int)Sfx::Clear] = loadOr("clear", [] { return synth(0.9f, [](float t, float L) {
        float f = t < 0.15f ? 523 : (t < 0.3f ? 659 : (t < 0.45f ? 784 : 1046));
        return tri(t * f) * 0.4f * env(t, L, 0.01f); }); });
    H[(int)Sfx::Select] = loadOr("select", [] { return synth(0.05f, [](float t, float L) { return sq(t * 1200) * env(t, L) * 0.2f; }); });
    auto blip = [] { return synth(0.08f, [](float t, float L) { return sq(t * 900) * env(t, L) * 0.25f; }); };
    H[(int)Sfx::Confirm] = loadOr("confirm", blip);
    H[(int)Sfx::Cancel] = loadOr("cancel", blip);
    H[(int)Sfx::Coin] = loadOr("coin", blip);
    H[(int)Sfx::BossHit] = loadOr("bosshit", blip);
    H[(int)Sfx::Death] = loadOr("death", blip);
    H[(int)Sfx::Fanfare] = loadOr("fanfare", blip);
    H[(int)Sfx::Warn] = loadOr("warn", blip);
    H[(int)Sfx::Shield] = loadOr("shield", blip);
    H[(int)Sfx::Heal] = loadOr("heal", blip);
    H[(int)Sfx::Open] = loadOr("open", blip);
    H[(int)Sfx::BossPhase] = loadOr("bossphase", blip);
    H[(int)Sfx::Spawn] = loadOr("spawn", blip);
    (void)sizeof(F);
    for (auto& f : g_lastPlayFrame) f = -100;
}

void tick() { ++g_frameCounter; }

void play(Sfx s, float vol) {
    int i = (int)s;
    int h = g_handles[i];
    if (h <= 0) return;
    // avoid stacking the same sound many times in the same instant
    if (g_frameCounter - g_lastPlayFrame[i] < 3 && (s == Sfx::Hit || s == Sfx::Shot || s == Sfx::Spawn || s == Sfx::Kill || s == Sfx::Coin)) return;
    g_lastPlayFrame[i] = g_frameCounter;
    ChangeNextPlayVolumeSoundMem((int)(255 * g_master * vol), h);
    PlaySoundMem(h, DX_PLAYTYPE_BACK);
}

void setMasterVolume(float v) { g_master = v; }
}  // namespace Sound

namespace {
struct Track { std::string name; int handle = -1; float vol = 0; bool fadingOut = false; };
Track g_cur, g_old;
float g_musicVol = 0.6f;
std::string g_pending;
bool g_pendingLoop = true;

void applyVol(Track& t) { if (t.handle >= 0) ChangeVolumeSoundMem((int)(255 * g_musicVol * t.vol), t.handle); }
}  // namespace

namespace Music {
void play(const std::string& name, bool loop) {
    if (name == g_cur.name) return;
    if (g_old.handle >= 0) { StopSoundMem(g_old.handle); DeleteSoundMem(g_old.handle); }
    g_old = g_cur;
    g_old.fadingOut = true;
    g_cur = Track();
    g_cur.name = name;
    std::string path = "data/bgm/" + name + ".mp3";
    SetCreateSoundDataType(DX_SOUNDDATATYPE_FILE);   // stream from disk
    g_cur.handle = LoadSoundMem(path.c_str());
    SetCreateSoundDataType(DX_SOUNDDATATYPE_MEMNOPRESS);
    if (g_cur.handle < 0 && !name.empty()) Log::write("music: failed to load %s", path.c_str());
    if (g_cur.handle >= 0) {
        g_cur.vol = 0.0f;
        applyVol(g_cur);
        PlaySoundMem(g_cur.handle, loop ? DX_PLAYTYPE_LOOP : DX_PLAYTYPE_BACK);
    }
}
void stop() { play(""); }
void setVolume(float v) { g_musicVol = v; applyVol(g_cur); applyVol(g_old); }
void update() {
    if (g_cur.handle >= 0 && g_cur.vol < 1.0f) { g_cur.vol = std::min(1.0f, g_cur.vol + DT / 0.8f); applyVol(g_cur); }
    if (g_old.handle >= 0) {
        g_old.vol -= DT / 0.6f;
        if (g_old.vol <= 0) { StopSoundMem(g_old.handle); DeleteSoundMem(g_old.handle); g_old = Track(); }
        else applyVol(g_old);
    }
}
}  // namespace Music
