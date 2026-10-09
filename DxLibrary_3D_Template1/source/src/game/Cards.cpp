#include "game/Cards.h"
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <algorithm>

// ---------------------------------------------------------------- Types
const char* elemName(Elem e) {
    switch (e) {
    case Elem::Fire: return "火";
    case Elem::Ice: return "氷";
    case Elem::Thunder: return "雷";
    case Elem::Earth: return "土";
    default: return "無";
    }
}

const char* reactionName(Reaction r) {
    switch (r) {
    case Reaction::Steam: return "蒸気";
    case Reaction::Blast: return "爆雷";
    case Reaction::Lava: return "溶岩";
    case Reaction::Supercond: return "超伝導";
    case Reaction::Permafrost: return "凍土";
    case Reaction::Magnet: return "磁砂";
    case Reaction::Purify: return "純化";
    case Reaction::Resonance: return "共鳴";
    default: return "";
    }
}

const char* reactionDesc(Reaction r) {
    switch (r) {
    case Reaction::Steam: return "命中地点に蒸気。敵の攻撃が鈍る";
    case Reaction::Blast: return "命中時に爆発する";
    case Reaction::Lava: return "命中地点に溶岩が残る";
    case Reaction::Supercond: return "凍えた敵から雷が連鎖する";
    case Reaction::Permafrost: return "命中地点が凍土になり敵が鈍る";
    case Reaction::Magnet: return "敵を命中地点へ引き寄せる";
    case Reaction::Purify: return "状態異常の効果が2倍";
    case Reaction::Resonance: return "威力+30%";
    default: return "";
    }
}

Reaction reactionOf(Elem a, Elem b) {
    if (a == Elem::None || b == Elem::None) return Reaction::Resonance;
    if (a == b) return Reaction::Purify;
    auto has = [&](Elem x, Elem y) { return (a == x && b == y) || (a == y && b == x); };
    if (has(Elem::Fire, Elem::Ice)) return Reaction::Steam;
    if (has(Elem::Fire, Elem::Thunder)) return Reaction::Blast;
    if (has(Elem::Fire, Elem::Earth)) return Reaction::Lava;
    if (has(Elem::Ice, Elem::Thunder)) return Reaction::Supercond;
    if (has(Elem::Ice, Elem::Earth)) return Reaction::Permafrost;
    if (has(Elem::Thunder, Elem::Earth)) return Reaction::Magnet;
    return Reaction::None;
}

// ---------------------------------------------------------------- DB
namespace {
std::vector<CardDef> g_defs;

std::vector<std::string> splitTab(const std::string& line) {
    std::vector<std::string> out;
    std::string cur;
    for (char c : line) {
        if (c == '\t') { out.push_back(cur); cur.clear(); }
        else if (c != '\r') cur.push_back(c);
    }
    out.push_back(cur);
    return out;
}

Form parseForm(const std::string& s) {
    if (s == "area") return Form::Area;
    if (s == "melee") return Form::Melee;
    if (s == "place") return Form::Place;
    if (s == "guard") return Form::Guard;
    return Form::Bolt;
}
Elem parseElem(const std::string& s) {
    if (s == "fire") return Elem::Fire;
    if (s == "ice") return Elem::Ice;
    if (s == "thunder") return Elem::Thunder;
    if (s == "earth") return Elem::Earth;
    return Elem::None;
}
}  // namespace

