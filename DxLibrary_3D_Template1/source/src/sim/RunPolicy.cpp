#include "sim/RunPolicy.h"
#include <algorithm>

float RunPolicy::cardScore(int def, const RunState& r) const {
    const CardDef& d = CardDB::get(def);
    float s = d.power / std::max(1, d.cost);
    if (d.form == Form::Area || d.count > 1 || d.pierce > 0) s *= 1.3f;
    if (d.special == "heal") s = r.hp < r.maxHp * 0.6f ? 30.0f : 6.0f;
    if (d.special == "shield") s *= 0.8f;
    s *= d.rarity == 'r' ? 1.3f : (d.rarity == 'u' ? 1.15f : 1.0f);
    return s;
}

Action RunPolicy::decide(const RunState& r) {
    Action a{Act::Leave};
    const int deckN = (int)r.deck.size();
    switch (r.phase) {
    case Phase::Map: {
        auto ch = RunLogic::choosableNodes(r);
        if (ch.empty()) return a;
        if (cfg_.random) return {Act::ChooseNode, ch[rng_.irange(0, (int)ch.size() - 1)]};
        float hpRatio = r.hp / r.maxHp;
        int best = ch[0];
        float bestS = -1e9f;
        for (int n : ch) {
            NodeType t = r.map.nodes[n].type;
            float s = 0;
            switch (t) {
            case NodeType::Battle: s = 3; break;
            case NodeType::Elite: s = hpRatio > 0.7f ? 5 : -4; break;
            case NodeType::Shop: s = r.gold >= 120 ? 6 : 1; break;
            case NodeType::Rest: s = hpRatio < 0.6f ? 8 : 2; break;
            case NodeType::Event: s = 3.5f; break;
            case NodeType::Forge: s = 5; break;
            case NodeType::Boss: s = 10; break;
            }
            s += rng_.range(0, 1.5f);
            if (s > bestS) { bestS = s; best = n; }
        }
        return {Act::ChooseNode, best};
    }
    case Phase::Reward: {
        if (!r.offer.relicDone) return {Act::TakeRelic, cfg_.random ? rng_.irange(0, (int)r.offer.relics.size() - 1) : 0};
        if (cfg_.random) return rng_.chance(0.7f) ? Action{Act::TakeCard, rng_.irange(0, (int)r.offer.cards.size() - 1)} : Action{Act::SkipCard};
        if (deckN >= 18) return {Act::SkipCard};
        int best = 0;
        float bs = -1;
        for (int i = 0; i < (int)r.offer.cards.size(); ++i) {
            float s = cardScore(r.offer.cards[i], r);
            if (s > bs) { bs = s; best = i; }
        }
        return bs >= 9.0f || deckN < 12 ? Action{Act::TakeCard, best} : Action{Act::SkipCard};
    }
    case Phase::Shop: {
        if (cfg_.random) {
            int x = rng_.irange(0, 4);
            if (x == 0 && !r.shop.cards.empty()) { int i = rng_.irange(0, (int)r.shop.cards.size() - 1); if (!r.shop.cardSold[i] && r.gold >= r.shop.cardPrices[i]) return {Act::ShopBuyCard, i}; }
            return a;
        }
        if (!r.shop.potionSold && r.hp < r.maxHp * 0.6f && r.gold >= POTION_PRICE) return {Act::ShopPotion};
        for (int i = 0; i < (int)r.shop.relics.size(); ++i)
            if (!r.shop.relicSold[i] && r.gold >= r.shop.relicPrices[i] + 20) return {Act::ShopBuyRelic, i};
        if (r.gold >= RunLogic::upgradePrice(r) && r.upgrades < 2) {
            for (int i = 0; i < deckN; ++i)
                if (RunLogic::canUpgrade(r.deck[i]) && r.deck[i].d().rarity != 'c') return {Act::ShopUpgrade, i};
        }
        // remove the weakest starter card
        if (r.gold >= RunLogic::removePrice(r) && deckN > 9 && r.removals < 3) {
            int worst = -1;
            float ws = 1e9f;
            for (int i = 0; i < deckN; ++i) {
                const CardInstance& c = r.deck[i];
                if (c.anchored || c.rank > 1 || c.d().isEvolved) continue;
                float s = cardScore(c.def, r);
                if (s < ws) { ws = s; worst = i; }
            }
            if (worst >= 0) return {Act::ShopRemove, worst};
        }
        for (int i = 0; i < (int)r.shop.cards.size(); ++i)
            if (!r.shop.cardSold[i] && r.gold >= r.shop.cardPrices[i] + 20 && cardScore(r.shop.cards[i], r) > 11 && deckN < 17)
                return {Act::ShopBuyCard, i};
        // upgrade the strongest upgradable card
        if (r.gold >= RunLogic::upgradePrice(r)) {
            int best = -1;
            float bs = 0;
            for (int i = 0; i < deckN; ++i)
                if (RunLogic::canUpgrade(r.deck[i]) && cardScore(r.deck[i].def, r) > bs) { bs = cardScore(r.deck[i].def, r); best = i; }
            if (best >= 0) return {Act::ShopUpgrade, best};
        }
        return a;
    }
    case Phase::Forge: {
        if (cfg_.random) return {Act::ForgeTrain, rng_.irange(0, deckN - 1)};
        if (cfg_.anchor && r.anchors < MAX_ANCHORS && deckN > MIN_DECK + 2) {
            int bi = -1, bj = -1;
            float best = 0;
            for (int i = 0; i < deckN; ++i)
                for (int j = 0; j < deckN; ++j) {
                    FusePreview p = RunLogic::anchorPreview(r, i, j);
                    if (p.kind != FuseKind::Hybrid) continue;
                    const CardInstance &x = r.deck[i], &y = r.deck[j];
                    float gain = p.result.power() / std::max(1, p.result.cost()) - (x.power() + y.power()) / std::max(1, x.cost() + y.cost());
                    if (p.result.reaction != Reaction::Resonance && p.result.reaction != Reaction::Purify) gain += 4;
                    if (gain > best) { best = gain; bi = i; bj = j; }
                }
            if (bi >= 0 && best > 5) return {Act::ForgeAnchor, bi, bj};
        }
        // rank up a duplicate pair
        for (int i = 0; i < deckN; ++i)
            for (int j = i + 1; j < deckN; ++j) {
                const CardInstance &x = r.deck[i], &y = r.deck[j];
                if (x.def == y.def && x.rank == y.rank && x.rank < 3 && !x.fused && !y.fused && !x.anchored && !y.anchored && deckN > MIN_DECK + 2)
                    return {Act::ForgeRankUp, i, j};
            }
        int t = -1;
        for (int i = 0; i < deckN; ++i)
            if (!r.deck[i].anchored && r.deck[i].d().evolveTo >= 0 && (t < 0 || r.deck[i].mastery > r.deck[t].mastery)) t = i;
        return t >= 0 ? Action{Act::ForgeTrain, t} : a;
    }
    case Phase::Rest: {
        if (r.hp < r.maxHp * 0.75f || cfg_.random) return {Act::RestHeal};
        int t = -1;
        for (int i = 0; i < deckN; ++i)
            if (!r.deck[i].anchored && r.deck[i].d().evolveTo >= 0 && (t < 0 || r.deck[i].mastery > r.deck[t].mastery)) t = i;
        return t >= 0 ? Action{Act::RestTrain, t} : Action{Act::RestHeal};
    }
    case Phase::Event: {
        const EventDef& ev = RunLogic::events()[r.eventId];
        if (cfg_.random) {
            int c = rng_.irange(0, (int)ev.choices.size() - 1);
            return {Act::EventChoice, RunLogic::eventChoiceAvailable(r, c) ? c : (int)ev.choices.size() - 1};
        }
        if (ev.id == "ghost_merchant") return {Act::EventChoice, r.hp > r.maxHp * 0.6f ? 0 : 1};
        if (ev.id == "dojo") return {Act::EventChoice, r.hp > 30 ? 0 : 1};
        if (ev.id == "fountain") return {Act::EventChoice, r.hp < r.maxHp * 0.9f ? 0 : 1};
        if (ev.id == "altar") return {Act::EventChoice, cfg_.anchor && deckN > MIN_DECK + 2 ? 0 : 2};
        if (ev.id == "gambler") return {Act::EventChoice, r.gold >= 90 ? 0 : 1};
        if (ev.id == "cursed_tome") return {Act::EventChoice, r.maxHp > 70 ? 0 : 1};
        if (ev.id == "lost_adventurer") return {Act::EventChoice, r.gold >= 70 ? 0 : (r.hp < r.maxHp * 0.7f ? 1 : 2)};
        return {Act::EventChoice, 0};
    }
    case Phase::ChapterClear: return {Act::NextChapter};
    default: return a;
    }
}
