#include "game/RunState.h"
#include "core/Tsv.h"
#include <algorithm>
#include <cmath>

const char* nodeTypeName(NodeType t) {
    switch (t) {
    case NodeType::Battle: return "戦闘";
    case NodeType::Elite: return "強敵";
    case NodeType::Shop: return "商店";
    case NodeType::Forge: return "鍛冶場";
    case NodeType::Event: return "イベント";
    case NodeType::Rest: return "休憩";
    case NodeType::Boss: return "ボス";
    }
    return "";
}

const char* phaseName(Phase p) {
    switch (p) {
    case Phase::Map: return "Map";
    case Phase::Battle: return "Battle";
    case Phase::Reward: return "Reward";
    case Phase::Shop: return "Shop";
    case Phase::Forge: return "Forge";
    case Phase::Rest: return "Rest";
    case Phase::Event: return "Event";
    case Phase::ChapterClear: return "ChapterClear";
    case Phase::Victory: return "Victory";
    case Phase::Defeat: return "Defeat";
    }
    return "";
}

bool RunState::hasRelic(const std::string& id) const {
    int r = RunLogic::findRelic(id);
    return r >= 0 && std::find(relics.begin(), relics.end(), r) != relics.end();
}

namespace {
std::vector<RelicDef> g_relics;
std::vector<EventDef> g_events;

uint32_t mixSeed(uint32_t seed, int a, int b, int purpose) {
    uint32_t h = seed * 0x9E3779B1u;
    h ^= (uint32_t)a * 0x85EBCA6Bu; h = (h << 13) | (h >> 19);
    h ^= (uint32_t)b * 0xC2B2AE35u; h = (h << 7) | (h >> 25);
    h ^= (uint32_t)purpose * 0x27D4EB2Fu;
    h ^= h >> 16; h *= 0x7FEB352Du; h ^= h >> 15;
    return h ? h : 1;
}
enum Purpose { P_MAP = 1, P_REWARD, P_SHOP, P_EVENT, P_BATTLE, P_EVENTFX };

Rng streamFor(const RunState& r, Purpose p) { return Rng(mixSeed(r.seed, r.chapter, r.node, p)); }

int pickRarityCard(Rng& rng, const std::vector<int>& pool, int wc, int wu, int wr, const std::vector<int>& exclude) {
    for (int tries = 0; tries < 40; ++tries) {
        int roll = rng.irange(1, wc + wu + wr);
        char want = roll <= wc ? 'c' : (roll <= wc + wu ? 'u' : 'r');
        std::vector<int> cands;
        for (int d : pool)
            if (CardDB::get(d).rarity == want && std::find(exclude.begin(), exclude.end(), d) == exclude.end()) cands.push_back(d);
        if (!cands.empty()) return cands[rng.irange(0, (int)cands.size() - 1)];
    }
    return pool.empty() ? 0 : pool[rng.irange(0, (int)pool.size() - 1)];
}

int pickRelic(Rng& rng, const RunState& r, char rarity, const std::vector<int>& exclude) {
    std::vector<int> cands;
    for (int i = 0; i < (int)g_relics.size(); ++i) {
        if (std::find(r.relics.begin(), r.relics.end(), i) != r.relics.end()) continue;
        if (std::find(exclude.begin(), exclude.end(), i) != exclude.end()) continue;
        if (rarity && g_relics[i].rarity != rarity) continue;
        cands.push_back(i);
    }
    if (cands.empty()) {
        if (rarity) return pickRelic(rng, r, 0, exclude);
        return -1;
    }
    return cands[rng.irange(0, (int)cands.size() - 1)];
}

char rollRelicRarity(Rng& rng) {
    int x = rng.irange(1, 100);
    return x <= 50 ? 'c' : (x <= 85 ? 'u' : 'r');
}

void gainRelic(RunState& r, int idx) {
    if (idx < 0) return;
    r.relics.push_back(idx);
    if (g_relics[idx].id == "iron_heart") { r.maxHp += 20; r.hp = std::min(r.maxHp, r.hp + 20); }
}

void gainGold(RunState& r, int g) {
    if (g <= 0) return;
    if (r.hasRelic("golden_idol")) g = (int)std::lround(g * 1.25f);
    r.gold += g;
    r.stats.goldEarned += g;
}

bool spend(RunState& r, int price) {
    if (r.gold < price) return false;
    r.gold -= price;
    r.stats.goldSpent += price;
    return true;
}

void heal(RunState& r, float amount) {
    float before = r.hp;
    r.hp = std::min(r.maxHp, r.hp + amount);
    r.stats.healed += r.hp - before;
}

void enterNode(RunState& r, int idx) {
    r.node = idx;
    const MapNode& n = r.map.nodes[idx];
    switch (n.type) {
    case NodeType::Battle:
    case NodeType::Elite:
    case NodeType::Boss:
        r.phase = Phase::Battle;
        break;
    case NodeType::Shop: {
        r.phase = Phase::Shop;
        Rng rng = streamFor(r, P_SHOP);
        r.shop = ShopStock();
        std::vector<int> ex;
        for (int i = 0; i < 5; ++i) {
            int d = pickRarityCard(rng, r.pool, 55, 35, 10, ex);
            ex.push_back(d);
            char ra = CardDB::get(d).rarity;
            int price = ra == 'r' ? rng.irange(140, 165) : (ra == 'u' ? rng.irange(72, 90) : rng.irange(45, 58));
            r.shop.cards.push_back(d);
            r.shop.cardPrices.push_back(price);
            r.shop.cardSold.push_back(0);
        }
        std::vector<int> rex;
        for (int i = 0; i < 2; ++i) {
            char ra = rollRelicRarity(rng);
            int rl = pickRelic(rng, r, ra, rex);
            if (rl < 0) break;
            rex.push_back(rl);
            r.shop.relics.push_back(rl);
            r.shop.relicPrices.push_back(ra == 'r' ? rng.irange(200, 225) : (ra == 'u' ? rng.irange(150, 170) : rng.irange(110, 125)));
            r.shop.relicSold.push_back(0);
        }
        break;
    }
    case NodeType::Forge: r.phase = Phase::Forge; break;
    case NodeType::Rest: r.phase = Phase::Rest; break;
    case NodeType::Event: {
        r.phase = Phase::Event;
        Rng rng = streamFor(r, P_EVENT);
        r.eventId = g_events.empty() ? -1 : rng.irange(0, (int)g_events.size() - 1);
        if (r.eventId < 0) r.phase = Phase::Map;
        break;
    }
    }
}

void makeRewards(RunState& r, NodeType type) {
    Rng rng = streamFor(r, P_REWARD);
    r.offer = Offer();
    std::vector<int> ex;
    int wc = 62, wu = 32, wr = 6;
    if (type == NodeType::Elite) { wc = 40; wu = 42; wr = 18; }
    if (type == NodeType::Boss) { wc = 0; wu = 55; wr = 45; }
    for (int i = 0; i < 3; ++i) {
        int d = pickRarityCard(rng, r.pool, wc, wu, wr, ex);
        ex.push_back(d);
        r.offer.cards.push_back(d);
    }
    r.offer.gold = type == NodeType::Boss ? rng.irange(60, 75) : (type == NodeType::Elite ? rng.irange(25, 35) : rng.irange(12, 18));
    if (type == NodeType::Elite) {
        int rl = pickRelic(rng, r, rollRelicRarity(rng), {});
        if (rl >= 0) r.offer.relics.push_back(rl);
    } else if (type == NodeType::Boss) {
        std::vector<int> rex;
        for (int i = 0; i < 3; ++i) {
            int rl = pickRelic(rng, r, i == 0 ? 'r' : rollRelicRarity(rng), rex);
            if (rl < 0) break;
            rex.push_back(rl);
            r.offer.relics.push_back(rl);
        }
    }
    r.offer.relicDone = r.offer.relics.empty();
}

void maybeFinishReward(RunState& r) {
    if (!(r.offer.cardDone && r.offer.relicDone)) return;
    if (r.map.nodes[r.node].type == NodeType::Boss) {
        r.phase = r.chapter >= CHAPTERS ? Phase::Victory : Phase::ChapterClear;
    } else {
        r.phase = Phase::Map;
    }
}

// fuse two deck cards permanently (forge / altar)
bool anchorCards(RunState& r, int a, int b) {
    if (a == b || a < 0 || b < 0 || a >= (int)r.deck.size() || b >= (int)r.deck.size()) return false;
    if (r.anchors >= MAX_ANCHORS || (int)r.deck.size() - 1 < MIN_DECK) return false;
    FusePreview p = RunLogic::anchorPreview(r, a, b);
    if (p.kind != FuseKind::Hybrid) return false;
    CardInstance c = p.result;
    c.materials.clear();
    c.anchored = true;
    c.uid = r.newUid();
    c.mastery = 0;
    int hi = std::max(a, b), lo = std::min(a, b);
    r.deck.erase(r.deck.begin() + hi);
    r.deck.erase(r.deck.begin() + lo);
    r.deck.push_back(c);
    ++r.anchors;
    return true;
}
}  // namespace

