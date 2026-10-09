// Headless balance simulator. No DxLib.
//  room mode : CardForgeSim [--policy P] [--skill S] [--seeds N] [--room ID] [--compare A B] [--hash SEED]
//  run mode  : CardForgeSim --runs N [--policy smart|nofusebattle|noanchor|nofuse|noevo|random] [--skill S] [--asc A]
//  tests     : --selftest  --reacttest
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>
#include <algorithm>
#include <cmath>
#include "game/World.h"
#include "game/RunState.h"
#include "game/Save.h"
#include "sim/Policy.h"
#include "sim/RunPolicy.h"

static bool loadData() {
    std::string err;
    for (const char* dir : {"data", "../data"})
        if (Content::load(dir, &err) && RunLogic::loadData(dir, &err)) return true;
    std::fprintf(stderr, "data load failed: %s\n", err.c_str());
    return false;
}

static float pct(std::vector<float> v, float q) {
    if (v.empty()) return 0;
    std::sort(v.begin(), v.end());
    size_t i = (size_t)std::clamp(q * (v.size() - 1), 0.0f, (float)(v.size() - 1));
    return v[i];
}
static void meanCi(const std::vector<float>& v, double& mean, double& ci) {
    mean = 0;
    for (float d : v) mean += d;
    mean /= std::max<size_t>(1, v.size());
    double var = 0;
    for (float d : v) var += (d - mean) * (d - mean);
    var /= std::max<size_t>(1, v.size() - 1);
    ci = 1.96 * std::sqrt(var / std::max<size_t>(1, v.size()));
}

// ---------------------------------------------------------------- single room
struct RoomRun {
    uint32_t seed;
    RoomResult res;
    uint32_t hash;
    int fuseChosen, fuseSkipped;
};

static RoomRun runRoom(const PolicyConfig& cfg, uint32_t seed, int roomIdx) {
    RunState rs = RunLogic::newRun(seed, 0, RunLogic::defaultPool());
    RoomSetup s;
    s.deck = rs.deck;
    s.hp = s.maxHp = 100;
    s.nextUid = rs.nextUid;
    s.room = &Content::rooms()[roomIdx];
    s.seed = seed;
    World w;
    w.init(s);
    Policy pol(cfg, seed * 7919u + 13u);
    uint32_t hash = 0;
    while (w.result() == 0) {
        w.update(pol.think(w));
        if (w.frame % 30 == 0) hash = hash * 31u + w.stateHash();
    }
    if (w.result() == 2 && std::getenv("CF_DEBUG_TIMEOUT")) {
        std::printf("  timeout seed %u wave %d/%d left:", seed, w.wave, w.waveCount);
        for (auto& e : w.enemies) if (e.alive) std::printf(" %s(%.0f,%.0f hp%.0f)", e.d().id.c_str(), e.pos.x, e.pos.y, e.hp);
        std::printf("\n");
    }
    return {seed, w.makeResult(), hash, pol.stats_fuseChosen, pol.stats_fuseSkipped};
}

// ---------------------------------------------------------------- full runs
static double g_cardDmg = 0, g_basicDmg = 0, g_cardsUsed = 0, g_battleFrames = 0;

struct RunOutcome {
    bool victory = false;
    int deathChapter = 0;      // 0 = survived
    bool diedToBoss = false;
    std::string deathRoom;
    RunState final;
    std::vector<float> bossSeconds[CHAPTERS + 1];
    int saveMismatch = 0;
    int illegal = 0;
    int nodes = 0;
};

static void policyPair(const std::string& name, float skill, PolicyConfig& combat, RunPolicyConfig& run) {
    combat = PolicyConfig::fromName("fuseevo", skill);
    run = RunPolicyConfig();
    if (name == "nofusebattle") combat = PolicyConfig::fromName("evo", skill);
    else if (name == "noanchor") run.anchor = false;
    else if (name == "nofuse") { combat = PolicyConfig::fromName("evo", skill); run.anchor = false; }
    else if (name == "noevo") combat = PolicyConfig::fromName("fuse", skill);
    else if (name == "random") { combat = PolicyConfig::fromName("random", skill); run.random = true; }
    else if (name == "mindless") { combat = PolicyConfig::fromName("random", 0.2f); run.random = true; }
}

