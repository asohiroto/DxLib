#pragma once
#include <string>
#include "core/InputState.h"
#include "core/Rng.h"

class World;

struct PolicyConfig {
    bool useCards = true;
    bool fuse = true;
    bool evolve = true;
    bool randomCards = false;  // pick legal card actions at random
    bool fuseAlways = false;   // fuse whenever any pair is valid (no judgement)
    float skill = 0.6f;        // 0..1 : reaction delay, aim error, dodge success
    static PolicyConfig fromName(const std::string& name, float skill);
};

// AI player used for headless balance runs and in-game autoplay.
class Policy {
public:
    Policy(const PolicyConfig& cfg, uint32_t seed) : cfg_(cfg), rng_(seed), attentionSalt_(seed * 0x9E3779B9u) {}
    InputState think(const World& w);
    int stats_fuseChosen = 0, stats_fuseSkipped = 0;   // decisions where fusing was / was not worth it

private:
    float cardValue(const World& w, int slot, Vec2* aimAt) const;
    PolicyConfig cfg_;
    Rng rng_;
    int decideT_ = 0;
    int dodgeT_ = -1;          // frames until a scheduled dash
    Vec2 strafeSign_{1, 0};
    int strafeT_ = 0;
    Vec2 lastAim_;
    int pendingCardAim_ = 0;   // frames to keep aiming at card target
    uint32_t attentionSalt_ = 0;
};
