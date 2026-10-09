// --gallery: QA mode that puts the game into every screen / state through the normal screen code,
// captures each at 1280x720, and checks the layout probe (overlaps, overflow, hit boxes), the OS cursor
// visibility and the mouse coordinate mapping. Writes <shotdir>/gallery_report.txt.
#include "app/App.h"
#include "app/Fx.h"
#include "app/Probe.h"
#include "core/Paths.h"
#include "core/Log.h"
#include "DxLib.h"
#include <cstdio>
#include <functional>
#include <set>
#include <windows.h>

namespace {
// ids of App.cpp / RunScreens.cpp (kept in sync by hand; the gallery fails loudly if a focus id is missing)
constexpr int G_ID_UNLOCK = 300, G_ID_NODE = 100, G_ID_CARD = 400, G_ID_PICK = 1000;

void centerCursor() {
    HWND hw = GetMainWindowHandle();
    if (IsIconic(hw)) ShowWindow(hw, SW_RESTORE);
    SetForegroundWindow(hw);   // a player's game window has focus; the cursor state applies to the active window
    RECT cr;
    GetClientRect(hw, &cr);
    POINT c{(cr.right - cr.left) / 2, (cr.bottom - cr.top) / 2};
    ClientToScreen(hw, &c);
    // a small real movement (the OS re-evaluates the cursor shape on WM_SETCURSOR, i.e. when the mouse moves)
    SetCursorPos(c.x + 7, c.y + 5);
    MSG m;
    for (int i = 0; i < 3; ++i) while (PeekMessage(&m, nullptr, 0, 0, PM_REMOVE)) { TranslateMessage(&m); DispatchMessage(&m); }
    SetCursorPos(c.x, c.y);
}

bool cursorShowing() {
    CURSORINFO ci{};
    ci.cbSize = sizeof ci;
    if (!GetCursorInfo(&ci)) return false;
    return (ci.flags & CURSOR_SHOWING) != 0 && ci.hCursor != nullptr;
}

// desktop capture (what the player really sees in fullscreen, incl. scaling / black bars), saved as BMP
bool captureDesktop(const std::string& path) {
    HDC screen = GetDC(nullptr);
    int w = GetSystemMetrics(SM_CXSCREEN), h = GetSystemMetrics(SM_CYSCREEN);
    HDC mem = CreateCompatibleDC(screen);
    BITMAPINFO bi{};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = w;
    bi.bmiHeader.biHeight = -h;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    void* bits = nullptr;
    HBITMAP bmp = CreateDIBSection(screen, &bi, DIB_RGB_COLORS, &bits, nullptr, 0);
    HGDIOBJ old = SelectObject(mem, bmp);
    BitBlt(mem, 0, 0, w, h, screen, 0, 0, SRCCOPY | CAPTUREBLT);
    bool ok = false;
    if (FILE* f = Paths::open(path, "wb")) {
        BITMAPFILEHEADER fh{};
        fh.bfType = 0x4D42;
        fh.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
        fh.bfSize = fh.bfOffBits + (DWORD)w * h * 4;
        BITMAPINFOHEADER ih = bi.bmiHeader;
        std::fwrite(&fh, sizeof fh, 1, f);
        std::fwrite(&ih, sizeof ih, 1, f);
        std::fwrite(bits, 4, (size_t)w * h, f);
        std::fclose(f);
        ok = true;
    }
    SelectObject(mem, old);
    DeleteObject(bmp);
    DeleteDC(mem);
    ReleaseDC(nullptr, screen);
    return ok;
}
}  // namespace

void App::galleryReset() {
    run_.reset();
    world_.reset();
    combatAI_.reset();
    screen_ = Screen::Title;
    optionsBack_ = Screen::Title;
    pick_ = Pick::None;
    pickFirst_ = -1;
    deckScroll_ = 0;
    paused_ = false;
    confirmNew_ = false;
    hasSave_ = false;
    hintT_ = 0;
    toastT_ = 0;
    rebind_ = -1;
    creditsY_ = 0;
    lastUiKey_ = -1;
    profile_ = Profile();
    profile_.tutorialDone = true;
    profile_.hintsSeen = 0xFFFFFFFFu;   // hints only where a step forces one
    profile_.settings.fullscreen = GetWindowModeFlag() == FALSE;   // match the real mode
    in.setNeutral(true, false);
    R.clearFx();
}

std::unique_ptr<RunState> App::galleryRun(int chapter, int deckSize, int relicCount) {
    auto r = std::make_unique<RunState>(RunLogic::newRun(4242u + chapter, 0, RunLogic::defaultPool()));
    if (chapter > 1) { r->chapter = chapter; r->map = RunLogic::generateMap(r->seed, chapter, 0); }
    r->gold = 260;
    const auto& pool = r->pool;
    for (int i = (int)r->deck.size(); i < deckSize; ++i) r->deck.push_back(makeCard(pool[i % pool.size()], r->newUid()));
    while ((int)r->deck.size() > deckSize && deckSize > 0) r->deck.pop_back();
    for (int i = 0; i < relicCount && i < (int)RunLogic::relics().size(); ++i) r->relics.push_back(i);
    return r;
}

