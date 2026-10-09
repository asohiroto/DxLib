#include "game/World.h"
#include <algorithm>
#include <cmath>

namespace {
bool hasElem(const Hit& h, Elem e) { return h.elems[0] == e || h.elems[1] == e; }

bool circleRect(Vec2 c, float r, const RectF& rc) {
    float nx = clampv(c.x, rc.x, rc.x + rc.w), ny = clampv(c.y, rc.y, rc.y + rc.h);
    return dist2(c, {nx, ny}) < r * r;
}
}  // namespace

// ---------------------------------------------------------------- init
void World::init(const RoomSetup& s) {
    *this = World();
    rng = Rng(s.seed);
    mods = s.mods;
    nextUid_ = s.nextUid;
    player.hp = s.hp;
    player.maxHp = s.maxHp;
    player.shield = mods.startShield;
    player.shieldT = mods.startShield > 0 ? 6.0f : 0.0f;
    mana = mods.manaStart;
    if (s.room) {
        const RoomDef& r = *s.room;
        pillars = Content::arenas()[r.arena].pillars;
        waves_ = r.waves;
        isBossRoom = r.kind == RoomKind::Boss;
        if (isBossRoom) timeoutFrames_ = BOSS_TIMEOUT_FRAMES;
    }
    waveCount = (int)waves_.size();
    wave = 0;
    // deck -> draw pile (shuffled)
    hand.draw = s.deck;
    for (int i = (int)hand.draw.size() - 1; i > 0; --i) std::swap(hand.draw[i], hand.draw[rng.irange(0, i)]);
    for (int i = 0; i < HAND_SIZE; ++i) { hand.slots[i].def = -1; drawCard(i); }
    if (mods.startRankUp)
        for (auto& c : hand.slots)
            if (c.def >= 0 && !c.fused && !c.anchored && c.rank < 3) { c.rank++; c.tempBoost = true; break; }
    clearDelay_ = 60;  // first wave after 1s
}

void World::drawCard(int slot) {
    if (hand.draw.empty()) {
        if (hand.discard.empty()) return;
        hand.draw.swap(hand.discard);
        for (int i = (int)hand.draw.size() - 1; i > 0; --i) std::swap(hand.draw[i], hand.draw[rng.irange(0, i)]);
    }
    hand.slots[slot] = hand.draw.back();
    hand.draw.pop_back();
}

// ---------------------------------------------------------------- update
void World::update(const InputState& inRaw) {
    events.clear();
    InputState in = inRaw;
    if (hasPending_) {  // edges pressed during hitstop
        in.dash |= pending_.dash; in.fuse |= pending_.fuse; in.evolve |= pending_.evolve;
        in.useSelected |= pending_.useSelected;
        // the earliest buffered press wins (keeps the player's order)
        if (pending_.useSlot >= 0) in.useSlot = pending_.useSlot;
        if (pending_.fuseSlot >= 0) in.fuseSlot = pending_.fuseSlot;
        if (pending_.cursorSet >= 0) in.cursorSet = pending_.cursorSet;
        in.cursorDelta += pending_.cursorDelta;
        hasPending_ = false;
        pending_ = InputState();
    }
    if (result_ != 0) return;
    ++frame;
    if (hitstop > 0) {
        --hitstop;
        // timers that protect the player keep running during hitstop
        player.invuln = std::max(0.0f, player.invuln - DT);
        pending_.dash |= in.dash; pending_.fuse |= in.fuse; pending_.evolve |= in.evolve;
        pending_.useSelected |= in.useSelected;
        if (in.useSlot >= 0 && pending_.useSlot < 0) pending_.useSlot = in.useSlot;
        if (in.fuseSlot >= 0 && pending_.fuseSlot < 0) pending_.fuseSlot = in.fuseSlot;
        if (in.cursorSet >= 0) pending_.cursorSet = in.cursorSet;
        pending_.cursorDelta += in.cursorDelta;
        hasPending_ = true;
        return;
    }
    stats.frames = frame;
    shake = std::max(0.0f, shake - DT * 10);
    evolveFlash = std::max(0.0f, evolveFlash - DT);

    stepPlayer(in);
    stepHand(in);
    stepEnemies();
    stepBullets();
    stepHazards();
    stepZones();
    stepPlaced();
    cleanup();
    stepWaves();

    if (player.hp <= 0 && !player.dead) {
        player.dead = true;
        result_ = -1;
        emit(Ev::PlayerDead, player.pos);
    }
    if (result_ == 0 && frame >= timeoutFrames_) result_ = 2;
}

RoomResult World::makeResult() const {
    RoomResult r;
    r.result = result_;
    r.hp = std::max(0.0f, player.hp);
    r.nextUid = nextUid_;
    r.stats = stats;
    r.goldDrop = goldDrop_;
    r.enemiesSeen = enemiesSeen_;
    auto add = [&](const CardInstance& c) {
        if (c.def < 0) return;
        auto put = [&](CardInstance k) {
            if (k.tempBoost) { k.rank = std::max(1, k.rank - 1); k.tempBoost = false; }
            r.deck.push_back(k);
        };
        if (c.isComposite()) for (auto& m : c.materials) put(m);
        else put(c);
    };
    for (auto& c : hand.slots) add(c);
    for (auto& c : hand.draw) add(c);
    for (auto& c : hand.discard) add(c);
    for (auto& c : hand.exhausted) add(c);
    return r;
}

// ---------------------------------------------------------------- collision helpers
bool World::isBlocked(Vec2 p, float r) const {
    if (p.x - r < ARENA_L || p.x + r > ARENA_R || p.y - r < ARENA_T || p.y + r > ARENA_B) return true;
    for (auto& rc : pillars) if (circleRect(p, r, rc)) return true;
    return false;
}

void World::collideCircle(Vec2& p, float r) const {
    p.x = clampv(p.x, ARENA_L + r, ARENA_R - r);
    p.y = clampv(p.y, ARENA_T + r, ARENA_B - r);
    for (auto& rc : pillars) {
        float nx = clampv(p.x, rc.x, rc.x + rc.w), ny = clampv(p.y, rc.y, rc.y + rc.h);
        Vec2 d = p - Vec2{nx, ny};
        float l = d.len();
        if (l < r) {
            if (l < 1e-4f) { p.y = rc.y - r; continue; }
            p += d / l * (r - l);
        }
    }
}

