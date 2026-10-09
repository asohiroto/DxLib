#pragma once
#include <string>
#include <vector>
#include "core/Vec2.h"
#include "game/Types.h"

struct RectF { float x, y, w, h; };

enum class AiKind : uint8_t { Chase, Archer, Charger, Caster, Splitter, Bomber, Shield, Sniper, Summoner, Turret,
                              BossSlime, BossLich, BossGolem };

struct EnemyDef {
    std::string id, name;
    AiKind ai = AiKind::Chase;
    float hp = 30, speed = 50, radius = 7, dmg = 10, cd = 1.5f, bspeed = 120;
    Elem weak = Elem::None, resist = Elem::None;
    float freezeRes = 1, kbRes = 1;
    int r = 200, g = 200, b = 200;
    int gold = 2;
    bool boss = false;
    std::string sprite;        // sprite sheet base name
    float sprScale = 1.0f;
    int tr = 255, tg = 255, tb = 255;   // sprite tint
};

struct ArenaDef {
    std::string id;
    std::vector<RectF> pillars;
};

enum class RoomKind : uint8_t { Normal, Elite, Boss };
struct SpawnSpec { int enemy; bool elite; };
struct RoomDef {
    std::string id;
    int chapter = 1;
    RoomKind kind = RoomKind::Normal;
    int arena = 0;
    std::vector<std::vector<SpawnSpec>> waves;
};

namespace Content {
bool load(const std::string& dataDir, std::string* err);   // also loads cards
const std::vector<EnemyDef>& enemies();
const std::vector<ArenaDef>& arenas();
const std::vector<RoomDef>& rooms();
int findEnemy(const std::string& id);
int findRoom(const std::string& id);
std::vector<int> roomsFor(int chapter, RoomKind kind);
Elem parseElem(const std::string& s);
}