std::vector<App::GalleryStep> App::galleryBuild() {
    std::vector<GalleryStep> S;
    auto add = [&](const std::string& n, std::function<void()> f, int focus = -1, bool cursor = true, bool pad = false, int frames = 4) {
        S.push_back({n, std::move(f), frames, focus, cursor, pad});
    };
    // each hint step uses the real text of its own phase (same strings as App.cpp hint())

    // ---- title
    add("title", [&] { screen_ = Screen::Title; });
    add("title_continue", [&] { screen_ = Screen::Title; hasSave_ = true; });
    add("title_confirm_new", [&] { screen_ = Screen::Title; hasSave_ = true; confirmNew_ = true; });
    add("title_toast", [&] { screen_ = Screen::Title; toast_ = "進行データを読み込めなかったため、別名で残して最初から始めます"; toastT_ = 3; });   // the longest real toast
    // ---- hub
    std::vector<int> locked;
    for (int d = 0; d < (int)CardDB::all().size(); ++d) if (CardDB::get(d).locked) locked.push_back(d);
    add("hub_none_unlocked", [&] { screen_ = Screen::Hub; profile_.shards = 0; });
    add("hub_rich", [&] { screen_ = Screen::Hub; profile_.shards = 999; profile_.runs = 12; profile_.wins = 3; profile_.ascensionUnlocked = 1; });   // 3 clears: trial 1 is open
    for (size_t i = 0; i < locked.size(); ++i) {
        int d = locked[i];
        add("hub_focus_" + std::to_string(i) + "_" + CardDB::get(d).id, [&] { screen_ = Screen::Hub; profile_.shards = 999; }, G_ID_UNLOCK + d);
    }
    add("hub_some_unlocked", [&, locked] {
        screen_ = Screen::Hub; profile_.shards = 120;
        for (size_t i = 0; i < locked.size(); i += 3) profile_.unlocked.push_back(CardDB::get(locked[i]).id);
    });
    add("hub_all_unlocked", [&, locked] {
        screen_ = Screen::Hub; profile_.shards = 40;
        for (int d : locked) profile_.unlocked.push_back(CardDB::get(d).id);
    });
    add("hub_easy", [&] { screen_ = Screen::Hub; profile_.ascension = -1; profile_.ascensionUnlocked = 0; });
    add("hub_trial5", [&] { screen_ = Screen::Hub; profile_.ascension = 5; profile_.ascensionUnlocked = 5; profile_.runs = 31; profile_.wins = 9; });
    // ---- options / keys / credits
    add("options", [&] { screen_ = Screen::Options; });
    add("options_all_toggled", [&] {
        screen_ = Screen::Options;
        auto& s = profile_.settings; s.bgm = 0; s.se = 1; s.shake = false; s.reduceFx = true; s.damageNumbers = false;
    });
    add("options_from_battle", [&] { screen_ = Screen::Options; optionsBack_ = Screen::Run; });
    add("keys", [&] { screen_ = Screen::Keys; });
    add("keys_waiting", [&] { screen_ = Screen::Keys; rebind_ = 3; });
    add("keys_long_names", [&] {
        screen_ = Screen::Keys;
        int* k = profile_.settings.keys; k[B_DASH] = KEY_INPUT_RSHIFT; k[B_FUSE] = KEY_INPUT_RCONTROL; k[B_EVOLVE] = KEY_INPUT_BACK;
        applySettings();
    });
    add("credits_top", [&] { screen_ = Screen::Credits; creditsY_ = 300; });
    add("credits_mid", [&] { screen_ = Screen::Credits; creditsY_ = 900; });
    add("credits_end", [&] { screen_ = Screen::Credits; creditsY_ = 99999; });   // clamped to the end of the roll
    // ---- map
    for (int ch = 1; ch <= 3; ++ch)
        add("map_ch" + std::to_string(ch), [&, ch] { run_ = galleryRun(ch, 12, 2); screen_ = Screen::Run; });
    add("map_hint", [&] { run_ = galleryRun(1, 12, 2); screen_ = Screen::Run; hintText_ = "強敵を倒すとレリックが手に入る。鍛冶場では2枚を永続的に1枚へ合成できる"; hintT_ = 5; });
    add("map_relics20", [&] { run_ = galleryRun(2, 12, 20); screen_ = Screen::Run; });
    // ---- battle (expect the OS cursor hidden while fighting; the game draws its own reticle)
    auto battle = [&](int ch, NodeType t, int settle) {
        run_ = galleryRun(ch, 14, 3);
        RunLogic::debugEnter(*run_, t);
        screen_ = Screen::Run;
        beginBattle();
        for (int i = 0; i < settle; ++i) world_->update(InputState{});
    };
    add("battle_normal", [&, battle] { battle(1, NodeType::Battle, 120); }, -1, false);
    add("battle_elite", [&, battle] { battle(2, NodeType::Elite, 120); }, -1, false);
    for (int ch = 1; ch <= 3; ++ch)
        add("battle_boss_ch" + std::to_string(ch), [&, battle, ch] { battle(ch, NodeType::Boss, 150); }, -1, false);
    add("battle_paused", [&, battle] { battle(1, NodeType::Battle, 60); paused_ = true; });
    add("battle_pad_hints", [&, battle] { battle(1, NodeType::Battle, 60); }, -1, false, true);
    add("battle_long_fused_names", [&, battle] {
        battle(3, NodeType::Battle, 60);
        auto& h = world_->hand;
        // the widest name any fusion of two base cards can produce
        CardInstance best;
        int bestW = -1;
        int n = (int)CardDB::all().size();
        for (int a = 0; a < n; ++a)
            for (int b = 0; b < n; ++b) {
                if (CardDB::get(a).isEvolved || CardDB::get(b).isEvolved) continue;
                FusePreview p = previewFuse(makeCard(a, 9001), makeCard(b, 9002), 9003);
                if (p.kind == FuseKind::Invalid) continue;
                p.result.rank = 3;   // "＋＋" suffix too
                int w = PStrWidth(p.result.displayName().c_str(), 0, R.fontT());
                if (w > bestW) { bestW = w; best = p.result; }
            }
        if (bestW > 0) { h.slots[0] = best; h.slots[1] = best; }
        h.slots[2].mastery = MASTERY_CAP;   // evolvable glow + label
    }, -1, false);
    add("battle_hint", [&, battle] { battle(1, NodeType::Battle, 60); hintText_ = "手札の間の印に合成結果が出る。Z/X/Cで隣の2枚を合成！"; hintT_ = 5; }, -1, false);
    add("battle_empty_hand", [&, battle] {
        battle(1, NodeType::Battle, 30);
        for (int i = 0; i < HAND_SIZE; ++i) { world_->hand.slots[i] = CardInstance(); world_->hand.refillT[i] = REFILL_DELAY; }
    }, -1, false);
    // ---- tutorial
    for (int st = 0; st < 8; ++st)
        add("tutorial_step" + std::to_string(st + 1), [&, st] {
            startTutorial();
            for (int s = 1; s <= st; ++s) tutEnterStep(s);   // replay the setup of every earlier step, as play does
            tutStep_ = st;
            for (int f = 0; f < 30; ++f) world_->update(InputState{});   // let spawns appear
        }, -1, false);
    add("tutorial_paused", [&] { startTutorial(); tutEnterStep(1); tutEnterStep(2); paused_ = true; });
    // ---- reward
    add("reward_cards", [&] { run_ = galleryRun(1, 12, 2); RunLogic::debugReward(*run_, NodeType::Battle); screen_ = Screen::Run; });
    add("reward_elite_relic", [&] { run_ = galleryRun(2, 12, 2); RunLogic::debugReward(*run_, NodeType::Elite); screen_ = Screen::Run; });
    add("reward_boss", [&] { run_ = galleryRun(3, 12, 2); RunLogic::debugReward(*run_, NodeType::Boss); screen_ = Screen::Run; });
    add("reward_hint", [&] {
        run_ = galleryRun(1, 12, 2); RunLogic::debugReward(*run_, NodeType::Battle); screen_ = Screen::Run; hintText_ = "カードは1枚選ぶかスキップ。デッキを絞るのも戦略だ"; hintT_ = 5;
    });
    add("reward_card_focused", [&] { run_ = galleryRun(1, 12, 2); RunLogic::debugReward(*run_, NodeType::Battle); screen_ = Screen::Run; }, G_ID_CARD + 2);
    // ---- shop
    add("shop", [&] { run_ = galleryRun(1, 12, 2); RunLogic::debugEnter(*run_, NodeType::Shop); screen_ = Screen::Run; });
    add("shop_poor", [&] { run_ = galleryRun(1, 12, 2); RunLogic::debugEnter(*run_, NodeType::Shop); run_->gold = 0; screen_ = Screen::Run; });
    add("shop_sold_out", [&] {
        run_ = galleryRun(1, 12, 2); RunLogic::debugEnter(*run_, NodeType::Shop); screen_ = Screen::Run;
        for (auto& s : run_->shop.cardSold) s = 1;
        for (auto& s : run_->shop.relicSold) s = 1;
        run_->shop.potionSold = true;
        run_->gold = 14;   // after buying everything (the screen used to show the starting 260G)
        run_->hp = run_->maxHp - 10;
    });
    add("shop_hint", [&] {
        run_ = galleryRun(1, 12, 2); RunLogic::debugEnter(*run_, NodeType::Shop); screen_ = Screen::Run; hintText_ = "カード強化やカード削除でデッキを鍛えよう"; hintT_ = 5;
    });
    add("shop_relics20", [&] { run_ = galleryRun(1, 12, 20); RunLogic::debugEnter(*run_, NodeType::Shop); screen_ = Screen::Run; });
    // ---- forge / rest
    add("forge", [&] { run_ = galleryRun(1, 12, 2); RunLogic::debugEnter(*run_, NodeType::Forge); screen_ = Screen::Run; });
    add("forge_anchor_limit", [&] { run_ = galleryRun(1, 12, 2); RunLogic::debugEnter(*run_, NodeType::Forge); run_->anchors = MAX_ANCHORS; screen_ = Screen::Run; });
    add("rest", [&] { run_ = galleryRun(1, 12, 2); RunLogic::debugEnter(*run_, NodeType::Rest); run_->hp = 30; screen_ = Screen::Run; });
    // ---- events (all, plus unaffordable choices)
    for (int e = 0; e < (int)RunLogic::events().size(); ++e) {
        add("event_" + RunLogic::events()[e].id, [&, e] {
            run_ = galleryRun(1, 12, 2); RunLogic::debugEnter(*run_, NodeType::Event); run_->eventId = e; screen_ = Screen::Run;
        });
        add("event_" + RunLogic::events()[e].id + "_poor", [&, e] {
            run_ = galleryRun(1, 12, 2); RunLogic::debugEnter(*run_, NodeType::Event); run_->eventId = e; run_->gold = 0; run_->hp = 5;
            screen_ = Screen::Run;
        });
    }
    // ---- deck pickers: every Pick, small and large deck
    struct PickCase { Pick p; NodeType where; const char* name; };
    const PickCase picks[] = {
        {Pick::Remove, NodeType::Shop, "remove"}, {Pick::Upgrade, NodeType::Shop, "upgrade"},
        {Pick::Anchor1, NodeType::Forge, "anchor1"}, {Pick::Anchor2, NodeType::Forge, "anchor2"},
        {Pick::RankUp1, NodeType::Forge, "rankup1"}, {Pick::RankUp2, NodeType::Forge, "rankup2"},
        {Pick::Train, NodeType::Forge, "train"}, {Pick::RestTrain, NodeType::Rest, "rest_train"},
        {Pick::View, NodeType::Forge, "view"},
    };
    for (const auto& pc : picks)
        for (int n : {5, 32}) {
            add("pick_" + std::string(pc.name) + "_" + std::to_string(n), [&, pc, n] {
                run_ = galleryRun(1, n, 2);
                // duplicates so rank-up has valid pairs
                for (int i = 1; i < n; i += 4) run_->deck[i] = makeCard(run_->deck[i - 1].def, run_->newUid());
                RunLogic::debugEnter(*run_, pc.where);
                screen_ = Screen::Run;
                pick_ = pc.p;
                pickFirst_ = (pc.p == Pick::Anchor2 || pc.p == Pick::RankUp2) ? 0 : -1;
            });
            if (n > 5) {   // keyboard: Down past the last visible row must scroll the page and move the focus with it
                S.back().keys = {KEY_INPUT_DOWN, KEY_INPUT_DOWN, KEY_INPUT_DOWN};
                S.back().verify = [&] {
                    int f = ui.focus() - G_ID_PICK;
                    bool onPageButton = ui.focus() == 966 || ui.focus() == 967;   // a page with nothing to pick focuses 前へ/次へ
                    if (deckScroll_ < 1 || (f < 24 && !onPageButton)) return std::string("keyboard scroll failed: deckScroll=") + std::to_string(deckScroll_) + " focus card " + std::to_string(f);
                    return std::string();
                };
            }
        }
    // ---- chapter clear / results
    add("chapter_clear", [&] { run_ = galleryRun(1, 12, 3); run_->phase = Phase::ChapterClear; screen_ = Screen::Run; });
    add("result_victory", [&] {
        auto r = galleryRun(3, 20, 8); r->stats.kills = 312; r->stats.fusions = 140; r->stats.evolutions = 11; r->stats.bossesKilled = 3;
        r->stats.battleSeconds = 1004; r->stats.reactions = 0x1F6;
        lastRun_ = *r; lastVictory_ = true; lastShards_ = 85; screen_ = Screen::Result;
    });
    add("result_defeat", [&] {
        auto r = galleryRun(2, 15, 4); r->stats.kills = 120; r->stats.battleSeconds = 431; r->stats.bossesKilled = 1; r->stats.fusions = 52; r->stats.reactions = 0x26;
        lastRun_ = *r; lastVictory_ = false; lastShards_ = 31; screen_ = Screen::Result;
    });
    return S;
}