const Enemy* World::nearestEnemy(Vec2 p, float maxDist) const {
    const Enemy* best = nullptr;
    float bd = maxDist * maxDist;
    for (auto& e : enemies) {
        if (!e.alive || e.spawnT > 0) continue;
        float d = dist2(p, e.pos);
        if (d < bd) { bd = d; best = &e; }
    }
    return best;
}

const Enemy* World::boss() const {
    for (auto& e : enemies) if (e.alive && e.d().boss) return &e;
    return nullptr;
}

int World::countAlive(int def) const {
    int n = 0;
    for (auto& e : enemies) if (e.alive && e.def == def) ++n;
    for (auto& e : newEnemies_) if (e.def == def) ++n;
    return n;
}

// ---------------------------------------------------------------- player
void World::stepPlayer(const InputState& in) {
    Player& p = player;
    p.invuln = std::max(0.0f, p.invuln - DT);
    p.dashCd = std::max(0.0f, p.dashCd - DT);
    p.attackCd = std::max(0.0f, p.attackCd - DT);
    p.evolveCd = std::max(0.0f, p.evolveCd - DT);
    p.fuseCd = std::max(0.0f, p.fuseCd - DT);
    p.castLock = std::max(0.0f, p.castLock - DT);
    p.hurtFlash = std::max(0.0f, p.hurtFlash - DT);
    if (p.shieldT > 0) { p.shieldT -= DT; if (p.shieldT <= 0) p.shield = 0; }

    Vec2 aim = in.aim - p.pos;
    if (aim.len2() > 1) p.aimDir = aim.norm();
    p.aimPoint = in.aim;

    Vec2 mv = in.move;
    if (mv.len2() > 1) mv = mv.norm();

    if (in.dash && p.dashCd <= 0 && p.dashT <= 0) {
        p.dashDir = mv.len2() > 0.01f ? mv.norm() : p.aimDir;
        p.dashT = DASH_TIME;
        p.invuln = std::max(p.invuln, DASH_INVULN);
        p.dashCd = mods.dashCd + DASH_TIME;
        emit(Ev::Dash, p.pos);
    }
    Vec2 vel = p.dashT > 0 ? p.dashDir * DASH_SPEED : mv * PLAYER_SPEED;
    if (p.dashT > 0) p.dashT -= DT;
    p.pos += vel * DT;
    collideCircle(p.pos, p.r);

    mana = std::min(mods.manaMax, mana + mods.manaRegen * DT);

    if (in.attack && p.attackCd <= 0 && p.dashT <= 0) {
        p.attackCd = BASIC_INTERVAL;
        int n = std::max(1, mods.basicShots);
        for (int i = 0; i < n; ++i) {
            float a = n > 1 ? (i - (n - 1) * 0.5f) * 0.18f : 0.0f;
            Bullet b;
            b.pos = p.pos + p.aimDir * 6;
            b.vel = p.aimDir.rotated(a) * BASIC_SPEED;
            b.r = 3;
            b.life = BASIC_RANGE / BASIC_SPEED;
            b.hit.dmg = mods.basicDmg;
            b.hit.knock = 25;
            b.hit.basic = true;
            b.hit.canReact = false;
            newBullets_.push_back(b);
        }
        emit(Ev::BasicShot, p.pos);
    }
}

// ---------------------------------------------------------------- hand / cards
void World::stepHand(const InputState& in) {
    for (int i = 0; i < HAND_SIZE; ++i) {
        if (!hand.empty(i)) continue;
        hand.refillT[i] -= DT;
        if (hand.refillT[i] <= 0) drawCard(i);
    }
    if (in.cursorSet >= 0 && in.cursorSet < HAND_SIZE) hand.cursor = in.cursorSet;
    if (in.cursorDelta) hand.cursor = ((hand.cursor + in.cursorDelta) % HAND_SIZE + HAND_SIZE) % HAND_SIZE;
    if (player.dead) return;
    if (player.castLock <= 0) {
        if (in.useSlot >= 0 && in.useSlot < HAND_SIZE) useCard(in.useSlot);
        else if (in.useSelected) useCard(hand.cursor);
    }
    if (in.fuseSlot >= 0 && in.fuseSlot < HAND_SIZE - 1) doFuse(in.fuseSlot);
    else if (in.fuse) doFuse(pairLeftForCursor());
    if (in.evolve) doEvolve();
}

void World::advanceCursorFrom(int slot) {
    for (int k = 1; k <= HAND_SIZE; ++k) {
        int i = (slot + k) % HAND_SIZE;
        if (!hand.empty(i)) { hand.cursor = i; return; }
    }
}

FusePreview World::previewPair(int l) const {
    if (l < 0 || l + 1 >= HAND_SIZE || hand.empty(l) || hand.empty(l + 1)) return {};
    FusePreview p = previewFuse(hand.slots[l], hand.slots[l + 1], 0);
    if (p.kind == FuseKind::Hybrid) {
        p.result.fusedPower *= mods.fusedPowerMul;
        p.result.fusedCost = std::max(1, p.result.fusedCost + mods.fusedCostDelta);
    }
    return p;
}

void World::useCard(int slot) {
    if (hand.empty(slot)) return;
    CardInstance& c = hand.slots[slot];
    if (mana + 1e-4f < c.cost()) { emit(Ev::NoMana, player.pos, 0, slot); return; }
    mana -= c.cost();
    CardInstance used = c;
    hand.slots[slot].def = -1;
    hand.slots[slot].materials.clear();
    hand.refillT[slot] = REFILL_DELAY * mods.refillMul;
    // Move the card(s) to their destination BEFORE casting so that instant kills can credit mastery.
    if (used.isComposite()) {
        // in-combat fusions are one-shot: the materials go back to the discard pile (no mastery gain)
        for (auto& m : used.materials) hand.discard.push_back(m);
    } else {
        CardInstance keep = used;
        if (!keep.anchored) keep.mastery = std::min(mods.masteryCap, keep.mastery + 1);
        if (keep.d().special == "heal") hand.exhausted.push_back(keep);  // heals: once per room
        else hand.discard.push_back(keep);
    }
    castCard(used);
    emit(Ev::CardUsed, player.pos, used.power(), (int)used.mainElem(), slot);
    stats.cardsUsed++;
    if (slot == hand.cursor) advanceCursorFrom(slot);
}

