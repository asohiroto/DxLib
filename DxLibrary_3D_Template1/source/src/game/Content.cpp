#include "game/Content.h"
#include "game/Cards.h"
#include "core/Tsv.h"
#include <cstdlib>

namespace {
std::vector<EnemyDef> g_enemies;
std::vector<ArenaDef> g_arenas;
std::vector<RoomDef> g_rooms;

AiKind parseAi(const std::string& s) {
    if (s == "archer") return AiKind::Archer;
    if (s == "charger") return AiKind::Charger;
    if (s == "caster") return AiKind::Caster;
    if (s == "splitter") return AiKind::Splitter;
    if (s == "bomber") return AiKind::Bomber;
    if (s == "shield") return AiKind::Shield;
    if (s == "sniper") return AiKind::Sniper;
    if (s == "summoner") return AiKind::Summoner;
    if (s == "turret") return AiKind::Turret;
    if (s == "boss_slime") return AiKind::BossSlime;
    if (s == "boss_lich") return AiKind::BossLich;
    if (s == "boss_golem") return AiKind::BossGolem;
    return AiKind::Chase;
}
}  // namespace

namespace Content {
Elem parseElem(const std::string& s) {
    if (s == "fire") return Elem::Fire;
    if (s == "ice") return Elem::Ice;
    if (s == "thunder") return Elem::Thunder;
    if (s == "earth") return Elem::Earth;
    return Elem::None;
}

bool load(const std::string& dir, std::string* err) {
    if (!CardDB::load(dir + "/cards.tsv", err)) return false;

    TsvTable te;
    if (!te.load(dir + "/enemies.tsv", err)) return false;
    std::vector<EnemyDef> enemies;
    for (size_t r = 0; r < te.rows.size(); ++r) {
        EnemyDef d;
        d.id = te.s(r, "id");
        if (d.id.empty()) continue;
        d.name = te.s(r, "name");
        d.ai = parseAi(te.s(r, "ai"));
        d.hp = te.f(r, "hp", 30); d.speed = te.f(r, "speed", 50); d.radius = te.f(r, "radius", 7);
        d.dmg = te.f(r, "dmg", 10); d.cd = te.f(r, "cd", 1.5f); d.bspeed = te.f(r, "bspeed", 120);
        d.weak = parseElem(te.s(r, "weak")); d.resist = parseElem(te.s(r, "resist"));
        d.freezeRes = te.f(r, "freeze", 1); d.kbRes = te.f(r, "kb", 1);
        auto c = TsvTable::split(te.s(r, "color"), ',');
        if (c.size() == 3) { d.r = atoi(c[0].c_str()); d.g = atoi(c[1].c_str()); d.b = atoi(c[2].c_str()); }
        d.gold = te.i(r, "gold", 2);
        d.boss = d.id.rfind("boss_", 0) == 0;
        d.sprite = te.s(r, "sprite");
        d.sprScale = te.f(r, "scale", 1.0f);
        auto t = TsvTable::split(te.s(r, "tint"), ',');
        if (t.size() == 3) { d.tr = atoi(t[0].c_str()); d.tg = atoi(t[1].c_str()); d.tb = atoi(t[2].c_str()); }
        enemies.push_back(d);
    }

    TsvTable ta;
    if (!ta.load(dir + "/arenas.tsv", err)) return false;
    std::vector<ArenaDef> arenas;
    for (size_t r = 0; r < ta.rows.size(); ++r) {
        ArenaDef a;
        a.id = ta.s(r, "id");
        for (auto& rect : TsvTable::split(ta.s(r, "rects"), ';')) {
            auto v = TsvTable::split(rect, ',');
            if (v.size() == 4) a.pillars.push_back({(float)atof(v[0].c_str()), (float)atof(v[1].c_str()), (float)atof(v[2].c_str()), (float)atof(v[3].c_str())});
        }
        arenas.push_back(a);
    }

    TsvTable tr;
    if (!tr.load(dir + "/rooms.tsv", err)) return false;
    std::vector<RoomDef> rooms;
    for (size_t r = 0; r < tr.rows.size(); ++r) {
        RoomDef d;
        d.id = tr.s(r, "id");
        if (d.id.empty()) continue;
        d.chapter = tr.i(r, "chapter", 1);
        std::string k = tr.s(r, "kind");
        d.kind = k == "elite" ? RoomKind::Elite : (k == "boss" ? RoomKind::Boss : RoomKind::Normal);
        std::string ar = tr.s(r, "arena");
        for (int i = 0; i < (int)arenas.size(); ++i) if (arenas[i].id == ar) d.arena = i;
        for (auto& wave : TsvTable::split(tr.s(r, "waves"), '|')) {
            std::vector<SpawnSpec> w;
            for (auto& part : TsvTable::split(wave, '+')) {
                if (part.empty()) continue;
                bool elite = part[0] == '!';
                std::string name = elite ? part.substr(1) : part;
                int count = 1;
                size_t star = name.find('*');
                if (star != std::string::npos) { count = atoi(name.substr(star + 1).c_str()); name = name.substr(0, star); }
                int idx = -1;
                for (int i = 0; i < (int)enemies.size(); ++i) if (enemies[i].id == name) idx = i;
                if (idx < 0) { if (err) *err = "room " + d.id + ": unknown enemy " + name; return false; }
                for (int c = 0; c < count; ++c) w.push_back({idx, elite});
            }
            if (!w.empty()) d.waves.push_back(w);
        }
        rooms.push_back(d);
    }
    g_enemies = std::move(enemies);
    g_arenas = std::move(arenas);
    g_rooms = std::move(rooms);
    return true;
}

const std::vector<EnemyDef>& enemies() { return g_enemies; }
const std::vector<ArenaDef>& arenas() { return g_arenas; }
const std::vector<RoomDef>& rooms() { return g_rooms; }
int findEnemy(const std::string& id) {
    for (int i = 0; i < (int)g_enemies.size(); ++i) if (g_enemies[i].id == id) return i;
    return -1;
}
int findRoom(const std::string& id) {
    for (int i = 0; i < (int)g_rooms.size(); ++i) if (g_rooms[i].id == id) return i;
    return -1;
}
std::vector<int> roomsFor(int chapter, RoomKind kind) {
    std::vector<int> out;
    for (int i = 0; i < (int)g_rooms.size(); ++i)
        if (g_rooms[i].chapter == chapter && g_rooms[i].kind == kind) out.push_back(i);
    return out;
}
}  // namespace Content
