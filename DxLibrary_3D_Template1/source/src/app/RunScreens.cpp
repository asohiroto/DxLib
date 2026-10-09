// Screens shown during a run. Each screen only draws RunState and sends Actions to RunLogic.
#include "app/App.h"
#include "app/Probe.h"
#include "app/Sound.h"
#include "app/Probe.h"
#include "app/Fx.h"
#include "app/Probe.h"
#include "game/Save.h"
#include "DxLib.h"
#include <algorithm>
#include <cmath>
#include <cstdio>

namespace {
enum : int {
    ID_NODE = 100,        // + node index
    ID_DECK = 950, ID_LEAVE = 951, ID_SKIP = 952, ID_POTION = 953, ID_REMOVE = 954, ID_CONTINUE = 955,
    ID_HEAL = 956, ID_TRAIN = 957, ID_ANCHOR = 958, ID_RANKUP = 959, ID_TRAIN_FORGE = 960,
    ID_RESUME = 961, ID_OPTIONS = 962, ID_ABANDON = 963, ID_PICK_CANCEL = 964, ID_UPGRADE = 965, ID_PAGE_PREV = 966, ID_PAGE_NEXT = 967,
    ID_CARD = 400,        // + offer / shop index
    ID_RELIC = 500,       // + offer / shop index
    ID_CHOICE = 600,      // + event choice
    ID_PICK = 1000,       // + deck index
};

unsigned nodeColor(NodeType t) {
    switch (t) {
    case NodeType::Battle: return GetColor(200, 200, 210);
    case NodeType::Elite: return GetColor(255, 110, 90);
    case NodeType::Shop: return GetColor(255, 210, 90);
    case NodeType::Forge: return GetColor(255, 150, 60);
    case NodeType::Event: return GetColor(140, 200, 255);
    case NodeType::Rest: return GetColor(120, 230, 140);
    case NodeType::Boss: return GetColor(220, 90, 255);
    }
    return GetColor(255, 255, 255);
}
const char* nodeGlyph(NodeType t) {
    switch (t) {
    case NodeType::Battle: return "戦";
    case NodeType::Elite: return "強";
    case NodeType::Shop: return "商";
    case NodeType::Forge: return "鍛";
    case NodeType::Event: return "？";
    case NodeType::Rest: return "休";
    case NodeType::Boss: return "王";
    }
    return "";
}
Vec2 nodePos(const MapNode& n) {
    if (n.type == NodeType::Boss) return {640, 92};
    return {640 + (n.col - 1.5f) * 190.0f, 648 - n.row * 75.0f};   // bottom row clear of the hint strip (y=689)
}
}  // namespace

void App::applyAction(const Action& a) {
    if (!run_) return;
    std::string err;
    if (!RunLogic::apply(*run_, a, &err)) {
        Sound::play(Sfx::NoMana, 0.6f);
        if (!opt_.autoplay.empty()) {   // autopilot asked for something illegal: back out
            Action leave{Act::Leave};
            if (!RunLogic::apply(*run_, leave) && run_->phase == Phase::Reward) RunLogic::apply(*run_, {Act::SkipCard});
        }
        return;
    }
    pick_ = Pick::None;
    pickFirst_ = -1;
    if (run_->phase == Phase::Defeat) { endRun(false); return; }
    if (run_->phase == Phase::Victory) { endRun(true); return; }
    saveRun();
}

void App::doRun() {
    if (!run_) { screen_ = Screen::Title; return; }
    if (run_->phase == Phase::Battle) { doBattle(); return; }
    ClearDrawScreen();
    drawBackground(0);
    if (pick_ != Pick::None) { doPicker(); drawTopBar(); return; }
    switch (run_->phase) {
    case Phase::Map: doMap(); break;
    case Phase::Reward: doReward(); break;
    case Phase::Shop: doShop(); break;
    case Phase::Forge: doForge(); break;
    case Phase::Rest: doRest(); break;
    case Phase::Event: doEvent(); break;
    case Phase::ChapterClear: doChapterClear(); break;
    case Phase::Victory: endRun(true); return;
    case Phase::Defeat: endRun(false); return;
    default: break;
    }
    drawTopBar();
}

