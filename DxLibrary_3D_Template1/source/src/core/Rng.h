#pragma once
#include <cstdint>

// mulberry32: small, fast, deterministic. State is a single uint32 so it can be saved.
struct Rng {
    uint32_t state = 1;
    Rng() = default;
    explicit Rng(uint32_t seed) : state(seed ? seed : 0x9E3779B9u) {}
    uint32_t next() {
        uint32_t z = (state += 0x6D2B79F5u);
        z = (z ^ (z >> 15)) * (z | 1u);
        z ^= z + (z ^ (z >> 7)) * (z | 61u);
        return z ^ (z >> 14);
    }
    float f01() { return (next() >> 8) * (1.0f / 16777216.0f); }
    float range(float lo, float hi) { return lo + (hi - lo) * f01(); }
    int irange(int lo, int hiInclusive) { return lo + (int)(next() % (uint32_t)(hiInclusive - lo + 1)); }
    bool chance(float p) { return f01() < p; }
};
