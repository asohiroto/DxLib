#pragma once
#include <vector>
#include <cstdint>
#include "core/Vec2.h"
#include "core/Rng.h"
#include "core/InputState.h"
#include "game/Cards.h"
#include "game/Content.h"
#include "game/Modifiers.h"

// ---------------------------------------------------------------- arena
constexpr float ARENA_L = 12, ARENA_T = 12, ARENA_R = 628, ARENA_B = 296;

// ---------------------------------------------------------------- fixed tuning (not touched by relics)
constexpr float PLAYER_SPEED = 140.0f;
constexpr float DASH_SPEED = 480.0f;
constexpr float DASH_TIME = 0.18f;
constexpr float DASH_INVULN = 0.25f;
constexpr float BASIC_INTERVAL = 0.28f;
constexpr float BASIC_SPEED = 300.0f;
constexpr float BASIC_RANGE = 140.0f;
constexpr float MANA_ON_BASIC_HIT = 0.1f;
constexpr float REFILL_DELAY = 0.7f;
constexpr int FUSE_COST = 0;
constexpr float FUSE_COOLDOWN = 0.4f;
constexpr float FUSE_CAST_LOCK = 0.25f;
constexpr float EVOLVE_INVULN = 0.4f;
constexpr float EVOLVE_CD = 3.0f;
constexpr float HURT_INVULN = 0.6f;
constexpr float SPAWN_TELEGRAPH = 0.8f;
constexpr float STATUS_MUL_CAP = 3.0f;
constexpr int MAX_PLACED = 2;
constexpr int MAX_ZONES = 14;
constexpr int ROOM_TIMEOUT_FRAMES = 180 * 60;
constexpr int BOSS_TIMEOUT_FRAMES = 300 * 60;
constexpr int HAND_SIZE = 4;
constexpr int MIN_DECK = 6;
constexpr float ELITE_HP = 2.2f, ELITE_SIZE = 1.3f, ELITE_DMG = 1.3f, ELITE_CD = 0.8f;

// ---------------------------------------------------------------- entities
struct Status {
    float burnT = 0; int burnStacks = 0; float burnTick = 0;
    float chill = 0; float frozenT = 0;
    float shockT = 0;
    float weightT = 0;
    float steamT = 0;      // attack slowed
    float slowT = 0;       // movement slowed (permafrost)
};

struct Player {
    Vec2 pos{320, 250};
    Vec2 aimDir{1, 0};
    Vec2 aimPoint{400, 250};
    float r = 6;
    float hp = 100, maxHp = 100;
    float invuln = 0;
    float dashT = 0, dashCd = 0;
    Vec2 dashDir{1, 0};
    float attackCd = 0;
    float shield = 0, shieldT = 0;
    float evolveCd = 0;
    float fuseCd = 0;
    float castLock = 0;
    float hurtFlash = 0;
    bool dead = false;
};

struct Enemy {
    int def = 0;            // index into Content::enemies()
    bool elite = false;
    Vec2 pos, vel, knock;
    float r = 7;
    float hp = 30, maxHp = 30;
    float speed = 50;
    float dmgMul = 1, cdMul = 1;
    int state = 0;
    float t = 0, cd = 0;
    Vec2 dir;
    Status st;
    float spawnT = SPAWN_TELEGRAPH;
    float contactCd = 0;
    float hitFlash = 0;
    uint32_t lastCard = 0;
    bool alive = true;
    int id = 0;
    int phase = 0;          // bosses
    int pattern = 0;        // bosses: attack cycle index
    Vec2 aux;               // AI scratch (jump target, laser dir...)
    Elem adaptResist = Elem::None;   // golem: element it currently resists
    float adaptDmg[(int)Elem::Count] = {};
    float adaptT = 0;
    float guard = 40, guardBreakT = 0;   // shield bearer: front shield durability / broken timer
    const EnemyDef& d() const { return Content::enemies()[def]; }
    AiKind ai() const { return d().ai; }
};