// ---------------------------------------------------------------- map
void App::doMap() {
    ui.begin();
    const ChapterMap& m = run_->map;
    auto choosable = RunLogic::choosableNodes(*run_);
    int focusedType = -1;
    // edges
    for (int i = 0; i < (int)m.nodes.size(); ++i)
        for (int j : m.nodes[i].next) {
            Vec2 a = nodePos(m.nodes[i]), b = nodePos(m.nodes[j]);
            bool fromHere = i == run_->node;
            unsigned c = fromHere ? GetColor(220, 200, 255) : GetColor(70, 64, 96);
            DrawLineAA(a.x, a.y, b.x, b.y, c, fromHere ? 3.0f : 2.0f);
        }
    for (int i = 0; i < (int)m.nodes.size(); ++i) {
        const MapNode& n = m.nodes[i];
        Vec2 p = nodePos(n);
        bool can = std::find(choosable.begin(), choosable.end(), i) != choosable.end();
        float r = n.type == NodeType::Boss ? 34.0f : 22.0f;
        if (can) ui.add(ID_NODE + i, p.x - r - 6, p.y - r - 6, 2 * r + 12, 2 * r + 12, true);
        bool f = ui.focused(ID_NODE + i);
        bool past = n.row <= (run_->node >= 0 ? m.nodes[run_->node].row : -1);
        unsigned col = nodeColor(n.type);
        if (!can && !past) SetDrawBlendMode(DX_BLENDMODE_ALPHA, 150);
        DrawCircleAA(p.x, p.y, r + 3, 32, GetColor(10, 8, 20), TRUE);
        DrawCircleAA(p.x, p.y, r, 32, past ? GetColor(50, 46, 64) : GetColor(34, 30, 52), TRUE);
        DrawCircleAA(p.x, p.y, r, 32, col, FALSE, f ? 4.0f : 2.0f);
        SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
        int font = n.type == NodeType::Boss ? R.fontL() : R.fontM();
        int tw = PStrWidth(nodeGlyph(n.type), (int)strlen(nodeGlyph(n.type)), font);
        // edge in the node's own fill colour: a black edge filled the gaps of dense kanji (鍛, 強)
        unsigned fill = past ? GetColor(50, 46, 64) : GetColor(34, 30, 52);
        PDrawString((int)p.x - tw / 2, (int)p.y - (n.type == NodeType::Boss ? 15 : 10), nodeGlyph(n.type), col, font, fill);
        if (i == run_->node) DrawCircleAA(p.x, p.y, r + 8, 32, GetColor(255, 255, 255), FALSE, 2.0f);
        if (can) {
            float pulse = 0.5f + 0.5f * std::sin(ui.pulse() * 5 + i);
            SetDrawBlendMode(DX_BLENDMODE_ADD, (int)(60 + 100 * pulse));
            DrawCircleAA(p.x, p.y, r + 6, 32, col, FALSE, 2.0f);
            SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
        }
        if (f) focusedType = (int)n.type;
    }
    // what the focused node is, in a fixed box (a tooltip next to the node covered its neighbours)
    if (focusedType >= 0) {
        static const char* what[] = {"敵と戦う", "勝つとレリック", "買い物ができる", "カードを鍛える", "何かが起こる", "回復か修練", "この章のボス"};
        NodeType t = (NodeType)focusedType;
        panel(60, 586, 250, 86, nodeColor(t));   // ends 12px above the hint strip (y=684)
        R.drawText(76, 594, ("選択中：" + std::string(nodeTypeName(t))).c_str(), nodeColor(t), 0);
        R.drawText(76, 630, what[focusedType], GetColor(210, 205, 230), 0);
    }
    // legend
    int ly = 120;
    for (NodeType t : {NodeType::Battle, NodeType::Elite, NodeType::Event, NodeType::Shop, NodeType::Forge, NodeType::Rest, NodeType::Boss}) {
        DrawCircleAA(1112.0f, (float)ly + 12, 13, 20, GetColor(30, 26, 44), TRUE);
        DrawCircleAA(1112.0f, (float)ly + 12, 13, 20, nodeColor(t), FALSE, 1.5f);
        int gw = PStrWidth(nodeGlyph(t), 0, R.fontS());
        PDrawString(1112 - gw / 2, ly, nodeGlyph(t), nodeColor(t), R.fontS(), GetColor(30, 26, 44));
        R.drawText(1146, ly, nodeTypeName(t), GetColor(200, 195, 220), 0);   // clear gap after the circle
        ly += 32;
    }
    {   // the glowing ring marks where you can go; the white ring (only once you have moved) where you are
        float pulse = 0.5f + 0.5f * std::sin(ui.pulse() * 5);
        // drawn like a reachable node (a node disc inside the glowing ring), not a bare circle
        DrawCircleAA(1112.0f, (float)ly + 12, 9, 20, GetColor(30, 26, 44), TRUE);
        DrawCircleAA(1112.0f, (float)ly + 12, 9, 20, GetColor(150, 140, 180), FALSE, 1.5f);
        SetDrawBlendMode(DX_BLENDMODE_ADD, (int)(60 + 100 * pulse));
        DrawCircleAA(1112.0f, (float)ly + 12, 14, 20, GetColor(220, 200, 255), FALSE, 2.0f);
        SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
        R.drawText(1146, ly, "進める", GetColor(200, 195, 220), 0);
        if (run_->node >= 0) {
            ly += 32;
            DrawCircleAA(1112.0f, (float)ly + 12, 11, 20, GetColor(255, 255, 255), FALSE, 2.0f);
            R.drawText(1146, ly, "現在地", GetColor(200, 195, 220), 0);
        }
    }
    button(ID_DECK, 1074, 628, 180, 44, "デッキを見る");
    bool cancel = false;
    int act = ui.end(in, &cancel);
    R.drawText(80, 70, "進む場所を選んでください", GetColor(220, 210, 255), 1);
    if (act >= ID_NODE && act < ID_NODE + 200) applyAction({Act::ChooseNode, act - ID_NODE});
    else if (act == ID_DECK) { pick_ = Pick::View; deckScroll_ = 0; }
    else if (cancel || in.pressed(KEY_INPUT_ESCAPE)) { optionsBack_ = Screen::Run; screen_ = Screen::Options; }
}

