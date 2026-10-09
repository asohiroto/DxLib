#include "sim/Policy.h"
#include "game/World.h"
#include <algorithm>
#include <cmath>

PolicyConfig PolicyConfig::fromName(const std::string& name, float skill) {
    PolicyConfig c;
    c.skill = skill;
    if (name == "basic") { c.useCards = false; c.fuse = false; c.evolve = false; }
    else if (name == "nofuse") { c.fuse = false; c.evolve = false; }
    else if (name == "fuse") { c.fuse = true; c.evolve = false; }
    else if (name == "fuseevo") { c.fuse = true; c.evolve = true; }
    else if (name == "evo") { c.fuse = false; c.evolve = true; }
    else if (name == "fuseall") { c.fuse = true; c.evolve = true; c.fuseAlways = true; }
    else if (name == "random") { c.randomCards = true; }
    return c;
}

namespace {
int countEnemiesNear(const World& w, Vec2 p, float r) {
    int n = 0;
    for (auto& e : w.enemies) if (e.alive && e.spawnT <= 0 && dist(e.pos, p) <= r + e.r) ++n;
    return n;
}
}  // namespace

// Rough expected damage per mana for using the card now. Writes where to aim.
static float affinity(const World& w, const CardInstance& c, const Enemy* t) {
    if (!t) return 1.0f;
    const EnemyDef& d = t->d();
    bool weak = false, resist = false;
    for (Elem e : c.elems) {
        if (e == Elem::None) continue;
        if (e == d.weak) weak = true;
        if (e == d.resist) resist = true;
    }
    return (weak ? w.mods.weakMul : 1.0f) * (resist ? RESIST_MUL : 1.0f);
}

float Policy::cardValue(const World& w, int slot, Vec2* aimAt) const {
    const CardInstance& c = w.hand.slots[slot];
    if (c.def < 0) return -1;
    const CardDef& d = c.d();
    const Player& p = w.player;
    float pw = c.power();
    float cost = std::max(0.6f, (float)c.cost());
    const Enemy* ne = w.nearestEnemy(p.pos);
    float reactBonus = (c.fused && c.reaction != Reaction::None) ? 1.25f : 1.0f;
    switch (d.form) {
    case Form::Bolt: {
        float range = d.range > 0 ? d.range : 260;
        const Enemy* t = w.nearestEnemy(p.pos, range * 0.9f);
        if (!t) return -1;
        *aimAt = t->pos;
        int n = d.count > 1 ? d.count + c.rank - 1 : 1;
        float hits = n > 1 ? std::min((float)n, 1.0f + 0.5f * countEnemiesNear(w, t->pos, 40)) : 1.0f;
        if (d.pierce > 0) hits += 0.5f * (countEnemiesNear(w, t->pos, 60) - 1);
        if (d.radius > 0) hits = std::max(hits, (float)countEnemiesNear(w, t->pos, d.radius));
        return pw * hits * reactBonus * affinity(w, c, t) / cost;
    }
    case Form::Area: {
        if (d.special == "self") {
            int n = countEnemiesNear(w, p.pos, d.radius);
            if (n == 0) return -1;
            *aimAt = ne ? ne->pos : p.pos;
            return pw * n * reactBonus / cost;
        }
        const Enemy* t = w.nearestEnemy(p.pos, d.range);
        if (!t) return -1;
        *aimAt = t->pos;
        int n = std::max(1, countEnemiesNear(w, t->pos, d.radius));
        float ticks = d.duration > 0 ? d.duration / std::max(0.1f, d.tick) * 0.5f : 1.0f;
        return pw * n * ticks * reactBonus * affinity(w, c, t) / cost;
    }
    case Form::Melee: {
        if (!ne || dist(ne->pos, p.pos) > d.radius + ne->r + 4) return -1;
        *aimAt = ne->pos;
        return pw * countEnemiesNear(w, p.pos, d.radius) * reactBonus * affinity(w, c, ne) / cost;
    }
    case Form::Place: {
        if (!ne) return -1;
        *aimAt = ne->pos;
        if (d.special == "turret") return pw * d.duration / d.tick * 0.4f / cost;
        // wall: worth it when enemies are close-ish
        if (dist(ne->pos, p.pos) > 120) return -1;
        return pw * 3 / cost;
    }
    case Form::Guard: {
        if (d.special == "heal") {
            if (p.hp > p.maxHp - pw) return -1;
            return pw * 1.5f / cost;
        }
        if (p.shield > 0) return -1;
        int threats = 0;
        for (auto& b : w.ebullets) if (dist(b.pos, p.pos) < 70) ++threats;
        if (ne && dist(ne->pos, p.pos) < 50) ++threats;
        if (threats == 0) return -1;
        return pw * (1 + threats * 0.5f) / cost;
    }
    }
    return -1;
}