void App::runGallery() {
    std::string dir = opt_.shotDir;
    Paths::makeDir(dir);
    std::vector<std::string> findings, cursorLog, missing, escLog;
    auto steps = galleryBuild();
    if (opt_.galleryTransitionsOnly) steps.clear();
    if (opt_.f11Stress > 0) {   // display-mode switches while a battle (and its effects) is running
        galleryReset();
        run_ = galleryRun(1, 12, 2); RunLogic::debugEnter(*run_, NodeType::Battle); screen_ = Screen::Run; beginBattle();
        for (int n = 0; n < opt_.f11Stress; ++n) {
            for (int f = 0; f < 20; ++f) { ProcessMessage(); frame(); ScreenFlip(); }   // let fights and effects run
            in.inject(KEY_INPUT_F11); ProcessMessage(); frame(); ScreenFlip();
            Log::write("f11stress %d mode=%s", n + 1, GetWindowModeFlag() ? "window" : "fullscreen");
        }
        if (GetWindowModeFlag() == FALSE) { changeDisplayMode(true); }
        return;
    }
    if (opt_.modeStress > 0) {   // the gallery step that crashed: title, a fresh battle, paused, then F11
        int switches = 0;
        bool lastFull = GetWindowModeFlag() == FALSE;
        for (int n = 0; n < opt_.modeStress; ++n) {
            galleryReset();   // also sets neutral input: without it the injected F11 never fires
            screen_ = Screen::Title;
            for (int f = 0; f < 3; ++f) { ProcessMessage(); frame(); ScreenFlip(); }
            run_ = galleryRun(1, 12, 2); RunLogic::debugEnter(*run_, NodeType::Battle); screen_ = Screen::Run; beginBattle();
            for (int f = 0; f < 3; ++f) { ProcessMessage(); frame(); ScreenFlip(); }
            paused_ = true;
            in.inject(KEY_INPUT_F11); ProcessMessage(); frame(); ScreenFlip();
            for (int f = 0; f < 8; ++f) { ProcessMessage(); frame(); ScreenFlip(); }
            bool fullNow = GetWindowModeFlag() == FALSE;
            if (fullNow != lastFull) { ++switches; lastFull = fullNow; }
            Log::write("modestress %d mode=%s switches=%d", n + 1, fullNow ? "fullscreen" : "window", switches);
        }
        if (GetWindowModeFlag() == FALSE) { changeDisplayMode(true); }
        return;
    }
    std::set<std::string> covered;
    galleryFullscreen_ = opt_.galleryFullscreen;
    if (galleryFullscreen_) { changeDisplayMode(false); }
    int idx = 0;
    for (auto& st : steps) {
        galleryReset();
        in.setNeutral(true, st.pad);
        st.setup();
        for (int f = 0; f < st.frames; ++f) {
            ProcessMessage();
            frame();
            ScreenFlip();
        }
        if (!st.keys.empty()) {
            for (int f = 0; f < 20; ++f) { ProcessMessage(); frame(); ScreenFlip(); }   // past the input lock
            for (int k : st.keys) {
                in.inject(k);
                for (int f = 0; f < 2; ++f) { ProcessMessage(); frame(); ScreenFlip(); }
            }
        }
        if (st.focus >= 0) {
            ui.setFocus(st.focus);
            ProcessMessage();
            frame();
            ScreenFlip();
            if (ui.focus() != st.focus) missing.push_back(st.name + "\tfocus id " + std::to_string(st.focus) + " not on screen");
        }
        // measured frame
        ProcessMessage();
        Probe::enabled = true;
        Probe::beginFrame();
        int gen0 = ui.generation();
        frame();
        bool usesMenuUi = ui.generation() != gen0;
        if (st.verify) { std::string v = st.verify(); if (!v.empty()) findings.push_back(st.name + "\tverify\t" + v); }
        Probe::enabled = false;
        {
            std::vector<std::string> f1;
            Probe::analyze(st.name, f1);
            for (auto& s : f1) {
                // the credits scroll by design: lines above/below the screen are expected
                if (st.name.rfind("credits", 0) == 0 && s.find("\toff-screen\t") != std::string::npos) continue;
                findings.push_back(s);
            }
        }
        char path[512];
        std::snprintf(path, sizeof path, "%s\\%03d_%s.png", dir.c_str(), idx, st.name.c_str());
        if (!galleryFullscreen_) SaveDrawScreenToPNG(0, 0, 1280, 720, path);
        ScreenFlip();
        // ---- keyboard / pad: every enabled item reachable with the arrow keys from the default focus
        {
            const auto items = ui.items();
            if (usesMenuUi && !items.empty() && rebind_ < 0) {   // while a key is awaited, focus is locked by design
                int start = ui.focus();
                std::set<int> seen{start};
                std::vector<int> q{start};
                const Vec2 dirs[4] = {{0, -1}, {0, 1}, {-1, 0}, {1, 0}};
                while (!q.empty()) {
                    int id = q.back();
                    q.pop_back();
                    for (Vec2 d : dirs) {
                        int n = ui.neighbor(id, d);
                        if (n >= 0 && seen.insert(n).second) q.push_back(n);
                    }
                }
                for (auto& it : items)
                    if (it.enabled && !seen.count(it.id)) {
                        char b[160];
                        std::snprintf(b, sizeof b, "ui id %d at (%.0f,%.0f %.0fx%.0f) cannot be reached with arrow keys from id %d", it.id, it.x, it.y, it.w, it.h, start);
                        findings.push_back(st.name + "\tunreachable-by-keys\t" + b);
                    }
                // ---- focus must be visible: compare each item focused vs. not focused (vs. animation noise)
                int soft = MakeARGB8ColorSoftImage(1280, 720);
                auto grab = [&]() {
                    int r = GetDrawScreenSoftImage(0, 0, 1280, 720, soft);
                    static bool dumped = false;
                    if (!dumped) {   // one-off sanity check of the capture path
                        dumped = true;
                        SaveSoftImageToPng((dir + "\\_softimage_check.png").c_str(), soft, 0);
                        char b[64]; std::snprintf(b, sizeof b, "GetDrawScreenSoftImage returned %d", r);
                        missing.push_back(std::string("softimage\t") + b);
                    }
                };
                auto diff = [&](const std::vector<int>& a, const std::vector<int>& b) {
                    long long s = 0;
                    for (size_t i = 0; i < a.size(); ++i) s += std::abs(a[i] - b[i]);
                    return a.empty() ? 0.0 : (double)s / a.size();
                };
                auto sample = [&](const UiItem& it) {
                    std::vector<int> v;
                    // the item plus an 8px margin: card focus is an outline drawn just outside the card
                    for (int yy = (int)it.y - 8; yy < (int)(it.y + it.h) + 8; yy += 3)
                        for (int xx = (int)it.x - 8; xx < (int)(it.x + it.w) + 8; xx += 3) {
                            if (xx < 0 || yy < 0 || xx >= 1280 || yy >= 720) continue;
                            int r = 0, g = 0, b2 = 0, a = 0;
                            if (GetPixelSoftImage(soft, xx, yy, &r, &g, &b2, &a) != 0) { missing.push_back("softimage\tGetPixelSoftImage failed"); return v; }
                            v.push_back(r + g + b2);
                        }
                    return v;
                };
                int checked = 0;
                for (auto& it : items) {
                    if (++checked > 40) break;
                    int other = -1;
                    for (auto& o : items) if (o.id != it.id) { other = o.id; break; }
                    if (other < 0) break;
                    ui.setFocus(it.id); ProcessMessage(); frame(); grab(); auto a1 = sample(it); ScreenFlip();
                    ui.setFocus(it.id); ProcessMessage(); frame(); grab(); auto a2 = sample(it); ScreenFlip();
                    ui.setFocus(other); ProcessMessage(); frame(); grab(); auto b1 = sample(it); ScreenFlip();
                    double noise = diff(a1, a2), change = diff(a1, b1);
                    if (change < noise + 6.0) {
                        char b[200];
                        std::snprintf(b, sizeof b, "ui id %d at (%.0f,%.0f %.0fx%.0f): focus change not visible (diff %.1f, noise %.1f)", it.id, it.x, it.y, it.w, it.h, change, noise);
                        findings.push_back(st.name + "\tfocus-invisible\t" + b);
                    }
                }
                DeleteSoftImage(soft);
                ui.setFocus(start);
            }
        }
        // OS cursor actually visible? Put the pointer inside the game window first (the cursor state is per window),
        // then let any mode switch settle for a few frames.
        centerCursor();
        for (int f = 0; f < 3; ++f) { ProcessMessage(); frame(); ScreenFlip(); }
        bool shown = cursorShowing();
        int retries = 0;
        // a mismatch must persist across real mouse movement (Windows also hides the pointer while someone types)
        while (shown != st.expectCursor && retries < 3) {
            ++retries;
            centerCursor();
            for (int f = 0; f < 4; ++f) { ProcessMessage(); frame(); ScreenFlip(); }
            shown = cursorShowing();
        }
        char line[256];
        bool focused = GetForegroundWindow() == GetMainWindowHandle();
        std::snprintf(line, sizeof line, "%s\t%s\texpected %s, actual %s\t(window %s, DxLib flag %s, retries %d)", st.name.c_str(), galleryFullscreen_ ? "fullscreen" : "window",
                      st.expectCursor ? "visible" : "hidden", shown ? "visible" : "hidden", focused ? "focused" : "NOT focused",
                      cursorShown_ ? "show" : "hide", retries);
        // another application in front (the PC is in use): the result says nothing about the game
        cursorLog.push_back(std::string(!focused ? "SKIP " : (shown == st.expectCursor ? "OK   " : "FAIL ")) + line);
        if (galleryFullscreen_ && (st.name == "title" || st.name.rfind("hub_focus_0_", 0) == 0 || st.name == "battle_normal" || st.name == "hub_rich")) {
            std::snprintf(path, sizeof path, "%s\\desktop_%s.bmp", dir.c_str(), st.name.c_str());
            captureDesktop(path);
        }
        // ---- Esc / B: does cancelling do anything here? (informational: some screens have no "back")
        {
            auto key = [&]() {
                return std::to_string((int)screen_) + "/" + std::to_string((int)pick_) + "/" + std::to_string(paused_) + "/" +
                       std::to_string(confirmNew_) + "/" + std::to_string(rebind_) + "/" + (run_ ? std::to_string((int)run_->phase) : "-");
            };
            std::string before = key();
            for (int f = 0; f < 20; ++f) { ProcessMessage(); frame(); ScreenFlip(); }   // past the input lock after a screen change
            before = key();
            in.inject(KEY_INPUT_ESCAPE);
            ProcessMessage(); frame(); ScreenFlip();
            if (key() == before) escLog.push_back(st.name + "\tEsc does nothing");
        }
        covered.insert(st.name);
        ++idx;
    }

    // ---- display-mode transitions: cursor must come back after every switch
    auto transition = [&](const char* name, std::function<void()> act, bool expect) {
        Log::write("gallery step: %s", name);   // the last line names the step if the process dies
        galleryReset();
        screen_ = Screen::Title;
        for (int f = 0; f < 3; ++f) { ProcessMessage(); frame(); ScreenFlip(); }
        act();
        for (int f = 0; f < 8; ++f) { ProcessMessage(); frame(); ScreenFlip(); }
        centerCursor();
        for (int f = 0; f < 2; ++f) { ProcessMessage(); frame(); ScreenFlip(); }
        bool shown = cursorShowing();
        bool full = GetWindowModeFlag() == FALSE;
        char line[256];
        std::snprintf(line, sizeof line, "transition %s\texpected %s, actual %s\tmode=%s setting=%s", name, expect ? "visible" : "hidden",
                      shown ? "visible" : "hidden", full ? "fullscreen" : "window", profile_.settings.fullscreen ? "fullscreen" : "window");
        bool consistent = full == profile_.settings.fullscreen;
        cursorLog.push_back(std::string(shown == expect && consistent ? "OK   " : "FAIL ") + line);
    };
    transition("F11 to fullscreen", [&] { in.inject(KEY_INPUT_F11); ProcessMessage(); frame(); ScreenFlip(); }, true);
    transition("F11 back", [&] { in.inject(KEY_INPUT_F11); ProcessMessage(); frame(); ScreenFlip(); }, true);
    transition("F11 x3", [&] { for (int k = 0; k < 3; ++k) { in.inject(KEY_INPUT_F11); ProcessMessage(); frame(); ScreenFlip(); ProcessMessage(); frame(); ScreenFlip(); } }, true);
    // the options screen's 画面モード button (switch deferred to the start of the next frame)
    for (int pass = 0; pass < 2; ++pass) {
        transition(pass == 0 ? "options button to fullscreen" : "options button back to window", [&] {
            screen_ = Screen::Options; optionsBack_ = Screen::Title;
            for (int f = 0; f < 22; ++f) { ProcessMessage(); frame(); ScreenFlip(); }   // past the new-screen input lock
            bool before = GetWindowModeFlag() == FALSE;
            ui.setFocus(64);   // ID_OPT_FULL
            in.inject(KEY_INPUT_RETURN);
            for (int f = 0; f < 3; ++f) { ProcessMessage(); frame(); ScreenFlip(); }
            bool after = GetWindowModeFlag() == FALSE;
            cursorLog.push_back(std::string(after != before ? "OK   " : "FAIL ") + "options button switched the display mode (" + (before ? "fullscreen" : "window") + " -> " + (after ? "fullscreen" : "window") + ")");
        }, true);
    }
    transition("battle then pause then F11", [&] {
        run_ = galleryRun(1, 12, 2); RunLogic::debugEnter(*run_, NodeType::Battle); screen_ = Screen::Run; beginBattle();
        for (int f = 0; f < 3; ++f) { ProcessMessage(); frame(); ScreenFlip(); }
        paused_ = true;
        in.inject(KEY_INPUT_F11); ProcessMessage(); frame(); ScreenFlip();
    }, true);
    transition("minimize + restore (Alt+Tab stand-in)", [&] {
        HWND hw = GetMainWindowHandle();
        ShowWindow(hw, SW_MINIMIZE);
        for (int f = 0; f < 10; ++f) { ProcessMessage(); frame(); ScreenFlip(); }
        ShowWindow(hw, SW_RESTORE);
        SetForegroundWindow(hw);
    }, true);
    transition("minimize + restore during battle", [&] {
        run_ = galleryRun(1, 12, 2); RunLogic::debugEnter(*run_, NodeType::Battle); screen_ = Screen::Run; beginBattle();
        galleryAutoPause_ = true;
        HWND hw = GetMainWindowHandle();
        ShowWindow(hw, SW_MINIMIZE);
        int activeWhileMin = 0, iconic = 0;
        bool pausedWhileMin = false;
        for (int f = 0; f < 10; ++f) { ProcessMessage(); frame(); ScreenFlip(); activeWhileMin += GetWindowActiveFlag() ? 1 : 0; iconic += IsIconic(hw) ? 1 : 0; pausedWhileMin = pausedWhileMin || paused_; }
        ShowWindow(hw, SW_RESTORE);
        SetForegroundWindow(hw);
        for (int f = 0; f < 3; ++f) { ProcessMessage(); frame(); ScreenFlip(); }
        // losing focus must pause the battle, and the pause menu shows the cursor
        char diag[160];
        std::snprintf(diag, sizeof diag, "  (while minimized: active %d/10, iconic %d/10, paused %d; screen %d phase %d)", activeWhileMin, iconic,
                      pausedWhileMin ? 1 : 0, (int)screen_, run_ ? (int)run_->phase : -1);
        if (!paused_) cursorLog.push_back(std::string("FAIL minimize during battle did not pause the game") + diag);
        else cursorLog.push_back("OK   minimize during battle paused the game");
        galleryAutoPause_ = false;
    }, true);
    // key rebinding through the real input path: wait for a key, press one, the binding must change
    {
        galleryReset();
        screen_ = Screen::Keys;
        for (int f = 0; f < 25; ++f) { ProcessMessage(); frame(); ScreenFlip(); }
        rebind_ = B_DASH;
        in.inject(KEY_INPUT_K);
        for (int f = 0; f < 3; ++f) { ProcessMessage(); frame(); ScreenFlip(); }
        bool ok = profile_.settings.keys[B_DASH] == KEY_INPUT_K && rebind_ == -1;
        cursorLog.push_back(std::string(ok ? "OK   " : "FAIL ") + "key rebinding: dash -> K via injected key press");
    }
    // leave the display the way we found it
    if (GetWindowModeFlag() == FALSE && !opt_.galleryFullscreen) { changeDisplayMode(true); }

    // ---- mouse mapping: put the OS cursor on known points and read back the logical position
    std::vector<std::string> mouseLog;
    {
        galleryReset();
        screen_ = Screen::Title;
        int sizes[][2] = {{1280, 720}, {960, 540}, {1600, 900}, {1200, 900}};   // the last one is not 16:9
        int nSizes = opt_.galleryFullscreen ? 1 : 4;   // fullscreen: the desktop size only
        for (int si = 0; si < nSizes; ++si) {
            auto& sz = sizes[si];
            if (!opt_.galleryFullscreen) SetWindowSize(sz[0], sz[1]);
            for (int f = 0; f < 6; ++f) { ProcessMessage(); frame(); ScreenFlip(); }
            HWND hw = GetMainWindowHandle();
            if (IsIconic(hw)) ShowWindow(hw, SW_RESTORE);
            SetForegroundWindow(hw);
            for (int f = 0; f < 4; ++f) { ProcessMessage(); frame(); ScreenFlip(); }
            RECT cr;
            GetClientRect(hw, &cr);
            int cw = cr.right - cr.left, ch = cr.bottom - cr.top;
            if (cw <= 0 || ch <= 0 || GetForegroundWindow() != hw) { mouseLog.push_back("SKIP window not in front / minimized (PC in use)"); continue; }
            const float pts[][2] = {{100, 100}, {640, 360}, {1180, 620}};
            for (auto& p : pts) {
                // where the logical point lands on screen: uniform scale, centred (black bars if the aspect differs)
                // fullscreen: uniform scale with black bars; window: DxLib stretches the picture to the client
                float scx = cw / 1280.0f, scy = ch / 720.0f, ox = 0, oy = 0;
                if (opt_.galleryFullscreen) { scx = scy = std::min(scx, scy); ox = (cw - 1280 * scx) / 2; oy = (ch - 720 * scy) / 2; }
                POINT sp{(LONG)(ox + p[0] * scx), (LONG)(oy + p[1] * scy)};
                ClientToScreen(hw, &sp);
                SetCursorPos(sp.x, sp.y);
                for (int f = 0; f < 2; ++f) { ProcessMessage(); frame(); ScreenFlip(); }
                int mx, my;
                GetMousePoint(&mx, &my);
                bool ok = std::abs(mx - p[0]) <= 3 && std::abs(my - p[1]) <= 3;
                char line[200];
                std::snprintf(line, sizeof line, "%s client %dx%d: point (%.0f,%.0f) -> GetMousePoint (%d,%d)", ok ? "OK  " : "FAIL", cw, ch, p[0], p[1], mx, my);
                mouseLog.push_back(line);
            }
        }
        if (!opt_.galleryFullscreen) {
            SetWindowSize(1280, 720);
            // dragging the window edge must keep 16:9 (the hook rewrites the WM_SIZING rectangle)
            HWND hw = GetMainWindowHandle();
            RECT wr;
            GetWindowRect(hw, &wr);
            const struct { WPARAM edge; int w, h; } drags[] = {{WMSZ_BOTTOMRIGHT, 1300, 1000}, {WMSZ_RIGHT, 900, 0}, {WMSZ_BOTTOM, 0, 500}};
            for (auto& d : drags) {
                RECT r = wr;
                if (d.w) r.right = r.left + d.w;
                if (d.h) r.bottom = r.top + d.h;
                SendMessage(hw, WM_SIZING, d.edge, (LPARAM)&r);
                RECT frame{0, 0, 0, 0};
                AdjustWindowRectEx(&frame, (DWORD)GetWindowLongPtr(hw, GWL_STYLE), FALSE, (DWORD)GetWindowLongPtr(hw, GWL_EXSTYLE));
                int cw = (r.right - r.left) - (frame.right - frame.left), ch = (r.bottom - r.top) - (frame.bottom - frame.top);
                bool ok = ch > 0 && std::abs(cw * 9 - ch * 16) <= 16;
                char line[160];
                std::snprintf(line, sizeof line, "%s drag edge %d to %dx%d -> client %dx%d", ok ? "OK  " : "FAIL", (int)d.edge, d.w, d.h, cw, ch);
                mouseLog.push_back(line);
            }
        }
    }

    // ---- report
    std::string rep = dir + "\\gallery_report.txt";
    if (FILE* f = Paths::open(rep, "w")) {
        std::fprintf(f, "Arcana Forge %s gallery (%s): %d steps\n\n", ARCANA_VERSION, opt_.galleryFullscreen ? "fullscreen" : "window", (int)steps.size());
        std::fprintf(f, "== layout findings (%d)\n", (int)findings.size());
        for (auto& s : findings) std::fprintf(f, "%s\n", s.c_str());
        std::fprintf(f, "\n== focus ids not found (%d)\n", (int)missing.size());
        for (auto& s : missing) std::fprintf(f, "%s\n", s.c_str());
        std::fprintf(f, "\n== cursor\n");
        for (auto& s : cursorLog) std::fprintf(f, "%s\n", s.c_str());
        std::fprintf(f, "\n== Esc / cancel (informational)\n");
        for (auto& s : escLog) std::fprintf(f, "%s\n", s.c_str());
        std::fprintf(f, "\n== mouse mapping\n");
        for (auto& s : mouseLog) std::fprintf(f, "%s\n", s.c_str());
        std::fclose(f);
    }
    in.setNeutral(false);
}
