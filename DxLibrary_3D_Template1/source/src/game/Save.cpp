#include "game/Save.h"
#include "core/Paths.h"
#include "core/Tsv.h"
#include "game/Content.h"
#include <cstdio>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <algorithm>

namespace {
std::string joinInts(const std::vector<int>& v) {
    std::string s;
    for (size_t i = 0; i < v.size(); ++i) { if (i) s += ','; s += std::to_string(v[i]); }
    return s;
}
std::vector<int> splitInts(const std::string& s) {
    std::vector<int> v;
    if (s.empty()) return v;
    for (auto& p : TsvTable::split(s, ',')) v.push_back(atoi(p.c_str()));
    return v;
}
std::string relicIds(const std::vector<int>& v) {
    std::string s;
    for (size_t i = 0; i < v.size(); ++i) { if (i) s += ','; s += RunLogic::relics()[v[i]].id; }
    return s;
}
bool parseRelicIds(const std::string& v, std::vector<int>& out) {
    out.clear();
    if (v.empty()) return true;
    for (auto& id : TsvTable::split(v, ',')) {
        int x = RunLogic::findRelic(id);
        if (x < 0) return false;
        out.push_back(x);
    }
    return true;
}
// fingerprint of the content a saved map depends on (room list)
uint32_t contentHash() {
    uint32_t h = 2166136261u;
    for (auto& r : Content::rooms()) for (char c : r.id) { h ^= (uint8_t)c; h *= 16777619u; }
    return h;
}

std::string cardStr(const CardInstance& c) {
    char buf[256];
    std::snprintf(buf, sizeof buf, "%s:%d:%d:%d:%d:%d:%d:%d:%.9g:%d:%u", c.d().id.c_str(), c.rank, c.mastery, c.anchored ? 1 : 0,
                  c.fused ? 1 : 0, (int)c.elems[0], (int)c.elems[1], (int)c.reaction, c.fusedPower, c.fusedCost, c.uid);
    return buf;
}
bool parseCard(const std::string& s, CardInstance& c) {
    auto f = TsvTable::split(s, ':');
    if (f.size() != 11) return false;
    int def = CardDB::find(f[0]);
    if (def < 0) return false;
    c = CardInstance();
    c.def = def;
    c.rank = atoi(f[1].c_str());
    c.mastery = atoi(f[2].c_str());
    c.anchored = f[3] == "1";
    c.fused = f[4] == "1";
    c.elems[0] = (Elem)atoi(f[5].c_str());
    c.elems[1] = (Elem)atoi(f[6].c_str());
    c.reaction = (Reaction)atoi(f[7].c_str());
    c.fusedPower = (float)atof(f[8].c_str());
    c.fusedCost = atoi(f[9].c_str());
    c.uid = (uint32_t)strtoul(f[10].c_str(), nullptr, 10);
    return c.rank >= 1 && c.rank <= 3;
}
}  // namespace