namespace CardDB {
bool load(const std::string& path, std::string* err) {
    std::ifstream f(path, std::ios::binary);
    if (!f) { if (err) *err = "cannot open " + path; return false; }
    std::string line;
    std::getline(f, line);
    if (line.size() >= 3 && (unsigned char)line[0] == 0xEF) line = line.substr(3);  // BOM
    auto header = splitTab(line);
    std::unordered_map<std::string, int> col;
    for (int i = 0; i < (int)header.size(); ++i) col[header[i]] = i;
    std::vector<CardDef> defs;
    std::vector<std::string> evolveIds;
    while (std::getline(f, line)) {
        if (line.empty() || line[0] == '#') continue;
        auto v = splitTab(line);
        auto S = [&](const char* k) -> std::string {
            auto it = col.find(k);
            return (it != col.end() && it->second < (int)v.size()) ? v[it->second] : std::string();
        };
        auto F = [&](const char* k) { std::string s = S(k); return s.empty() ? 0.0f : (float)atof(s.c_str()); };
        CardDef d;
        d.id = S("id"); d.name = S("name"); d.desc = S("desc");
        d.form = parseForm(S("form")); d.elem = parseElem(S("elem"));
        d.cost = (int)F("cost"); d.power = F("power"); d.count = std::max(1, (int)F("count"));
        d.spread = F("spread"); d.speed = F("speed"); d.pierce = (int)F("pierce");
        d.radius = F("radius"); d.duration = F("duration"); d.tick = F("tick");
        d.status = (int)F("status"); d.range = F("range"); d.knock = F("knock");
        d.special = S("special"); if (d.special == "-") d.special.clear();
        { std::string ra = S("rarity"); d.rarity = ra.empty() ? 'c' : ra[0]; }
        d.locked = S("locked") == "1";
        d.evolveId = S("evolve"); if (d.evolveId == "-") d.evolveId.clear();
        if (d.id.empty()) continue;
        defs.push_back(d);
    }
    for (auto& d : defs) {
        if (d.evolveId.empty()) continue;
        for (int i = 0; i < (int)defs.size(); ++i)
            if (defs[i].id == d.evolveId) { d.evolveTo = i; defs[i].isEvolved = true; }
        if (d.evolveTo < 0 && err) { *err = "unknown evolve id " + d.evolveId; return false; }
    }
    if (defs.empty()) { if (err) *err = "no cards"; return false; }
    g_defs = std::move(defs);
    return true;
}
const std::vector<CardDef>& all() { return g_defs; }
const CardDef& get(int idx) { return g_defs[(idx >= 0 && idx < (int)g_defs.size()) ? idx : 0]; }
int find(const std::string& id) {
    for (int i = 0; i < (int)g_defs.size(); ++i) if (g_defs[i].id == id) return i;
    return -1;
}
}  // namespace CardDB

// ---------------------------------------------------------------- Instance
float rankMul(int rank) { return rank >= 3 ? 2.1f : (rank == 2 ? 1.6f : 1.0f); }

float CardInstance::power() const { return fused ? fusedPower : d().power * rankMul(rank); }
int CardInstance::cost() const { return fused ? fusedCost : d().cost; }

std::string CardInstance::displayName() const {
    std::string s;
    if (fused && reaction != Reaction::None) { s += reactionName(reaction); s += "・"; }
    s += d().name;
    if (rank >= 2) s += rank == 2 ? "＋" : "＋＋";
    return s;
}

CardInstance makeCard(int def, uint32_t uid) {
    CardInstance c;
    c.uid = uid;
    c.def = def;
    c.elems[0] = CardDB::get(def).elem;
    return c;
}

static void appendMaterials(std::vector<CardInstance>& out, const CardInstance& c) {
    if (!c.materials.empty()) for (auto& m : c.materials) out.push_back(m);
    else out.push_back(c);
}