// ---------------------------------------------------------------- deck picker / viewer
bool App::doPicker() {
    ui.begin();
    const char* title = "";
    switch (pick_) {
    case Pick::Remove: title = "削除するカードを選ぶ"; break;
    case Pick::Anchor1: title = "定着合成：1枚目（型）を選ぶ"; break;
    case Pick::Anchor2: title = "定着合成：2枚目（属性）を選ぶ"; break;
    case Pick::RankUp1: title = "ランクアップ：同じカードを2枚選ぶ"; break;
    case Pick::RankUp2: title = "ランクアップ：同じカードをもう1枚選ぶ"; break;
    case Pick::Train: case Pick::RestTrain: title = "修練するカードを選ぶ（熟練度+4）"; break;
    case Pick::Upgrade: title = "強化するカードを選ぶ（ランク+1）"; break;
    case Pick::View: title = "デッキ"; break;
    default: break;
    }
    R.drawText(640, 52, title, GetColor(255, 230, 170), 1, true);   // one size down: leaves room for the subtitle line
    const auto& deck = run_->deck;
    if (pick_ == Pick::Anchor1)   // what anchoring is, before the first choice
        R.drawText(640, 88, "2枚を永続的に1枚へ合成する（1枚目の型＋2枚目の属性）", GetColor(200, 190, 220), 0, true);
    if (pickFirst_ >= 0 && pickFirst_ < (int)deck.size()) {   // the step-1 choice may be on another page
        std::string sub = "1枚目：" + deck[pickFirst_].displayName() + "（灰色は選べない・Enterで決定）";
        R.drawText(640, 88, sub.c_str(), GetColor(255, 200, 80), 0, true);
    }
    const int cols = 8, cw = 120, ch = 120, gx = 20, gy = 18, rowsVisible = 3;
    const int perPage = cols * rowsVisible;
    int rowsTotal = ((int)deck.size() + cols - 1) / cols;
    // last page starts on a page boundary (1-24 / 25-32), not "the last 3 rows"
    int maxScroll = rowsTotal > rowsVisible ? ((rowsTotal - 1) / rowsVisible) * rowsVisible : 0;
    // keyboard / pad: moving past the visible rows scrolls (otherwise cards beyond the page are unreachable)
    {
        int fi = ui.focus() - ID_PICK;
        if (fi >= 0 && fi < (int)deck.size()) {
            int row = fi / cols - deckScroll_;
            bool down = in.pressed(KEY_INPUT_DOWN) || in.pressed(KEY_INPUT_S) || in.padPressed(PAD_INPUT_DOWN);
            bool up = in.pressed(KEY_INPUT_UP) || in.pressed(KEY_INPUT_W) || in.padPressed(PAD_INPUT_UP);
            // turns whole pages, like the 前へ/次へ buttons, so the page always matches the "a〜b枚目" line
            if (down && row == rowsVisible - 1 && deckScroll_ < maxScroll) { deckScroll_ = std::min(maxScroll, deckScroll_ + rowsVisible); ui.setFocus(ID_PICK + std::min((int)deck.size() - 1, fi + cols)); ui.consumeNav(); }
            if (up && row == 0 && deckScroll_ > 0) { deckScroll_ = std::max(0, deckScroll_ - rowsVisible); ui.setFocus(ID_PICK + fi - cols); ui.consumeNav(); }
        }
    }
    deckScroll_ = std::clamp(deckScroll_ - in.wheel() * rowsVisible, 0, maxScroll);
    if (in.pressed(KEY_INPUT_PGDN)) deckScroll_ = std::min(maxScroll, deckScroll_ + rowsVisible);
    if (in.pressed(KEY_INPUT_PGUP)) deckScroll_ = std::max(0, deckScroll_ - rowsVisible);
    int x0 = (1280 - (cols * cw + (cols - 1) * gx)) / 2, y0 = 124;   // below title and step-1 line
    for (int i = deckScroll_ * cols; i < (int)deck.size() && i < deckScroll_ * cols + perPage; ++i) {
        int k = i - deckScroll_ * cols;
        int x = x0 + (k % cols) * (cw + gx), y = y0 + (k / cols) * (ch + gy);
        const CardInstance& c = deck[i];
        bool ok = true;
        if (pick_ == Pick::Train || pick_ == Pick::RestTrain) ok = !c.anchored && c.d().evolveTo >= 0;
        if (pick_ == Pick::Anchor1 || pick_ == Pick::Anchor2) ok = !c.anchored && i != pickFirst_;
        if (pick_ == Pick::Anchor2) ok = ok && RunLogic::anchorPreview(*run_, pickFirst_, i).kind == FuseKind::Hybrid;
        if (pick_ == Pick::RankUp1) {
            ok = false;
            for (int j = 0; j < (int)deck.size(); ++j)
                if (j != i && deck[j].def == c.def && deck[j].rank == c.rank && c.rank < 3 && !c.fused && !c.anchored && !deck[j].fused) ok = true;
        }
        if (pick_ == Pick::RankUp2) ok = i != pickFirst_ && c.def == deck[pickFirst_].def && c.rank == deck[pickFirst_].rank && !c.fused && !c.anchored;
        if (pick_ == Pick::Upgrade) ok = RunLogic::canUpgrade(c);
        if (pick_ == Pick::View) ok = true;
        ui.add(ID_PICK + i, (float)x, (float)y, (float)cw, (float)ch, ok);
        R.drawCard(x, y, cw, ch, c, ui.focused(ID_PICK + i), ok, 0);
        if (i == pickFirst_) {   // the card chosen in step 1: gold frame + tag, distinct from the white focus outline
            DrawRoundRectAA((float)x - 2, (float)y - 2, (float)(x + cw + 2), (float)(y + ch + 2), 10, 10, 8, GetColor(255, 200, 80), FALSE, 3.0f);
            DrawRoundRectAA((float)x + cw / 2 - 30, (float)y - 20, (float)x + cw / 2 + 30, (float)y - 4, 4, 4, 6, GetColor(120, 80, 10), TRUE);
            int tw2 = PStrWidth("1枚目", (int)strlen("1枚目"), R.fontT());
            PDrawString(x + cw / 2 - tw2 / 2, y - 18, "1枚目", GetColor(255, 230, 150), R.fontT(), GetColor(40, 20, 0));
        }
    }
    {   // a page where nothing can be picked says so (the matching card is on another page)
        bool any = false;
        for (auto& it : ui.items()) if (it.id >= ID_PICK && it.id < ID_PICK + 1000 && it.enabled) any = true;
        static int autoFocusedPage = -1;   // nothing to pick on this page: focus the way out (once per page)
        if (any) autoFocusedPage = -1;
        else if (maxScroll > 0 && autoFocusedPage != deckScroll_ && ui.focus() >= ID_PICK && ui.focus() < ID_PICK + 1000) {
            ui.setFocus(deckScroll_ > 0 ? ID_PAGE_PREV : ID_PAGE_NEXT);
            autoFocusedPage = deckScroll_;
        }
        if (!any && maxScroll > 0)   // the first-picked card is the usual match: point at its page
        {
            char nb[128];
            if (pickFirst_ >= 0)   // say which page the first card is on (its twin is usually beside it)
                std::snprintf(nb, sizeof nb, "このページに選べるカードはない（1枚目は%dページ目。「前へ」「次へ」で切替）", pickFirst_ / cols / std::max(1, rowsVisible) + 1);
            else std::snprintf(nb, sizeof nb, "このページに選べるカードはない（「前へ」「次へ」で探す）");
            R.drawText(640, 300, nb, GetColor(255, 200, 150), 0, true);
        }
        bool anyGrey = false;
        for (auto& it : ui.items()) if (it.id >= ID_PICK && it.id < ID_PICK + 1000 && !it.enabled) anyGrey = true;
        if (anyGrey && pick_ != Pick::Anchor1 && pickFirst_ < 0)   // the subtitle line is free: say what grey means
            R.drawText(640, 88, "灰色のカードは選べない（カーソルを合わせると理由が出る）", GetColor(200, 190, 220), 0, true);
    }
    if (maxScroll > 0) {
        int gy0 = y0 + rowsVisible * (ch + gy) - gy + 12;   // just under the grid
        button(ID_PAGE_PREV, x0, gy0, 150, 40, "前へ", deckScroll_ > 0);
        button(ID_PAGE_NEXT, 1280 - x0 - 150, gy0, 150, 40, "次へ", deckScroll_ < maxScroll);
        char pb[96];
        std::snprintf(pb, sizeof pb, "全%d枚中 %d〜%d枚目を表示", (int)deck.size(), deckScroll_ * cols + 1,
                      std::min((int)deck.size(), deckScroll_ * cols + perPage));
        R.drawText(640, gy0 + 8, pb, GetColor(190, 180, 220), 0, true);
    }
    // detail of the focused card (left) and the anchor preview (right), both under the grid
    int f = ui.focus() - ID_PICK;
    if (f >= 0 && f < (int)deck.size()) {
        // with the anchor preview beside it, the detail panel gives up width so the preview text fits in 2 lines
        const int dw = pick_ == Pick::Anchor2 ? 460 : 560;
        panel(60, 580, dw, 122, GetColor(140, 120, 200));
        R.drawText(76, 588, deck[f].displayName().c_str(), GetColor(255, 230, 170), 1);
        {   // what the two numbers on the card are (only when it fits after the name)
            char pb[64];
            std::snprintf(pb, sizeof pb, "威力%d コスト%d", (int)std::lround(deck[f].power()), deck[f].cost());
            if ((pick_ == Pick::Train || pick_ == Pick::RestTrain) && deck[f].d().evolveTo >= 0)   // how far from evolving
                std::snprintf(pb, sizeof pb, "熟練度 %d／%d", std::min(deck[f].mastery, MASTERY_CAP), MASTERY_CAP);
            if (pick_ == Pick::Upgrade && RunLogic::canUpgrade(deck[f])) {   // what the upgrade changes
                CardInstance up = deck[f];
                ++up.rank;
                std::snprintf(pb, sizeof pb, "強化後 威力%d", (int)std::lround(up.power()));   // the current value is on the card
            }
            int nx = 76 + PStrWidth(deck[f].displayName().c_str(), 0, R.fontM()) + 20;
            if (nx + PStrWidth(pb, 0, R.fontS()) <= 60 + dw - 14) R.drawText(nx, 590, pb, GetColor(190, 180, 220), 0);
        }
        bool focusOk = false;
        for (auto& it : ui.items()) if (it.id == ID_PICK + f) focusOk = it.enabled;
        if (!focusOk) {   // say why this card cannot be picked here
            const char* why = "このカードは選べない";
            const CardInstance& c = deck[f];
            if (f == pickFirst_) why = "1枚目に選んだカード";
            else if (c.anchored) why = "定着カードは選べない";
            else if (pick_ == Pick::Train || pick_ == Pick::RestTrain) why = "進化しないカードは修練できない";
            else if (pick_ == Pick::RankUp1) why = "同じカード（同ランク）が2枚ないとランクアップできない";
            else if (pick_ == Pick::RankUp2) why = "1枚目と同じカード（同ランク）を選ぶ";
            else if (pick_ == Pick::Anchor2) why = "1枚目と組めない（同じ属性・同じカードなど）";
            else if (pick_ == Pick::Upgrade) why = "合成・定着したカードや最大ランクのカードは強化できない";
            wrapText(76, 618, dw - 30, why, GetColor(230, 150, 150), R.fontS(), 19);
        } else {
            wrapText(76, 618, dw - 30, cardDesc(deck[f]), GetColor(220, 215, 240), R.fontS(), 19);
        }
        if (pick_ == Pick::Anchor2) {
            FusePreview p = RunLogic::anchorPreview(*run_, pickFirst_, f);
            if (p.kind == FuseKind::Hybrid) {
                panel(532, 580, 494, 122, GetColor(255, 200, 120));
                R.drawText(546, 588, ("結果：" + p.result.displayName()).c_str(), GetColor(255, 220, 140), 1);
                char buf[96];
                std::snprintf(buf, sizeof buf, "威力%d コスト%d", (int)std::lround(p.result.power()), p.result.cost());
                R.drawText(546, 618, buf, GetColor(230, 230, 240), 0);
                wrapText(546, 646, 466, reactionDesc(p.result.reaction), GetColor(210, 205, 230), R.fontS(), 24);
            }
        }
    }    button(ID_PICK_CANCEL, 1040, 640, 180, 44, pick_ == Pick::View ? "閉じる" : "やめる");
    bool cancel = false;
    int act = ui.end(in, &cancel);
    if (act == ID_PICK_CANCEL || cancel) { pick_ = Pick::None; pickFirst_ = -1; return false; }
    if (act == ID_PAGE_PREV) { deckScroll_ = std::max(0, deckScroll_ - rowsVisible); return true; }
    if (act == ID_PAGE_NEXT) { deckScroll_ = std::min(maxScroll, deckScroll_ + rowsVisible); return true; }
    if (act >= ID_PICK) {
        int i = act - ID_PICK;
        switch (pick_) {
        case Pick::Remove: applyAction({Act::ShopRemove, i}); break;
        case Pick::Train: applyAction({Act::ForgeTrain, i}); break;
        case Pick::RestTrain: applyAction({Act::RestTrain, i}); break;
        case Pick::Upgrade: applyAction({Act::ShopUpgrade, i}); break;
        case Pick::Anchor1: pickFirst_ = i; pick_ = Pick::Anchor2; break;
        case Pick::Anchor2: applyAction({Act::ForgeAnchor, pickFirst_, i}); break;
        case Pick::RankUp1: pickFirst_ = i; pick_ = Pick::RankUp2; break;
        case Pick::RankUp2: applyAction({Act::ForgeRankUp, pickFirst_, i}); break;
        default: break;
        }
    }
    return pick_ != Pick::None;
}

