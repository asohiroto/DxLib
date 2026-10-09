#pragma once
// Layout probe for the --gallery QA mode: every text, panel, button and card the game draws records its
// rectangle here, so overlaps / overflows / hit-box mismatches can be detected in code instead of by eye.
// Disabled (zero work beyond one branch) in normal play.
#include <string>
#include <vector>
#include "DxLib.h"

namespace Probe {
enum class Kind { Text, Panel, Button, Card, Hit };
struct Rect {
    Kind kind;
    int x, y, w, h;
    std::string label;
    int order;           // draw order within the frame
    int clipX0, clipY0, clipX1, clipY1;   // draw area in effect
    unsigned color = 0;
    bool overlay = false;   // notices drawn over everything by design (toast, hint, modal dialog)
};
void setOverlay(bool on);
extern bool enabled;
void beginFrame();
void add(Kind k, int x, int y, int w, int h, const std::string& label = {}, unsigned color = 0);
void setClip(int x0, int y0, int x1, int y1);
const std::vector<Rect>& rects();
// Checks the recorded frame; appends human-readable findings (one per line) and returns their count.
int analyze(const std::string& screen, std::vector<std::string>& out);
}  // namespace Probe

// The bundled pixel font draws ASCII letters, digits and symbols at about 40% of the width/height of Japanese
// glyphs ("%" read as "x", key names unreadable). All UI text therefore draws ASCII as full-width forms
// (U+FF01..U+FF5E); measuring and drawing both go through this so layout math stays consistent.
inline std::string toWide(const char* s) {
    std::string out;
    if (!s) return out;
    for (const unsigned char* p = (const unsigned char*)s; *p; ++p) {
        unsigned c = *p;
        if (c >= 0x21 && c <= 0x7E) {
            unsigned u = 0xFF01 + (c - 0x21);   // 3-byte UTF-8
            out += (char)(0xE0 | (u >> 12));
            out += (char)(0x80 | ((u >> 6) & 0x3F));
            out += (char)(0x80 | (u & 0x3F));
        } else {
            out += (char)c;
        }
    }
    return out;
}
// The font's full-width letters and digits keep their ink at the left of a full-width cell, so "14" read as
// "1 4" and a lone digit sat off-centre. These glyphs are therefore laid out by their ink width instead.
// Measuring and drawing share the same layout.
int PStrWidth(const char* s, int len, int font);
// Drop-in replacements for the DxLib calls the UI uses, so nothing is drawn without being recorded.
int PDrawString(int x, int y, const char* s, unsigned col, int font, unsigned edge = 0);
// Centre of the ink of a one-character string drawn at (0,0) (for centring a lone digit in a badge).
void PInkCenter(const char* s, int font, int& cx, int& cy);
inline int PSetDrawArea(int x0, int y0, int x1, int y1) {
    if (Probe::enabled) Probe::setClip(x0, y0, x1, y1);
    return SetDrawArea(x0, y0, x1, y1);
}