static RunOutcome playRun(const std::string& policyName, float skill, uint32_t seed, int asc) {
    PolicyConfig cc;
    RunPolicyConfig rc;
    policyPair(policyName, skill, cc, rc);
    RunOutcome o;
    RunState r = RunLogic::newRun(seed, asc, RunLogic::defaultPool());
    RunPolicy rp(rc, seed * 31u + 7u);
    int guard = 0;
    while (r.phase != Phase::Victory && r.phase != Phase::Defeat && ++guard < 2000) {
        if (r.phase == Phase::Map) {
            // save / load round trip must reproduce the state exactly
            RunState back;
            std::string text = Save::serializeRun(r);
            if (!Save::deserializeRun(text, back) || RunLogic::hashRun(back) != RunLogic::hashRun(r) ||
                Save::serializeRun(back) != text) ++o.saveMismatch;
        }
        if (r.phase == Phase::Battle) {
            World w;
            RoomSetup s = RunLogic::roomSetup(r);
            w.init(s);
            Policy pol(cc, s.seed ^ 0xABCDu);
            while (w.result() == 0) w.update(pol.think(w));
            RoomResult res = w.makeResult();
            g_cardDmg += res.stats.cardDamage; g_basicDmg += res.stats.basicDamage; g_cardsUsed += res.stats.cardsUsed; g_battleFrames += res.stats.frames;
            bool boss = r.map.nodes[r.node].type == NodeType::Boss;
            if (boss && res.result == 1) o.bossSeconds[r.chapter].push_back(res.stats.frames / 60.0f);
            if (res.result != 1) { o.deathChapter = r.chapter; o.diedToBoss = boss; o.deathRoom = Content::rooms()[r.map.nodes[r.node].room].id; }
            Action a{Act::BattleDone};
            a.result = &res;
            RunLogic::apply(r, a);
            ++o.nodes;
            continue;
        }
        Action a = rp.decide(r);
        std::string err;
        if (!RunLogic::apply(r, a, &err)) {
            ++o.illegal;
            Action leave{Act::Leave};
            if (!RunLogic::apply(r, leave)) {
                if (r.phase == Phase::Reward) RunLogic::apply(r, {Act::SkipCard});
            }
        }
    }
    o.victory = r.phase == Phase::Victory;
    o.final = r;
    return o;
}

static int runMode(const std::string& policy, float skill, int runs, int asc) {
    g_cardDmg = g_basicDmg = 0; g_cardsUsed = 0; g_battleFrames = 0;
    int wins = 0, saveBad = 0, illegal = 0;
    int deaths[CHAPTERS + 1] = {}, bossDeaths[CHAPTERS + 1] = {}, reachedBoss[CHAPTERS + 1] = {};
    std::vector<float> bossSec[CHAPTERS + 1];
    std::vector<float> unusedGold, earned, healRatio, battleMin, fusions, evolutions, anchors, deckSize, roomDmg;
    double spendCards = 0, spendRelics = 0, spendRemove = 0, spendPotion = 0, spendUpgrade = 0;
    std::vector<std::pair<std::string, int>> deathRooms;
    for (int i = 1; i <= runs; ++i) {
        RunOutcome o = playRun(policy, skill, (uint32_t)(1000 + i), asc);
        const RunState& r = o.final;
        wins += o.victory;
        saveBad += o.saveMismatch;
        illegal += o.illegal;
        if (o.deathChapter) { deaths[o.deathChapter]++; if (o.diedToBoss) bossDeaths[o.deathChapter]++; }
        for (int c = 1; c <= CHAPTERS; ++c) for (float s : o.bossSeconds[c]) bossSec[c].push_back(s);
        for (int c = 1; c <= CHAPTERS; ++c)
            if ((int)o.bossSeconds[c].size() > 0 || (o.deathChapter == c && o.diedToBoss)) reachedBoss[c]++;
        if (!o.deathRoom.empty()) {
            auto it = std::find_if(deathRooms.begin(), deathRooms.end(), [&](auto& p) { return p.first == o.deathRoom; });
            if (it == deathRooms.end()) deathRooms.push_back({o.deathRoom, 1}); else it->second++;
        }
        unusedGold.push_back((float)r.gold);
        earned.push_back((float)r.stats.goldEarned);
        healRatio.push_back(r.stats.damageTaken > 0 ? r.stats.healed / r.stats.damageTaken : 0);
        battleMin.push_back(r.stats.battleSeconds / 60.0f);
        fusions.push_back((float)r.stats.fusions);
        evolutions.push_back((float)r.stats.evolutions);
        anchors.push_back((float)r.anchors);
        deckSize.push_back((float)r.deck.size());
        roomDmg.push_back(r.stats.damageTaken / std::max(1, r.stats.roomsCleared + (o.deathChapter ? 1 : 0)));
        spendCards += r.stats.shopSpendCards; spendRelics += r.stats.shopSpendRelics;
        spendRemove += r.stats.shopSpendRemove; spendPotion += r.stats.shopSpendPotion; spendUpgrade += r.stats.shopSpendUpgrade;
    }
    std::printf("RUNS policy=%s skill=%.1f asc=%d runs=%d\n", policy.c_str(), skill, asc, runs);
    std::printf("  victory %d (%.1f%%)\n", wins, 100.0 * wins / runs);
    for (int c = 1; c <= CHAPTERS; ++c)
        std::printf("  chapter %d: deaths %d (boss %d / reached boss %d = %.0f%%)  boss time median %.0fs (n=%d)\n", c, deaths[c],
                    bossDeaths[c], reachedBoss[c], reachedBoss[c] ? 100.0 * bossDeaths[c] / reachedBoss[c] : 0.0,
                    pct(bossSec[c], 0.5f), (int)bossSec[c].size());
    double m, ci;
    meanCi(roomDmg, m, ci);
    std::printf("  damage per room: %.1f +- %.1f\n", m, ci);
    std::printf("  heal/damage ratio median %.2f   battle minutes median %.1f\n", pct(healRatio, 0.5f), pct(battleMin, 0.5f));
    std::printf("  gold earned median %.0f  unused median %.0f  shop spend: cards %.0f relics %.0f remove %.0f potion %.0f upgrade %.0f (per run)\n",
                pct(earned, 0.5f), pct(unusedGold, 0.5f), spendCards / runs, spendRelics / runs, spendRemove / runs, spendPotion / runs, spendUpgrade / runs);
    std::printf("  per run: fusions %.0f  evolutions %.1f  anchors %.1f  final deck %.0f\n", pct(fusions, 0.5f), pct(evolutions, 0.5f),
                pct(anchors, 0.5f), pct(deckSize, 0.5f));
    std::printf("  damage share: cards %.0f%% basic %.0f%%   cards used per minute %.1f\n", 100.0 * g_cardDmg / std::max(1.0, g_cardDmg + g_basicDmg),
                100.0 * g_basicDmg / std::max(1.0, g_cardDmg + g_basicDmg), g_cardsUsed / std::max(1.0, g_battleFrames / 3600.0));
    std::printf("  deaths by room:");
    for (auto& p : deathRooms) std::printf(" %s=%d", p.first.c_str(), p.second);
    std::printf("\n  save roundtrip mismatches %d   illegal actions %d\n", saveBad, illegal);
    return saveBad ? 1 : 0;
}

