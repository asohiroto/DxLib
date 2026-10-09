#include "app/App.h"
#include <unordered_map>
#include "app/Probe.h"
#include "core/Paths.h"
#include "core/Log.h"
#include "app/Sound.h"
#include "app/Probe.h"
#include "app/Fx.h"
#include "app/Probe.h"
#include "game/Save.h"
#include "DxLib.h"
#include "EffekseerForDXLib.h"
#include <cstdio>
#include <ctime>
#include <algorithm>
#include <cmath>

const char* g_crashScreen = "boot";
unsigned g_crashSeed = 0;
namespace {
std::string kRunSave() { return Paths::save("run.txt"); }
std::string kProfileSave() { return Paths::save("profile.txt"); }
}
std::string keyName(int k);
std::string difficultyName(int asc) {
    if (asc < 0) return "やさしい";
    if (asc == 0) return "ふつう";
    return "試練" + std::to_string(asc);
}
static const int* g_keys = nullptr;
std::string bindName(int bind) { static const int def[B_COUNT] = {0x11, 0x1F, 0x1E, 0x20, 0x39, 0x02, 0x03, 0x04, 0x05, 0x10, 0x2C, 0x2D, 0x2E, 0x12}; return keyName(g_keys ? g_keys[bind] : def[bind]); }
std::string bindText(const char* s) {
    static const char* tok[B_COUNT] = {"{UP}", "{DOWN}", "{LEFT}", "{RIGHT}", "{DASH}", "{CARD1}", "{CARD2}", "{CARD3}", "{CARD4}", "{FUSE}", "{FUSE12}", "{FUSE23}", "{FUSE34}", "{EVOLVE}"};
    std::string out = s;
    for (int i = 0; i < B_COUNT; ++i) for (size_t p; (p = out.find(tok[i])) != std::string::npos;) out.replace(p, strlen(tok[i]), bindName(i));
    return out;
}
namespace {

// ids shared by menu screens
enum : int {
    ID_NEW = 1, ID_CONTINUE, ID_HUB, ID_OPTIONS, ID_QUIT, ID_TRAINING,
    ID_BACK = 90, ID_START = 91, ID_ASC_DOWN = 92, ID_ASC_UP = 93, ID_RETRY = 94,
    ID_OPT_BGM_DN = 60, ID_OPT_BGM_UP, ID_OPT_SE_DN, ID_OPT_SE_UP, ID_OPT_FULL, ID_OPT_SHAKE, ID_OPT_REDUCE, ID_OPT_NUMBERS, ID_OPT_KEYS, ID_OPT_CREDITS,
    ID_KEY = 200, ID_KEY_RESET = 230,
    ID_UNLOCK = 300,   // + card def
};
}  // namespace

// ---------------------------------------------------------------- setup
bool App::init(const AppOptions& o) {
    opt_ = o;
    std::string text;
    if (!opt_.fresh && Save::readFile(kProfileSave(), text) && !ProfileIO::deserialize(text, profile_)) {
        // never overwrite an unreadable profile silently
        char name[96];
        std::snprintf(name, sizeof name, "profile_broken_%lld.txt", (long long)time(nullptr));
        Paths::rename(kProfileSave(), Paths::save(name));
        toast_ = "進行データを読み込めなかったため、別名で残して最初から始めます";
        toastT_ = 4;
    }
    hasSave_ = !opt_.fresh && Save::readFile(kRunSave(), text);
    Log::write("start %s, save dir %s", ARCANA_VERSION, Paths::saveDir().c_str());
    if (!R.init()) { Log::write("renderer init failed (data folder missing?)"); return false; }
    Sound::init();
    Fx::init();
    applySettings();
    if (opt_.mute) Sound::setMasterVolume(0);
    seedCounter_ = opt_.seed ? opt_.seed : (uint32_t)time(nullptr);
    if (!opt_.autoplay.empty()) runAI_ = std::make_unique<RunPolicy>(RunPolicyConfig(), seedCounter_ * 17u + 3u);
    if (opt_.startScreen == "options") screen_ = Screen::Options;
    else if (opt_.startScreen == "keys") screen_ = Screen::Keys;
    else if (opt_.startScreen == "credits") { screen_ = Screen::Credits; creditsY_ = 300; }
    else if (opt_.startScreen == "continue" && hasSave_) continueRun();   // QA: load the run save exactly as つづきから does
    return true;
}

// DxLib 3.24f copies the current back buffer into the rebuilt one while it switches modes
// (ChangeWindowMode -> Graphics_D3D11_SetupSubBackBuffer -> StretchRect), and that copy could fault in
// d3d11.dll (crash.txt call stack; reproduced 4/4 with --modestress 40). Presenting a freshly cleared
// back buffer right before the switch removed it (0/5 runs, 200 switches). Every mode switch goes through here.
void changeDisplayMode(bool windowed) {
    SetDrawScreen(DX_SCREEN_BACK);
    ClearDrawScreen();
    ScreenFlip();
    SetDrawScreen(DX_SCREEN_BACK);
    ClearDrawScreen();
    ChangeWindowMode(windowed ? TRUE : FALSE);
    SetDrawScreen(DX_SCREEN_BACK);
}

// Switches from the mode the window is really in (not the saved flag), then records what actually happened.
void App::toggleFullscreen() {
    bool nowFull = GetWindowModeFlag() == FALSE;
    changeDisplayMode(nowFull);
    profile_.settings.fullscreen = GetWindowModeFlag() == FALSE;
    saveProfile();
    cursorShown_ = -1;   // re-apply the cursor state after the device reset
}

// The OS cursor is hidden only while actually fighting (the game draws its own reticle there);
// every menu, pause screen and result shows it. Decided once per frame from the screen state.
void App::updateCursor() {
    bool fighting = (screen_ == Screen::Run && run_ && run_->phase == Phase::Battle && world_ && !paused_ && world_->result() == 0) ||
                    (screen_ == Screen::Tutorial && !paused_);
    // applied every frame on purpose: a display-mode switch or Alt+Tab can reset DxLib's cursor state
    cursorShown_ = fighting ? 0 : 1;
    SetMouseDispFlag(cursorShown_ ? TRUE : FALSE);
}

void App::shutdown() {
    saveProfile();
    Log::write("exit ok (%d frames, %d runs finished)", totalFrames_, runsFinished_);
}

void App::applySettings() {
    Sound::setMasterVolume(opt_.mute ? 0.0f : profile_.settings.se);
    Music::setVolume(opt_.mute ? 0.0f : profile_.settings.bgm);
    R.setFxOptions(profile_.settings.reduceFx, profile_.settings.damageNumbers);
    in.setBindings(profile_.settings.keys);
    g_keys = profile_.settings.keys;
}