struct Hit {
    float dmg = 0;
    Elem elems[2] = {Elem::None, Elem::None};
    Reaction re = Reaction::None;
    int status = 0;
    float knock = 0;
    Vec2 from;
    uint32_t card = 0;      // card uid for mastery credit (0 = none)
    bool melee = false;
    bool canReact = true;     // may trigger the card's element reaction
    bool primary = true;      // direct hit (thunder chains, kill hitstop)
    bool basic = false;
};

struct Bullet {
    Vec2 pos, vel;
    float r = 3, life = 1;
    Hit hit;
    int pierce = 0;
    float explodeR = 0;
    bool homing = false;
    std::vector<int> hitIds;
    bool alive = true;
};

struct EnemyBullet {
    Vec2 pos, vel;
    float r = 3, dmg = 7, life = 4;
    float age = 0;
    uint32_t id = 0;        // stable id (used by AI "attention" model)
    bool alive = true;
};

enum class ZoneKind : uint8_t { Damage, Steam, Lava, Permafrost };
struct Zone {
    ZoneKind kind = ZoneKind::Damage;
    Vec2 pos;
    float r = 20, life = 1, maxLife = 1, tick = 0.5f, tickT = 0;
    int ticks = 0;
    Hit hit;
    bool alive = true;
};

enum class PlacedKind : uint8_t { Wall, Turret };
struct Placed {
    PlacedKind kind = PlacedKind::Wall;
    Vec2 pos;
    std::vector<Vec2> blocks;  // wall circles
    float blockR = 9;
    float life = 8, maxLife = 8, tick = 0.5f, tickT = 0;
    float range = 130, speed = 300;
    int count = 1;
    float explodeR = 0;
    int volleys = 0;
    Hit hit;
    bool alive = true;
};

// Telegraphed enemy attack: shown for `delay` seconds, then hurts the player if inside.
enum class HazardKind : uint8_t { Circle, Line };
struct Hazard {
    HazardKind kind = HazardKind::Circle;
    Vec2 pos, dir;
    float r = 30;           // circle radius / line half-width
    float len = 0;          // line length
    float t = 0, delay = 1; // t counts up to delay
    float dmg = 10;
    bool fired = false;
    float linger = 0.15f;   // visual after firing
    bool alive = true;
};

// ---------------------------------------------------------------- events (logic -> presentation)
enum class Ev : uint8_t {
    Damage, Kill, PlayerHurt, CardUsed, Fused, Evolved, Explosion, Reaction,
    Spawn, WaveStart, Shatter, Freeze, Dash, BasicShot, Heal, Shield, RoomClear, PlayerDead, NoMana,
    BossPhase, Telegraph
};
struct GameEvent {
    Ev type;
    Vec2 pos;
    float value = 0;
    int a = 0, b = 0;       // generic (elem, reaction, slot...)
};

// ---------------------------------------------------------------- stats
struct RoomStats {
    int frames = 0;
    float damageTaken = 0, healed = 0;
    int cardsUsed = 0, fusions = 0, rankUps = 0, evolutions = 0, kills = 0;
    float cardDamage = 0, basicDamage = 0;
    float fuseGainSum = 0;
    uint32_t reactionsSeen = 0;   // bitmask of reactions triggered
};

// What a finished room hands back to the run. World never modifies run state directly.
struct RoomResult {
    int result = 0;               // 1 cleared, -1 dead, 2 timeout
    float hp = 0;
    std::vector<CardInstance> deck;   // composites decomposed, evolutions/mastery kept
    uint32_t nextUid = 1;
    RoomStats stats;
    int goldDrop = 0;
    uint64_t enemiesSeen = 0;
};

// ---------------------------------------------------------------- hand
struct Hand {
    CardInstance slots[HAND_SIZE];  // def < 0 = empty
    float refillT[HAND_SIZE] = {};
    std::vector<CardInstance> draw, discard, exhausted;
    int cursor = 0;
    bool empty(int i) const { return slots[i].def < 0; }
};

struct RoomSetup {
    std::vector<CardInstance> deck;
    float hp = 100, maxHp = 100;
    uint32_t nextUid = 1;
    const RoomDef* room = nullptr;   // nullptr = empty test arena
    Modifiers mods;
    uint32_t seed = 1;
};

