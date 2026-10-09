#pragma once
#include <vector>
#include "core/Vec2.h"

class InputDx;

// Immediate-mode focus UI shared by every menu screen.
// Each frame a screen calls begin(), add() for every selectable item, then end().
// Navigation: arrows/WASD/D-pad move focus spatially, mouse hover focuses, click/Enter/A activates.
// An autopilot can request an item id; it is focused and activated through the same path.
struct UiItem {
    int id;
    float x, y, w, h;
    bool enabled;
};

class Ui {
public:
    void begin();
    void add(int id, float x, float y, float w, float h, bool enabled = true);
    // Returns the activated item id, or -1. `cancelled` is set when Esc / B / right click is pressed.
    int end(const InputDx& in, bool* cancelled = nullptr);
    bool focused(int id) const { return focus_ == id; }
    bool hovered(int id) const { return focus_ == id; }
    int focus() const { return focus_; }
    void setFocus(int id) { focus_ = id; }
    // autopilot: activate this id on the next end() (if it exists and is enabled)
    void request(int id) { request_ = id; }
    void requestCancel() { requestCancel_ = true; }
    // ignore player confirm/cancel for a few frames after a screen change (autopilot still works)
    void lockFor(int frames) { lock_ = frames; }
    // the screen already used this frame's arrow key (e.g. to scroll): skip spatial navigation once
    void consumeNav() { skipNav_ = true; }
    const std::vector<UiItem>& items() const { return items_; }
    bool has(int id) const;
    int neighbor(int fromId, Vec2 dir) const;
    float pulse() const { return pulse_; }
    int generation() const { return generation_; }   // bumps on every begin(): tells whether items() belong to this frame

private:
    std::vector<UiItem> items_;
    int focus_ = -1;
    int request_ = -1;
    bool requestCancel_ = false;
    float pulse_ = 0;
    int lock_ = 0;
    int generation_ = 0;
    bool skipNav_ = false;
};
