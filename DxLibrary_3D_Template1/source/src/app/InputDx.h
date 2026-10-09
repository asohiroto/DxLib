#pragma once
#include "core/InputState.h"
#include <vector>

// Keyboard / mouse / gamepad -> InputState (with edge detection).
class InputDx {
public:
    void poll();                       // call once per frame
    InputState makeState(Vec2 playerPos);
    void setBindings(const int* keys) { keys_ = keys; }
    int firstPressedKey() const;   // for key rebinding, -1 if none
    bool pressed(int key) const { return cur_[key] && !prev_[key]; }
    bool held(int key) const { return cur_[key] != 0; }
    bool mousePressed(int btn) const { return (mouse_ & btn) && !(prevMouse_ & btn); }
    bool padPressed(int btn) const { return (pad_ & btn) && !(prevPad_ & btn); }
    Vec2 mouseLogical() const { return mouseL_; }
    int wheel() const { return wheel_; }
    bool usingPad() const { return usingPad_; }
    // Any "confirm" (Enter / Space / click / pad A)
    bool confirm() const;
    bool cancel() const;

private:
    char cur_[256] = {}, prev_[256] = {};
    int mouse_ = 0, prevMouse_ = 0;
    int pad_ = 0, prevPad_ = 0;
    int wheel_ = 0;
    Vec2 mouseL_{320, 180};
    Vec2 stickR_;
    Vec2 stickL_;
    bool usingPad_ = false;
    Vec2 lastMouseRaw_;
    const int* keys_ = nullptr;
    bool neutral_ = false, neutralPad_ = false;
    std::vector<int> injected_;
public:
    // QA gallery: ignore real devices; `pad` decides which button hints are shown. inject() presses a key for one poll.
    void setNeutral(bool on, bool pad = false) { neutral_ = on; neutralPad_ = pad; usingPad_ = pad; }
    void inject(int key) { injected_.push_back(key); }
private:
    int key(int bind, int def) const { return keys_ ? keys_[bind] : def; }
};