namespace Save {
std::string serializeRun(const RunState& r) {
    std::ostringstream o;
    o << "version=" << SAVE_VERSION << "\n";
    o << "seed=" << r.seed << "\n";
    o << "ascension=" << r.ascension << "\n";
    o << "chapter=" << r.chapter << "\n";
    o << "node=" << r.node << "\n";
    o << "phase=" << (int)r.phase << "\n";
    char buf[64];
    std::snprintf(buf, sizeof buf, "%.9g", r.hp); o << "hp=" << buf << "\n";
    std::snprintf(buf, sizeof buf, "%.9g", r.maxHp); o << "maxhp=" << buf << "\n";
    o << "gold=" << r.gold << "\n";
    o << "relics=" ;
    for (size_t i = 0; i < r.relics.size(); ++i) { if (i) o << ','; o << RunLogic::relics()[r.relics[i]].id; }
    o << "\n";
    o << "anchors=" << r.anchors << "\nremovals=" << r.removals << "\nupgrades=" << r.upgrades << "\nnextuid=" << r.nextUid << "\n";
    for (auto& c : r.deck) o << "card=" << cardStr(c) << "\n";
    auto ids = [](const std::vector<int>& v) {
        std::string s;
        for (size_t i = 0; i < v.size(); ++i) { if (i) s += ','; s += CardDB::get(v[i]).id; }
        return s;
    };
    o << "pool=" << ids(r.pool) << "\n";
    o << "offer_cards=" << ids(r.offer.cards) << "\n";
    o << "offer_relics=" << relicIds(r.offer.relics) << "\n";
    o << "offer_gold=" << r.offer.gold << "\noffer_done=" << (int)r.offer.cardDone << "," << (int)r.offer.relicDone << "\n";
    o << "shop_cards=" << ids(r.shop.cards) << "\n";
    o << "shop_card_prices=" << joinInts(r.shop.cardPrices) << "\n";
    o << "shop_card_sold=" << joinInts(std::vector<int>(r.shop.cardSold.begin(), r.shop.cardSold.end())) << "\n";
    o << "shop_relics=" << relicIds(r.shop.relics) << "\n";
    o << "shop_relic_prices=" << joinInts(r.shop.relicPrices) << "\n";
    o << "shop_relic_sold=" << joinInts(std::vector<int>(r.shop.relicSold.begin(), r.shop.relicSold.end())) << "\n";
    o << "shop_potion=" << (int)r.shop.potionSold << "\n";
    o << "event=" << (r.eventId >= 0 ? RunLogic::events()[r.eventId].id : std::string("-")) << "\n";
    o << "content=" << contentHash() << "\n";
    const RunStats& s = r.stats;
    o << "stats=" << s.kills << "," << s.fusions << "," << s.rankUps << "," << s.evolutions << "," << s.roomsCleared << ","
      << s.goldEarned << "," << s.goldSpent << "," << s.damageTaken << "," << s.healed << "," << s.reactions << ","
      << s.bossesKilled << "," << s.battleSeconds << "," << s.elitesKilled << "," << s.shopSpendCards << "," << s.shopSpendRelics << ","
      << s.shopSpendRemove << "," << s.shopSpendPotion << "," << s.shopSpendUpgrade << "," << s.rankUps << "\n";
    return o.str();
}

bool deserializeRun(const std::string& text, RunState& out) {
    RunState r;
    r.deck.clear();
    std::istringstream in(text);
    std::string line;
    bool versionOk = false;
    bool contentOk = false;
    auto cardIds = [](const std::string& v, std::vector<int>& dst) {
        dst.clear();
        if (v.empty()) return true;
        for (auto& id : TsvTable::split(v, ',')) {
            int d = CardDB::find(id);
            if (d < 0) return false;
            dst.push_back(d);
        }
        return true;
    };
    while (std::getline(in, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        size_t eq = line.find('=');
        if (eq == std::string::npos) continue;
        std::string k = line.substr(0, eq), v = line.substr(eq + 1);
        if (k == "version") versionOk = atoi(v.c_str()) == SAVE_VERSION;
        else if (k == "seed") r.seed = (uint32_t)strtoul(v.c_str(), nullptr, 10);
        else if (k == "ascension") r.ascension = atoi(v.c_str());
        else if (k == "chapter") r.chapter = atoi(v.c_str());
        else if (k == "node") r.node = atoi(v.c_str());
        else if (k == "phase") r.phase = (Phase)atoi(v.c_str());
        else if (k == "hp") r.hp = (float)atof(v.c_str());
        else if (k == "maxhp") r.maxHp = (float)atof(v.c_str());
        else if (k == "gold") r.gold = atoi(v.c_str());
        else if (k == "relics") {
            if (!v.empty()) for (auto& id : TsvTable::split(v, ',')) {
                int x = RunLogic::findRelic(id);
                if (x < 0) return false;
                r.relics.push_back(x);
            }
        }
        else if (k == "anchors") r.anchors = atoi(v.c_str());
        else if (k == "removals") r.removals = atoi(v.c_str());
        else if (k == "upgrades") r.upgrades = atoi(v.c_str());
        else if (k == "nextuid") r.nextUid = (uint32_t)strtoul(v.c_str(), nullptr, 10);
        else if (k == "card") { CardInstance c; if (!parseCard(v, c)) return false; r.deck.push_back(c); }
        else if (k == "pool") { if (!cardIds(v, r.pool)) return false; }
        else if (k == "offer_cards") { if (!cardIds(v, r.offer.cards)) return false; }
        else if (k == "offer_relics") { if (!parseRelicIds(v, r.offer.relics)) return false; }
        else if (k == "offer_gold") r.offer.gold = atoi(v.c_str());
        else if (k == "offer_done") { auto x = splitInts(v); if (x.size() == 2) { r.offer.cardDone = x[0]; r.offer.relicDone = x[1]; } }
        else if (k == "shop_cards") { if (!cardIds(v, r.shop.cards)) return false; }
        else if (k == "shop_card_prices") r.shop.cardPrices = splitInts(v);
        else if (k == "shop_card_sold") { auto x = splitInts(v); r.shop.cardSold.assign(x.begin(), x.end()); }
        else if (k == "shop_relics") { if (!parseRelicIds(v, r.shop.relics)) return false; }
        else if (k == "shop_relic_prices") r.shop.relicPrices = splitInts(v);
        else if (k == "shop_relic_sold") { auto x = splitInts(v); r.shop.relicSold.assign(x.begin(), x.end()); }
        else if (k == "shop_potion") r.shop.potionSold = atoi(v.c_str()) != 0;
        else if (k == "event") {
            r.eventId = -1;
            for (int i = 0; i < (int)RunLogic::events().size(); ++i) if (RunLogic::events()[i].id == v) r.eventId = i;
        }
        else if (k == "content") contentOk = strtoul(v.c_str(), nullptr, 10) == contentHash();
        else if (k == "stats") {
            auto x = TsvTable::split(v, ',');
            if (x.size() >= 13) {
                RunStats& s = r.stats;
                s.kills = atoi(x[0].c_str()); s.fusions = atoi(x[1].c_str()); s.rankUps = atoi(x[2].c_str());
                s.evolutions = atoi(x[3].c_str()); s.roomsCleared = atoi(x[4].c_str()); s.goldEarned = atoi(x[5].c_str());
                s.goldSpent = atoi(x[6].c_str()); s.damageTaken = (float)atof(x[7].c_str()); s.healed = (float)atof(x[8].c_str());
                s.reactions = (uint32_t)strtoul(x[9].c_str(), nullptr, 10); s.bossesKilled = atoi(x[10].c_str());
                s.battleSeconds = (float)atof(x[11].c_str()); s.elitesKilled = atoi(x[12].c_str());
                if (x.size() >= 19) {
                    s.shopSpendCards = atoi(x[13].c_str()); s.shopSpendRelics = atoi(x[14].c_str()); s.shopSpendRemove = atoi(x[15].c_str());
                    s.shopSpendPotion = atoi(x[16].c_str()); s.shopSpendUpgrade = atoi(x[17].c_str());
                }
            }
        }
    }
    if (!versionOk || !contentOk || r.chapter < 1 || r.chapter > CHAPTERS || r.deck.empty()) return false;
    if ((int)r.phase < 0 || (int)r.phase > (int)Phase::Defeat) return false;
    if (r.shop.cardSold.size() != r.shop.cards.size() || r.shop.cardPrices.size() != r.shop.cards.size()) return false;
    if (r.shop.relicSold.size() != r.shop.relics.size() || r.shop.relicPrices.size() != r.shop.relics.size()) return false;
    if (r.phase == Phase::Event && r.eventId < 0) return false;
    if (r.hp <= 0 || r.maxHp <= 0) return false;
    r.map = RunLogic::generateMap(r.seed, r.chapter, r.ascension);
    if (r.node < -1 || r.node >= (int)r.map.nodes.size()) return false;
    bool needsNode = r.phase != Phase::Map && r.phase != Phase::ChapterClear;
    if (needsNode && r.node < 0) return false;
    out = std::move(r);
    return true;
}

bool writeFile(const std::string& path, const std::string& text) {
    std::string tmp = path + ".tmp";
    {
        FILE* f = Paths::open(tmp, "wb");
        if (!f) return false;
        bool ok = std::fwrite(text.data(), 1, text.size(), f) == text.size();
        ok = (std::fflush(f) == 0) && ok;
        std::fclose(f);
        if (!ok) { Paths::remove(tmp); return false; }   // e.g. disk full: keep the old file intact
    }
    // keep one backup per session: the first good file we replace
    static std::vector<std::string> backedUp;
    if (std::find(backedUp.begin(), backedUp.end(), path) == backedUp.end()) {
        if (Paths::exists(path)) Paths::copy(path, path + ".bak");   // the live file stays in place until the atomic replace
        backedUp.push_back(path);
    }
    return Paths::rename(tmp, path);   // atomic replace
}

bool readFile(const std::string& path, std::string& out) {
    std::ifstream f(Paths::widen(path), std::ios::binary);
    if (!f && path.size() > 4 && path.compare(path.size() - 4, 4, ".bak") != 0 && Paths::exists(path + ".bak"))
        return readFile(path + ".bak", out);   // main file lost (e.g. crash mid-save): recover the backup
    if (!f) return false;
    std::ostringstream ss;
    ss << f.rdbuf();
    out = ss.str();
    return true;
}
}  // namespace Save
