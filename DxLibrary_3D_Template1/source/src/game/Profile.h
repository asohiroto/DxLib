#pragma once
#include <string>
#include <vector>
#include <cstdint>
#include "game/RunState.h"

// Persistent player profile: meta progression, settings and codex.
// Remappable keyboard actions (DxLib KEY_INPUT_* codes)
enum Bind { B_UP, B_DOWN, B_LEFT, B_RIGHT, B_DASH, B_CARD1, B_CARD2, B_CARD3, B_CARD4, B_FUSE, B_FUSE12, B_FUSE23, B_FUSE34, B_EVOLVE, B_COUNT };

struct Settings {
    float bgm = 0.6f, se = 0.8f;
    bool fullscreen = false;
    bool shake = true;
    bool reduceFx = false;      // no screen flashes, weaker shake / hitstop
    bool damageNumbers = true;
    int keys[B_COUNT] = {0x11, 0x1F, 0x1E, 0x20, 0x39, 0x02, 0x03, 0x04, 0x05, 0x10, 0x2C, 0x2D, 0x2E, 0x12};
    //                  W     S     A     D     Space 1     2     3     4     Q     Z     X     C     E
};

struct Profile {
    int shards = 0;
    std::vector<std::string> unlocked;   // card ids unlocked through the hub
    int ascensionUnlocked = 0;           // highest selectable ascension
    int ascension = 0;                   // currently selected
    int runs = 0, wins = 0;
    int bestChapter = 0;
    uint32_t reactionsSeen = 0;          // codex
    uint64_t enemiesSeen = 0;            // codex bitmask by enemy index
    bool tutorialDone = false;
    uint32_t hintsSeen = 0;              // one-time contextual hints
    Settings settings;

    std::vector<int> cardPool() const;   // default pool + unlocked cards
    static int unlockCost(const CardDef& d) { return d.rarity == 'r' ? 60 : 40; }
    // shards granted when a run ends
    static int shardsFor(const RunState& r, bool victory);
    void recordRun(const RunState& r, bool victory);
};

namespace ProfileIO {
std::string serialize(const Profile& p);
bool deserialize(const std::string& text, Profile& out);
}