// ---------------------------------------------------------------- reward
void App::doReward() {
    ui.begin();
    const Offer& o = run_->offer;
    R.drawText(640, 70, "戦利品", GetColor(255, 230, 170), 2, true);
    char buf[96];
    if (o.goldGained > 0) std::snprintf(buf, sizeof buf, "%dGを獲得した（所持 %dG）", o.goldGained, run_->gold);
    else std::snprintf(buf, sizeof buf, "所持 %dG", run_->gold);
    R.drawText(640, 112, buf, GetColor(255, 220, 110), 1, true);
    if (!o.relicDone) {
        R.drawText(640, 150, o.relics.size() == 1 ? "レリックを獲得（クリック／Enterで受け取る）" : "レリックを1つ選ぶ（クリック／Enterで決定）", GetColor(230, 220, 255), 1, true);
        int n = (int)o.relics.size();
        for (int i = 0; i < n; ++i) {
            int x = 640 - n * 172 + i * 344 + 7, y = 190;
            const RelicDef& d = RunLogic::relics()[o.relics[i]];
            ui.add(ID_RELIC + i, (float)x, (float)y, 330, 170, true);
            bool f = ui.focused(ID_RELIC + i);
            // common relics: warm white, so the name stands apart from the grey-white description
            unsigned c = d.rarity == 'r' ? GetColor(255, 200, 90) : (d.rarity == 'u' ? GetColor(150, 200, 255) : GetColor(255, 235, 190));
            panel(x, y, 330, 170, f ? GetColor(255, 240, 180) : c);   // room for 4 lines of description
            if (f) focusFrame(x, y, 330, 170);
            DrawCircleAA((float)x + 46, (float)y + 85, 34, 32, c, FALSE, 3.0f);
            R.drawIcon(R.relicIcon(d.id), x + 46, y + 85, 2);
            R.drawText(x + 92, y + 20, d.name.c_str(), c, 1);
            wrapText(x + 92, y + 54, 228, d.desc, GetColor(225, 220, 240), R.fontS(), 19);
        }
    } else if (!o.cardDone) {
        R.drawText(640, 150, "カードを1枚選ぶ（スキップ可）", GetColor(230, 220, 255), 1, true);
        for (int i = 0; i < (int)o.cards.size(); ++i) {
            int n = (int)o.cards.size(), x = 640 - (n * kTileW + (n - 1) * 30) / 2 + i * (kTileW + 30), y = 190;
            CardInstance c = makeCard(o.cards[i], 0);
            ui.add(ID_CARD + i, (float)x, (float)y, (float)kTileW, (float)cardTileH(false), true);
            drawCardBig(x, y, c, ui.focused(ID_CARD + i));
        }
        button(ID_SKIP, 560, 560, 160, 44, "スキップ");
    }
    int act = ui.end(in);
    if (act >= ID_RELIC && act < ID_RELIC + 10) applyAction({Act::TakeRelic, act - ID_RELIC});
    else if (act >= ID_CARD && act < ID_CARD + 10) applyAction({Act::TakeCard, act - ID_CARD});
    else if (act == ID_SKIP) applyAction({Act::SkipCard});
}