// Fire every (form x element) fusion at motionless dummies and check that the reaction really happens.
static int reactionTest() {
    const char* lefts[] = {"fireball", "frost_nova", "quake", "flame_slash", "thunder_totem", "ice_wall", "stone_guard"};
    const char* rights[] = {"fireball", "ice_shard", "lightning", "rock_blast", "magic_missile"};
    int failures = 0, total = 0;
    int dummy = Content::findEnemy("charger");
    for (const char* ln : lefts)
        for (const char* rn : rights) {
            CardInstance L = makeCard(CardDB::find(ln), 1);
            CardInstance R = makeCard(CardDB::find(rn), 2);
            FusePreview pv = previewFuse(L, R, 3);
            if (pv.kind == FuseKind::Invalid) continue;
            RoomSetup s;
            s.deck = {L};
            s.nextUid = 10;
            World w;
            w.init(s);
            w.player.pos = {200, 150};
            w.hand.slots[0] = pv.result;
            w.mana = 6;
            w.debugSpawnDummy(dummy, {238, 150}, 999);
            w.debugSpawnDummy(dummy, {252, 166}, 999);
            w.debugSpawnDummy(dummy, {252, 134}, 999);
            int fired = 0;
            float dmg = 0;
            for (int f = 0; f < 240; ++f) {
                InputState in;
                in.aim = {240, 150};
                if (f == 1) in.useSlot = 0;
                w.update(in);
                for (auto& e : w.events) {
                    if (e.type == Ev::Reaction && e.b == 0 && e.a == (int)pv.result.reaction) ++fired;
                    if (e.type == Ev::Damage) dmg += e.value;
                }
            }
            Reaction re = pv.result.reaction;
            bool passive = re == Reaction::Purify || re == Reaction::Resonance || pv.kind == FuseKind::RankUp;
            bool ok = dmg > 0 && (passive || fired > 0) && (passive || fired <= 6);
            ++total;
            if (!ok) {
                ++failures;
                std::printf("FAIL %s + %s (%s) dmg %.0f fired %d\n", ln, rn, reactionName(re), dmg, fired);
            }
        }
    std::printf("reaction test: %d / %d failed\n", failures, total);
    return failures;
}