std::string keyName(int k) {
    static const struct { int code; const char* name; } names[] = {
        {0x02, "1"}, {0x03, "2"}, {0x04, "3"}, {0x05, "4"}, {0x06, "5"}, {0x07, "6"}, {0x08, "7"}, {0x09, "8"}, {0x0A, "9"}, {0x0B, "0"},
        {0x10, "Q"}, {0x11, "W"}, {0x12, "E"}, {0x13, "R"}, {0x14, "T"}, {0x15, "Y"}, {0x16, "U"}, {0x17, "I"}, {0x18, "O"}, {0x19, "P"},
        {0x1E, "A"}, {0x1F, "S"}, {0x20, "D"}, {0x21, "F"}, {0x22, "G"}, {0x23, "H"}, {0x24, "J"}, {0x25, "K"}, {0x26, "L"},
        {0x2C, "Z"}, {0x2D, "X"}, {0x2E, "C"}, {0x2F, "V"}, {0x30, "B"}, {0x31, "N"}, {0x32, "M"},
        {0x39, "Space"}, {0x2A, "LShift"}, {0x36, "RShift"}, {0x1D, "LCtrl"}, {0x9D, "RCtrl"}, {0x0F, "Tab"}, {0x38, "LAlt"},
        {0xC8, "Up"}, {0xD0, "Down"}, {0xCB, "Left"}, {0xCD, "Right"}, {0x1C, "Enter"}, {0x0E, "BackSpace"},
    };
    for (auto& n : names) if (n.code == k) return n.name;
    char buf[16];
    std::snprintf(buf, sizeof buf, "#%02X", k);
    return buf;
}

void App::hint(int bit, const char* text) {
    if (profile_.hintsSeen & (1u << bit)) return;
    profile_.hintsSeen |= 1u << bit;
    hintText_ = text;
    hintT_ = 7.0f;
    saveProfile();
}

// background music follows the current screen
void App::updateMusic() {
    std::string want;
    switch (screen_) {
    case Screen::Title: case Screen::Options: want = "title"; break;
    case Screen::Tutorial: want = "battle1"; break;
    case Screen::Hub: want = "hub"; break;
    case Screen::Result: want = lastVictory_ ? "victory" : "defeat"; break;
    case Screen::Run:
        if (!run_) break;
        if (run_->phase == Phase::Battle) {
            bool boss = run_->node >= 0 && run_->map.nodes[run_->node].type == NodeType::Boss;
            want = boss ? "boss" : ("battle" + std::to_string(std::clamp(run_->chapter, 1, 3)));
        } else {
            want = "map";
        }
        break;
    }
    if (screen_ == Screen::Options && optionsBack_ == Screen::Run) return;   // keep the run music
    Music::play(want, screen_ != Screen::Result);
    Music::update();
}

void App::saveProfile() {
    if (opt_.fresh && !opt_.autoplay.empty()) return;   // automated test runs never touch the player's profile
    Save::writeFile(kProfileSave(), ProfileIO::serialize(profile_));
}

void App::saveRun() {
    if (!run_ || (opt_.fresh && !opt_.autoplay.empty())) return;
    Save::writeFile(kRunSave(), Save::serializeRun(*run_));
    hasSave_ = true;
}

// ---------------------------------------------------------------- frame
bool App::frame() {
    in.poll();
#ifndef NDEBUG
    if (in.pressed(KEY_INPUT_F1)) debug_ = !debug_;   // collision overlay (development builds only)
#endif
    // display-mode switches happen here, before anything is drawn this frame (the options button only requests one)
    if (in.pressed(KEY_INPUT_F11)) fullscreenPending_ = true;
    if (fullscreenPending_) { fullscreenPending_ = false; toggleFullscreen(); }
    Sound::tick();
    updateMusic();
    if (toastT_ > 0) toastT_ -= DT;

    // new screen: reset focus and swallow inputs that were meant for the previous one
    {
        int key = (int)screen_ * 1000 + (run_ ? (int)run_->phase * 50 : 0) + (int)pick_ + (paused_ ? 7 : 0) + (confirmNew_ ? 3 : 0);
        if (key != lastUiKey_) { lastUiKey_ = key; ui.setFocus(-1); ui.lockFor(18); }
    }
    if (!opt_.autoplay.empty()) autopilotMenus();

    {
        static const char* names[] = {"title", "hub", "options", "run", "result", "tutorial", "keys", "credits"};
        g_crashScreen = (int)screen_ < 8 ? names[(int)screen_] : "?";
        if (screen_ == Screen::Run && run_) g_crashScreen = phaseName(run_->phase);
        g_crashSeed = run_ ? run_->seed : 0;
    }
    switch (screen_) {
    case Screen::Title: doTitle(); break;
    case Screen::Hub: doHub(); break;
    case Screen::Options: doOptions(); break;
    case Screen::Result: doResult(); break;
    case Screen::Run: doRun(); break;
    case Screen::Tutorial: doTutorial(); break;
    case Screen::Keys: doKeys(); break;
    case Screen::Credits: doCredits(); break;
    }
    if (screen_ == Screen::Run && run_) {
        switch (run_->phase) {
        case Phase::Map: hint(0, "強敵を倒すとレリックが手に入る。鍛冶場では2枚を永続的に1枚へ合成できる"); break;
        case Phase::Battle: hint(1, "手札の間の印に合成結果が出る。Z/X/Cで隣の2枚を合成！"); break;
        case Phase::Reward: hint(2, "カードは1枚選ぶかスキップ。デッキを絞るのも戦略だ"); break;
        case Phase::Shop: hint(3, "カード強化やカード削除でデッキを鍛えよう"); break;
        case Phase::Forge: hint(4, "定着合成：2枚を永続的に1枚へ。ランで3回まで"); break;
        case Phase::Rest: hint(5, "休んで回復するか、修練で進化に近づけるか"); break;
        default: break;
        }
    }
    if (hintT_ > 0) {
        hintT_ -= DT;
        if (in.confirm() || in.mousePressed(MOUSE_INPUT_LEFT)) hintT_ = std::min(hintT_, 0.3f);
        SetDrawBlendMode(DX_BLENDMODE_ALPHA, (int)(235 * std::min(1.0f, hintT_ * 3)));
        // battle: above the hand; menus: a slim strip under every button so it never hides one.
        // Text that does not fit on one line drops to the small font and wraps to two lines.
        bool battle = screen_ == Screen::Run && run_ && run_->phase == Phase::Battle;
        int y = battle ? 58 : 684, hgt = battle ? 44 : 30;   // menus: 6px above the bottom edge
        const int bx = 140, bw = 1000;
        if (Probe::enabled) Probe::setOverlay(true);
        if (Probe::enabled) Probe::add(Probe::Kind::Panel, bx, y, bw, hgt, "hint");
        DrawRoundRectAA((float)bx, (float)y, (float)(bx + bw), (float)(y + hgt), 8, 8, 8, GetColor(40, 30, 10), TRUE);
        DrawRoundRectAA((float)bx, (float)y, (float)(bx + bw), (float)(y + hgt), 8, 8, 8, GetColor(255, 210, 120), FALSE, 2.0f);
        std::string ht = "ヒント：" + hintText_;
        int tw = PStrWidth(ht.c_str(), (int)ht.size(), R.fontS());
        if (tw <= bw - 24) R.drawText(640, y + (hgt - 24) / 2, ht.c_str(), GetColor(255, 235, 190), 0, true);
        else wrapText(bx + 12, y + (hgt - 26) / 2, bw - 24, ht, GetColor(255, 235, 190), R.fontT(), 13);
        if (Probe::enabled) Probe::setOverlay(false);
        SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
    }
    if (toastT_ > 0) {
        SetDrawBlendMode(DX_BLENDMODE_ALPHA, (int)(255 * std::min(1.0f, toastT_ * 2)));
        // a notice over everything (by design); below the tutorial / battle banners. Long text wraps.
        bool playing = screen_ == Screen::Tutorial || (screen_ == Screen::Run && run_ && run_->phase == Phase::Battle);
        // menus: above the title logo / headings; battle: under the banners
        int ty = playing ? 150 : (screen_ == Screen::Title ? 12 : 84);
        int tw = PStrWidth(toast_.c_str(), (int)toast_.size(), R.fontM());
        bool wrap = tw > 1100;
        int lines = wrap ? (tw + 1099) / 1100 : 1;
        int lineW = wrap ? tw / lines + 40 : tw;   // balanced lines: no single orphaned word
        int boxW = std::max(400, lineW + 60);
        int boxH = wrap ? 20 + lines * 30 : 48;
        int bx = 640 - boxW / 2;
        if (Probe::enabled) { Probe::setOverlay(true); Probe::add(Probe::Kind::Panel, bx, ty, boxW, boxH, "toast"); }
        DrawRoundRectAA((float)bx, (float)ty, (float)(bx + boxW), (float)(ty + boxH), 10, 10, 8, GetColor(30, 22, 48), TRUE);
        DrawRoundRectAA((float)bx, (float)ty, (float)(bx + boxW), (float)(ty + boxH), 10, 10, 8, GetColor(255, 210, 120), FALSE, 2.0f);
        if (!wrap) R.drawText(640, ty + 10, toast_.c_str(), GetColor(255, 230, 160), 1, true);
        else wrapText(bx + 30, ty + 10, lineW, toast_, GetColor(255, 230, 160), R.fontS(), 30);
        if (Probe::enabled) Probe::setOverlay(false);
        SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
    }    updateCursor();
    // capture each distinct screen once (review / regression material)
    if (opt_.screenShots) {
        int key = (int)screen_ * 1000 + (run_ ? (int)run_->phase * 50 : 0) + (int)pick_;
        if (key != lastScreenKey_) { lastScreenKey_ = key; screenShotT_ = run_ && run_->phase == Phase::Battle ? 240 : 12; }
        if (screenShotT_ > 0 && --screenShotT_ == 0) {
            Paths::makeDir(opt_.shotDir);
            char path[256];
            std::snprintf(path, sizeof path, "%s/screen_%03d_k%d.png", opt_.shotDir.c_str(), screenShotN_++, key);
            SaveDrawScreenToPNG(0, 0, 1280, 720, path);
        }
    }
    ++totalFrames_;
    if ((opt_.shotInterval > 0 && totalFrames_ >= opt_.shotFrom && totalFrames_ % opt_.shotInterval == 0) || in.pressed(KEY_INPUT_F12)) {
        Paths::makeDir(opt_.shotDir);
        char path[256];
        std::snprintf(path, sizeof path, "%s/shot_%06d.png", opt_.shotDir.c_str(), totalFrames_);
        SaveDrawScreenToPNG(0, 0, 1280, 720, path);
    }
    if (opt_.maxFrames > 0 && totalFrames_ >= opt_.maxFrames) return false;
    if (opt_.maxRuns > 0 && runsFinished_ >= opt_.maxRuns && screen_ == Screen::Title) return false;
    return !quit_;
}