void World::doFuse(int l) {
    if (l < 0 || l + 1 >= HAND_SIZE) return;
    if (hand.empty(l) || hand.empty(l + 1) || player.fuseCd > 0) return;
    if (mana + 1e-4f < FUSE_COST) { emit(Ev::NoMana, player.pos, 0, l); return; }
    CardInstance& L = hand.slots[l];
    CardInstance& R = hand.slots[l + 1];
    FusePreview p = previewFuse(L, R, nextUid_);
    if (p.kind == FuseKind::Invalid) return;
    ++nextUid_;
    if (p.kind == FuseKind::Hybrid) {
        p.result.fusedPower *= mods.fusedPowerMul;
        p.result.fusedCost = std::max(1, p.result.fusedCost + mods.fusedCostDelta);
    }
    float before = (L.power() + R.power()) / std::max(1, L.cost() + R.cost());
    float after = p.result.power() / std::max(1, p.result.cost() + FUSE_COST);
    stats.fuseGainSum += after - before;
    mana = std::min(mods.manaMax, mana - FUSE_COST);
    hand.slots[l] = p.result;
    hand.slots[l + 1].def = -1;
    hand.slots[l + 1].materials.clear();
    hand.refillT[l + 1] = REFILL_DELAY * mods.refillMul;
    hand.cursor = l;
    if (p.kind == FuseKind::RankUp) stats.rankUps++; else stats.fusions++;
    if (p.result.reaction != Reaction::None) stats.reactionsSeen |= 1u << (int)p.result.reaction;
    emit(Ev::Fused, player.pos, 0, (int)p.result.reaction, l);
    player.fuseCd = FUSE_COOLDOWN;
    player.castLock = FUSE_CAST_LOCK;
}

void World::doEvolve() {
    if (player.evolveCd > 0) return;
    // the card under the cursor, otherwise the first evolvable card in hand
    int s = hand.cursor;
    if (hand.empty(s) || !hand.slots[s].canEvolve(mods.masteryCap)) {
        s = -1;
        for (int i = 0; i < HAND_SIZE; ++i)
            if (!hand.empty(i) && hand.slots[i].canEvolve(mods.masteryCap)) { s = i; break; }
        if (s < 0) return;
    }
    CardInstance& c = hand.slots[s];
    if (!evolveCard(c, mods.masteryCap)) return;
    player.invuln = std::max(player.invuln, EVOLVE_INVULN);
    player.evolveCd = EVOLVE_CD;
    evolveFlash = 0.6f;
    stats.evolutions++;
    emit(Ev::Evolved, player.pos, 0, c.def, s);
}

void World::creditMastery(uint32_t uid, int amount) {
    if (!uid) return;
    auto bump = [&](CardInstance& c) {
        if (c.uid == uid) {
            if (!c.anchored) c.mastery = std::min(mods.masteryCap, c.mastery + amount);
            return true;
        }
        return false;
    };
    for (auto& c : hand.slots) if (c.def >= 0 && bump(c)) return;
    for (auto& c : hand.discard) if (bump(c)) return;
    for (auto& c : hand.draw) if (bump(c)) return;
}