// ---------------------------------------------------------------- shop
void App::doShop() {
    ui.begin();
    R.drawText(640, 60, "商店", GetColor(255, 230, 170), 2, true);
    const ShopStock& s = run_->shop;
    char buf[64];
    for (int i = 0; i < (int)s.cards.size(); ++i) {
        int x = 60 + i * (kTileW + 12), y = 110;
        CardInstance c = makeCard(s.cards[i], 0);
        bool ok = !s.cardSold[i] && run_->gold >= s.cardPrices[i];
        ui.add(ID_CARD + i, (float)x, (float)y, (float)kTileW, (float)cardTileH(true), ok);
        std::snprintf(buf, sizeof buf, s.cardSold[i] ? "売り切れ" : "%dG", s.cardPrices[i]);
        drawCardBig(x, y, c, ui.focused(ID_CARD + i), buf, s.cardSold[i] || run_->gold >= s.cardPrices[i], s.cardSold[i] != 0);
        if (s.cardSold[i]) {   // sold: dimmed (still readable), following the tile's rounded corners
            SetDrawBlendMode(DX_BLENDMODE_ALPHA, 45);
            DrawRoundRectAA((float)x - 4, (float)y - 4, (float)(x + kTileW + 4), (float)(y + cardTileH(true)), 10, 10, 8, GetColor(0, 0, 0), TRUE);
            SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
        }
    }
    for (int i = 0; i < (int)s.relics.size(); ++i) {
        int x = 60 + i * 316, y = 450;
        const RelicDef& d = RunLogic::relics()[s.relics[i]];
        bool ok = !s.relicSold[i] && run_->gold >= s.relicPrices[i];
        ui.add(ID_RELIC + i, (float)x, (float)y, 300, 132, ok);
        unsigned c = d.rarity == 'r' ? GetColor(255, 200, 90) : (d.rarity == 'u' ? GetColor(150, 200, 255) : GetColor(200, 200, 210));
        if (s.relicSold[i]) c = GetColor(165, 160, 182);   // sold: grey like a sold card (still readable)
        panel(x, y, 300, 132, ui.focused(ID_RELIC + i) ? GetColor(255, 240, 180) : c);
        if (ui.focused(ID_RELIC + i)) focusFrame(x, y, 300, 132);
        R.drawIcon(R.relicIcon(d.id), x + 30, y + 30, 1);
        R.drawText(x + 54, y + 10, d.name.c_str(), c, 1);
        wrapText(x + 14, y + 46, 272, d.desc, GetColor(220, 215, 235), R.fontS(), 19);
        // price bottom-right inside the panel
        std::snprintf(buf, sizeof buf, s.relicSold[i] ? "売り切れ" : "%dG", s.relicPrices[i]);
        bool rAfford = s.relicSold[i] || run_->gold >= s.relicPrices[i];
        R.drawText(x + 288 - PStrWidth(buf, (int)strlen(buf), R.fontM()), y + 102, buf, s.relicSold[i] ? GetColor(175, 170, 190) : (rAfford ? GetColor(255, 220, 110) : GetColor(255, 140, 140)), 1);
        if (s.relicSold[i]) { SetDrawBlendMode(DX_BLENDMODE_ALPHA, 80); DrawBox(x, y, x + 300, y + 132, GetColor(0, 0, 0), TRUE); SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0); }
    }
    std::snprintf(buf, sizeof buf, "HP回復 %dG", POTION_PRICE);
    if (!s.potionSold && run_->hp >= run_->maxHp) std::snprintf(buf, sizeof buf, "HP満タン：回復不要");   // a price here was too wide, and "(満タン)" read ambiguously
    button(ID_POTION, 696, 450, 294, 48, s.potionSold ? "HP回復 購入済み" : buf, !s.potionSold && run_->gold >= POTION_PRICE && run_->hp < run_->maxHp);
    if (s.relics.empty()) R.drawText(90, 450, "購入できるレリックはない（すべて所持している）", GetColor(170, 160, 200), 0);
    const int rp = RunLogic::removePrice(*run_), up = RunLogic::upgradePrice(*run_);
    const bool deckOk = (int)run_->deck.size() > MIN_DECK;
    {   // say what the red prices mean when something cannot be bought
        bool short_ = false;   // about the goods; the service buttons carry their own red price
        for (int i = 0; i < (int)s.cards.size(); ++i) if (!s.cardSold[i] && run_->gold < s.cardPrices[i]) short_ = true;
        for (int i = 0; i < (int)s.relics.size(); ++i) if (!s.relicSold[i] && run_->gold < s.relicPrices[i]) short_ = true;
        if (short_) R.drawText(720, 72, "赤い価格：所持金が足りない", GetColor(255, 140, 140), 0);
    }
    {   // the price stays on the button; when it cannot be paid it is red, like the goods (the legend above)
        const unsigned shortCol = GetColor(255, 140, 140);
        if (!deckOk) std::snprintf(buf, sizeof buf, "削除：枚数が最少");
        else std::snprintf(buf, sizeof buf, "カード削除 %dG", rp);
        button(ID_REMOVE, 696, 510, 294, 48, buf, run_->gold >= rp && deckOk, deckOk && run_->gold < rp ? shortCol : 0);
        std::snprintf(buf, sizeof buf, "カード強化 %dG", up);
        button(ID_UPGRADE, 696, 570, 294, 48, buf, run_->gold >= up, run_->gold < up ? shortCol : 0);
    }
    button(ID_DECK, 1004, 450, 240, 48, "デッキを見る");
    button(ID_LEAVE, 1004, 600, 240, 52, "店を出る");
    bool cancel = false;
    int act = ui.end(in, &cancel);
    if (act >= ID_CARD && act < ID_CARD + 10) applyAction({Act::ShopBuyCard, act - ID_CARD});
    else if (act >= ID_RELIC && act < ID_RELIC + 10) applyAction({Act::ShopBuyRelic, act - ID_RELIC});
    else if (act == ID_POTION) applyAction({Act::ShopPotion});
    else if (act == ID_REMOVE) { pick_ = Pick::Remove; deckScroll_ = 0; }
    else if (act == ID_UPGRADE) { pick_ = Pick::Upgrade; deckScroll_ = 0; }
    else if (act == ID_DECK) { pick_ = Pick::View; deckScroll_ = 0; }
    else if (act == ID_LEAVE || cancel) applyAction({Act::Leave});
}