// ---------------------------------------------------------------- drawing helpers
void App::drawBackground(int variant) {
    unsigned top = variant == 1 ? GetColor(30, 22, 40) : GetColor(18, 16, 30);
    unsigned bot = variant == 1 ? GetColor(12, 10, 20) : GetColor(8, 8, 16);
    for (int y = 0; y < 720; y += 4) {
        float t = y / 720.0f;
        int r = (int)(((top >> 16) & 255) * (1 - t) + ((bot >> 16) & 255) * t);
        int g = (int)(((top >> 8) & 255) * (1 - t) + ((bot >> 8) & 255) * t);
        int b = (int)((top & 255) * (1 - t) + (bot & 255) * t);
        DrawBox(0, y, 1280, y + 4, GetColor(r, g, b), TRUE);
    }
    // drifting motes
    float tt = ui.pulse();
    SetDrawBlendMode(DX_BLENDMODE_ADD, 70);
    for (int i = 0; i < 40; ++i) {
        float x = std::fmod(i * 97.0f + tt * (8 + i % 5), 1280.0f);
        float y = 720 - std::fmod(i * 53.0f + tt * (12 + i % 7) * 2, 760.0f);
        DrawCircleAA(x, y, 1.5f + (i % 3), 8, GetColor(140, 120, 255), TRUE);
    }
    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

// The same white focus outline cards use, for any focusable panel (relics etc.).
void App::focusFrame(int x, int y, int w, int h) {
    DrawRoundRectAA((float)x - 5, (float)y - 5, (float)(x + w + 5), (float)(y + h + 5), 12, 12, 8, GetColor(255, 255, 255), FALSE, 3.0f);
}

void App::panel(int x, int y, int w, int h, unsigned border) {
    if (Probe::enabled) Probe::add(Probe::Kind::Panel, x, y, w, h);
    SetDrawBlendMode(DX_BLENDMODE_ALPHA, 220);
    DrawRoundRectAA((float)x, (float)y, (float)(x + w), (float)(y + h), 10, 10, 8, GetColor(24, 20, 38), TRUE);
    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
    DrawRoundRectAA((float)x, (float)y, (float)(x + w), (float)(y + h), 10, 10, 8, border, FALSE, 2.0f);
}

void App::button(int id, int x, int y, int w, int h, const char* label, bool enabled, unsigned textColor) {
    ui.add(id, (float)x, (float)y, (float)w, (float)h, enabled);
    if (Probe::enabled) Probe::add(Probe::Kind::Button, x, y, w, h, label);
    bool f = ui.focused(id);
    unsigned fill = !enabled ? GetColor(26, 24, 32) : (f ? GetColor(90, 70, 150) : GetColor(46, 40, 70));
    unsigned border = !enabled ? GetColor(58, 54, 66) : (f ? GetColor(255, 230, 150) : GetColor(120, 110, 170));
    DrawRoundRectAA((float)x, (float)y, (float)(x + w), (float)(y + h), 8, 8, 8, fill, TRUE);
    DrawRoundRectAA((float)x, (float)y, (float)(x + w), (float)(y + h), 8, 8, 8, border, FALSE, f ? 3.0f : 1.5f);
    unsigned tc = textColor ? textColor : (enabled ? GetColor(245, 240, 255) : GetColor(188, 182, 200));   // disabled: a darker box tells it apart, the text stays readable
    int fw = PStrWidth(label, (int)strlen(label), R.fontM());
    // edge in the fill colour: a black 2px edge filled in dense kanji (薬 read as 票)
    PDrawString(x + (w - fw) / 2, y + (h - 20) / 2, label, tc, R.fontM(), fill);
}

namespace {
// UTF-8 code points with their byte ranges
struct Cp { unsigned u; size_t at, len; };
std::vector<Cp> codepoints(const std::string& s) {
    std::vector<Cp> out;
    for (size_t i = 0; i < s.size();) {
        unsigned char c = (unsigned char)s[i];
        size_t len = c < 0x80 ? 1 : (c < 0xE0 ? 2 : (c < 0xF0 ? 3 : 4));
        unsigned u = len == 1 ? c : (c & (0xFF >> (len + 1)));
        for (size_t k = 1; k < len && i + k < s.size(); ++k) u = (u << 6) | ((unsigned char)s[i + k] & 0x3F);
        out.push_back({u, i, len});
        i += len;
    }
    return out;
}
enum class Cls { Hira, Kata, Kanji, Num, Latin, Other };
Cls cls(unsigned u) {
    if (u >= 0x3041 && u <= 0x309F) return Cls::Hira;
    if ((u >= 0x30A0 && u <= 0x30FF) || u == 0xFF0B) return Cls::Kata;   // incl. ー and "+" of "ファイアボール+"
    if ((u >= 0x4E00 && u <= 0x9FFF) || u == 0x3005) return Cls::Kanji;
    if ((u >= '0' && u <= '9') || u == '+' || u == '-' || u == '%' || u == '.' || u == '/') return Cls::Num;
    if ((u >= 'A' && u <= 'Z') || (u >= 'a' && u <= 'z')) return Cls::Latin;
    return Cls::Other;
}
bool noStart(unsigned u) {   // never begins a line
    static const unsigned list[] = {0x3001, 0x3002, 0xFF0C, 0xFF0E, 0xFF09, 0x300D, 0x300F, 0x3011, 0xFF01, 0xFF1F, 0x30FC, 0x3063,
                                    0x3083, 0x3085, 0x3087, 0x30C3, 0x30E3, 0x30E5, 0x30E7, 0x30A1, 0x30A3, 0x30A5, 0x30A7, 0x30A9,
                                    0x30FB, 0xFF1A, 0x2026, ')', '!', '?', ',', '.', '%', ':'};
    for (unsigned v : list) if (u == v) return true;
    return false;
}
bool noEnd(unsigned u) { return u == 0xFF08 || u == 0x300C || u == 0x300E || u == 0x3010 || u == '('; }
bool breakAfter(unsigned u) {   // punctuation after which a break reads naturally
    return u == 0x3001 || u == 0x3002 || u == 0xFF01 || u == 0xFF1F || u == 0x300D || u == 0xFF09 || u == 0x300F ||
           u == 0x30FB || u == 0xFF1A || u == ' ' || u == 0x3000 || u == ')' || u == ':' || u == '!' || u == '?' || u == '/';
}
// a natural break between a and b: after punctuation, or after a particle/okurigana (hiragana) before a new word
bool goodBreak(unsigned a, unsigned b) {
    if (noStart(b) || noEnd(a)) return false;
    if (breakAfter(a)) return true;
    Cls ca = cls(a), cb = cls(b);
    if (ca == Cls::Hira && (cb == Cls::Kanji || cb == Cls::Kata || cb == Cls::Num || noEnd(b))) return true;
    if (noEnd(b) && ca != Cls::Other) return true;
    return false;
}
std::vector<std::string> breakParagraph(const std::string& s, int w, int font) {
    std::vector<std::string> lines;
    auto cps = codepoints(s);
    auto width = [&](size_t a, size_t b) {   // [a, b) in code points
        if (b <= a) return 0;
        std::string sub = s.substr(cps[a].at, (b < cps.size() ? cps[b].at : s.size()) - cps[a].at);
        return PStrWidth(sub.c_str(), (int)sub.size(), font);
    };
    size_t a = 0;
    while (a < cps.size()) {
        size_t b = a + 1;
        while (b < cps.size() && width(a, b + 1) <= w) ++b;
        if (b >= cps.size()) { lines.push_back(s.substr(cps[a].at)); break; }
        // b = first code point that does not fit. Prefer the last natural break in the second half of the line.
        size_t k = 0;
        for (size_t j = b; j > a + std::max<size_t>(1, (b - a) / 3); --j)
            if (goodBreak(cps[j - 1].u, cps[j].u)) { k = j; break; }
        if (!k) {
            k = b;
            // kinsoku: pull the previous character down rather than start a line with closing punctuation
            while (k > a + 1 && noStart(cps[k].u)) --k;
            while (k > a + 1 && noEnd(cps[k - 1].u)) --k;
            // keep "+25%" style numbers and katakana words whole when the line can spare them
            Cls ck = cls(cps[k].u);
            if ((ck == Cls::Num || ck == Cls::Kata || ck == Cls::Latin) && cls(cps[k - 1].u) == ck) {
                size_t m = k;
                while (m > a + 1 && cls(cps[m - 1].u) == ck) --m;
                if (m > a + (b - a) / 2) k = m;
            }
            // a particle never starts a line ("ダメージ／を防ぐ"): take the word before it down with it
            if (cls(cps[k].u) == Cls::Hira && cls(cps[k - 1].u) != Cls::Hira && cls(cps[k - 1].u) != Cls::Other) {
                Cls cw = cls(cps[k - 1].u);
                size_t m = k;
                while (m > a + 1 && cls(cps[m - 1].u) == cw) --m;
                if (m > a + (b - a) / 3) k = m;
            }
        }
        lines.push_back(s.substr(cps[a].at, cps[k].at - cps[a].at));
        a = k;
    }
    return lines;
}
}  // namespace

// Lines of s wrapped to w: natural Japanese break points, kinsoku, and no one-or-two-character orphan last line
// (the width is narrowed until the lines are even, keeping the same line count). Cached: called every frame.
std::vector<std::string> App::wrapLines(const std::string& s, int w, int font) {
    static std::unordered_map<std::string, std::vector<std::string>> cache;
    std::string key = s + '\x1f' + std::to_string(w) + '\x1f' + std::to_string(font);
    auto it = cache.find(key);
    if (it != cache.end()) return it->second;
    if (cache.size() > 1024) cache.clear();
    std::vector<std::string> out;
    size_t start = 0;
    while (start <= s.size()) {
        size_t nl = s.find('\n', start);
        std::string para = s.substr(start, nl == std::string::npos ? std::string::npos : nl - start);
        auto lines = breakParagraph(para, w, font);
        if (lines.size() >= 2) {
            int lastW = PStrWidth(lines.back().c_str(), (int)lines.back().size(), font);
            if (lastW < w * 3 / 10) {
                // narrow only as far as needed: stop at the first width whose last line is no longer an orphan
                for (int w2 = w - w / 25; w2 > w / 2; w2 -= std::max(4, w / 25)) {
                    auto l2 = breakParagraph(para, w2, font);
                    if (l2.size() != lines.size()) break;
                    lines = l2;
                    if (PStrWidth(lines.back().c_str(), (int)lines.back().size(), font) >= w2 * 3 / 10) break;
                }
            }
        }
        if (lines.empty()) lines.push_back("");
        out.insert(out.end(), lines.begin(), lines.end());
        if (nl == std::string::npos) break;
        start = nl + 1;
    }
    cache.emplace(key, out);
    return out;
}

void App::wrapText(int x, int y, int w, const std::string& s, unsigned col, int font, int lineH) {
    lineH = std::max(lineH, GetFontSizeToHandle(font) + 3);
    int cy = y;
    for (const auto& ln : wrapLines(s, w, font)) {
        if (!ln.empty()) PDrawString(x, cy, ln.c_str(), col, font, GetColor(10, 8, 20));
        cy += lineH;
    }
}
std::string App::cardDesc(const CardInstance& c) const {
    std::string s = c.d().desc;
    if (c.fused) { s += "\n【"; s += reactionName(c.reaction); s += "】"; s += reactionDesc(c.reaction); }
    if (c.anchored) s += "\n定着（進化・合成不可）";
    return s;
}

// A big card tile = card (160x150) + description panel + optional footer (price). Its full extent is
// (x, y, kTileW, cardTileH(footer)); callers use the same rectangle as the clickable area. The panel is wider
// than the card so descriptions get 8 characters a line (at 5 they broke inside words: 押し/返す).
int App::cardTileH(bool footer) { return footer ? 330 : 300; }

void App::drawCardBig(int x, int y, const CardInstance& c, bool focus, const char* footer, bool affordable, bool sold) {
    if (Probe::enabled) Probe::add(Probe::Kind::Button, x, y, kTileW, cardTileH(footer != nullptr), "tile");
    R.drawCard(x + (kTileW - 160) / 2, y, 160, 150, c, focus, affordable && !sold, 0);   // sold: the same grey as "cannot buy"
    panel(x, y + 158, kTileW, 138, focus ? GetColor(255, 230, 150) : GetColor(90, 80, 130));
    wrapText(x + 10, y + 164, kTileW - 20, cardDesc(c), sold ? GetColor(150, 145, 165) : GetColor(225, 220, 240), R.fontS(), 26);   // sold: dimmed text too
    if (!c.anchored && c.d().evolveTo >= 0)
    {
        // two short lines (the label and the name) so long names never get cut
        PDrawString(x + 10, y + 260, "進化先", GetColor(200, 170, 110), R.fontT(), GetColor(0, 0, 0));
        PDrawString(x + 10, y + 276, CardDB::get(c.d().evolveTo).name.c_str(), GetColor(255, 215, 120), R.fontT(), GetColor(0, 0, 0));
    }
    if (footer) R.drawText(x + kTileW / 2, y + 302, footer, sold ? GetColor(175, 170, 190) : (affordable ? GetColor(255, 220, 120) : GetColor(255, 140, 140)), 1, true);
}

void App::drawTopBar() {
    if (!run_) return;
    DrawBox(0, 0, 1280, 44, GetColor(14, 12, 24), TRUE);
    DrawLine(0, 44, 1280, 44, GetColor(90, 80, 120));
    char buf[128];
    // items laid out by their measured width (full-width digits made fixed positions overlap)
    int tx = 16;
    auto item = [&](const char* s, unsigned col) {
        R.drawText(tx, 10, s, col, 1);
        tx += PStrWidth(s, 0, R.fontM()) + 28;
    };
    std::snprintf(buf, sizeof buf, "第%d章", run_->chapter);
    item(buf, GetColor(230, 220, 255));
    std::snprintf(buf, sizeof buf, "HP %d/%d", (int)std::ceil(run_->hp), (int)run_->maxHp);
    item(buf, GetColor(255, 140, 140));
    std::snprintf(buf, sizeof buf, "%dG", run_->gold);
    item(buf, GetColor(255, 220, 100));
    std::snprintf(buf, sizeof buf, "デッキ %d枚", (int)run_->deck.size());
    item(buf, GetColor(200, 200, 230));
    if (run_->ascension != 0) item(difficultyName(run_->ascension).c_str(), GetColor(255, 120, 120));    // relic chips (after the text; the rest is summarised as "+N" so nothing runs off screen)
    int x = std::max(tx, 600);
    int fit = std::max(0, (1180 - x) / 34);   // leaves room for "+N"
    int shown = 0;
    for (int idx : run_->relics) {
        if (shown == fit && (int)run_->relics.size() > fit) {
            std::string more = "+" + std::to_string((int)run_->relics.size() - fit);
            R.drawText(x + 2, 10, more.c_str(), GetColor(200, 190, 230), 1);
            break;
        }
        ++shown;
        const RelicDef& d = RunLogic::relics()[idx];
        unsigned c = d.rarity == 'r' ? GetColor(255, 200, 90) : (d.rarity == 'u' ? GetColor(150, 200, 255) : GetColor(190, 190, 200));
        DrawCircleAA((float)x + 12, 22, 14, 20, GetColor(30, 26, 44), TRUE);
        DrawCircleAA((float)x + 12, 22, 14, 20, c, FALSE, 2.0f);
        int ic = R.relicIcon(d.id);
        if (ic >= 0) R.drawIcon(ic, x + 12, 22, 1);
        else PDrawString(x + 5, 14, d.name.substr(0, 3).c_str(), c, R.fontT(), GetColor(0, 0, 0));
        Vec2 m = in.mouseLogical() * 2.0f;
        if (m.x >= x && m.x <= x + 24 && m.y >= 8 && m.y <= 36) {
            panel(x - 100, 50, 300, 64, c);
            R.drawText(x - 90, 56, d.name.c_str(), c, 1);
            R.drawText(x - 90, 82, d.desc.c_str(), GetColor(220, 220, 235), 0);
        }
        x += 34;
    }
}

// ---------------------------------------------------------------- title
void App::doTitle() {
    ClearDrawScreen();
    drawBackground(1);
    ui.begin();
    {   // logo rings: decoration only, drawn faint and behind the buttons
        float p = ui.pulse();
        SetDrawBlendMode(DX_BLENDMODE_ADD, 70);
        DrawCircleAA(640, 200, 150 + 6 * std::sin(p * 1.5f), 64, GetColor(120, 80, 220), FALSE, 3.0f);
        DrawCircleAA(640, 200, 120 - 4 * std::sin(p * 1.3f), 64, GetColor(220, 160, 80), FALSE, 2.0f);
        SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
    }
    int bx = 540, by = 326;   // 6 buttons end at y=650, above the two help lines
    if (hasSave_) { button(ID_CONTINUE, bx, by, 200, 44, "つづきから"); by += 56; }
    button(ID_NEW, bx, by, 200, 44, "はじめから");
    R.drawText(bx + 214, by + 16, ("難易度：" + difficultyName(profile_.ascension) + "（拠点で変更）").c_str(), GetColor(185, 175, 215), 0);
    by += 56;
    button(ID_HUB, bx, by, 200, 44, "拠点"); by += 56;
    button(ID_TRAINING, bx, by, 200, 44, "訓練場"); by += 56;
    button(ID_OPTIONS, bx, by, 200, 44, "オプション"); by += 56;
    button(ID_QUIT, bx, by, 200, 44, "終了");
    int act = confirmNew_ ? -1 : ui.end(in);   // the dialog below is modal: the title buttons must not react
    // logo
    R.drawText(640, 150, "アルカナフォージ", GetColor(255, 230, 170), 3, true);
    R.drawText(640, 220, "ARCANA FORGE", GetColor(190, 170, 255), 2, true);
    R.drawText(1266 - PStrWidth(ARCANA_VERSION, (int)strlen(ARCANA_VERSION), R.fontT()), 702, ARCANA_VERSION, GetColor(170, 160, 200), -1);
    {
        const int* k = profile_.settings.keys;
        std::string l1 = keyName(k[B_UP]) + keyName(k[B_LEFT]) + keyName(k[B_DOWN]) + keyName(k[B_RIGHT]) + ":移動  マウス:照準  左クリック:攻撃  " + keyName(k[B_DASH]) + ":回避";
        std::string l2 = keyName(k[B_CARD1]) + "〜" + keyName(k[B_CARD4]) + "・右クリック:カード  " + keyName(k[B_FUSE12]) + "・" + keyName(k[B_FUSE23]) + "・" + keyName(k[B_FUSE34]) + "・" + keyName(k[B_FUSE]) + ":合成  " + keyName(k[B_EVOLVE]) + ":進化";
        R.drawText(640, 660, l1.c_str(), GetColor(150, 140, 180), 0, true);
        R.drawText(640, 688, l2.c_str(), GetColor(150, 140, 180), 0, true);
    }
    if (confirmNew_) {
        // confirmation dialog drawn on top
        ui.begin();
        if (Probe::enabled) Probe::setOverlay(true);
        SetDrawBlendMode(DX_BLENDMODE_ALPHA, 225);
        DrawBox(0, 0, 1280, 720, GetColor(8, 6, 16), TRUE);
        SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
        DrawRoundRectAA(240, 250, 1040, 460, 10, 10, 8, GetColor(24, 20, 38), TRUE);   // opaque: nothing shows through
        panel(240, 250, 800, 210, GetColor(255, 160, 120));
        R.drawText(640, 286, "進行中の冒険があります。破棄して新しく始めますか？", GetColor(255, 230, 200), 1, true);
        if (ui.focus() != 70 && ui.focus() != 71) ui.setFocus(71);   // destructive choice is never the default
        button(70, 390, 380, 240, 48, "破棄して始める");
        button(71, 650, 380, 240, 48, "やめる");
        bool cancel = false;
        int a2 = ui.end(in, &cancel);
        if (Probe::enabled) Probe::setOverlay(false);
        if (a2 == 70) { confirmNew_ = false; startRun(); }
        else if (a2 == 71 || cancel) confirmNew_ = false;
        return;
    }
    if (act == ID_NEW) {
        if (hasSave_) confirmNew_ = true;
        else if (!profile_.tutorialDone) startTutorial();   // first launch: learn the basics first
        else startRun();
    }
    else if (act == ID_TRAINING) startTutorial();
    else if (act == ID_CONTINUE) continueRun();
    else if (act == ID_HUB) screen_ = Screen::Hub;
    else if (act == ID_OPTIONS) { optionsBack_ = Screen::Title; screen_ = Screen::Options; }
    else if (act == ID_QUIT) quit_ = true;
}

void App::startRun() {
    Paths::remove(kRunSave()); Paths::remove(kRunSave() + ".bak");
    hasSave_ = false;
    uint32_t seed = seedCounter_ * 2654435761u + 12345u;
    ++seedCounter_;
    run_ = std::make_unique<RunState>(RunLogic::newRun(seed, profile_.ascension, profile_.cardPool()));
    screen_ = Screen::Run;
    pick_ = Pick::None;
    saveRun();
}

void App::continueRun() {
    std::string text;
    RunState r;
    if (Save::readFile(kRunSave(), text) && Save::deserializeRun(text, r)) {
        run_ = std::make_unique<RunState>(r);
        screen_ = Screen::Run;
        pick_ = Pick::None;
    } else {
        // broken save: keep a copy and start over
        {
            char name[64];
            std::snprintf(name, sizeof name, "run_broken_%lld.txt", (long long)time(nullptr));
            Paths::rename(kRunSave(), Paths::save(name));
            Paths::remove(kRunSave() + ".bak");
        }
        hasSave_ = false;
        toast_ = "セーブデータを読み込めませんでした";
        toastT_ = 3;
    }
}

// ---------------------------------------------------------------- hub (meta progression)
void App::doHub() {
    ClearDrawScreen();
    drawBackground(0);
    ui.begin();
    R.drawText(640, 30, "拠点：魂の工房", GetColor(255, 230, 170), 2, true);
    char buf[128];
    std::snprintf(buf, sizeof buf, "魂片 %d", profile_.shards);
    R.drawText(640, 80, buf, GetColor(190, 160, 255), 1, true);
    std::snprintf(buf, sizeof buf, "挑戦 %d回 / 踏破 %d回", profile_.runs, profile_.wins);
    R.drawText(640, 108, buf, GetColor(170, 170, 200), 0, true);

    // locked cards: one row of 10 (wraps if more are ever added), a fixed description panel under it
    R.drawText(80, 150, "カードの解放（カードの下の数字が必要な魂片）", GetColor(230, 220, 255), 1);
    {   // what the numbers on a card are, without having to focus one
        const char* lg = "左上：コスト　右下：威力";
        R.drawText(1200 - PStrWidth(lg, 0, R.fontS()), 152, lg, GetColor(180, 170, 210), 0);
    }
    const auto& all = CardDB::all();
    std::vector<int> lockedDefs;
    for (int i = 0; i < (int)all.size(); ++i) if (all[i].locked) lockedDefs.push_back(i);
    const int cw = 100, chh = 110, gap = 12, perRow = 10;
    const int rowW = perRow * cw + (perRow - 1) * gap;
    const int x0 = (1280 - rowW) / 2, y0 = 185, rowH = chh + 40;
    int rows = ((int)lockedDefs.size() + perRow - 1) / perRow;
    int focusedDef = -1;
    for (int k = 0; k < (int)lockedDefs.size(); ++k) {
        int i = lockedDefs[k];
        int x = x0 + (k % perRow) * (cw + gap), y = y0 + (k / perRow) * rowH;
        bool owned = std::find(profile_.unlocked.begin(), profile_.unlocked.end(), all[i].id) != profile_.unlocked.end();
        int cost = Profile::unlockCost(all[i]);
        CardInstance c = makeCard(i, 0);
        bool afford = profile_.shards >= cost;
        // always focusable (the explanation lives in the panel); dimmed only when it cannot be bought
        ui.add(ID_UNLOCK + i, (float)x, (float)y, (float)cw, (float)chh, true);
        R.drawCard(x, y, cw, chh, c, ui.focused(ID_UNLOCK + i), owned || afford, 0);
        std::snprintf(buf, sizeof buf, owned ? "解放済" : "%d", cost);   // the unit is in the header ("魂片")
        R.drawText(x + cw / 2, y + chh + 6, buf, owned ? GetColor(120, 220, 140) : (afford ? GetColor(200, 170, 255) : GetColor(150, 120, 140)), 0, true);
        if (ui.focused(ID_UNLOCK + i)) focusedDef = i;
    }
    int py = y0 + rows * rowH + 6;
    panel(80, py, 1120, 76, GetColor(150, 130, 220));
    if (focusedDef >= 0) {
        const CardDef& fd = all[focusedDef];
        bool owned = std::find(profile_.unlocked.begin(), profile_.unlocked.end(), fd.id) != profile_.unlocked.end();
        int cost = Profile::unlockCost(fd);
        R.drawText(96, py + 8, fd.name.c_str(), GetColor(255, 230, 170), 1);
        {   // what the two numbers on the card are
            CardInstance fc = makeCard(focusedDef, 0);
            char pb[64];
            std::snprintf(pb, sizeof pb, "威力%d  コスト%d", (int)std::lround(fc.power()), fc.cost());
            R.drawText(96 + PStrWidth(fd.name.c_str(), 0, R.fontM()) + 24, py + 10, pb, GetColor(190, 180, 220), 0);
        }
        R.drawText(96, py + 42, fd.desc.c_str(), GetColor(220, 220, 235), 0);
        std::string st = owned ? std::string("解放済：報酬や商店に並びます")
                       : profile_.shards >= cost ? "決定（クリック / Enter）で解放：" + std::to_string(cost) + "魂片"
                                                 : "魂片が足りません（あと" + std::to_string(cost - profile_.shards) + "）";
        R.drawText(1184 - PStrWidth(st.c_str(), 0, R.fontS()), py + 8, st.c_str(), owned ? GetColor(120, 220, 140) : (profile_.shards >= cost ? GetColor(255, 220, 120) : GetColor(220, 150, 150)), 0);
    } else {
        R.drawText(96, py + 26, "カードを選ぶと説明が出ます。解放したカードは報酬や商店に並びます", GetColor(160, 150, 190), 0);
    }
    // difficulty
    int ay = py + 100;
    R.drawText(80, ay, "難易度", GetColor(230, 220, 255), 1);
    button(ID_ASC_DOWN, 80, ay + 34, 50, 44, "<", profile_.ascension > -1);   // a lone arrow read as a puzzle: both stay, the end one disabled
    std::string dl = difficultyName(profile_.ascension);
    R.drawText(205, ay + 44, dl.c_str(), profile_.ascension < 0 ? GetColor(170, 230, 170) : GetColor(255, 200, 200), 1, true);
    button(ID_ASC_UP, 280, ay + 34, 50, 44, ">", profile_.ascension < profile_.ascensionUnlocked);
    static const char* ascDesc[] = {"敵の攻撃力が30%、HPが20%下がる（初めての方に）", "標準の難易度", "試練1：敵のHPが10%上がる", "試練2：試練1＋強敵のマスが増える",
                                    "試練3：試練2＋敵の攻撃力が15%上がる", "試練4：試練3＋回復量が減る", "試練5：試練4＋最大HPが8減る"};
    R.drawText(350, ay + 44, ascDesc[std::clamp(profile_.ascension, -1, 5) + 1], GetColor(200, 190, 220), 0);
    std::string note = profile_.ascension < 0 ? std::string("「やさしい」で踏破しても試練は解放されません")
                     : profile_.ascensionUnlocked >= 5 ? std::string("すべての試練が解放されています")
                     : std::string(profile_.ascensionUnlocked >= 1 ? "試練" + std::to_string(profile_.ascensionUnlocked) + "まで解放済み。" : "") + "「" + difficultyName(profile_.ascensionUnlocked) + "」を踏破すると「試練" + std::to_string(profile_.ascensionUnlocked + 1) + "」が解放されます";
    R.drawText(80, ay + 90, note.c_str(), GetColor(170, 160, 200), 0);
    button(ID_START, 900, 600, 220, 50, "冒険に出る");
    button(ID_BACK, 80, 640, 160, 44, "戻る");
    bool cancel = false;
    int act = ui.end(in, &cancel);
    if (act >= ID_UNLOCK) {
        int d = act - ID_UNLOCK;
        int cost = Profile::unlockCost(all[d]);
        bool owned = std::find(profile_.unlocked.begin(), profile_.unlocked.end(), all[d].id) != profile_.unlocked.end();
        if (owned) {
            Sound::play(Sfx::NoMana, 0.5f);
        } else if (profile_.shards < cost) {
            Sound::play(Sfx::NoMana, 0.5f);   // the panel already says how many are missing
        } else {
            profile_.shards -= cost;
            profile_.unlocked.push_back(all[d].id);
            saveProfile();
            Sound::play(Sfx::Evolve, 0.6f);
        }
    } else if (act == ID_ASC_DOWN) profile_.ascension--;
    else if (act == ID_ASC_UP) profile_.ascension++;
    else if (act == ID_START) { if (hasSave_) { confirmNew_ = true; screen_ = Screen::Title; } else startRun(); }
    else if (act == ID_BACK || cancel) { saveProfile(); screen_ = Screen::Title; }
}

// ---------------------------------------------------------------- options
void App::doOptions() {
    ClearDrawScreen();
    drawBackground(0);
    ui.begin();
    R.drawText(640, 80, "オプション", GetColor(255, 230, 170), 2, true);
    Settings& s = profile_.settings;
    char buf[64];
    int y = 180;
    R.drawText(380, y + 10, "効果音", GetColor(230, 220, 255), 1);
    button(ID_OPT_SE_DN, 590, y, 50, 44, "<", s.se > 0.01f);
    std::snprintf(buf, sizeof buf, "%d", (int)std::lround(s.se * 10));
    R.drawText(690, y + 10, buf, GetColor(255, 255, 255), 1, true);
    button(ID_OPT_SE_UP, 740, y, 50, 44, ">", s.se < 0.99f);
    y += 70;
    R.drawText(380, y + 10, "BGM", GetColor(230, 220, 255), 1);
    button(ID_OPT_BGM_DN, 590, y, 50, 44, "<", s.bgm > 0.01f);
    std::snprintf(buf, sizeof buf, "%d", (int)std::lround(s.bgm * 10));
    R.drawText(690, y + 10, buf, GetColor(255, 255, 255), 1, true);
    button(ID_OPT_BGM_UP, 740, y, 50, 44, ">", s.bgm < 0.99f);
    y += 70;
    R.drawText(380, y + 10, "画面モード", GetColor(230, 220, 255), 1);
    button(ID_OPT_FULL, 590, y, 240, 44, s.fullscreen ? "フルスクリーン" : "ウィンドウ");
    y += 70;
    R.drawText(380, y + 10, "画面の揺れ", GetColor(230, 220, 255), 1);
    button(ID_OPT_SHAKE, 590, y, 240, 44, s.shake ? "あり" : "なし");
    y += 70;
    R.drawText(380, y + 10, "演出の軽減", GetColor(230, 220, 255), 1);
    button(ID_OPT_REDUCE, 590, y, 240, 44, s.reduceFx ? "軽減する" : "通常");
    y += 70;
    R.drawText(380, y + 10, "ダメージ数字", GetColor(230, 220, 255), 1);
    button(ID_OPT_NUMBERS, 590, y, 240, 44, s.damageNumbers ? "表示" : "非表示");
    button(ID_OPT_KEYS, 880, 180, 200, 44, "キー設定");
    button(ID_OPT_CREDITS, 880, 290, 200, 44, "クレジット");
    button(ID_BACK, 540, 640, 200, 48, "戻る");
    R.drawText(880, 236, "F11で画面モードを切替", GetColor(170, 160, 200), 0);
    bool cancel = false;
    int act = ui.end(in, &cancel);
    if (act == ID_OPT_SE_DN) s.se = std::max(0.0f, s.se - 0.1f);
    else if (act == ID_OPT_SE_UP) s.se = std::min(1.0f, s.se + 0.1f);
    else if (act == ID_OPT_BGM_DN) s.bgm = std::max(0.0f, s.bgm - 0.1f);
    else if (act == ID_OPT_BGM_UP) s.bgm = std::min(1.0f, s.bgm + 0.1f);
    else if (act == ID_OPT_FULL) fullscreenPending_ = true;   // applied at the start of the next frame
    else if (act == ID_OPT_SHAKE) s.shake = !s.shake;
    else if (act == ID_OPT_REDUCE) s.reduceFx = !s.reduceFx;
    else if (act == ID_OPT_NUMBERS) s.damageNumbers = !s.damageNumbers;
    else if (act == ID_OPT_KEYS) { screen_ = Screen::Keys; rebind_ = -1; }
    else if (act == ID_OPT_CREDITS) { screen_ = Screen::Credits; creditsY_ = 0; }
    else if (act == ID_BACK || cancel) { saveProfile(); screen_ = optionsBack_; if (screen_ == Screen::Run) paused_ = true; }
    if (act >= 0) applySettings();
}

// ---------------------------------------------------------------- key bindings
void App::doKeys() {
    ClearDrawScreen();
    drawBackground(0);
    ui.begin();
    R.drawText(640, 40, "キー設定", GetColor(255, 230, 170), 2, true);
    static const char* labels[B_COUNT] = {"上へ移動", "下へ移動", "左へ移動", "右へ移動", "回避", "カード1", "カード2", "カード3", "カード4",
                                          "選択ペアを合成", "合成（1と2）", "合成（2と3）", "合成（3と4）", "進化"};
    Settings& s = profile_.settings;
    if (rebind_ >= 0) ui.setFocus(ID_KEY + rebind_);   // the item waiting for a key is the highlighted one
    for (int i = 0; i < B_COUNT; ++i) {
        int col = i < 7 ? 0 : 1, row = i < 7 ? i : i - 7;
        int x = 160 + col * 500, y = 124 + row * 60;   // clear of the guidance line (y=84..108)
        R.drawText(x, y + 10, labels[i], GetColor(230, 220, 255), 0);
        std::string k = rebind_ == i ? "入力待ち" : keyName(s.keys[i]);
        button(ID_KEY + i, x + 230, y, 240, 46, k.c_str());
    }
    button(ID_KEY_RESET, 380, 560, 220, 48, "初期設定に戻す");
    button(ID_BACK, 680, 560, 220, 48, "戻る");
    R.drawText(640, 640, "マウス：照準／左クリックで攻撃／右クリックで選択中のカードを使う（変更不可）", GetColor(170, 160, 200), 0, true);
    R.drawText(640, 84, rebind_ >= 0 ? "新しいキーを押してください（Escで取り消し）" : "変えたい操作を選んで決定し、新しいキーを押します", GetColor(200, 190, 230), 0, true);
    if (rebind_ >= 0) {
        int k = in.firstPressedKey();
        if (k > 0) {
            for (int i = 0; i < B_COUNT; ++i) if (i != rebind_ && s.keys[i] == k) s.keys[i] = s.keys[rebind_];   // swap duplicates
            s.keys[rebind_] = k;
            rebind_ = -1;
            applySettings();
            saveProfile();
            ui.lockFor(10);
        } else if (in.pressed(KEY_INPUT_ESCAPE)) rebind_ = -1;
        ui.end(in);
        return;
    }
    bool cancel = false;
    int act = ui.end(in, &cancel);
    if (act >= ID_KEY && act < ID_KEY + B_COUNT) rebind_ = act - ID_KEY;
    else if (act == ID_KEY_RESET) { s = Settings{s.bgm, s.se, s.fullscreen, s.shake, s.reduceFx, s.damageNumbers}; applySettings(); saveProfile(); }
    else if (act == ID_BACK || cancel) { saveProfile(); screen_ = Screen::Options; }
}

// ---------------------------------------------------------------- credits
void App::doCredits() {
    static std::vector<std::string> lines;
    if (lines.empty()) {
        std::string text;
        if (Save::readFile("data/credits.txt", text)) {
            size_t s = 0;
            while (s <= text.size()) {
                size_t e = text.find('\n', s);
                if (e == std::string::npos) e = text.size();
                std::string l = text.substr(s, e - s);
                if (!l.empty() && l.back() == '\r') l.pop_back();
                lines.push_back(l);
                s = e + 1;
            }
        }
    }
    ClearDrawScreen();
    drawBackground(0);
    creditsY_ += in.held(KEY_INPUT_DOWN) || in.held(KEY_INPUT_S) ? 4.0f : 0.6f;
    if (in.held(KEY_INPUT_UP) || in.held(KEY_INPUT_W)) creditsY_ -= 4.0f;
    int y = 720 - (int)creditsY_;
    for (auto& l : lines) {
        if (!l.empty() && l[0] == '#') { y += 18; if (l.size() > 1) R.drawText(640, y, l.substr(1).c_str(), GetColor(255, 220, 150), 2, true); y += 48; }
        else if (PStrWidth(l.c_str(), 0, l.rfind("  ", 0) == 0 ? R.fontS() : R.fontM()) > 820) {
            // too wide for the area left of the back button: wrap, centred block
            bool isSub = l.rfind("  ", 0) == 0;
            int f = isSub ? R.fontS() : R.fontM();
            std::string rest = isSub ? l.substr(2) : l;
            // balanced lines (no lone "CC0" / "License" at the end), breaking at spaces and "/" first
            for (const auto& ln : wrapLines(rest, 820, f)) {
                R.drawText(640, y, ln.c_str(), isSub ? GetColor(195, 185, 225) : GetColor(235, 225, 255), isSub ? 0 : 1, true);
                y += isSub ? 30 : 42;
            }
        }
        else { if (l.rfind("  ", 0) != 0 && &l != &lines.front() && (&l)[-1].rfind("  ", 0) == 0) y += 14;
            R.drawText(640, y, l.c_str(), l.rfind("  ", 0) == 0 ? GetColor(195, 185, 225) : GetColor(235, 225, 255), l.rfind("  ", 0) == 0 ? 0 : 1, true); y += l.rfind("  ", 0) == 0 ? 30 : 42; }
    }
    creditsY_ = std::max(0.0f, creditsY_);
    // the roll stops with the last line above the 戻る button, so the end is visible and clearly the end
    if (y < 660) creditsY_ -= (float)(660 - y);
    ui.begin();
    button(ID_BACK, 1080, 660, 180, 44, "戻る");
    bool cancel = false;
    int act = ui.end(in, &cancel);
    if (act == ID_BACK || cancel) screen_ = Screen::Options;
}

// ---------------------------------------------------------------- result
void App::endRun(bool victory) {
    lastVictory_ = victory;
    lastRun_ = *run_;
    lastShards_ = Profile::shardsFor(*run_, victory);
    profile_.recordRun(*run_, victory);
    saveProfile();
    Paths::remove(kRunSave()); Paths::remove(kRunSave() + ".bak");
    hasSave_ = false;
    run_.reset();
    world_.reset();
    screen_ = Screen::Result;
    ++runsFinished_;
}

void App::doResult() {
    ClearDrawScreen();
    drawBackground(lastVictory_ ? 1 : 0);
    ui.begin();
    R.drawText(640, 70, lastVictory_ ? "踏破！" : "力尽きた", lastVictory_ ? GetColor(255, 220, 120) : GetColor(255, 120, 120), 3, true);
    const RunStats& s = lastRun_.stats;
    char buf[160];
    panel(340, 170, 600, 330, GetColor(140, 120, 200));
    int y = 190;
    auto line = [&](const char* k, const std::string& v) {
        R.drawText(380, y, k, GetColor(200, 190, 230), 1);
        // values right-aligned inside the panel (it ends at x=940)
        R.drawText(910 - PStrWidth(v.c_str(), (int)v.size(), R.fontM()), y, v.c_str(), GetColor(255, 255, 255), 1);
        y += 34;
    };
    std::snprintf(buf, sizeof buf, "第%d章", lastRun_.chapter);
    line("到達", buf);
    line("撃破した敵", std::to_string(s.kills));
    line("倒したボス", std::to_string(s.bossesKilled));
    line("合成した回数", std::to_string(s.fusions + s.rankUps));
    line("進化させた回数", std::to_string(s.evolutions));
    int reacts = 0;
    for (int i = 1; i < (int)Reaction::Count; ++i) if (s.reactions & (1u << i)) ++reacts;
    std::snprintf(buf, sizeof buf, "%d / 8", reacts);
    line("発見した反応", buf);
    std::snprintf(buf, sizeof buf, "%d:%02d", (int)s.battleSeconds / 60, (int)s.battleSeconds % 60);
    line("戦闘時間", buf);
    std::snprintf(buf, sizeof buf, "+%d魂片", lastShards_);
    R.drawText(640, 520, buf, GetColor(200, 170, 255), 2, true);
    button(ID_RETRY, 400, 600, 220, 50, "もう一度挑戦");
    button(ID_BACK, 660, 600, 220, 50, "タイトルへ");
    bool cancel = false;
    int act = ui.end(in, &cancel);
    if (act == ID_BACK || cancel) screen_ = Screen::Title;
    else if (act == ID_RETRY) startRun();
}
