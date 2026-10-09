#include "app/Probe.h"
#include <algorithm>
#include <cstdio>

namespace Probe {
bool enabled = false;
namespace {
std::vector<Rect> g_rects;
int g_order = 0;
int g_clip[4] = {0, 0, 1280, 720};
bool g_overlay = false;

const char* kindName(Kind k) {
    switch (k) {
    case Kind::Text: return "text";
    case Kind::Panel: return "panel";
    case Kind::Button: return "button";
    case Kind::Card: return "card";
    case Kind::Hit: return "hitbox";
    }
    return "?";
}
int interArea(const Rect& a, const Rect& b) {
    int x0 = std::max(a.x, b.x), y0 = std::max(a.y, b.y);
    int x1 = std::min(a.x + a.w, b.x + b.w), y1 = std::min(a.y + a.h, b.y + b.h);
    return (x1 > x0 && y1 > y0) ? (x1 - x0) * (y1 - y0) : 0;
}
bool contains(const Rect& outer, const Rect& in, int slack = 1) {
    return in.x >= outer.x - slack && in.y >= outer.y - slack && in.x + in.w <= outer.x + outer.w + slack &&
           in.y + in.h <= outer.y + outer.h + slack;
}
std::string desc(const Rect& r) {
    char buf[200];
    std::string l = r.label.size() > 40 ? r.label.substr(0, 40) + "…" : r.label;
    std::snprintf(buf, sizeof buf, "%s(%d,%d %dx%d)%s%s", kindName(r.kind), r.x, r.y, r.w, r.h, l.empty() ? "" : " ", l.c_str());
    return buf;
}
// what part of a text survives the clip rectangle
Rect visible(const Rect& r) {
    Rect v = r;
    int x0 = std::max(r.x, r.clipX0), y0 = std::max(r.y, r.clipY0);
    int x1 = std::min(r.x + r.w, r.clipX1), y1 = std::min(r.y + r.h, r.clipY1);
    v.x = x0; v.y = y0; v.w = std::max(0, x1 - x0); v.h = std::max(0, y1 - y0);
    return v;
}
}  // namespace

void beginFrame() {
    g_rects.clear();
    g_order = 0;
    g_overlay = false;
    g_clip[0] = 0; g_clip[1] = 0; g_clip[2] = 1280; g_clip[3] = 720;
}

void setOverlay(bool on) { g_overlay = on; }

void setClip(int x0, int y0, int x1, int y1) { g_clip[0] = x0; g_clip[1] = y0; g_clip[2] = x1; g_clip[3] = y1; }

void add(Kind k, int x, int y, int w, int h, const std::string& label, unsigned color) {
    g_rects.push_back({k, x, y, w, h, label, g_order++, g_clip[0], g_clip[1], g_clip[2], g_clip[3], color, g_overlay});
}

const std::vector<Rect>& rects() { return g_rects; }

int analyze(const std::string& screen, std::vector<std::string>& out) {
    int n = 0;
    auto report = [&](const char* what, const std::string& detail) {
        out.push_back(screen + "\t" + what + "\t" + detail);
        ++n;
    };
    const auto& R = g_rects;
    for (size_t i = 0; i < R.size(); ++i) {
        const Rect& a = R[i];
        if (a.kind == Kind::Text) {
            Rect va = visible(a);
            // off screen / truncated by a clip rectangle
            if (a.x < 0 || a.y < 0 || a.x + a.w > 1280 || a.y + a.h > 720) report("off-screen", desc(a));
            else if (va.w < a.w - 1 || va.h < a.h - 1) report("truncated", desc(a));
            if (va.w <= 0 || va.h <= 0) continue;
            for (size_t j = i + 1; j < R.size(); ++j) {
                const Rect& b = R[j];
                if (b.kind == Kind::Hit || b.overlay != a.overlay) continue;
                if (b.kind == Kind::Text) {
                    Rect vb = visible(b);
                    int ia = interArea(va, vb);
                    int smaller = std::min(va.w * va.h, vb.w * vb.h);   // ("small" is a Windows macro)
                    if (smaller > 0 && ia * 100 > smaller * 15) report("text-overlap", desc(a) + "  <->  " + desc(b));
                    else if (ia == 0 && vb.w > 0) {
                        // two separate texts on one line with (almost) no gap read as one word
                        int vov = std::min(va.y + va.h, vb.y + vb.h) - std::max(va.y, vb.y);
                        int gap = std::max(vb.x - (va.x + va.w), va.x - (vb.x + vb.w));
                        if (vov * 2 > std::min(va.h, vb.h) && gap >= 0 && gap < 10) report("text-touching", desc(a) + "  <->  " + desc(b));
                    }
                } else {
                    // a box drawn after the text covers part of it
                    int ia = interArea(va, b);
                    if (ia > 0 && !contains(b, va)) report("covered-by-box", desc(a) + "  under  " + desc(b));
                    else if (ia > 0 && contains(b, va) && b.kind != Kind::Panel) report("covered-by-box", desc(a) + "  under  " + desc(b));
                }
            }
            // text straddling the edge of a box drawn before it
            for (size_t j = 0; j < i; ++j) {
                const Rect& b = R[j];
                if (b.kind == Kind::Text || b.kind == Kind::Hit || b.overlay != a.overlay) continue;
                int ia = interArea(va, b);
                if (ia > 0 && !contains(b, va) && ia * 100 > va.w * va.h * 10) report("crosses-box-edge", desc(a) + "  over  " + desc(b));
            }
        } else if (a.kind == Kind::Panel || a.kind == Kind::Card || a.kind == Kind::Button) {
            // a later box partially covering this one (a background panel that fully contains it is fine)
            for (size_t j = i + 1; j < R.size(); ++j) {
                const Rect& b = R[j];
                if (b.kind == Kind::Text || b.kind == Kind::Hit || b.overlay != a.overlay) continue;
                int ia = interArea(a, b);
                if (ia <= 0 || contains(a, b) || (contains(b, a, 0) && b.kind == Kind::Panel)) continue;
                if (ia > 16) report("box-overlap", desc(b) + "  covers  " + desc(a));
            }
        } else if (a.kind == Kind::Hit) {
            // the clickable area must match something drawn at that spot (card, button or whole tile)
            const Rect* closest = nullptr;   // ("near" is a Windows macro)
            bool match = false;
            for (const Rect& b : R) {
                if (b.kind != Kind::Card && b.kind != Kind::Button) continue;
                if (std::abs(b.x - a.x) <= 2 && std::abs(b.y - a.y) <= 2) {
                    if (std::abs(b.w - a.w) <= 4 && std::abs(b.h - a.h) <= 4) match = true;
                    else if (!closest) closest = &b;
                }
            }
            if (!match && closest) report("hitbox-mismatch", desc(a) + "  vs drawn  " + desc(*closest));
        }
    }
    return n;
}
}  // namespace Probe