InputState Policy::think(const World& w) {
    InputState in;
    const Player& p = w.player;
    const float sk = clampv(cfg_.skill, 0.0f, 1.0f);
    const int reactFrames = (int)(20 - 12 * sk);

    // ------------------------------------------------ movement
    const Enemy* ne = w.nearestEnemy(p.pos);
    Vec2 force{0, 0};
    if (ne) {
        Vec2 to = ne->pos - p.pos;
        float d = to.len();
        Vec2 dir = to.norm();
        float want = 120;
        if (d < want - 30) force -= dir * 1.2f;
        else if (d > want + 50) force += dir * 0.8f;
        if (--strafeT_ <= 0) { strafeT_ = 60 + rng_.irange(0, 90); strafeSign_.x = rng_.chance(0.5f) ? 1.0f : -1.0f; }
        force += Vec2{-dir.y, dir.x} * (0.6f * strafeSign_.x);
    } else {
        force += (Vec2{320, 200} - p.pos) * 0.01f;
    }
    // other enemies push
    for (auto& e : w.enemies) {
        if (!e.alive || e.spawnT > 0) continue;
        Vec2 d = p.pos - e.pos;
        float l = d.len();
        if (l < 60 && l > 0.1f) force += d / l * ((60 - l) / 60.0f) * 1.2f;
        bool charging = (e.ai() == AiKind::Charger || e.ai() == AiKind::BossGolem) && (e.state == 1 || e.state == 2);
        bool sniping = e.ai() == AiKind::Sniper && e.state == 1;
        if (charging || sniping) {
            Vec2 perp{-e.dir.y, e.dir.x};
            float side = (p.pos - e.pos).dot(perp) >= 0 ? 1.0f : -1.0f;
            if (l < 260) force += perp * side * 1.5f;
        }
    }
    // telegraphed hazards: step out once noticed
    for (auto& h : w.hazards) {
        if (h.fired || h.t < reactFrames / 60.0f) continue;
        if (h.kind == HazardKind::Circle) {
            Vec2 d = p.pos - h.pos;
            float l = d.len();
            if (l < h.r + p.r + 10) force += (l > 0.1f ? d / l : Vec2{1, 0}) * 2.0f * (0.5f + sk);
        } else {
            Vec2 rel = p.pos - h.pos;
            Vec2 perp{-h.dir.y, h.dir.x};
            float side = rel.dot(perp);
            if (rel.dot(h.dir) > 0 && std::fabs(side) < h.r + p.r + 10) force += perp * (side >= 0 ? 1.0f : -1.0f) * 2.0f * (0.5f + sk);
        }
    }
    // bullets: sidestep, with reaction skill weighting; some bullets are simply not noticed
    float soonest = 99;
    Vec2 soonestVel;
    const float seenAge = reactFrames / 60.0f;
    const uint32_t missThreshold = (uint32_t)((1.0f - sk) * 50.0f);
    for (auto& b : w.ebullets) {
        if (b.age < seenAge) continue;   // not yet noticed
        uint32_t hh = (b.id * 2654435761u) ^ attentionSalt_;
        if ((hh >> 8) % 100 < missThreshold) continue;   // attention miss
        Vec2 rel = p.pos - b.pos;
        float sp2 = b.vel.len2();
        if (sp2 < 1) continue;
        float t = rel.dot(b.vel) / sp2;
        if (t < 0 || t > 1.0f) continue;
        Vec2 closest = b.pos + b.vel * t;
        float miss = dist(closest, p.pos);
        if (miss < p.r + b.r + 8) {
            Vec2 perp = Vec2{-b.vel.y, b.vel.x}.norm();
            float side = (p.pos - closest).dot(perp) >= 0 ? 1.0f : -1.0f;
            force += perp * side * (1.6f * sk) * (1.0f - t);
            if (miss < p.r + b.r + 1 && t < soonest) { soonest = t; soonestVel = b.vel; }
        }
    }
    // walls / pillars
    if (p.pos.x < ARENA_L + 30) force.x += 1; if (p.pos.x > ARENA_R - 30) force.x -= 1;
    if (p.pos.y < ARENA_T + 30) force.y += 1; if (p.pos.y > ARENA_B - 30) force.y -= 1;
    for (auto& rc : w.pillars) {
        Vec2 c{rc.x + rc.w / 2, rc.y + rc.h / 2};
        Vec2 d = p.pos - c;
        float l = d.len();
        if (l < 34 && l > 0.1f) force += d / l * 0.8f;
    }
    in.move = force.len() > 0.15f ? force.norm() : Vec2{0, 0};

    // dash: schedule after reaction delay, may fail on low skill
    bool danger = soonest < 0.35f;
    if (ne && dist(ne->pos, p.pos) < ne->r + p.r + 6) danger = true;
    if (danger && dodgeT_ < 0 && p.dashCd <= 0) {
        if (rng_.chance(0.35f + 0.6f * sk)) dodgeT_ = std::max(1, reactFrames / 2);
        else dodgeT_ = 9999;  // gave up this one
    }
    if (dodgeT_ >= 0) {
        if (dodgeT_ == 9999) { if (!danger) dodgeT_ = -1; }
        else if (--dodgeT_ <= 0) {
            in.dash = true;
            if (soonest < 99) {
                Vec2 perp = Vec2{-soonestVel.y, soonestVel.x}.norm();
                in.move = (in.move.len2() > 0.01f && std::fabs(in.move.dot(perp)) > 0.3f) ? in.move : perp;
            }
            dodgeT_ = -1;
        }
    }

    // ------------------------------------------------ aim
    Vec2 aim = p.pos + p.aimDir * 50;
    if (ne) {
        Vec2 lead = ne->pos;
        float err = (1.0f - sk) * 0.3f;
        Vec2 d = lead - p.pos;
        aim = p.pos + d.rotated(rng_.range(-err, err));
    }
    in.attack = ne != nullptr;

    // ------------------------------------------------ cards
    if (pendingCardAim_ > 0) { --pendingCardAim_; aim = lastAim_; }
    if (cfg_.useCards && --decideT_ <= 0) {
        decideT_ = reactFrames;
        // evolve first
        bool acted = false;
        if (cfg_.evolve && p.evolveCd <= 0) {
            for (int i = 0; i < HAND_SIZE; ++i)
                if (w.hand.slots[i].def >= 0 && w.hand.slots[i].canEvolve(w.mods.masteryCap)) { in.evolve = true; acted = true; break; }
        }
        if (!acted && cfg_.randomCards) {
            int r = rng_.irange(0, 5);
            if (r <= 3) in.useSlot = r;
            else if (r == 4) in.fuseSlot = rng_.irange(0, 2);
            acted = true;
        }
        if (!acted && cfg_.fuse && p.fuseCd <= 0) {
            float bestGain = cfg_.fuseAlways ? -1e9f : 0.0f;
            int bestL = -1, valid = 0;
            for (int l = 0; l + 1 < HAND_SIZE; ++l) {
                FusePreview pv = w.previewPair(l);
                if (pv.kind == FuseKind::Invalid) continue;
                ++valid;
                const CardInstance &L = w.hand.slots[l], &R = w.hand.slots[l + 1];
                // don't burn training on a card that is close to evolving
                if (!cfg_.fuseAlways && cfg_.evolve &&
                    (L.canEvolve(w.mods.masteryCap) || R.canEvolve(w.mods.masteryCap) || L.mastery >= w.mods.masteryCap - 2 || R.mastery >= w.mods.masteryCap - 2)) continue;
                float sep = (L.power() + R.power()) / std::max(1, L.cost() + R.cost());
                float fused = pv.result.power() / std::max(1, pv.result.cost());
                float rv = 1.0f;
                switch (pv.result.reaction) {
                case Reaction::Blast: rv = 1.35f; break;
                case Reaction::Lava: rv = 1.25f; break;
                case Reaction::Supercond: rv = 1.25f; break;
                case Reaction::Permafrost: rv = 1.15f; break;
                case Reaction::Steam: rv = 1.1f; break;
                case Reaction::Magnet: rv = 1.1f; break;
                case Reaction::Purify: rv = 1.15f; break;
                default: break;
                }
                // ranking up also widens the card (more shots / radius)
                if (pv.kind == FuseKind::RankUp) rv = 1.15f;
                {   // prefer fusions that hit the current target's weakness
                    const Enemy* tgt = w.boss() ? w.boss() : w.nearestEnemy(w.player.pos);
                    float before = std::max(affinity(w, L, tgt), affinity(w, R, tgt));
                    rv *= affinity(w, pv.result, tgt) / before;
                }
                float gain = fused * rv - sep;
                if (gain > bestGain) { bestGain = gain; bestL = l; }
            }
            if (bestL >= 0) {
                in.fuseSlot = bestL;
                acted = true;
                stats_fuseChosen++;
            } else if (valid > 0) {
                stats_fuseSkipped++;
            }
        }
        if (!acted) {
            float best = 0;
            int bestS = -1;
            Vec2 bestAim;
            for (int i = 0; i < HAND_SIZE; ++i) {
                const CardInstance& c = w.hand.slots[i];
                if (c.def < 0 || c.cost() > w.mana + 1e-4f) continue;
                Vec2 a;
                float v = cardValue(w, i, &a);
                if (v > best) { best = v; bestS = i; bestAim = a; }
            }
            if (bestS >= 0) {
                in.useSlot = bestS;
                float err = (1.0f - sk) * 12.0f;
                aim = bestAim + Vec2{rng_.range(-err, err), rng_.range(-err, err)};
                lastAim_ = aim;
                pendingCardAim_ = 0;
            }
        }
    }
    in.aim = aim;
    return in;
}
