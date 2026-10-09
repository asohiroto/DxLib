// Training room: teaches move / attack / dash / cards / fusion / reactions / evolution, then a short fight.
// Every step completes from what actually happens in the World, so it works with keyboard, pad and autopilot.
#include "app/App.h"
#include "app/Probe.h"
#include "app/Sound.h"
#include "DxLib.h"
#include <algorithm>
#include <cmath>

std::string bindText(const char* s);
namespace {
struct TutStep { const char* kb; const char* pad; };
const TutStep kSteps[] = {
    {"{UP}{LEFT}{DOWN}{RIGHT} で移動してみよう", "左スティックで移動してみよう"},
    {"マウスで狙って 左クリックで攻撃！（練習用のスライムを攻撃）", "右スティックで狙って攻撃！（練習用のスライムを攻撃）"},
    {"{DASH} で回避。回避中は無敵になる", "A ボタンで回避。回避中は無敵になる"},
    {"{CARD1}〜{CARD4} キー（または右クリック）でカードを使う。マナを消費する", "X ボタンで選択中のカードを使う。LB/RB で選択"},
    {"同じカードが隣り合っている！ {FUSE12} で 1番と2番を合成してランクアップ", "同じカードが隣り合っている！ Y で合成してランクアップ"},
    {"違う属性を合成すると反応が付く。火と氷なら蒸気。{FUSE12} で合成して使ってみよう", "違う属性を合成すると反応が付く。火と氷なら蒸気。Y で合成して使おう"},
    {"何度も使ったカードには「進化」の印が出る。{EVOLVE} で進化！", "何度も使ったカードには「進化」の印が出る。B で進化！"},
    {"仕上げだ。襲ってくる敵をすべて倒そう", "仕上げだ。襲ってくる敵をすべて倒そう"},
};
constexpr int kStepCount = (int)(sizeof(kSteps) / sizeof(kSteps[0]));

CardInstance card(const char* id, uint32_t uid) { return makeCard(CardDB::find(id), uid); }
}  // namespace

void App::startTutorial() {
    RoomSetup s;
    uint32_t uid = 1;
    for (const char* id : {"fireball", "ice_shard", "lightning", "magic_missile", "rock_blast", "stone_guard", "fireball", "spark_burst"})
        s.deck.push_back(card(id, uid++));
    s.nextUid = 100;
    s.hp = s.maxHp = 80;
    s.seed = 99;
    world_ = std::make_unique<World>();
    world_->init(s);
    world_->player.pos = {200, 200};
    tutStep_ = 0;
    tutT_ = 0;
    tutCount_ = 0;
    tutStart_ = world_->player.pos;
    R.clearFx();
    screen_ = Screen::Tutorial;
    if (!opt_.autoplay.empty()) combatAI_ = std::make_unique<Policy>(PolicyConfig::fromName("fuseevo", 0.9f), 4242);
}

// Sets up tutorial step s (also used by the QA gallery to reach each step the same way play does).
void App::tutEnterStep(int s) {
    World& w = *world_;
    const int scarecrow = Content::findEnemy("turret") >= 0 ? Content::findEnemy("slime") : 0;
        tutStep_ = s;
        tutT_ = 0;
        tutCount_ = 0;
        Sound::play(Sfx::Fanfare, 0.5f);
        uint32_t uid = 500 + s * 10;
        switch (s) {
        case 1: w.debugSpawnDummy(scarecrow, {330, 200}, 9999); break;
        case 3: w.mana = w.mods.manaMax; break;
        case 4:   // identical neighbours
            w.hand.slots[0] = card("fireball", uid); w.hand.slots[1] = card("fireball", uid + 1);
            w.hand.cursor = 0; w.mana = w.mods.manaMax; break;
        case 5:   // fire + ice -> steam
            w.hand.slots[0] = card("fireball", uid); w.hand.slots[1] = card("ice_shard", uid + 1);
            w.hand.cursor = 0; w.mana = w.mods.manaMax; break;
        case 6: {
            CardInstance c = card("lightning", uid);
            c.mastery = w.mods.masteryCap;
            w.hand.slots[2] = c;
            w.hand.cursor = 2;
            break;
        }
        case 7: {
            for (auto& e : w.enemies) e.alive = false;   // remove the scarecrow
            int slime = Content::findEnemy("slime"), archer = Content::findEnemy("archer");
            for (Vec2 p : {Vec2{500, 80}, Vec2{540, 220}, Vec2{120, 70}}) w.spawnAt(slime, p);
            w.spawnAt(archer, {560, 150});
            break;
        }
        default: break;
        }
}