int main(int argc, char** argv) {
    std::string policy = "fuseevo", csv, cmpA, cmpB, room = "c1_a";
    float skill = 0.6f;
    int seeds = 50, runs = 0, asc = 0;
    bool selftest = false, reacttest = false;
    long long hashSeed = -1;
    for (int i = 1; i < argc; ++i) {
        std::string a = argv[i];
        auto next = [&]() { return (i + 1 < argc) ? std::string(argv[++i]) : std::string(); };
        if (a == "--policy") policy = next();
        else if (a == "--skill") skill = (float)atof(next().c_str());
        else if (a == "--seeds") seeds = atoi(next().c_str());
        else if (a == "--room") room = next();
        else if (a == "--selftest") selftest = true;
        else if (a == "--reacttest") reacttest = true;
        else if (a == "--hash") hashSeed = atoll(next().c_str());
        else if (a == "--compare") { cmpA = next(); cmpB = next(); }
        else if (a == "--runs") runs = atoi(next().c_str());
        else if (a == "--asc") asc = atoi(next().c_str());
    }
    if (!loadData()) return 2;
    if (selftest) {
        std::string log;
        int bad = fusionSelfTest(&log);
        std::printf("fusion selftest: %d violations\n%s", bad, log.c_str());
        if (bad) return 1;
    }
    if (reacttest) return reactionTest() ? 1 : 0;
    if (selftest && seeds == 0) return 0;
    if (runs > 0) return runMode(policy == "fuseevo" ? "smart" : policy, skill, runs, asc);

    int roomIdx = Content::findRoom(room);
    if (roomIdx < 0) { std::fprintf(stderr, "unknown room %s\n", room.c_str()); return 2; }
    if (!cmpA.empty()) {
        PolicyConfig a = PolicyConfig::fromName(cmpA, skill), b = PolicyConfig::fromName(cmpB, skill);
        std::vector<float> dDmg, dTime;
        int winsA = 0, winsB = 0;
        for (int s = 1; s <= seeds; ++s) {
            RoomRun ra = runRoom(a, (uint32_t)s, roomIdx), rb = runRoom(b, (uint32_t)s, roomIdx);
            float ta = ra.res.result == 1 ? ra.res.stats.frames / 60.0f : ROOM_TIMEOUT_FRAMES / 60.0f;
            float tb = rb.res.result == 1 ? rb.res.stats.frames / 60.0f : ROOM_TIMEOUT_FRAMES / 60.0f;
            dDmg.push_back(rb.res.stats.damageTaken - ra.res.stats.damageTaken);
            dTime.push_back(tb - ta);
            winsA += ra.res.result == 1; winsB += rb.res.result == 1;
        }
        double m, c;
        std::printf("compare %s -> %s  room=%s skill=%.1f seeds=%d\n  clear: %d vs %d\n", cmpA.c_str(), cmpB.c_str(), room.c_str(), skill, seeds, winsA, winsB);
        meanCi(dDmg, m, c);
        std::printf("  damage taken diff (B-A): %+.1f +- %.1f%s\n", m, c, std::fabs(m) > c ? "  [significant]" : "");
        meanCi(dTime, m, c);
        std::printf("  clear time diff  (B-A): %+.1f +- %.1f sec%s\n", m, c, std::fabs(m) > c ? "  [significant]" : "");
        return 0;
    }
    PolicyConfig cfg = PolicyConfig::fromName(policy, skill);
    if (hashSeed >= 0) {
        RoomRun r = runRoom(cfg, (uint32_t)hashSeed, roomIdx);
        std::printf("hash %08x result %d frames %d\n", r.hash, r.res.result, r.res.stats.frames);
        return 0;
    }
    std::vector<float> allSec, dmg;
    int wins = 0, deaths = 0, timeouts = 0;
    double fus = 0, evo = 0, used = 0, chosen = 0, skipped = 0;
    for (int s = 1; s <= seeds; ++s) {
        RoomRun r = runRoom(cfg, (uint32_t)s, roomIdx);
        const RoomStats& st = r.res.stats;
        if (r.res.result == 1) ++wins; else if (r.res.result == -1) ++deaths; else ++timeouts;
        allSec.push_back(r.res.result == 1 ? st.frames / 60.0f : ROOM_TIMEOUT_FRAMES / 60.0f);
        dmg.push_back(st.damageTaken);
        fus += st.fusions; evo += st.evolutions; used += st.cardsUsed;
        chosen += r.fuseChosen; skipped += r.fuseSkipped;
    }
    double mean, ci;
    meanCi(dmg, mean, ci);
    std::printf("policy=%s room=%s skill=%.1f seeds=%d\n", policy.c_str(), room.c_str(), skill, seeds);
    std::printf("  clear %d  dead %d  timeout %d\n", wins, deaths, timeouts);
    std::printf("  time sec (fail=limit): p10 %.1f  median %.1f  p90 %.1f\n", pct(allSec, 0.1f), pct(allSec, 0.5f), pct(allSec, 0.9f));
    std::printf("  damage taken: mean %.1f +- %.1f\n", mean, ci);
    std::printf("  per room: cards %.1f  fusions %.2f  evolutions %.2f\n", used / seeds, fus / seeds, evo / seeds);
    if (chosen + skipped > 0) std::printf("  fuse decisions: chose %.0f%%\n", 100 * chosen / (chosen + skipped));
    return 0;
}
