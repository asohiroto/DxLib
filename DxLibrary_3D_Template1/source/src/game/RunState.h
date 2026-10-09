#pragma once
#include <string>
#include <vector>
#include <cstdint>
#include "game/Cards.h"
#include "game/World.h"

enum class NodeType : uint8_t { Battle, Elite, Shop, Forge, Event, Rest, Boss };
const char* nodeTypeName(NodeType t);

struct MapNode {
    int row = 0, col = 0;
    NodeType type = NodeType::Battle;
    std::vector<int> next;   // indices of nodes in the next row
    int room = -1;           // RoomDef index for battle / elite / boss nodes
};

struct ChapterMap {
    std::vector<MapNode> nodes;
    int rows = 0;
    std::vector<int> starts;   // row-0 node indices
    int boss = -1;
};

enum class Phase : uint8_t { Map, Battle, Reward, Shop, Forge, Rest, Event, ChapterClear, Victory, Defeat };
const char* phaseName(Phase p);

struct RelicDef { std::string id, name, desc; char rarity = 'c'; };
struct EventDef { std::string id, title, text; std::vector<std::string> choices; };

struct Offer {
    std::vector<int> cards;    // card defs
    std::vector<int> relics;   // relic indices
    int gold = 0;
    int goldGained = 0;        // total actually added after the battle (display only, not saved)
    bool cardDone = false, relicDone = false;
};

struct ShopStock {
    std::vector<int> cards, cardPrices;
    std::vector<int> relics, relicPrices;
    std::vector<uint8_t> cardSold, relicSold;
    bool potionSold = false;
};

struct RunStats {
    int kills = 0, fusions = 0, rankUps = 0, evolutions = 0, roomsCleared = 0;
    int goldEarned = 0, goldSpent = 0;
    float damageTaken = 0, healed = 0;
    uint32_t reactions = 0;
    int bossesKilled = 0;
    float battleSeconds = 0;
    int elitesKilled = 0;
    int shopSpendCards = 0, shopSpendRelics = 0, shopSpendRemove = 0, shopSpendPotion = 0, shopSpendUpgrade = 0;
};

struct RunState {
    uint32_t seed = 1;
    int ascension = 0;
    int chapter = 1;          // 1..3
    ChapterMap map;
    int node = -1;            // current node (-1 = before the first row)
    Phase phase = Phase::Map;
    std::vector<CardInstance> deck;
    float hp = 80, maxHp = 80;
    int gold = 0;
    std::vector<int> relics;
    int anchors = 0, removals = 0, upgrades = 0;
    uint32_t nextUid = 1;
    Offer offer;
    ShopStock shop;
    int eventId = -1;
    std::vector<int> pool;    // card defs that may appear as rewards
    RunStats stats;

    bool hasRelic(const std::string& id) const;
    uint32_t newUid() { return nextUid++; }
};

enum class Act : uint8_t {
    ChooseNode, BattleDone, TakeCard, SkipCard, TakeRelic,
    ShopBuyCard, ShopBuyRelic, ShopRemove, ShopPotion, ShopUpgrade, Leave,
    ForgeAnchor, ForgeRankUp, ForgeTrain, RestHeal, RestTrain, EventChoice, NextChapter
};
struct Action {
    Act type;
    int a = -1, b = -1;
    const RoomResult* result = nullptr;
};

constexpr int MAX_ANCHORS = 3;
constexpr int POTION_PRICE = 40;
constexpr int REMOVE_BASE = 75, REMOVE_STEP = 25;
constexpr float REST_HEAL = 0.3f, CHAPTER_HEAL = 0.3f;
constexpr int TRAIN_AMOUNT = 4;
constexpr int CHAPTERS = 3;
constexpr int MAP_ROWS = 7;

namespace RunLogic {
bool loadData(const std::string& dir, std::string* err);   // relics / events
const std::vector<RelicDef>& relics();
const std::vector<EventDef>& events();
int findRelic(const std::string& id);

std::vector<int> defaultPool();
RunState newRun(uint32_t seed, int ascension, const std::vector<int>& pool);
ChapterMap generateMap(uint32_t seed, int chapter, int ascension = 0);
std::vector<int> choosableNodes(const RunState& r);
Modifiers modifiersFor(const RunState& r);
RoomSetup roomSetup(const RunState& r);
int removePrice(const RunState& r);
int upgradePrice(const RunState& r);
bool canUpgrade(const CardInstance& c);
// Validates and applies an action. Returns false (and leaves the state untouched) if illegal.
bool apply(RunState& r, const Action& a, std::string* err = nullptr);
// Forge preview for two deck cards (also used by the UI)
FusePreview anchorPreview(const RunState& r, int a, int b);
bool eventChoiceAvailable(const RunState& r, int choice);
// Short note shown after a choice: why it is disabled, or what a cost will really do in the current state.
std::string eventChoiceNote(const RunState& r, int choice);
uint32_t hashRun(const RunState& r);
// QA gallery only: jump into the first node of a type (ignores reachability) / fill a reward as after that room.
bool debugEnter(RunState& r, NodeType t);
void debugReward(RunState& r, NodeType t);
}