// ---- text layout with ink-width letters and digits (see Probe.h) ----
#include <map>
namespace {
struct InkInfo { int left, adv, ink, top, h; };
// converted ASCII letters/digits only; full-width punctuation and Japanese keep the font's own advance
bool tightGlyph(unsigned c) { return (c >= '0' && c <= '9') || (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'); }
const InkInfo& inkOf(int font, unsigned c) {
    static std::map<long long, InkInfo> cache;
    long long key = ((long long)font << 8) | c;
    auto it = cache.find(key);
    if (it != cache.end()) return it->second;
    char one[2] = {(char)c, 0};
    std::string w = toWide(one);
    int dx = 0, dy = 0, next = 0, sx = 0, sy = 0;
    InkInfo info{0, GetDrawStringWidthToHandle(w.c_str(), (int)w.size(), font), 0, 0, GetFontSizeToHandle(font)};
    info.ink = info.adv;
    if (GetFontCharInfo(font, w.c_str(), &dx, &dy, &next, &sx, &sy) == 0 && sx > 0 && sx < info.adv) {
        int gap = std::max(1, GetFontSizeToHandle(font) / 8);   // 1px at 12px, 3px at 24px
        info.left = dx;
        info.adv = dx + sx + gap;
        info.ink = sx;
        if (sy > 0) { info.top = dy; info.h = sy; }
    }
    return cache.emplace(key, info).first->second;
}
// Lays the string out as runs: plain runs drawn by DxLib in one call, tight glyphs placed one by one.
template <class F> int layout(const char* s, int font, F&& emit) {
    int x = 0;
    std::string run;
    auto flush = [&] {
        if (run.empty()) return;
        std::string w = toWide(run.c_str());
        emit(x, w);
        x += GetDrawStringWidthToHandle(w.c_str(), (int)w.size(), font);
        run.clear();
    };
    for (const unsigned char* p = (const unsigned char*)s; p && *p; ++p) {
        if (tightGlyph(*p)) {
            flush();
            char one[2] = {(char)*p, 0};
            emit(x, toWide(one));
            x += inkOf(font, *p).adv;
        } else {
            run += (char)*p;
        }
    }
    flush();
    return x;
}
}  // namespace

int PStrWidth(const char* s, int, int font) {
    return layout(s, font, [](int, const std::string&) {});
}
int PDrawString(int x, int y, const char* s, unsigned col, int font, unsigned edge) {
    if (!s || !*s) return 0;
    int tw = layout(s, font, [&](int ox, const std::string& w) { DrawStringToHandle(x + ox, y, w.c_str(), col, font, edge); });
    if (Probe::enabled) Probe::add(Probe::Kind::Text, x, y, tw, GetFontSizeToHandle(font), s, col);
    return 0;
}
void PInkCenter(const char* s, int font, int& cx, int& cy) {
    if (!s || !*s || s[1] || !tightGlyph((unsigned char)s[0])) { cx = PStrWidth(s, 0, font) / 2; cy = GetFontSizeToHandle(font) / 2; return; }
    const InkInfo& i = inkOf(font, (unsigned char)s[0]);
    cx = i.left + i.ink / 2;
    cy = i.top + i.h / 2;
}