void World::castCard(const CardInstance& c) {
    const CardDef& d = c.d();
    Player& p = player;
    const float rk = 1.0f + 0.15f * (c.rank - 1);
    Hit h;
    h.dmg = c.power();
    if (c.elems[0] == Elem::Earth || c.elems[1] == Elem::Earth) h.dmg *= mods.earthPowerMul;
    h.elems[0] = c.elems[0];
    h.elems[1] = c.elems[1];
    h.re = c.fused ? c.reaction : Reaction::None;
    h.status = d.status + (c.rank - 1);
    h.knock = d.knock;
    h.card = c.isComposite() ? 0 : c.uid;  // fused shots do not train their materials
    h.from = p.pos;
    Vec2 dir = p.aimDir;

    switch (d.form) {
    case Form::Bolt: {
        int n = d.count > 1 ? d.count + (c.rank - 1) : d.count;
        float spread = d.spread * PI / 180.0f;
        for (int i = 0; i < n; ++i) {
            float a = n > 1 ? -spread / 2 + spread * i / (n - 1) : 0;
            Bullet b;
            b.pos = p.pos + dir * 6;
            b.vel = dir.rotated(a) * d.speed;
            b.r = (c.mainElem() == Elem::Earth) ? 5.0f : 3.5f;
            b.life = (d.range > 0 ? d.range : 260) / std::max(1.0f, d.speed);
            b.hit = h;
            b.pierce = d.pierce > 0 ? d.pierce + (c.rank - 1) : 0;
            b.explodeR = d.radius * rk;
            b.homing = d.special == "homing";
            newBullets_.push_back(b);
        }
        break;
    }
    case Form::Area: {
        Vec2 at = p.pos;
        if (d.special == "target") {
            // aim point, clamped to the card's range and to the arena
            Vec2 to = p.aimPoint - p.pos;
            if (to.len() > d.range) to = to.norm() * d.range;
            at = p.pos + to;
            at.x = clampv(at.x, ARENA_L, ARENA_R);
            at.y = clampv(at.y, ARENA_T, ARENA_B);
        }
        float r = d.radius * rk;
        if (d.duration > 0) {
            Zone z;
            z.kind = ZoneKind::Damage;
            z.pos = at; z.r = r;
            z.life = z.maxLife = d.duration;
            z.tick = d.tick > 0 ? d.tick : 0.33f;
            z.tickT = 0;
            z.hit = h;
            addZone(z);
        } else {
            areaHit(at, r, h, true);
            emit(Ev::Explosion, at, r, (int)c.mainElem());
        }
        break;
    }
    case Form::Melee: {
        float r = d.radius * rk;
        float half = d.spread * PI / 360.0f;
        h.melee = true;
        Hit h2 = h;
        h2.canReact = false;
        for (auto& e : enemies) {
            if (!e.alive || e.spawnT > 0) continue;
            Vec2 to = e.pos - p.pos;
            if (to.len() > r + e.r) continue;
            float ang = std::fabs(std::remainder(to.angle() - dir.angle(), 2 * PI));
            if (ang > half + 0.2f) continue;
            applyHit(e, h2, e.pos);
        }
        if (d.special == "lifesteal") {
            p.hp = std::min(p.maxHp, p.hp + h.dmg * 0.25f);
            stats.healed += h.dmg * 0.25f;
        }
        if (h.re != Reaction::None) triggerReaction(h, p.pos + dir * (r * 0.6f), nullptr);
        emit(Ev::Explosion, p.pos + dir * (r * 0.5f), r, (int)c.mainElem(), 1);
        p.pos += dir * 10;
        collideCircle(p.pos, p.r);
        break;
    }
    case Form::Place: {
        int alive = 0;
        for (auto& pl : placed) if (pl.alive) ++alive;
        if (alive >= MAX_PLACED)
            for (auto& pl : placed) if (pl.alive) { pl.alive = false; break; }
        Placed pl;
        pl.hit = h;
        pl.life = pl.maxLife = d.duration;
        pl.tick = d.tick > 0 ? d.tick : 0.5f;
        pl.count = d.count + (c.rank - 1);
        if (d.special == "turret") {
            pl.kind = PlacedKind::Turret;
            pl.pos = p.pos + dir * 18;
            collideCircle(pl.pos, 6);
            pl.range = d.radius * rk;
            pl.speed = d.speed;
            pl.count = d.count;
        } else {
            pl.kind = PlacedKind::Wall;
            pl.blockR = d.radius * rk;
            pl.pos = p.pos + dir * d.range;
            Vec2 perp{-dir.y, dir.x};
            int n = d.count + (c.rank - 1);
            for (int i = 0; i < n; ++i) {
                float o = (i - (n - 1) * 0.5f) * pl.blockR * 1.8f;
                Vec2 bp = pl.pos + perp * o;
                if (!isBlocked(bp, 1)) pl.blocks.push_back(bp);
            }
            if (d.special == "wall_explode") pl.explodeR = 50 * rk;
        }
        if (h.re != Reaction::None) triggerReaction(h, pl.pos, nullptr);
        placed.push_back(pl);
        break;
    }
    case Form::Guard: {
        if (d.special == "heal") {
            float before = p.hp;
            p.hp = std::min(p.maxHp, p.hp + h.dmg);
            stats.healed += p.hp - before;
            emit(Ev::Heal, p.pos, h.dmg);
        } else {
            p.shield = std::max(p.shield, h.dmg);
            p.shieldT = d.duration;
            emit(Ev::Shield, p.pos, h.dmg);
        }
        if (c.fused) {
            Hit b = h;
            b.dmg = h.dmg * 0.6f;
            areaHit(p.pos, 52, b, true);
            emit(Ev::Explosion, p.pos, 52, (int)c.mainElem());
        }
        break;
    }
    }
}

// ---------------------------------------------------------------- damage
void World::areaHit(Vec2 at, float r, const Hit& h, bool reactOnce) {
    Hit hh = h;
    if (reactOnce) hh.canReact = false;
    for (auto& e : enemies) {
        if (!e.alive || e.spawnT > 0) continue;
        if (dist(e.pos, at) <= r + e.r) applyHit(e, hh, at);
    }
    if (reactOnce && h.canReact && h.re != Reaction::None) triggerReaction(h, at, nullptr);
}

