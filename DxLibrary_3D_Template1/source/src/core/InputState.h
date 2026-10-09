#pragma once
#include "core/Vec2.h"

// One frame of player intent. Produced by keyboard/pad, scripts, or AI policies.
// All "pressed" flags are edges (true only on the frame the button went down).
struct InputState {
    Vec2 move;              // -1..1 each axis
    Vec2 aim;               // aim target in world coordinates
    bool attack = false;    // held
    bool dash = false;      // pressed
    int cursorDelta = 0;    // -1 / 0 / +1
    int cursorSet = -1;     // set cursor directly (0..3), -1 = none
    int useSlot = -1;       // use card in slot directly (0..3), -1 = none
    bool useSelected = false;
    bool fuse = false;          // fuse the cursor pair
    int fuseSlot = -1;          // fuse pair (fuseSlot, fuseSlot+1) directly
    bool evolve = false;
};