namespace RunLogic {
bool loadData(const std::string& dir, std::string* err) {
    TsvTable tr;
    if (!tr.load(dir + "/relics.tsv", err)) return false;
    std::vector<RelicDef> relics;
    for (size_t i = 0; i < tr.rows.size(); ++i) {
        RelicDef d;
        d.id = tr.s(i, "id"); d.name = tr.s(i, "name"); d.desc = tr.s(i, "desc");
        std::string ra = tr.s(i, "rarity");
        d.rarity = ra.empty() ? 'c' : ra[0];
        if (!d.id.empty()) relics.push_back(d);
    }
    TsvTable te;
    if (!te.load(dir + "/events.tsv", err)) return false;
    std::vector<EventDef> events;
    for (size_t i = 0; i < te.rows.size(); ++i) {
        EventDef d;
        d.id = te.s(i, "id"); d.title = te.s(i, "title"); d.text = te.s(i, "text");
        for (const char* k : {"c1", "c2", "c3"}) { std::string c = te.s(i, k); if (!c.empty()) d.choices.push_back(c); }
        if (!d.id.empty()) events.push_back(d);
    }
    g_relics = std::move(relics);
    g_events = std::move(events);
    return true;
}
const std::vector<RelicDef>& relics() { return g_relics; }
const std::vector<EventDef>& events() { return g_events; }
int findRelic(const std::string& id) {
    for (int i = 0; i < (int)g_relics.size(); ++i) if (g_relics[i].id == id) return i;
    return -1;
}

std::vector<int> defaultPool() {
    std::vector<int> p;
    const auto& all = CardDB::all();
    for (int i = 0; i < (int)all.size(); ++i)
        if (!all[i].isEvolved && all[i].rarity != '-' && !all[i].locked) p.push_back(i);
    return p;
}

ChapterMap generateMap(uint32_t seed, int chapter, int ascension) {
    Rng rng(mixSeed(seed, chapter, 0, P_MAP));
    ChapterMap m;
    m.rows = MAP_ROWS;
    const int COLS = 4;
    int grid[MAP_ROWS][COLS];
    for (auto& row : grid) for (int& v : row) v = -1;
    std::vector<std::pair<int, int>> edges;   // node index pairs
    auto nodeAt = [&](int r, int c) {
        if (grid[r][c] < 0) {
            MapNode n; n.row = r; n.col = c;
            grid[r][c] = (int)m.nodes.size();
            m.nodes.push_back(n);
        }
        return grid[r][c];
    };
    // carve 4 paths from bottom to top
    for (int p = 0; p < 4; ++p) {
        int c = (p < 2) ? rng.irange(0, COLS - 1) : (p == 2 ? 0 : COLS - 1);
        int prev = nodeAt(0, c);
        for (int r = 1; r < MAP_ROWS; ++r) {
            int nc = clampv(c + rng.irange(-1, 1), 0, COLS - 1);
            int cur = nodeAt(r, nc);
            edges.push_back({prev, cur});
            prev = cur;
            c = nc;
        }
    }
    for (auto& e : edges) {
        auto& nx = m.nodes[e.first].next;
        if (std::find(nx.begin(), nx.end(), e.second) == nx.end()) nx.push_back(e.second);
    }
    // types by row
    for (auto& n : m.nodes) {
        int x = rng.irange(1, 100);
        switch (n.row) {
        case 0: n.type = NodeType::Battle; break;
        case 1: n.type = x <= 75 ? NodeType::Battle : NodeType::Event; break;
        case 2: n.type = x <= 45 ? NodeType::Battle : (x <= (ascension >= 2 ? 72 : 65) ? NodeType::Elite : (x <= 82 ? NodeType::Event : NodeType::Shop)); break;
        case 3: n.type = NodeType::Forge; break;
        case 4: n.type = x <= 40 ? NodeType::Battle : (x <= 60 ? NodeType::Elite : (x <= 80 ? NodeType::Shop : NodeType::Event)); break;
        case 5: n.type = x <= (ascension >= 2 ? 40 : 45) ? NodeType::Battle : (x <= 65 ? NodeType::Elite : (x <= 75 ? NodeType::Event : NodeType::Shop)); break;
        default: n.type = NodeType::Rest; break;
        }
    }
    // guarantee a shop in rows 2-4 and another in row 5 (gold should always have an outlet before the boss)
    for (int lo : {2, 5}) {
        bool shop = false;
        for (auto& n : m.nodes) if (n.type == NodeType::Shop && n.row >= lo && n.row <= (lo == 2 ? 4 : 5)) shop = true;
        if (!shop) for (auto& n : m.nodes) if (n.row == (lo == 2 ? 4 : 5) && n.type != NodeType::Elite) { n.type = NodeType::Shop; break; }
    }
    // boss
    MapNode boss; boss.row = MAP_ROWS; boss.col = 1; boss.type = NodeType::Boss;
    m.boss = (int)m.nodes.size();
    for (auto& n : m.nodes) if (n.row == MAP_ROWS - 1) n.next.push_back(m.boss);
    m.nodes.push_back(boss);
    for (int c = 0; c < COLS; ++c) if (grid[0][c] >= 0) m.starts.push_back(grid[0][c]);
    // rooms (avoid repeats while possible)
    auto normals = Content::roomsFor(chapter, RoomKind::Normal);
    auto elites = Content::roomsFor(chapter, RoomKind::Elite);
    auto bosses = Content::roomsFor(chapter, RoomKind::Boss);
    int ni = rng.irange(0, 99), ei = rng.irange(0, 99);
    for (auto& n : m.nodes) {
        if (n.type == NodeType::Battle && !normals.empty()) n.room = normals[(ni++) % normals.size()];
        if (n.type == NodeType::Elite && !elites.empty()) n.room = elites[(ei++) % elites.size()];
        if (n.type == NodeType::Boss && !bosses.empty()) n.room = bosses[0];
        if (n.type == NodeType::Elite && elites.empty()) { n.type = NodeType::Battle; n.room = normals.empty() ? -1 : normals[0]; }
    }
    return m;
}

RunState newRun(uint32_t seed, int ascension, const std::vector<int>& pool) {
    RunState r;
    r.seed = seed;
    r.ascension = ascension;
    r.pool = pool;
    r.maxHp = r.hp = ascension >= 5 ? 72.0f : 80.0f;
    r.gold = 60;
    for (const char* id : {"fireball", "fireball", "ice_shard", "ice_shard", "lightning", "lightning",
                           "rock_blast", "frost_nova", "stone_guard", "magic_missile"}) {
        int d = CardDB::find(id);
        if (d >= 0) r.deck.push_back(makeCard(d, r.newUid()));
    }
    r.chapter = 1;
    r.map = generateMap(seed, 1, ascension);
    r.node = -1;
    r.phase = Phase::Map;
    return r;
}

std::vector<int> choosableNodes(const RunState& r) {
    if (r.phase != Phase::Map) return {};
    if (r.node < 0) return r.map.starts;
    return r.map.nodes[r.node].next;
}

Modifiers modifiersFor(const RunState& r) {
    Modifiers m;
    for (int idx : r.relics) {
        const std::string& id = g_relics[idx].id;
        if (id == "mana_crystal") m.manaMax += 1;
        else if (id == "swift_boots") m.dashCd *= 0.7f;
        else if (id == "stone_skin") m.startShield += 15;
        else if (id == "frost_heart") m.freezeTimeMul *= 1.5f;
        else if (id == "ember") m.burnCap += 3;
        else if (id == "vampire_fang") m.killHealCap += 5;
        else if (id == "sage_tome") m.masteryCap -= 2;
        else if (id == "mana_spring") m.manaStart = 99;
        else if (id == "hunter_mark") m.weakMul += 0.5f;
        else if (id == "forge_hammer") m.fusedCostDelta -= 1;
        else if (id == "lucky_star") m.startRankUp = true;
        else if (id == "alchemist_ring") m.fusedPowerMul *= 1.2f;
        else if (id == "triple_shot") { m.basicShots = 3; m.basicDmg *= 0.6f; }
        else if (id == "burning_core") m.burnDmgMul *= 1.5f;
        else if (id == "glacier_crown") m.chillBonus += 1;
        else if (id == "storm_sigil") m.chainMul *= 2.0f;
        else if (id == "earth_altar") m.earthPowerMul *= 1.25f;
        else if (id == "card_satchel") m.refillMul *= 0.7f;
    }
    m.manaStart = std::min(m.manaStart, m.manaMax);
    if (r.ascension < 0) { m.enemyHpMul *= 0.8f; m.enemyDmgMul *= 0.7f; }   // easy
    if (r.ascension >= 1) m.enemyHpMul *= 1.1f;
    if (r.ascension >= 3) m.enemyDmgMul *= 1.15f;
    return m;
}

RoomSetup roomSetup(const RunState& r) {
    RoomSetup s;
    s.deck = r.deck;
    s.hp = r.hp;
    s.maxHp = r.maxHp;
    s.nextUid = r.nextUid;
    int room = (r.node >= 0) ? r.map.nodes[r.node].room : -1;
    s.room = room >= 0 ? &Content::rooms()[room] : nullptr;
    s.mods = modifiersFor(r);
    // chapters scale enemies as the deck grows (bosses are tuned by their own HP)
    if (!s.room || s.room->kind != RoomKind::Boss) {
        static const float hpMul[] = {1.35f, 1.35f, 1.6f, 2.0f};
        static const float dmgMul[] = {1.25f, 1.25f, 1.35f, 1.45f};
        s.mods.enemyHpMul *= hpMul[std::clamp(r.chapter, 1, 3)];
        s.mods.enemyDmgMul *= dmgMul[std::clamp(r.chapter, 1, 3)];
    }
    s.seed = mixSeed(r.seed, r.chapter, r.node, P_BATTLE);
    return s;
}

int removePrice(const RunState& r) { return REMOVE_BASE + REMOVE_STEP * r.removals; }
bool eventChoiceAvailable(const RunState& r, int choice) {
    if (r.eventId < 0) return false;
    const EventDef& ev = g_events[r.eventId];
    if (choice < 0 || choice >= (int)ev.choices.size()) return false;
    if (ev.id == "gambler" && choice == 0) return r.gold >= 50;
    if (ev.id == "lost_adventurer" && choice == 0) return r.gold >= 40;
    // healing at full HP would waste the event
    if ((ev.id == "fountain" && choice == 0) || (ev.id == "lost_adventurer" && choice == 1)) return r.hp < r.maxHp;
    return true;
}
std::string eventChoiceNote(const RunState& r, int choice) {
    if (r.eventId < 0) return {};
    const EventDef& ev = g_events[r.eventId];
    if (ev.id == "gambler" && choice == 0 && r.gold < 50) return "（所持金が足りない）";
    if (ev.id == "lost_adventurer" && choice == 0 && r.gold < 40) return "（所持金が足りない）";
    if (ev.id == "dojo" && choice == 0 && r.hp <= 8) return "（HPは1で止まる）";
    if (ev.id == "fountain" && choice == 0 && r.hp >= r.maxHp) return "（満タンなので選べない）";
    if (ev.id == "lost_adventurer" && choice == 1 && r.hp >= r.maxHp) return "（満タンなので選べない）";
    if (ev.id == "cursed_tome" && choice == 0 && r.hp <= std::max(20.0f, r.maxHp - 15)) return "（今のHPはそのまま）";
    if (ev.id == "ghost_merchant" && choice == 0) {   // the cut in numbers, like the cursed tome's fixed 15
        int cut = (int)std::lround(r.maxHp - std::max(20.0f, std::round(r.maxHp * 0.9f)));
        return "（最大HPが" + std::to_string(cut) + "下がる" + (r.hp <= std::round(r.maxHp * 0.9f) ? "・今のHPはそのまま）" : "）");
    }
    return {};
}
int upgradePrice(const RunState& r) { return 75 + 25 * r.upgrades; }
bool canUpgrade(const CardInstance& c) { return !c.fused && !c.anchored && c.rank < 3; }

FusePreview anchorPreview(const RunState& r, int a, int b) {
    if (a == b || a < 0 || b < 0 || a >= (int)r.deck.size() || b >= (int)r.deck.size()) return {};
    return previewFuse(r.deck[a], r.deck[b], 0);
}

bool apply(RunState& r, const Action& a, std::string* err) {
    auto fail = [&](const char* m) { if (err) *err = m; return false; };
    const int deckN = (int)r.deck.size();
    switch (a.type) {
    case Act::ChooseNode: {
        if (r.phase != Phase::Map) return fail("not on map");
        auto ch = choosableNodes(r);
        if (std::find(ch.begin(), ch.end(), a.a) == ch.end()) return fail("node not reachable");
        enterNode(r, a.a);
        return true;
    }
    case Act::BattleDone: {
        if (r.phase != Phase::Battle || !a.result) return fail("no battle");
        const RoomResult& res = *a.result;
        r.stats.damageTaken += res.stats.damageTaken;
        r.stats.healed += res.stats.healed;
        r.stats.kills += res.stats.kills;
        r.stats.fusions += res.stats.fusions;
        r.stats.rankUps += res.stats.rankUps;
        r.stats.evolutions += res.stats.evolutions;
        r.stats.reactions |= res.stats.reactionsSeen;
        r.stats.battleSeconds += res.stats.frames / 60.0f;
        r.deck = res.deck;
        r.nextUid = std::max(r.nextUid, res.nextUid);
        r.hp = res.hp;
        if (res.result != 1) { r.hp = 0; r.phase = Phase::Defeat; return true; }
        r.stats.roomsCleared++;
        NodeType t = r.map.nodes[r.node].type;
        if (t == NodeType::Boss) r.stats.bossesKilled++;
        if (t == NodeType::Elite) r.stats.elitesKilled++;
        makeRewards(r, t);
        {
            int before = r.gold;
            gainGold(r, r.offer.gold + res.goldDrop / 3);
            r.offer.goldGained = r.gold - before;
        }
        r.phase = Phase::Reward;
        return true;
    }
    case Act::TakeCard: {
        if (r.phase != Phase::Reward || r.offer.cardDone) return fail("no card offer");
        if (a.a < 0 || a.a >= (int)r.offer.cards.size()) return fail("bad index");
        r.deck.push_back(makeCard(r.offer.cards[a.a], r.newUid()));
        r.offer.cardDone = true;
        maybeFinishReward(r);
        return true;
    }
    case Act::SkipCard: {
        if (r.phase != Phase::Reward || r.offer.cardDone) return fail("no card offer");
        r.offer.cardDone = true;
        maybeFinishReward(r);
        return true;
    }
    case Act::TakeRelic: {
        if (r.phase != Phase::Reward || r.offer.relicDone) return fail("no relic offer");
        if (a.a < 0 || a.a >= (int)r.offer.relics.size()) return fail("bad index");
        gainRelic(r, r.offer.relics[a.a]);
        r.offer.relicDone = true;
        maybeFinishReward(r);
        return true;
    }
    case Act::ShopBuyCard: {
        if (r.phase != Phase::Shop || a.a < 0 || a.a >= (int)r.shop.cards.size() || r.shop.cardSold[a.a]) return fail("bad item");
        if (!spend(r, r.shop.cardPrices[a.a])) return fail("not enough gold");
        r.stats.shopSpendCards += r.shop.cardPrices[a.a];
        r.shop.cardSold[a.a] = 1;
        r.deck.push_back(makeCard(r.shop.cards[a.a], r.newUid()));
        return true;
    }
    case Act::ShopBuyRelic: {
        if (r.phase != Phase::Shop || a.a < 0 || a.a >= (int)r.shop.relics.size() || r.shop.relicSold[a.a]) return fail("bad item");
        if (!spend(r, r.shop.relicPrices[a.a])) return fail("not enough gold");
        r.stats.shopSpendRelics += r.shop.relicPrices[a.a];
        r.shop.relicSold[a.a] = 1;
        gainRelic(r, r.shop.relics[a.a]);
        return true;
    }
    case Act::ShopRemove: {
        if (r.phase != Phase::Shop || a.a < 0 || a.a >= deckN || deckN - 1 < MIN_DECK) return fail("cannot remove");
        int price = removePrice(r);
        if (!spend(r, price)) return fail("not enough gold");
        r.stats.shopSpendRemove += price;
        r.deck.erase(r.deck.begin() + a.a);
        r.removals++;
        return true;
    }
    case Act::ShopUpgrade: {
        if (r.phase != Phase::Shop || a.a < 0 || a.a >= deckN || !canUpgrade(r.deck[a.a])) return fail("cannot upgrade");
        int price = upgradePrice(r);
        if (!spend(r, price)) return fail("not enough gold");
        r.stats.shopSpendUpgrade += price;
        r.deck[a.a].rank++;
        r.upgrades++;
        return true;
    }
    case Act::ShopPotion: {
        if (r.phase != Phase::Shop || r.shop.potionSold || r.hp >= r.maxHp) return fail("cannot buy potion");
        if (!spend(r, POTION_PRICE)) return fail("not enough gold");
        r.stats.shopSpendPotion += POTION_PRICE;
        r.shop.potionSold = true;
        heal(r, r.maxHp * 0.25f);
        return true;
    }
    case Act::Leave: {
        if (r.phase == Phase::Shop || r.phase == Phase::Forge || r.phase == Phase::Rest || r.phase == Phase::Event) {
            r.phase = Phase::Map;
            return true;
        }
        if (r.phase == Phase::Reward) {   // leave the rest of the rewards
            r.offer.cardDone = r.offer.relicDone = true;
            maybeFinishReward(r);
            return true;
        }
        return fail("nothing to leave");
    }
    case Act::ForgeAnchor: {
        if (r.phase != Phase::Forge) return fail("not at forge");
        if (!anchorCards(r, a.a, a.b)) return fail("cannot anchor");
        r.phase = Phase::Map;
        return true;
    }
    case Act::ForgeRankUp: {
        if (r.phase != Phase::Forge || a.a == a.b || a.a < 0 || a.b < 0 || a.a >= deckN || a.b >= deckN) return fail("bad cards");
        CardInstance &x = r.deck[a.a], &y = r.deck[a.b];
        if (x.def != y.def || x.rank != y.rank || x.rank >= 3 || x.fused || y.fused || x.anchored || y.anchored) return fail("not a pair");
        if (deckN - 1 < MIN_DECK) return fail("deck too small");
        x.rank++;
        x.mastery = std::max(x.mastery, y.mastery);
        r.deck.erase(r.deck.begin() + a.b);
        r.phase = Phase::Map;
        return true;
    }
    case Act::ForgeTrain:
    case Act::RestTrain: {
        Phase need = a.type == Act::ForgeTrain ? Phase::Forge : Phase::Rest;
        if (r.phase != need || a.a < 0 || a.a >= deckN) return fail("bad card");
        CardInstance& c = r.deck[a.a];
        if (c.anchored || c.d().evolveTo < 0) return fail("cannot train");
        c.mastery = std::min(modifiersFor(r).masteryCap, c.mastery + TRAIN_AMOUNT);
        r.phase = Phase::Map;
        return true;
    }
    case Act::RestHeal: {
        if (r.phase != Phase::Rest) return fail("not resting");
        heal(r, r.maxHp * (r.ascension >= 4 ? REST_HEAL * 0.7f : REST_HEAL));
        r.phase = Phase::Map;
        return true;
    }
    case Act::EventChoice: {
        if (r.phase != Phase::Event || r.eventId < 0) return fail("no event");
        const EventDef& ev = g_events[r.eventId];
        if (a.a < 0 || a.a >= (int)ev.choices.size()) return fail("bad choice");
        if (!eventChoiceAvailable(r, a.a)) return fail("choice unavailable");
        Rng rng = streamFor(r, P_EVENTFX);
        const std::string& id = ev.id;
        if (id == "fountain") {
            if (a.a == 0) heal(r, r.maxHp * 0.25f);
            else gainGold(r, 30);
        } else if (id == "ghost_merchant") {
            if (a.a == 0) {
                r.maxHp = std::max(20.0f, std::round(r.maxHp * 0.9f));
                r.hp = std::min(r.hp, r.maxHp);
                int d = pickRarityCard(rng, r.pool, 0, 0, 1, {});
                r.deck.push_back(makeCard(d, r.newUid()));
            }
        } else if (id == "altar") {
            if (a.a == 0) {
                // try a few random pairs
                bool done = false;
                for (int t = 0; t < 30 && !done; ++t) {
                    int x = rng.irange(0, deckN - 1), y = rng.irange(0, deckN - 1);
                    done = anchorCards(r, x, y);
                }
                if (!done)   // the altar cannot bind more cards: it strengthens one instead
                    for (int t = 0; t < 30; ++t) {
                        int x = rng.irange(0, (int)r.deck.size() - 1);
                        if (canUpgrade(r.deck[x])) { r.deck[x].rank++; break; }
                    }
            } else if (a.a == 1 && deckN - 1 >= MIN_DECK) {
                r.deck.erase(r.deck.begin() + rng.irange(0, deckN - 1));
            }
        } else if (id == "gambler") {
            if (a.a == 0) {
                if (r.gold < 50) return fail("not enough gold");
                spend(r, 50);
                if (rng.chance(0.5f)) gainGold(r, 120);
            }
        } else if (id == "forge_spirit") {
            if (a.a == 0) {
                std::vector<int> ok;
                for (int i = 0; i < deckN; ++i) if (canUpgrade(r.deck[i])) ok.push_back(i);
                if (!ok.empty()) r.deck[ok[rng.irange(0, (int)ok.size() - 1)]].rank++;
            }
        } else if (id == "cursed_tome") {
            if (a.a == 0) {
                r.maxHp = std::max(20.0f, r.maxHp - 15);
                r.hp = std::min(r.hp, r.maxHp);
                gainRelic(r, pickRelic(rng, r, 'r', {}));
            }
        } else if (id == "lost_adventurer") {
            if (a.a == 0) {
                if (r.gold < 40) return fail("not enough gold");
                spend(r, 40);
                r.deck.push_back(makeCard(pickRarityCard(rng, r.pool, 30, 55, 15, {}), r.newUid()));
            } else if (a.a == 1) {
                heal(r, 20);
            }
        } else if (id == "dojo") {
            if (a.a == 0) {
                r.hp = std::max(1.0f, r.hp - 8);
                int cap = modifiersFor(r).masteryCap;
                for (auto& c : r.deck) if (!c.anchored && c.d().evolveTo >= 0) c.mastery = std::min(cap, c.mastery + 2);
            }
        }
        r.phase = Phase::Map;
        return true;
    }
    case Act::NextChapter: {
        if (r.phase != Phase::ChapterClear) return fail("chapter not cleared");
        r.chapter++;
        r.map = generateMap(r.seed, r.chapter, r.ascension);
        r.node = -1;
        heal(r, r.maxHp * (r.ascension >= 4 ? CHAPTER_HEAL * 0.7f : CHAPTER_HEAL));
        r.phase = Phase::Map;
        return true;
    }
    }
    return fail("unknown action");
}

uint32_t hashRun(const RunState& r) {
    uint32_t h = 2166136261u;
    auto mix = [&](uint32_t v) { for (int i = 0; i < 4; ++i) { h ^= (v >> (i * 8)) & 0xFF; h *= 16777619u; } };
    mix(r.seed); mix(r.chapter); mix((uint32_t)r.node); mix((uint32_t)r.phase); mix((uint32_t)r.gold);
    mix((uint32_t)std::lround(r.hp * 10)); mix((uint32_t)std::lround(r.maxHp * 10));
    for (auto& c : r.deck) { mix(c.def); mix(c.rank); mix(c.mastery); mix(c.anchored); mix((uint32_t)c.reaction); mix((uint32_t)std::lround(c.power() * 10)); }
    for (int x : r.relics) mix(x);
    mix(r.anchors); mix(r.removals); mix(r.upgrades); mix(r.nextUid);
    for (int x : r.offer.cards) mix(x);
    for (int x : r.offer.relics) mix(x);
    mix(r.offer.cardDone); mix(r.offer.relicDone);
    for (size_t i = 0; i < r.shop.cards.size(); ++i) { mix(r.shop.cards[i]); mix(r.shop.cardSold[i]); }
    mix(r.shop.potionSold);
    mix((uint32_t)r.eventId);
    return h;
}
bool debugEnter(RunState& r, NodeType t) {
    for (int i = 0; i < (int)r.map.nodes.size(); ++i)
        if (r.map.nodes[i].type == t) { r.phase = Phase::Map; enterNode(r, i); return true; }
    return false;
}

void debugReward(RunState& r, NodeType t) {
    if (!debugEnter(r, t)) return;
    makeRewards(r, t);
    r.phase = Phase::Reward;
}
}  // namespace RunLogic