void App::doTutorial() {
    World& w = *world_;
    bool human = opt_.autoplay.empty();
    auto enterStep = [&](int s) { tutEnterStep(s); };
    if (tutT_ == 0 && tutStep_ == 0 && tutCount_ == 0) { tutCount_ = 1; }

    if (human && (in.pressed(KEY_INPUT_ESCAPE) || in.padPressed(PAD_INPUT_8))) paused_ = !paused_;
    {   // like a battle: switching away (Alt+Tab, minimize) pauses
        HWND hw = GetMainWindowHandle();
        bool focused = GetWindowActiveFlag() != 0 && !IsIconic(hw) && GetForegroundWindow() == hw;
        if (human && !focused && !opt_.gallery) paused_ = true;
    }
    if (!paused_) {
        int steps = human ? 1 : opt_.speed;
        for (int k = 0; k < steps; ++k) {
            InputState is = human ? in.makeState(w.player.pos) : combatAI_->think(w);
            // keep the lesson focused: the autopilot performs exactly the taught action
            if (!human) {
                if (tutStep_ == 0) is.move = {1, 0.3f};
                if (tutStep_ == 2 && tutT_ > 0.5f) is.dash = true;
                if (tutStep_ == 3 && tutT_ > 0.5f) is.useSlot = 0;
                if ((tutStep_ == 4 || tutStep_ == 5) && tutT_ > 0.5f && w.hand.slots[0].isComposite()) is.useSlot = 0;
                else if ((tutStep_ == 4 || tutStep_ == 5) && tutT_ > 0.5f) is.fuseSlot = 0;
                if (tutStep_ == 6 && tutT_ > 0.5f) is.evolve = true;
            }
            w.update(is);
            R.onEvents(w);
            R.update();
            tutT_ += DT;
            // keep the player alive and stocked while learning
            if (tutStep_ < 7) { w.player.hp = std::max(w.player.hp, 40.0f); w.mana = std::max(w.mana, 2.0f); }
            for (auto& e : w.events) {
                if (tutStep_ == 2 && e.type == Ev::Dash) ++tutCount_;
                if (tutStep_ == 3 && e.type == Ev::CardUsed) ++tutCount_;
                if (tutStep_ == 4 && e.type == Ev::Fused) ++tutCount_;
                if (tutStep_ == 5 && e.type == Ev::Fused && e.a != (int)Reaction::None) tutCount_ = std::max(tutCount_, 1);
                if (tutStep_ == 5 && e.type == Ev::CardUsed && tutCount_ >= 1) tutCount_ = 2;
                if (tutStep_ == 6 && e.type == Ev::Evolved) ++tutCount_;
            }
            bool done = false;
            switch (tutStep_) {
            case 0: done = dist(w.player.pos, tutStart_) > 90; break;
            case 1: done = w.stats.basicDamage + w.stats.cardDamage >= 30; break;
            case 2: done = tutCount_ >= 2; break;
            case 3: done = tutCount_ >= 2; break;
            case 4: done = tutCount_ >= 1; break;
            case 5: done = tutCount_ >= 2; break;
            case 6: done = tutCount_ >= 1; break;
            case 7: {
                bool any = false;
                for (auto& e : w.enemies) if (e.alive) any = true;
                done = !any && tutT_ > 2.0f;
                break;
            }
            }
            if (w.player.hp <= 0) { startTutorial(); return; }
            if (done && tutT_ > 0.6f) {
                if (tutStep_ + 1 < kStepCount) enterStep(tutStep_ + 1);
                else {
                    profile_.tutorialDone = true;
                    saveProfile();
                    world_.reset();
                    toast_ = "訓練完了！ 冒険に出よう";
                    toastT_ = 3;
                    screen_ = Screen::Title;
                    return;
                }
            }
        }
    }
    R.draw(w, in.mouseLogical(), debug_, in.usingPad());
    // lesson panel
    const TutStep& st = kSteps[std::clamp(tutStep_, 0, kStepCount - 1)];
    std::string textS = bindText(in.usingPad() ? st.pad : st.kb);
    const char* text = textS.c_str();
    // high on the field with "Esc：中断" inside it, so enemy spawn markers are not hidden under the UI
    panel(160, 34, 960, 70, GetColor(255, 220, 140));
    char buf[32];
    std::snprintf(buf, sizeof buf, "訓練 %d/%d", tutStep_ + 1, kStepCount);
    R.drawText(178, 42, buf, GetColor(255, 220, 140), 0);
    const int escW = PStrWidth("Esc：中断", 0, R.fontS());
    {   // the instruction starts after the measured step counter and stops before the Esc label
        int tx = 178 + PStrWidth(buf, 0, R.fontS()) + 24;
        wrapText(tx, 42, 1100 - tx - escW - 20, text, GetColor(245, 240, 255), R.fontS(), 28);
    }
    // pointer arrows at the relevant HUD part
    float pulse = 0.5f + 0.5f * std::sin(ui.pulse() * 6);
    auto arrow = [&](float x, float y) {
        DrawTriangleAA(x - 12, y - 22 - 6 * pulse, x + 12, y - 22 - 6 * pulse, x, y - 6 - 6 * pulse, GetColor(255, 220, 120), TRUE);
    };
    if (tutStep_ == 3) arrow(310 + 74, 606);
    if (tutStep_ == 4 || tutStep_ == 5) arrow(310 + 148 + 13, 640);
    if (tutStep_ == 6) arrow(310 + 2 * 174 + 74, 574);   // above the evolve badge over the card
    R.drawText(1104 - escW, 72, "Esc：中断", GetColor(185, 175, 215), 0);   // bottom-right inside the panel
    if (paused_) {
        SetDrawBlendMode(DX_BLENDMODE_ALPHA, 160);
        DrawBox(0, 0, 1280, 720, GetColor(0, 0, 0), TRUE);
        SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
        ui.begin();
        panel(360, 190, 560, 200, GetColor(255, 220, 140));
        R.drawText(640, 220, "訓練を中断しますか？", GetColor(255, 255, 255), 2, true);
        button(80, 400, 310, 220, 48, "続ける");
        button(81, 660, 310, 220, 48, "訓練をやめる");
        bool cancel = false;
        int act = ui.end(in, &cancel);
        if (act == 80 || cancel) { paused_ = false; }
        else if (act == 81) {
            paused_ = false;
            profile_.tutorialDone = true;   // never force it again
            saveProfile();
            world_.reset();
            screen_ = Screen::Title;
        }
    }
}