void World::applyHit(Enemy& e, const Hit& h, Vec2 at) {
    if (!e.alive || e.spawnT > 0) return;
    const EnemyDef& sp = e.d();
    float mul = 1.0f;
    bool shatter = false;
    if (e.st.frozenT > 0 && (h.melee || hasElem(h, Elem::Earth))) {
        mul *= 2.5f; shatter = true; e.st.frozenT = 0;
    }
    if (e.st.shockT > 0) mul *= 1.15f;
    bool weakHit = sp.weak != Elem::None && hasElem(h, sp.weak);
    bool resistHit = sp.resist != Elem::None && hasElem(h, sp.resist);
    if (weakHit) mul *= mods.weakMul;
    mul = std::min(mul, STATUS_MUL_CAP);
    // resistances are applied outside the cap
    if (resistHit) mul *= RESIST_MUL;
    if (e.adaptResist != Elem::None && hasElem(h, e.adaptResist)) mul *= 0.4f;
    float dmg = h.dmg * mul;
    e.hp -= dmg;
    if (e.hitFlash <= -0.12f) e.hitFlash = 0.08f;   // rate-limited so DoT/multi-hits do not keep the sprite white
    if (sp.ai == AiKind::BossGolem && h.elems[0] != Elem::None) e.adaptDmg[(int)h.elems[0]] += dmg;
    if (h.card) e.lastCard = h.card;
    if (h.basic) { stats.basicDamage += dmg; mana = std::min(mods.manaMax, mana + MANA_ON_BASIC_HIT / std::max(1, mods.basicShots)); }
    else stats.cardDamage += dmg;
    emit(Ev::Damage, e.pos, dmg, (int)h.elems[0], shatter ? 1 : (weakHit && !resistHit ? 5 : (mul > 1.01f ? 2 : (mul < 0.99f ? 4 : 0))));
    if (shatter) { emit(Ev::Shatter, e.pos, dmg); hitstop = std::max(hitstop, 4); shake = std::max(shake, 3.0f); }
    if (dmg >= 25 && h.primary) { hitstop = std::max(hitstop, 3); shake = std::max(shake, 2.0f); }

    // status
    int purify = (h.re == Reaction::Purify) ? 2 : 1;
    int bonus = (h.re == Reaction::Resonance) ? 1 : 0;
    for (int k = 0; k < 2; ++k) {
        Elem el = h.elems[k];
        if (el == Elem::None) continue;
        int amt = h.status * purify + bonus;
        if (amt <= 0) continue;
        switch (el) {
        case Elem::Fire:
            e.st.burnStacks = std::min(mods.burnCap, e.st.burnStacks + amt);
            e.st.burnT = 3.0f;
            break;
        case Elem::Ice:
            e.st.chill += (float)(amt + mods.chillBonus);
            if (e.st.chill >= 3.0f * sp.freezeRes && e.st.frozenT <= 0) {
                e.st.frozenT = 1.5f * mods.freezeTimeMul / (sp.boss ? 2.0f : 1.0f);
                e.st.chill = 0;
                emit(Ev::Freeze, e.pos);
            }
            break;
        case Elem::Thunder: {
            e.st.shockT = 2.5f;
            Enemy* best = nullptr;
            float bd = 60 * 60;
            for (auto& o : enemies) {
                if (&o == &e || !o.alive || o.spawnT > 0) continue;
                float d2 = dist2(o.pos, e.pos);
                if (d2 < bd) { bd = d2; best = &o; }
            }
            if (best && h.primary) {
                float cd = 3.0f * amt * mods.chainMul;
                best->hp -= cd;
                if (best->hitFlash <= -0.12f) best->hitFlash = 0.08f;
                best->st.shockT = std::max(best->st.shockT, 1.0f);
                stats.cardDamage += cd;
                emit(Ev::Reaction, best->pos, cd, (int)Reaction::None, 1);
                emit(Ev::Damage, best->pos, cd, (int)Elem::Thunder, 0);
                if (best->hp <= 0 && best->alive) onKill(*best, false);
            }
            break;
        }
        case Elem::Earth:
            e.st.weightT = 2.0f;
            break;
        default: break;
        }
    }
    if (h.knock > 0 && e.st.frozenT <= 0) {
        Vec2 dir = (e.pos - h.from).norm();
        if (dir.len2() < 0.01f) dir = (e.pos - at).norm();
        float k = h.knock * (e.st.weightT > 0 ? 1.5f : 1.0f) / sp.kbRes;
        e.knock += dir * k;
    }
    if (h.canReact && h.re != Reaction::None) triggerReaction(h, at, &e);
    if (e.hp <= 0 && e.alive) onKill(e, h.primary);
}

void World::triggerReaction(const Hit& h, Vec2 at, Enemy* target) {
    Hit sub;
    sub.card = h.card;
    sub.canReact = false;
    sub.primary = false;
    sub.from = at;
    switch (h.re) {
    case Reaction::Steam: {
        Zone z; z.kind = ZoneKind::Steam; z.pos = at; z.r = 30; z.life = z.maxLife = 3; z.tick = 0.5f;
        z.hit = sub; z.hit.dmg = 2;
        addZone(z);
        break;
    }
    case Reaction::Blast: {
        sub.dmg = h.dmg * 0.5f;
        sub.knock = 60;
        sub.elems[0] = Elem::Fire;
        sub.status = 1;
        areaHit(at, 32, sub, false);
        emit(Ev::Explosion, at, 32, (int)Elem::Fire, 2);
        shake = std::max(shake, 2.5f);
        break;
    }
    case Reaction::Lava: {
        Zone z; z.kind = ZoneKind::Lava; z.pos = at; z.r = 24; z.life = z.maxLife = 2.5f; z.tick = 0.4f;
        z.hit = sub; z.hit.dmg = 3; z.hit.elems[0] = Elem::Fire; z.hit.status = 0;
        addZone(z);
        break;
    }
    case Reaction::Supercond: {
        // sources: the struck enemy, or (area/melee/placed) every chilled enemy near the impact
        auto chilled = [](const Enemy& o) { return o.st.chill > 0 || o.st.frozenT > 0; };
        std::vector<int> srcIds;
        if (target) { if (chilled(*target)) srcIds.push_back(target->id); }
        else for (auto& o : enemies)
            if (o.alive && o.spawnT <= 0 && chilled(o) && dist(o.pos, at) <= 60) srcIds.push_back(o.id);
        if (srcIds.empty()) break;
        int n = 0;
        for (auto& o : enemies) {
            if (!o.alive || o.spawnT > 0) continue;
            if (std::find(srcIds.begin(), srcIds.end(), o.id) != srcIds.end()) continue;
            if (dist(o.pos, at) > 90) continue;
            sub.dmg = h.dmg * 0.6f;
            sub.elems[0] = Elem::Ice;
            sub.status = 1;
            applyHit(o, sub, o.pos);
            emit(Ev::Reaction, o.pos, sub.dmg, (int)Reaction::Supercond, 1);
            if (++n >= 3) break;
        }
        break;
    }
    case Reaction::Permafrost: {
        Zone z; z.kind = ZoneKind::Permafrost; z.pos = at; z.r = 32; z.life = z.maxLife = 4; z.tick = 1.0f;
        z.hit = sub; z.hit.dmg = 1; z.hit.elems[0] = Elem::Ice; z.hit.status = 1;
        addZone(z);
        break;
    }
    case Reaction::Magnet: {
        for (auto& o : enemies) {
            if (!o.alive || o.spawnT > 0) continue;
            Vec2 d = at - o.pos;
            float l = d.len();
            if (l > 80 || l < 4) continue;
            o.knock += d / l * (160.0f / o.d().kbRes);
        }
        break;
    }
    default: return;
    }
    stats.reactionsSeen |= 1u << (int)h.re;
    emit(Ev::Reaction, at, 0, (int)h.re, 0);
}

