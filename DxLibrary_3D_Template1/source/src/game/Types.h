#pragma once
#include <cstdint>

enum class Elem : uint8_t { None, Fire, Ice, Thunder, Earth, Count };
enum class Form : uint8_t { Bolt, Area, Melee, Place, Guard };

// Reaction produced by combining two elements on one card.
enum class Reaction : uint8_t {
    None,
    Steam,       // fire + ice
    Blast,       // fire + thunder
    Lava,        // fire + earth
    Supercond,   // ice + thunder
    Permafrost,  // ice + earth
    Magnet,      // thunder + earth
    Purify,      // same element twice
    Resonance,   // with a neutral card
    Count
};

const char* elemName(Elem e);
const char* reactionName(Reaction r);
const char* reactionDesc(Reaction r);
Reaction reactionOf(Elem a, Elem b);