FusePreview previewFuse(const CardInstance& L, const CardInstance& R, uint32_t newUid) {
    FusePreview p;
    if (L.def < 0 || R.def < 0) { p.reason = "カードがない"; return p; }
    if (L.anchored || R.anchored) { p.reason = "定着カードは合成できない"; return p; }
    // Rank up: identical unfused cards
    if (!L.fused && !R.fused && L.def == R.def && L.rank == R.rank) {
        if (L.rank >= 3) { p.reason = "ランクは3が最大"; return p; }
        p.kind = FuseKind::RankUp;
        p.result = L;
        p.result.uid = newUid;
        p.result.rank = L.rank + 1;
        p.result.mastery = std::max(L.mastery, R.mastery);
        p.result.materials.clear();
        appendMaterials(p.result.materials, L);
        appendMaterials(p.result.materials, R);
        return p;
    }
    if (L.materialCount() + R.materialCount() > MAX_MATERIALS) { p.reason = "これ以上は不安定"; return p; }
    FusePreview h;
    h.kind = FuseKind::Hybrid;
    CardInstance& c = h.result;
    c.uid = newUid;
    c.def = L.def;
    c.rank = L.rank;
    c.fused = true;
    Elem a = L.elems[0], b = R.elems[0];
    Reaction re = reactionOf(a, b);
    c.reaction = re;
    if (re == Reaction::Resonance) {
        c.elems[0] = (a != Elem::None) ? a : b;
        c.elems[1] = Elem::None;
    } else if (re == Reaction::Purify) {
        c.elems[0] = a; c.elems[1] = Elem::None;
    } else {
        c.elems[0] = a; c.elems[1] = b;
    }
    float pw = (L.power() + R.power()) * FUSE_POWER_MUL;
    if (re == Reaction::Resonance) pw *= RESONANCE_MUL;
    c.fusedPower = pw;
    c.fusedCost = std::max(1, std::min(MAX_COST, L.cost() + R.cost() - 1));
    c.mastery = std::min(MASTERY_CAP, (L.mastery + R.mastery) / 2);
    appendMaterials(c.materials, L);
    appendMaterials(c.materials, R);
    return h;
}

bool evolveCard(CardInstance& c, int cap) {
    if (!c.canEvolve(cap)) return false;
    const CardDef& from = c.d();
    int to = from.evolveTo;
    if (c.fused) c.fusedPower *= CardDB::get(to).power / std::max(1.0f, from.power);
    c.def = to;
    c.mastery = 0;
    return true;
}

int fusionSelfTest(std::string* log) {
    int bad = 0;
    auto check = [&](const CardInstance& c, const char* what) {
        bool ok = c.power() > 0 && c.cost() >= 0 && c.cost() <= MAX_COST &&
                  !(c.elems[1] != Elem::None && c.elems[0] == c.elems[1]) &&
                  c.materialCount() <= MAX_MATERIALS && c.mastery <= MASTERY_CAP;
        if (!ok) {
            ++bad;
            if (log && bad < 20) *log += std::string(what) + ": " + c.displayName() + "\n";
        }
    };
    const int n = (int)CardDB::all().size();
    std::vector<CardInstance> singles;
    uint32_t uid = 1;
    for (int i = 0; i < n; ++i)
        for (int r = 1; r <= 3; ++r) {
            CardInstance c = makeCard(i, uid++);
            c.rank = r;
            c.mastery = MASTERY_CAP;
            check(c, "single");
            singles.push_back(c);
        }
    std::vector<CardInstance> fusedOnce;
    for (auto& a : singles)
        for (auto& b : singles) {
            auto p = previewFuse(a, b, uid++);
            if (p.kind == FuseKind::Invalid) continue;
            check(p.result, "fuse1");
            if (p.kind == FuseKind::Hybrid && (fusedOnce.size() < 4000)) fusedOnce.push_back(p.result);
            CardInstance e = p.result;
            e.mastery = MASTERY_CAP;
            if (evolveCard(e)) check(e, "evolve");
        }
    for (size_t i = 0; i < fusedOnce.size(); i += 7)
        for (size_t j = 0; j < fusedOnce.size(); j += 13) {
            auto p = previewFuse(fusedOnce[i], fusedOnce[j], uid++);
            if (p.kind != FuseKind::Invalid) check(p.result, "fuse2");
            auto q = previewFuse(fusedOnce[i], singles[j % singles.size()], uid++);
            if (q.kind != FuseKind::Invalid) check(q.result, "fuse2s");
        }
    return bad;
}