// ---------------------------------------------------------------- forge / rest / event
void App::doForge() {
    ui.begin();
    R.drawText(640, 80, "鍛冶場", GetColor(255, 200, 140), 2, true);
    R.drawText(640, 124, "ひとつだけ選べる", GetColor(200, 190, 220), 1, true);
    char buf[96];
    std::snprintf(buf, sizeof buf, "定着合成（残り%d回）", MAX_ANCHORS - run_->anchors);
    bool canAnchor = run_->anchors < MAX_ANCHORS && (int)run_->deck.size() - 1 >= MIN_DECK;
    button(ID_ANCHOR, 150, 200, 300, 60, buf, canAnchor);
    button(ID_RANKUP, 490, 200, 300, 60, "ランクアップ");
    button(ID_TRAIN_FORGE, 830, 200, 300, 60, "修練（熟練度+4）");
    {
        // a disabled option says why, and its description is dimmed with it
        unsigned dc = canAnchor ? GetColor(210, 205, 230) : GetColor(165, 160, 185);
        int ty = 280;
        if (!canAnchor) {   // the reason wraps inside this column (300px) like the description
            const char* why = run_->anchors >= MAX_ANCHORS ? "この冒険の定着合成は使い切った（3回まで）" : "デッキの枚数が足りない";
            wrapText(150, ty, 300, why, GetColor(240, 160, 160), R.fontS(), 20);
            ty += (int)wrapLines(why, 300, R.fontS()).size() * 27 + 8;
        }
        wrapText(150, ty, 300, "デッキの2枚を永続的に合成する。左の型に右の属性が加わる。定着カードは進化・再合成できない。", dc, R.fontS(), 20);
    }
    wrapText(490, 280, 300, "同じカード2枚を1枚にまとめ、ランクを1つ上げる。", GetColor(210, 205, 230), R.fontS(), 20);
    wrapText(830, 280, 300, "カード1枚の熟練度を4上げる。満タンになれば戦闘中に進化できる。", GetColor(210, 205, 230), R.fontS(), 20);
    button(ID_LEAVE, 540, 600, 200, 48, "立ち去る");
    bool cancel = false;
    int act = ui.end(in, &cancel);
    if (act == ID_ANCHOR) { pick_ = Pick::Anchor1; pickFirst_ = -1; }
    else if (act == ID_RANKUP) { pick_ = Pick::RankUp1; pickFirst_ = -1; }
    else if (act == ID_TRAIN_FORGE) pick_ = Pick::Train;
    else if (act == ID_LEAVE || cancel) applyAction({Act::Leave});
    deckScroll_ = 0;
}

