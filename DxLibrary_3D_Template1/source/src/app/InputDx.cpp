#include "app/InputDx.h"
#include "DxLib.h"
#include <cstring>
#include <cmath>

void InputDx::poll() {
    std::memcpy(prev_, cur_, sizeof(cur_));
    GetHitKeyStateAll(cur_);
    prevMouse_ = mouse_;
    mouse_ = GetMouseInput();
    wheel_ = GetMouseWheelRotVol();
    int mx, my;
    GetMousePoint(&mx, &my);
    Vec2 raw{(float)mx, (float)my};
    if (dist2(raw, lastMouseRaw_) > 4) usingPad_ = false;
    lastMouseRaw_ = raw;
    // window is 1280x720 client; logical is 640x360
    mouseL_ = Vec2{mx * 0.5f, my * 0.5f};

    if (neutral_) {   // QA gallery: no real devices, only injected keys
        std::memset(cur_, 0, sizeof(cur_));
        for (int k : injected_) cur_[k & 255] = 1;
        injected_.clear();
        mouse_ = 0;
        wheel_ = 0;
        mouseL_ = Vec2{-500, -500};
        prevPad_ = pad_ = 0;
        stickL_ = stickR_ = {};
        usingPad_ = neutralPad_;
        return;
    }
    prevPad_ = pad_;
    pad_ = GetJoypadInputState(DX_INPUT_PAD1);
    DINPUT_JOYSTATE js;
    if (GetJoypadDirectInputState(DX_INPUT_PAD1, &js) == 0) {
        stickL_ = Vec2{js.X / 1000.0f, js.Y / 1000.0f};
        stickR_ = Vec2{js.Rx / 1000.0f, js.Ry / 1000.0f};
        if (stickL_.len() < 0.25f) stickL_ = {};
        if (stickR_.len() < 0.3f) stickR_ = {};
    } else {
        stickL_ = stickR_ = {};
    }
    if (pad_ != 0 || stickL_.len2() > 0 || stickR_.len2() > 0) usingPad_ = true;
}

int InputDx::firstPressedKey() const {
    for (int k = 1; k < 256; ++k)
        if (cur_[k] && !prev_[k] && k != KEY_INPUT_ESCAPE && k != KEY_INPUT_F1 && k != KEY_INPUT_F11 && k != KEY_INPUT_F12) return k;
    return -1;
}

bool InputDx::confirm() const {
    return pressed(KEY_INPUT_RETURN) || pressed(KEY_INPUT_SPACE) || mousePressed(MOUSE_INPUT_LEFT) ||
           padPressed(PAD_INPUT_1);
}
bool InputDx::cancel() const {
    return pressed(KEY_INPUT_ESCAPE) || mousePressed(MOUSE_INPUT_RIGHT) || padPressed(PAD_INPUT_2);
}

InputState InputDx::makeState(Vec2 playerPos) {
    InputState s;
    Vec2 mv;
    if (held(key(0 + 2, KEY_INPUT_A)) || held(KEY_INPUT_LEFT)) mv.x -= 1;
    if (held(key(3, KEY_INPUT_D)) || held(KEY_INPUT_RIGHT)) mv.x += 1;
    if (held(key(0, KEY_INPUT_W)) || held(KEY_INPUT_UP)) mv.y -= 1;
    if (held(key(1, KEY_INPUT_S)) || held(KEY_INPUT_DOWN)) mv.y += 1;
    if (stickL_.len2() > 0) mv = stickL_;
    s.move = mv;

    if (usingPad_) {
        static Vec2 lastDir{1, 0};
        if (stickR_.len2() > 0) lastDir = stickR_.norm();
        s.aim = playerPos + lastDir * 80;
    } else {
        s.aim = mouseL_;
    }
    // Pad: RT (button 8?) mapping differs per pad; use buttons generically.
    // PAD_INPUT_1=A 2=B 3=X 4=Y 5=LB 6=RB 7=Back 8=Start 9=LS 10=RS
    s.attack = (mouse_ & MOUSE_INPUT_LEFT) != 0 || (usingPad_ && stickR_.len2() > 0);
    s.dash = pressed(key(4, KEY_INPUT_SPACE)) || pressed(KEY_INPUT_LSHIFT) || padPressed(PAD_INPUT_1);
    if (wheel_ > 0) s.cursorDelta = -1;
    if (wheel_ < 0) s.cursorDelta = 1;
    if (padPressed(PAD_INPUT_5)) s.cursorDelta = -1;
    if (padPressed(PAD_INPUT_6)) s.cursorDelta = 1;
    if (pressed(key(5, KEY_INPUT_1))) s.useSlot = 0;
    if (pressed(key(6, KEY_INPUT_2))) s.useSlot = 1;
    if (pressed(key(7, KEY_INPUT_3))) s.useSlot = 2;
    if (pressed(key(8, KEY_INPUT_4))) s.useSlot = 3;
    s.useSelected = mousePressed(MOUSE_INPUT_RIGHT) || padPressed(PAD_INPUT_3);
    s.fuse = pressed(key(9, KEY_INPUT_Q)) || padPressed(PAD_INPUT_4);
    if (pressed(key(10, KEY_INPUT_Z))) s.fuseSlot = 0;
    if (pressed(key(11, KEY_INPUT_X))) s.fuseSlot = 1;
    if (pressed(key(12, KEY_INPUT_C))) s.fuseSlot = 2;
    s.evolve = pressed(key(13, KEY_INPUT_E)) || padPressed(PAD_INPUT_2);
    return s;
}
