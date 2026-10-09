#pragma once
#include "game/RunState.h"
#include "sim/Policy.h"

// Decides out-of-combat actions (map, rewards, shop, forge, rest, events) for automated runs.
struct RunPolicyConfig {
    bool anchor = true;      // uses forge anchoring
    bool random = false;     // random legal choices
};

class RunPolicy {
public:
    RunPolicy(const RunPolicyConfig& cfg, uint32_t seed) : cfg_(cfg), rng_(seed) {}
    Action decide(const RunState& r);   // never called in Phase::Battle
private:
    float cardScore(int def, const RunState& r) const;
    RunPolicyConfig cfg_;
    Rng rng_;
};