void App::doRest() {
    ui.begin();
    R.drawText(640, 90, "焚き火", GetColor(140, 240, 160), 2, true);
    char buf[96];
    std::snprintf(buf, sizeof buf, "休む（HP+%d）", (int)std::lround(run_->maxHp * REST_HEAL * (run_->ascension >= 4 ? 0.7f : 1.0f)));
    bool canHeal = run_->hp < run_->maxHp;
    button(ID_HEAL, 330, 240, 300, 64, buf, canHeal);
    button(ID_TRAIN, 650, 240, 300, 64, "修練（熟練度+4）");
    wrapText(330, 320, 300, canHeal ? "HPを回復する" : "HPは満タン", canHeal ? GetColor(210, 205, 230) : GetColor(230, 150, 150), R.fontS(), 20);
    wrapText(650, 320, 300, "カード1枚の熟練度を4上げる。満タンになったカードは戦闘中に進化できる。", GetColor(210, 205, 230), R.fontS(), 20);
    button(ID_LEAVE, 540, 600, 200, 48, "立ち去る");
    bool cancel = false;
    int act = ui.end(in, &cancel);
    if (act == ID_HEAL) applyAction({Act::RestHeal});
    else if (act == ID_TRAIN) { pick_ = Pick::RestTrain; deckScroll_ = 0; }
    else if (act == ID_LEAVE || cancel) applyAction({Act::Leave});
}

void App::doEvent() {
    ui.begin();
    if (run_->eventId < 0) { applyAction({Act::Leave}); return; }
    const EventDef& ev = RunLogic::events()[run_->eventId];
    panel(240, 90, 800, 260, GetColor(140, 200, 255));
    R.drawText(640, 110, ev.title.c_str(), GetColor(170, 220, 255), 2, true);
    wrapText(280, 170, 720, ev.text, GetColor(230, 228, 245), R.fontM(), 28);
    // choice buttons as wide as the longest choice needs (min 700, max 1160), all the same width
    int bw = 700;
    for (auto& c : ev.choices) bw = std::max(bw, PStrWidth(c.c_str(), (int)c.size(), R.fontM()) + 48);
    bw = std::min(bw, 1160);
    // labels include why a choice is disabled / what it will really cost now; button width fits the longest
    std::vector<std::string> labels;
    for (int i = 0; i < (int)ev.choices.size(); ++i) {
        std::string l = ev.choices[i], note = RunLogic::eventChoiceNote(*run_, i);
        const std::string open = "（", close = "）";
        bool reason = !RunLogic::eventChoiceAvailable(*run_, i) && note.rfind(open, 0) == 0 && note.size() > open.size() + close.size();
        // why a choice is closed stands after the label (effect bracket, then a colon and the reason), never inside it;
        // other notes (extra effects) join the effect's bracket with a middle dot
        if (reason)
            l += "：" + note.substr(open.size(), note.size() - open.size() - close.size());
        else if (!note.empty() && l.size() > close.size() && l.compare(l.size() - close.size(), close.size(), close) == 0 && note.rfind(open, 0) == 0)
            l = l.substr(0, l.size() - close.size()) + "・" + note.substr(open.size());
        else l += note;
        labels.push_back(l);
    }
    bw = 700;
    for (auto& c : labels) bw = std::max(bw, PStrWidth(c.c_str(), 0, R.fontM()) + 48);
    bw = std::min(bw, 1240);
    for (int i = 0; i < (int)labels.size(); ++i) button(ID_CHOICE + i, 640 - bw / 2, 390 + i * 64, bw, 52, labels[i].c_str(), RunLogic::eventChoiceAvailable(*run_, i));
    int act = ui.end(in);
    if (act >= ID_CHOICE && act < ID_CHOICE + 5) applyAction({Act::EventChoice, act - ID_CHOICE});
}

void App::doChapterClear() {
    ui.begin();
    char buf[64];
    std::snprintf(buf, sizeof buf, "第%d章 突破！", run_->chapter);
    R.drawText(640, 220, buf, GetColor(255, 220, 120), 3, true);
    if (run_->hp >= run_->maxHp) std::snprintf(buf, sizeof buf, "次の章へ進む（HPは満タン）");
    else std::snprintf(buf, sizeof buf, "次の章へ進むとHPが%d%%回復する", (int)(CHAPTER_HEAL * 100));
    R.drawText(640, 320, buf, GetColor(200, 230, 200), 1, true);
    button(ID_CONTINUE, 540, 420, 200, 52, "先へ進む");
    int act = ui.end(in);
    if (act == ID_CONTINUE) applyAction({Act::NextChapter});
}

