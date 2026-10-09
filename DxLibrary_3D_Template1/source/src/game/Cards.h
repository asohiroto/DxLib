#pragma once
#include <string>
#include <vector>
#include "game/Types.h"

// Static definition loaded from data/cards.tsv
struct CardDef {
    std::string id, name, desc, special, evolveId;
    Form form = Form::Bolt;
    Elem elem = Elem::None;
    int cost = 1;
    float power = 10;
    int count = 1;
    float spread = 0;    // degrees (bolt fan / melee arc)
    float speed = 0;
    int pierce = 0;
    float radius = 0;
    float duration = 0;
    float tick = 0;
    int status = 0;      // status stacks applied per hit
    float range = 0;
    float knock = 0;
    int evolveTo = -1;   // index of evolved def, -1 = none
    bool isEvolved = false;
    char rarity = 'c';     // c / u / r ; '-' = not offered (evolved forms)
    bool locked = false;   // needs a meta unlock before it appears in rewards
};

namespace CardDB {
bool load(const std::string& path, std::string* err);
const std::vector<CardDef>& all();
const CardDef& get(int idx);
int find(const std::string& id);   // -1 if missing
}

constexpr int MASTERY_CAP = 8;
constexpr int MAX_MATERIALS = 4;
constexpr int MAX_COST = 4;
constexpr float FUSE_POWER_MUL = 0.65f;
constexpr float RESONANCE_MUL = 1.3f;
constexpr float RESIST_MUL = 0.4f;
float rankMul(int rank);

// A card owned by the player during a run.
struct CardInstance {
    uint32_t uid = 0;
    int def = -1;            // current def (evolved def after evolution)
    int rank = 1;            // 1..3
    Elem elems[2] = {Elem::None, Elem::None};
    bool fused = false;
    float fusedPower = 0;    // used when fused
    int fusedCost = 0;
    Reaction reaction = Reaction::None;
    int mastery = 0;
    bool anchored = false;
    bool tempBoost = false;  // +1 rank for this room only (lucky_star); never saved   // permanent fusion made at a forge: never decomposes, cannot evolve or be fused
    std::vector<CardInstance> materials;  // flat list of unfused cards (fused only)

    const CardDef& d() const { return CardDB::get(def); }
    float power() const;
    int cost() const;
    bool canEvolve(int cap = MASTERY_CAP) const { return materials.empty() && !anchored && d().evolveTo >= 0 && mastery >= cap; }
    // Cards built in combat (rank-up or hybrid) keep their materials and
    // decompose at the end of the room unless the player "anchors" them.
    bool isComposite() const { return !materials.empty(); }
    int materialCount() const { return materials.empty() ? 1 : (int)materials.size(); }
    std::string displayName() const;
    Elem mainElem() const { return elems[0]; }
};

CardInstance makeCard(int def, uint32_t uid);

enum class FuseKind { Invalid, RankUp, Hybrid };
struct FusePreview {
    FuseKind kind = FuseKind::Invalid;
    CardInstance result;
    const char* reason = "";
};

// Pure function: what would fusing L (left) and R (right) produce?
FusePreview previewFuse(const CardInstance& L, const CardInstance& R, uint32_t newUid);
// Evolve in place. Returns false if not possible.
bool evolveCard(CardInstance& c, int cap = MASTERY_CAP);
// Exhaustive self-test of fusion rules; returns number of violations.
int fusionSelfTest(std::string* log);