// ---------------------------------------------------------------- world (one room)
class World {
public:
    void init(const RoomSetup& setup);
    void update(const InputState& in);
    RoomResult makeResult() const;

    int result() const { return result_; }   // 0 running, 1 cleared, -1 dead, 2 timeout
    uint32_t stateHash() const;

    // read-only access for renderer / policies
    Player player;
    std::vector<Enemy> enemies;
    std::vector<Bullet> bullets;
    std::vector<EnemyBullet> ebullets;
    std::vector<Zone> zones;
    std::vector<Placed> placed;
    std::vector<Hazard> hazards;
    std::vector<RectF> pillars;
    Hand hand;
    Modifiers mods;
    float mana = 3;
    int wave = 0, waveCount = 0;
    int frame = 0;
    int hitstop = 0;
    float shake = 0;
    float evolveFlash = 0;
    bool isBossRoom = false;
    std::vector<GameEvent> events;
    RoomStats stats;
    Rng rng;

    FusePreview previewPair(int leftSlot) const;
    int pairLeftForCursor() const { return hand.cursor >= HAND_SIZE - 1 ? HAND_SIZE - 2 : hand.cursor; }
    bool isBlocked(Vec2 p, float r) const;
    // tests: a motionless enemy that never attacks
    void debugSpawnDummy(int enemyDef, Vec2 pos, float hp);
    // tutorial / scripted rooms: spawn a normal enemy (with telegraph)
    void spawnAt(int enemyDef, Vec2 pos) { spawnEnemy(enemyDef, false, pos); }
    const Enemy* nearestEnemy(Vec2 p, float maxDist = 1e9f) const;
    const Enemy* boss() const;

private:
    void stepPlayer(const InputState& in);
    void stepHand(const InputState& in);
    void useCard(int slot);
    void doFuse(int leftSlot);
    void advanceCursorFrom(int slot);
    void doEvolve();
    void castCard(const CardInstance& c);
    void stepEnemies();
    void stepEnemyAI(Enemy& e, float spd, float atkMul, Vec2 toP, float dP, Vec2 dirP, Vec2& move);
    void stepBullets();
    void stepHazards();
    void addHazard(const Hazard& h) { hazards.push_back(h); emit(Ev::Telegraph, h.pos, h.r, (int)h.kind); }
    int countAlive(int def) const;
    void stepZones();
    void stepPlaced();
    void stepWaves();
    void cleanup();

    void spawnEnemy(int def, bool elite, Vec2 pos);
    void fireEnemyBullet(Vec2 pos, Vec2 vel, float dmg, float r = 3, float life = 4);
    Vec2 randomSpawnPos();
    void applyHit(Enemy& e, const Hit& h, Vec2 at);
    void areaHit(Vec2 at, float r, const Hit& h, bool reactOnce);
    void triggerReaction(const Hit& h, Vec2 at, Enemy* target);
    void hurtPlayer(float dmg, Vec2 from);
    void onKill(Enemy& e, bool direct);
    void creditMastery(uint32_t uid, int amount);
    void collideCircle(Vec2& p, float r) const;
    void addZone(const Zone& z);
    void emit(Ev t, Vec2 p, float v = 0, int a = 0, int b = 0) { events.push_back({t, p, v, a, b}); }
    void drawCard(int slot);

    int result_ = 0;
    int clearDelay_ = 0;
    int nextEnemyId_ = 1;
    uint32_t nextUid_ = 1;
    uint32_t nextBulletId_ = 1;
    int timeoutFrames_ = ROOM_TIMEOUT_FRAMES;
    int killHeals_ = 0;
    int goldDrop_ = 0;
    uint64_t enemiesSeen_ = 0;
    InputState pending_;
    bool hasPending_ = false;
    std::vector<std::vector<SpawnSpec>> waves_;
    // spawned during update; merged at end of frame so references stay valid
    std::vector<Bullet> newBullets_;
    std::vector<Zone> newZones_;
    std::vector<Enemy> newEnemies_;
};