// ---------------------------------------------------------------- battle
void App::beginBattle() {
    world_ = std::make_unique<World>();
    world_->init(RunLogic::roomSetup(*run_));
    R.clearFx();
    battleEnd_ = 0;
    paused_ = false;
    if (!opt_.autoplay.empty()) combatAI_ = std::make_unique<Policy>(PolicyConfig::fromName(opt_.autoplay, opt_.skill), seedCounter_++ * 7919u);
}

void App::doBattle() {
    if (!world_) beginBattle();
    World& w = *world_;
    bool human = opt_.autoplay.empty();
    if (human && (in.pressed(KEY_INPUT_ESCAPE) || in.padPressed(PAD_INPUT_8)) && w.result() == 0) paused_ = !paused_;
    // DxLib's active flag stayed TRUE while the window was minimized (gallery check), so ask Windows directly
    HWND hw = GetMainWindowHandle();
    bool focused = GetWindowActiveFlag() != 0 && !IsIconic(hw) && GetForegroundWindow() == hw;
    bool active = focused || !human || (opt_.gallery && !galleryAutoPause_);
    if (!active && human && w.result() == 0) paused_ = true;

    if (!paused_) {
        int steps = human ? 1 : opt_.speed;
        bool done = false;
        for (int k = 0; k < steps && !done; ++k) {
            InputState is = human ? in.makeState(w.player.pos) : combatAI_->think(w);
            w.update(is);
            if (!profile_.settings.shake) w.shake = 0;
            R.onEvents(w);
            R.update();
            done = w.result() != 0 && ++battleEnd_ > (w.result() == 1 ? 110 : 160);
        }
        if (done) {
            RoomResult res = w.makeResult();
            profile_.enemiesSeen |= res.enemiesSeen;
            world_.reset();
            Action a{Act::BattleDone};
            a.result = &res;
            applyAction(a);
            return;
        }
    }
    R.draw(w, in.mouseLogical(), debug_, in.usingPad());   // autopilot shows keyboard hints (store screenshots)
    if (paused_) {
        SetDrawBlendMode(DX_BLENDMODE_ALPHA, 160);
        DrawBox(0, 0, 1280, 720, GetColor(0, 0, 0), TRUE);
        SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
        ui.begin();
        panel(470, 170, 340, 320, GetColor(150, 130, 220));   // a dialog box: the hero under the menu no longer reads as touching a button
        R.drawText(640, 200, "ポーズ", GetColor(255, 255, 255), 3, true);
        button(ID_RESUME, 510, 300, 260, 48, "再開");
        button(ID_OPTIONS, 510, 360, 260, 48, "オプション");
        button(ID_ABANDON, 510, 420, 260, 48, "冒険をあきらめる");
        bool cancel = false;
        int act = ui.end(in, &cancel);
        if (act == ID_RESUME) { paused_ = false; }
        else if (act == ID_OPTIONS) { optionsBack_ = Screen::Run; screen_ = Screen::Options; }
        else if (act == ID_ABANDON) { run_->phase = Phase::Defeat; world_.reset(); endRun(false); }
    }
}

// ---------------------------------------------------------------- autopilot
int App::autoIdFor(const Action& a) {
    switch (a.type) {
    case Act::ChooseNode: return ID_NODE + a.a;
    case Act::TakeCard: return ID_CARD + a.a;
    case Act::SkipCard: return ID_SKIP;
    case Act::TakeRelic: return ID_RELIC + a.a;
    case Act::ShopBuyCard: return ID_CARD + a.a;
    case Act::ShopBuyRelic: return ID_RELIC + a.a;
    case Act::ShopPotion: return ID_POTION;
    case Act::ShopRemove: return pick_ == Pick::Remove ? ID_PICK + a.a : ID_REMOVE;
    case Act::ShopUpgrade: return pick_ == Pick::Upgrade ? ID_PICK + a.a : ID_UPGRADE;
    case Act::Leave: return ID_LEAVE;
    case Act::ForgeAnchor:
        if (pick_ == Pick::Anchor1) return ID_PICK + a.a;
        if (pick_ == Pick::Anchor2) return ID_PICK + a.b;
        return ID_ANCHOR;
    case Act::ForgeRankUp:
        if (pick_ == Pick::RankUp1) return ID_PICK + a.a;
        if (pick_ == Pick::RankUp2) return ID_PICK + a.b;
        return ID_RANKUP;
    case Act::ForgeTrain: return pick_ == Pick::Train ? ID_PICK + a.a : ID_TRAIN_FORGE;
    case Act::RestHeal: return ID_HEAL;
    case Act::RestTrain: return pick_ == Pick::RestTrain ? ID_PICK + a.a : ID_TRAIN;
    case Act::EventChoice: return ID_CHOICE + a.a;
    case Act::NextChapter: return ID_CONTINUE;
    default: return -1;
    }
}

void App::autopilotMenus() {
    if (++autoDelay_ < (opt_.screenShots ? 40 : (opt_.speed > 1 ? 6 : 20))) return;   // act at a visible pace so screenshots capture every screen
    autoDelay_ = 0;
    switch (screen_) {
    case Screen::Title: ui.request(hasSave_ ? 2 /*ID_CONTINUE*/ : 1 /*ID_NEW*/); break;
    case Screen::Result: ui.request(90 /*ID_BACK*/); break;
    case Screen::Hub: ui.request(91); break;
    case Screen::Options: ui.request(90); break;
    case Screen::Run: {
        if (!run_ || run_->phase == Phase::Battle) break;
        if (pick_ == Pick::View) { ui.request(ID_PICK_CANCEL); break; }
        Action a = runAI_->decide(*run_);
        int id = autoIdFor(a);
        if (id >= 0) ui.request(id);
        if (a.type == Act::Leave && run_->phase == Phase::Reward) ui.request(ID_SKIP);
        break;
    }
    }
}