void World::onKill(Enemy& e, bool direct) {
    e.alive = false;
    stats.kills++;
    creditMastery(e.lastCard, 1);
    goldDrop_ += e.d().gold * (e.elite ? 3 : 1);
    if (mods.killHealCap > 0 && killHeals_ < mods.killHealCap) {
        ++killHeals_;
        player.hp = std::min(player.maxHp, player.hp + 1);
        stats.healed += 1;
    }
    if (direct) {
        hitstop = std::max(hitstop, 3);
        shake = std::max(shake, 1.5f);
    }
    emit(Ev::Kill, e.pos, e.r, e.def);
    // splitters burst into smaller slimes
    if (e.ai() == AiKind::Splitter) {
        int mini = Content::findEnemy("minislime");
        if (mini >= 0)
            for (int k = 0; k < (e.elite ? 3 : 2); ++k) {
                Vec2 p = e.pos + Vec2::fromAngle(rng.range(0, 2 * PI)) * 8;
                collideCircle(p, 5);
                spawnEnemy(mini, false, p);
                newEnemies_.back().spawnT = 0.2f;
            }
    }
}

void World::hurtPlayer(float dmg, Vec2 from) {
    Player& p = player;
    if (p.invuln > 0 || p.dead) return;
    dmg *= mods.enemyDmgMul;
    if (p.shield > 0) {
        float a = std::min(p.shield, dmg);
        p.shield -= a;
        dmg -= a;
        if (p.shield <= 0) { p.shieldT = 0; }
    }
    p.invuln = HURT_INVULN;
    if (dmg <= 0) { emit(Ev::Shield, p.pos, 0, 1); return; }
    p.hp -= dmg;
    p.hurtFlash = 0.3f;
    stats.damageTaken += dmg;
    shake = std::max(shake, 4.0f);
    hitstop = std::max(hitstop, 4);
    Vec2 d = (p.pos - from).norm();
    p.pos += d * 8;
    collideCircle(p.pos, p.r);
    emit(Ev::PlayerHurt, p.pos, dmg);
}

void World::addZone(const Zone& z) { newZones_.push_back(z); }

// ---------------------------------------------------------------- enemies
void World::spawnEnemy(int def, bool elite, Vec2 pos) {
    const EnemyDef& sp = Content::enemies()[def];
    Enemy e;
    e.def = def;
    e.elite = elite;
    e.pos = pos;
    e.hp = e.maxHp = sp.hp * mods.enemyHpMul * (elite ? ELITE_HP : 1.0f);
    e.r = sp.radius * (elite ? ELITE_SIZE : 1.0f);
    e.speed = sp.speed;
    e.dmgMul = elite ? ELITE_DMG : 1.0f;
    e.cdMul = elite ? ELITE_CD : 1.0f;
    e.cd = rng.range(0.8f, 1.8f);
    e.id = nextEnemyId_++;
    if (sp.boss) e.cd = 2.0f;
    enemiesSeen_ |= 1ull << (def & 63);
    newEnemies_.push_back(e);
    emit(Ev::Spawn, pos, 0, def);
}

void World::debugSpawnDummy(int def, Vec2 pos, float hp) {
    spawnEnemy(def, false, pos);
    Enemy& e = newEnemies_.back();
    e.spawnT = 0;
    e.speed = 0;
    e.cd = 9999;
    e.hp = e.maxHp = hp;
    enemies.push_back(e);
    newEnemies_.pop_back();
}

void World::fireEnemyBullet(Vec2 pos, Vec2 vel, float dmg, float r, float life) {
    EnemyBullet b;
    b.pos = pos;
    b.vel = vel;
    b.dmg = dmg;
    b.r = r;
    b.life = life;
    b.id = nextBulletId_++;
    ebullets.push_back(b);
}

Vec2 World::randomSpawnPos() {
    for (int tries = 0; tries < 50; ++tries) {
        Vec2 p{rng.range(ARENA_L + 20, ARENA_R - 20), rng.range(ARENA_T + 20, ARENA_B - 20)};
        if (isBlocked(p, 12)) continue;
        if (dist(p, player.pos) < 130) continue;
        bool close = false;
        for (auto& e : enemies) if (e.alive && dist(e.pos, p) < 24) close = true;
        for (auto& e : newEnemies_) if (dist(e.pos, p) < 24) close = true;
        if (close) continue;
        return p;
    }
    return {ARENA_L + 30, ARENA_T + 30};
}

void World::stepWaves() {
    if (waveCount == 0) return;
    for (auto& e : enemies) if (e.alive) return;
    if (!newEnemies_.empty()) return;
    if (clearDelay_ > 0) { --clearDelay_; return; }
    if (wave >= waveCount) {
        result_ = 1;
        emit(Ev::RoomClear, player.pos);
        return;
    }
    for (const SpawnSpec& s : waves_[wave]) {
        Vec2 p = Content::enemies()[s.enemy].boss ? Vec2{320, 90} : randomSpawnPos();
        spawnEnemy(s.enemy, s.elite, p);
    }
    emit(Ev::WaveStart, player.pos, 0, wave);
    ++wave;
    clearDelay_ = 50;
}

