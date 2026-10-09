#pragma once

// Tunables that relics and ascension can change. World reads these instead of constants.
struct Modifiers {
    // player
    float manaMax = 6.0f;
    float manaRegen = 1.0f / 1.4f;     // per second
    float manaStart = 3.0f;
    float dashCd = 0.6f;
    float basicDmg = 6.0f;
    int basicShots = 1;
    float startShield = 0;
    // cards
    int masteryCap = 8;
    float fusedPowerMul = 1.0f;        // extra multiplier on in-combat fusions
    int fusedCostDelta = 0;            // cost change of in-combat fusions (min 1)
    float weakMul = 1.75f;
    int burnCap = 6;
    float freezeTimeMul = 1.0f;
    bool startRankUp = false;          // one card in the opening hand starts at +1 rank
    int killHealCap = 0;               // heal 1 HP per kill, up to this many per room
    float burnDmgMul = 1.0f;
    int chillBonus = 0;                // extra chill stacks from ice hits
    float chainMul = 1.0f;             // thunder chain damage
    float earthPowerMul = 1.0f;
    float refillMul = 1.0f;            // hand refill delay multiplier
    // enemies (ascension)
    float enemyHpMul = 1.0f;
    float enemyDmgMul = 1.0f;
    // economy
    float goldMul = 1.0f;
};
