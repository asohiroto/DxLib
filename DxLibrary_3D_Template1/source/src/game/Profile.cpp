#include "game/Profile.h"
#include "core/Tsv.h"
#include <sstream>
#include <algorithm>
#include <cstdlib>

std::vector<int> Profile::cardPool() const {
    std::vector<int> pool = RunLogic::defaultPool();
    for (auto& id : unlocked) {
        int d = CardDB::find(id);
        if (d >= 0 && std::find(pool.begin(), pool.end(), d) == pool.end()) pool.push_back(d);
    }
    std::sort(pool.begin(), pool.end());
    return pool;
}

int Profile::shardsFor(const RunState& r, bool victory) {
    int s = (r.chapter - 1) * 12 + r.stats.bossesKilled * 15 + r.stats.elitesKilled * 4 + r.stats.kills / 15;
    if (victory) s += 30;
    s += std::max(0, r.ascension) * 5;
    return std::max(5, s);
}

void Profile::recordRun(const RunState& r, bool victory) {
    ++runs;
    if (victory) {
        ++wins;
        if (r.ascension >= 0 && r.ascension >= ascensionUnlocked && ascensionUnlocked < 5) ascensionUnlocked = r.ascension + 1;
    }
    bestChapter = std::max(bestChapter, victory ? CHAPTERS + 1 : r.chapter);
    reactionsSeen |= r.stats.reactions;
    shards += shardsFor(r, victory);
}

namespace ProfileIO {
std::string serialize(const Profile& p) {
    std::ostringstream o;
    o << "version=1\n";
    o << "shards=" << p.shards << "\n";
    o << "unlocked=";
    for (size_t i = 0; i < p.unlocked.size(); ++i) { if (i) o << ','; o << p.unlocked[i]; }
    o << "\nascension_unlocked=" << p.ascensionUnlocked << "\nascension=" << p.ascension << "\n";
    o << "runs=" << p.runs << "\nwins=" << p.wins << "\nbest_chapter=" << p.bestChapter << "\n";
    o << "reactions=" << p.reactionsSeen << "\nenemies=" << p.enemiesSeen << "\n";
    o << "tutorial=" << (p.tutorialDone ? 1 : 0) << "\nhints=" << p.hintsSeen << "\n";
    o << "bgm=" << p.settings.bgm << "\nse=" << p.settings.se << "\nfullscreen=" << (p.settings.fullscreen ? 1 : 0)
      << "\nshake=" << (p.settings.shake ? 1 : 0) << "\nreducefx=" << (p.settings.reduceFx ? 1 : 0)
      << "\ndmgnum=" << (p.settings.damageNumbers ? 1 : 0) << "\nkeys=";
    for (int i = 0; i < B_COUNT; ++i) { if (i) o << ','; o << p.settings.keys[i]; }
    o << "\n";
    return o.str();
}

bool deserialize(const std::string& text, Profile& out) {
    Profile p;
    std::istringstream in(text);
    std::string line;
    bool ok = false;
    while (std::getline(in, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        size_t eq = line.find('=');
        if (eq == std::string::npos) continue;
        std::string k = line.substr(0, eq), v = line.substr(eq + 1);
        if (k == "version") ok = v == "1";
        else if (k == "shards") p.shards = atoi(v.c_str());
        else if (k == "unlocked") { if (!v.empty()) p.unlocked = TsvTable::split(v, ','); }
        else if (k == "ascension_unlocked") p.ascensionUnlocked = std::clamp(atoi(v.c_str()), 0, 5);
        else if (k == "ascension") p.ascension = std::clamp(atoi(v.c_str()), -1, 5);   // -1 = easy
        else if (k == "runs") p.runs = atoi(v.c_str());
        else if (k == "wins") p.wins = atoi(v.c_str());
        else if (k == "best_chapter") p.bestChapter = atoi(v.c_str());
        else if (k == "reactions") p.reactionsSeen = (uint32_t)strtoul(v.c_str(), nullptr, 10);
        else if (k == "enemies") p.enemiesSeen = strtoull(v.c_str(), nullptr, 10);
        else if (k == "tutorial") p.tutorialDone = v == "1";
        else if (k == "hints") p.hintsSeen = (uint32_t)strtoul(v.c_str(), nullptr, 10);
        else if (k == "bgm") p.settings.bgm = std::clamp((float)atof(v.c_str()), 0.0f, 1.0f);
        else if (k == "se") p.settings.se = std::clamp((float)atof(v.c_str()), 0.0f, 1.0f);
        else if (k == "fullscreen") p.settings.fullscreen = v == "1";
        else if (k == "shake") p.settings.shake = v == "1";
        else if (k == "reducefx") p.settings.reduceFx = v == "1";
        else if (k == "dmgnum") p.settings.damageNumbers = v == "1";
        else if (k == "keys") {
            auto ks = TsvTable::split(v, ',');
            if (ks.size() == B_COUNT) for (int i = 0; i < B_COUNT; ++i) { int c = atoi(ks[i].c_str()); if (c > 0 && c < 256) p.settings.keys[i] = c; }
        }
    }
    if (!ok) return false;
    p.ascension = std::min(p.ascension, p.ascensionUnlocked);
    out = p;
    return true;
}
}  // namespace ProfileIO
