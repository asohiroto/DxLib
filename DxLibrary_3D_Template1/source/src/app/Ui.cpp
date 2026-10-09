#include "app/Ui.h"
#include "app/InputDx.h"
#include "app/Sound.h"
#include "DxLib.h"
#include "app/Probe.h"
#include <string>
#include <cmath>

void Ui::begin() {
    items_.clear();
    ++generation_;
    pulse_ += DT;
}

void Ui::add(int id, float x, float y, float w, float h, bool enabled) {
    items_.push_back({id, x, y, w, h, enabled});
    if (Probe::enabled) Probe::add(Probe::Kind::Hit, (int)x, (int)y, (int)w, (int)h, std::to_string(id));
}

bool Ui::has(int id) const {
    for (auto& it : items_) if (it.id == id) return true;
    return false;
}

// Spatial navigation: the nearest item in direction `dir` from item `fromId` (-1 if none).
int Ui::neighbor(int fromId, Vec2 dir) const {
    const UiItem* cur = nullptr;
    for (auto& it : items_) if (it.id == fromId) cur = &it;
    if (!cur) return -1;
    Vec2 c{cur->x + cur->w / 2, cur->y + cur->h / 2};
    float best = 1e9f;
    int bestId = -1;
    for (auto& it : items_) {
        if (it.id == fromId) continue;
        Vec2 o{it.x + it.w / 2, it.y + it.h / 2};
        Vec2 d = o - c;
        float along = d.dot(dir);
        if (along <= 4) continue;
        float side = std::fabs(d.dot(Vec2{-dir.y, dir.x}));
        float score = along + side * 2.0f;
        if (score < best) { best = score; bestId = it.id; }
    }
    return bestId;
}

int Ui::end(const InputDx& in, bool* cancelled) {
    if (cancelled) *cancelled = false;
    if (items_.empty()) {
        if (requestCancel_ || in.cancel()) { requestCancel_ = false; if (cancelled) *cancelled = true; }
        return -1;
    }
    // keep focus valid
    if (!has(focus_)) {
        focus_ = -1;
        for (auto& it : items_) if (it.enabled) { focus_ = it.id; break; }
        if (focus_ < 0) focus_ = items_[0].id;
    }
    // autopilot
    if (request_ >= 0) {
        int id = request_;
        request_ = -1;
        for (auto& it : items_)
            if (it.id == id && it.enabled) { focus_ = id; Sound::play(Sfx::Confirm, 0.6f); return id; }
    }
    if (requestCancel_) { requestCancel_ = false; if (cancelled) *cancelled = true; return -1; }
    if (lock_ > 0) { --lock_; return -1; }

    // mouse hover / click
    Vec2 m = in.mouseLogical() * 2.0f;
    if (!in.usingPad()) {
        for (auto& it : items_)
            if (m.x >= it.x && m.x <= it.x + it.w && m.y >= it.y && m.y <= it.y + it.h) {
                if (focus_ != it.id) { focus_ = it.id; }
                if (in.mousePressed(MOUSE_INPUT_LEFT)) {
                    if (it.enabled) { Sound::play(Sfx::Confirm, 0.7f); return it.id; }
                    Sound::play(Sfx::NoMana, 0.5f);
                    return -1;
                }
            }
    }
    // directional navigation
    Vec2 dir;
    if (in.pressed(KEY_INPUT_UP) || in.pressed(KEY_INPUT_W) || in.padPressed(PAD_INPUT_UP)) dir = {0, -1};
    else if (in.pressed(KEY_INPUT_DOWN) || in.pressed(KEY_INPUT_S) || in.padPressed(PAD_INPUT_DOWN)) dir = {0, 1};
    else if (in.pressed(KEY_INPUT_LEFT) || in.pressed(KEY_INPUT_A) || in.padPressed(PAD_INPUT_LEFT)) dir = {-1, 0};
    else if (in.pressed(KEY_INPUT_RIGHT) || in.pressed(KEY_INPUT_D) || in.padPressed(PAD_INPUT_RIGHT)) dir = {1, 0};
    if (skipNav_) { skipNav_ = false; dir = {}; }
    if (dir.len2() > 0) {
        int next = neighbor(focus_, dir);
        if (next >= 0) { focus_ = next; Sound::play(Sfx::Select, 0.35f); }
    }
    if (in.pressed(KEY_INPUT_RETURN) || in.pressed(KEY_INPUT_SPACE) || in.padPressed(PAD_INPUT_1)) {
        for (auto& it : items_)
            if (it.id == focus_) {
                if (it.enabled) { Sound::play(Sfx::Confirm, 0.7f); return it.id; }
                Sound::play(Sfx::NoMana, 0.5f);
            }
    }
    if (in.cancel() && cancelled) { *cancelled = true; Sound::play(Sfx::Cancel, 0.6f); }
    return -1;
}