void World::stepEnemies() {
    Player& p = player;
    for (size_t i = 0; i < enemies.size(); ++i) {
        Enemy& e = enemies[i];
        if (!e.alive) continue;
        if (e.spawnT > 0) { e.spawnT -= DT; continue; }
        e.hitFlash = std::max(-0.3f, e.hitFlash - DT);
        e.contactCd = std::max(0.0f, e.contactCd - DT);
        Status& s = e.st;
        if (s.burnT > 0) {
            s.burnT -= DT;
            s.burnTick -= DT;
            if (s.burnTick <= 0) {
                s.burnTick = 0.5f;
                float d = 1.5f * s.burnStacks * mods.burnDmgMul;
                e.hp -= d;
                stats.cardDamage += d;
                emit(Ev::Damage, e.pos, d, (int)Elem::Fire, 3);
                if (e.hp <= 0) { onKill(e, false); continue; }
            }
            if (s.burnT <= 0) s.burnStacks = 0;
        }
        s.chill = std::max(0.0f, s.chill - DT * 0.35f);
        s.shockT = std::max(0.0f, s.shockT - DT);
        s.weightT = std::max(0.0f, s.weightT - DT);
        s.steamT = std::max(0.0f, s.steamT - DT);
        s.slowT = std::max(0.0f, s.slowT - DT);
        e.pos += e.knock * DT;
        e.knock *= 0.85f;
        if (s.frozenT > 0) { s.frozenT -= DT; collideCircle(e.pos, e.r); continue; }

        float spd = e.speed * (s.weightT > 0 ? 0.7f : 1.0f) * (s.slowT > 0 ? 0.4f : 1.0f);
        float atkMul = (s.steamT > 0 ? 1.4f : 1.0f) * e.cdMul;
        Vec2 toP = p.pos - e.pos;
        float dP = toP.len();
        Vec2 dirP = toP.norm();
        Vec2 move{0, 0};
        e.t += DT;
        e.cd -= DT / atkMul;
        stepEnemyAI(e, spd, atkMul, toP, dP, dirP, move);
        if (!e.alive) continue;

        e.pos += move * DT;
        for (auto& pl : placed) {
            if (!pl.alive || pl.kind != PlacedKind::Wall) continue;
            for (auto& bp : pl.blocks) {
                Vec2 d = e.pos - bp;
                float l = d.len(), rr = e.r + pl.blockR;
                if (l < rr && l > 1e-4f) e.pos += d / l * (rr - l);
            }
        }
        collideCircle(e.pos, e.r);
        // contact damage
        float contact = e.d().dmg * e.dmgMul;
        if (e.ai() == AiKind::Charger || e.ai() == AiKind::BossGolem) contact *= (e.state == 2 ? 1.0f : 0.35f);
        else if (e.ai() != AiKind::Chase && e.ai() != AiKind::Splitter && e.ai() != AiKind::BossSlime) contact = std::min(contact, 5.0f);
        if (dP < e.r + p.r && e.contactCd <= 0 && p.invuln <= 0) {
            hurtPlayer(contact, e.pos);
            e.contactCd = 0.8f;
        }
    }
    // separation
    for (size_t i = 0; i < enemies.size(); ++i)
        for (size_t j = i + 1; j < enemies.size(); ++j) {
            Enemy &a = enemies[i], &b = enemies[j];
            if (!a.alive || !b.alive || a.spawnT > 0 || b.spawnT > 0) continue;
            Vec2 d = b.pos - a.pos;
            float l = d.len(), rr = a.r + b.r;
            if (l < rr && l > 1e-4f) {
                Vec2 push = d / l * ((rr - l) * 0.5f);
                float wa = a.d().boss ? 0.1f : 1.0f, wb = b.d().boss ? 0.1f : 1.0f;
                a.pos -= push * wa; b.pos += push * wb;
            }
        }
}

// ---------------------------------------------------------------- projectiles
void World::stepBullets() {
    for (size_t i = 0; i < bullets.size(); ++i) {
        Bullet& b = bullets[i];
        if (!b.alive) continue;
        if (b.homing) {
            const Enemy* t = nearestEnemy(b.pos, 160);
            if (t) {
                Vec2 want = (t->pos - b.pos).norm() * b.vel.len();
                b.vel = (b.vel * 0.9f + want * 0.1f).norm() * b.vel.len();
            }
        }
        b.pos += b.vel * DT;
        b.life -= DT;
        bool dead = b.life <= 0;
        if (!dead && isBlocked(b.pos, 1)) dead = true;
        if (!dead) {
            for (auto& e : enemies) {
                if (!e.alive || e.spawnT > 0) continue;
                if (dist2(e.pos, b.pos) > (e.r + b.r) * (e.r + b.r)) continue;
                bool already = false;
                for (int id : b.hitIds) if (id == e.id) already = true;
                if (already) continue;
                // shield bearers block projectiles from the front
                if (e.ai() == AiKind::Shield && e.st.frozenT <= 0 && e.guardBreakT <= 0) {
                    Vec2 face = e.dir.len2() > 0.01f ? e.dir : Vec2{0, 1};
                    if (face.dot(b.vel.norm()) < -0.5f) {
                        e.guard -= b.hit.dmg;
                        emit(Ev::Shield, b.pos, 0, 2);
                        if (e.guard <= 0) {   // shield shattered: defenceless for a while
                            e.guardBreakT = 4.0f;
                            e.guard = 40 * (e.elite ? 1.5f : 1.0f);
                            emit(Ev::Shatter, e.pos, 0);
                            shake = std::max(shake, 2.0f);
                        }
                        dead = true;
                        break;
                    }
                }
                Hit h = b.hit;
                h.from = b.pos - b.vel.norm() * 10;
                if (b.explodeR > 0) {
                    // explode on every hit; piercing explosive shells keep rolling
                    for (auto& o : enemies)
                        if (o.alive && dist(o.pos, b.pos) <= b.explodeR + o.r) b.hitIds.push_back(o.id);
                    areaHit(b.pos, b.explodeR, h, true);
                    emit(Ev::Explosion, b.pos, b.explodeR, (int)h.elems[0]);
                } else {
                    applyHit(e, h, b.pos);
                }
                b.hitIds.push_back(e.id);
                if (b.pierce > 0) --b.pierce;
                else dead = true;
                if (dead) break;
            }
        } else if (b.explodeR > 0) {
            Hit h = b.hit;
            h.from = b.pos;
            areaHit(b.pos, b.explodeR, h, true);
            emit(Ev::Explosion, b.pos, b.explodeR, (int)h.elems[0]);
        }
        if (dead) b.alive = false;
    }
    Player& p = player;
    for (auto& b : ebullets) {
        if (!b.alive) continue;
        b.pos += b.vel * DT;
        b.life -= DT;
        b.age += DT;
        if (b.life <= 0 || isBlocked(b.pos, 1)) { b.alive = false; continue; }
        bool blocked = false;
        for (auto& pl : placed) {
            if (!pl.alive || pl.kind != PlacedKind::Wall) continue;
            for (auto& bp : pl.blocks) if (dist2(bp, b.pos) < (pl.blockR + b.r) * (pl.blockR + b.r)) blocked = true;
        }
        if (blocked) { b.alive = false; continue; }
        if (dist2(b.pos, p.pos) < (b.r + p.r - 1) * (b.r + p.r - 1) && p.invuln <= 0) {
            hurtPlayer(b.dmg, b.pos);
            b.alive = false;
        }
    }
}

void World::stepHazards() {
    Player& p = player;
    for (auto& h : hazards) {
        if (!h.alive) continue;
        h.t += DT;
        if (!h.fired && h.t >= h.delay) {
            h.fired = true;
            bool hit = false;
            if (h.kind == HazardKind::Circle) hit = dist(p.pos, h.pos) <= h.r + p.r;
            else {
                Vec2 rel = p.pos - h.pos;
                float along = rel.dot(h.dir);
                Vec2 perp{-h.dir.y, h.dir.x};
                hit = along >= 0 && along <= h.len && std::fabs(rel.dot(perp)) <= h.r + p.r;
            }
            if (hit) hurtPlayer(h.dmg, h.pos);
            emit(Ev::Explosion, h.kind == HazardKind::Circle ? h.pos : h.pos + h.dir * (h.len * 0.5f),
                 h.kind == HazardKind::Circle ? h.r : 20, 0, 3);
        }
        if (h.fired && h.t >= h.delay + h.linger) h.alive = false;
    }
}

void World::stepZones() {
    for (auto& z : zones) {
        if (!z.alive) continue;
        z.life -= DT;
        if (z.life <= 0) { z.alive = false; continue; }
        z.tickT -= DT;
        bool tick = z.tickT <= 0;
        if (tick) z.tickT += z.tick;
        for (auto& e : enemies) {
            if (!e.alive || e.spawnT > 0) continue;
            if (dist(e.pos, z.pos) > z.r + e.r) continue;
            if (z.kind == ZoneKind::Steam) e.st.steamT = std::max(e.st.steamT, 0.5f);
            if (z.kind == ZoneKind::Permafrost) e.st.slowT = std::max(e.st.slowT, 0.3f);
            if (tick) {
                Hit h = z.hit;
                h.canReact = false;              // the zone reacts once, below
                h.primary = (z.ticks == 0);
                applyHit(e, h, e.pos);
            }
        }
        if (tick) {
            if (z.kind == ZoneKind::Damage && z.ticks == 0 && z.hit.re != Reaction::None) triggerReaction(z.hit, z.pos, nullptr);
            ++z.ticks;
        }
    }
}

void World::stepPlaced() {
    for (auto& pl : placed) {
        if (!pl.alive) continue;
        pl.life -= DT;
        if (pl.life <= 0) {
            pl.alive = false;
            if (pl.explodeR > 0) {
                Hit h = pl.hit;
                h.dmg *= 2.0f;
                h.from = pl.pos;
                areaHit(pl.pos, pl.explodeR, h, true);
                emit(Ev::Explosion, pl.pos, pl.explodeR, (int)h.elems[0]);
            }
            continue;
        }
        pl.tickT -= DT;
        if (pl.tickT > 0) continue;
        pl.tickT = pl.tick;
        if (pl.kind == PlacedKind::Wall) {
            Hit h = pl.hit;
            h.canReact = false;
            h.primary = false;
            h.knock = 30;
            for (auto& e : enemies) {
                if (!e.alive || e.spawnT > 0) continue;
                for (auto& bp : pl.blocks)
                    if (dist(e.pos, bp) <= e.r + pl.blockR + 3) { h.from = bp; applyHit(e, h, e.pos); break; }
            }
        } else {
            std::vector<const Enemy*> targets;
            for (auto& e : enemies)
                if (e.alive && e.spawnT <= 0 && dist(e.pos, pl.pos) <= pl.range) targets.push_back(&e);
            std::sort(targets.begin(), targets.end(), [&](const Enemy* a, const Enemy* b) {
                return dist2(a->pos, pl.pos) < dist2(b->pos, pl.pos);
            });
            for (int k = 0; k < pl.count && k < (int)targets.size(); ++k) {
                Bullet b;
                b.pos = pl.pos;
                b.vel = (targets[k]->pos - pl.pos).norm() * pl.speed;
                b.r = 3;
                b.life = pl.range / pl.speed + 0.1f;
                b.hit = pl.hit;
                b.hit.canReact = (pl.volleys % 4 == 0);   // turrets react on every 4th volley
                newBullets_.push_back(b);
            }
            if (!targets.empty()) ++pl.volleys;
        }
    }
}

void World::cleanup() {
    auto rm = [](auto& v) { v.erase(std::remove_if(v.begin(), v.end(), [](auto& x) { return !x.alive; }), v.end()); };
    rm(bullets); rm(ebullets); rm(zones); rm(placed); rm(enemies); rm(hazards);
    for (auto& b : newBullets_) bullets.push_back(b);
    newBullets_.clear();
    for (auto& z : newZones_) zones.push_back(z);
    newZones_.clear();
    for (auto& e : newEnemies_) enemies.push_back(e);
    newEnemies_.clear();
    while ((int)zones.size() > MAX_ZONES) zones.erase(zones.begin());
}

uint32_t World::stateHash() const {
    uint32_t h = 2166136261u;
    auto mix = [&](const void* p, size_t n) {
        const uint8_t* b = (const uint8_t*)p;
        for (size_t i = 0; i < n; ++i) { h ^= b[i]; h *= 16777619u; }
    };
    mix(&player.pos, sizeof(Vec2));
    mix(&player.hp, sizeof(float));
    mix(&mana, sizeof(float));
    for (auto& e : enemies) { mix(&e.pos, sizeof(Vec2)); mix(&e.hp, sizeof(float)); }
    mix(&rng.state, sizeof(uint32_t));
    for (auto& c : hand.slots) { mix(&c.uid, sizeof(uint32_t)); mix(&c.mastery, sizeof(int)); }
    uint32_t counts[4] = {(uint32_t)bullets.size(), (uint32_t)ebullets.size(), (uint32_t)zones.size(), (uint32_t)hand.discard.size()};
    mix(counts, sizeof counts);
    return h;
}
