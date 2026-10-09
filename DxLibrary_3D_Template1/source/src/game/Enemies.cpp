// Enemy and boss behaviour. Every AI writes a desired velocity into `move`
// and may fire bullets / place hazards / summon (summons go through the pending queue).
#include "game/World.h"
#include <algorithm>
#include <cmath>

namespace {
Vec2 perpOf(Vec2 d) { return {-d.y, d.x}; }
}

void World::stepEnemyAI(Enemy& e, float spd, float atkMul, Vec2 toP, float dP, Vec2 dirP, Vec2& move) {
    const EnemyDef& D = e.d();
    const float dmg = D.dmg * e.dmgMul;
    const float slowShot = e.st.steamT > 0 ? 0.7f : 1.0f;
    const float bs = D.bspeed * slowShot;
    Player& p = player;
    (void)toP;

    switch (D.ai) {
    case AiKind::Chase:
    case AiKind::Splitter: {
        float hop = 0.6f + 0.4f * std::sin(e.t * 6.0f + e.id);
        move = dirP * spd * hop;
        break;
    }
    case AiKind::Archer: {
        if (dP < 110) move = -dirP * spd;
        else if (dP > 170) move = dirP * spd;
        else move = perpOf(dirP) * (spd * 0.6f * ((e.id & 1) ? 1.0f : -1.0f));
        if (e.cd <= 0 && dP < 260) {
            e.cd = D.cd;
            int n = e.elite ? 5 : 3;
            for (int k = 0; k < n; ++k)
                fireEnemyBullet(e.pos, dirP.rotated(0.22f * (k - (n - 1) * 0.5f)) * bs, dmg);
        }
        break;
    }
    case AiKind::Charger: {
        // state 0 walk, 1 windup, 2 charge, 3 recover
        if (e.state == 0) {
            move = dirP * spd;
            if (dP < 150 && e.cd <= 0) { e.state = 1; e.t = 0; e.dir = dirP; }
        } else if (e.state == 1) {
            e.dir = (e.dir * 0.9f + dirP * 0.1f).norm();
            if (e.t >= 0.7f * atkMul) { e.state = 2; e.t = 0; }
        } else if (e.state == 2) {
            move = e.dir * bs;
            Vec2 np = e.pos + move * DT;
            if (e.t >= 0.5f || isBlocked(np, e.r)) { e.state = 3; e.t = 0; move = {0, 0}; shake = std::max(shake, 1.0f); }
        } else {
            if (e.t >= 0.6f) { e.state = 0; e.cd = D.cd; }
        }
        break;
    }
    case AiKind::Caster: {
        if (dP < 120) move = -dirP * spd;
        else if (dP > 200) move = dirP * spd;
        if (e.cd <= 0) {
            e.cd = D.cd;
            const int n = e.elite ? 16 : 12;
            for (int k = 0; k < n; ++k) fireEnemyBullet(e.pos, Vec2::fromAngle(e.t + 2 * PI * k / n) * bs, dmg, 3.5f, 4.5f);
        }
        break;
    }
    case AiKind::Bomber: {
        // rush in, arm (telegraph), explode
        if (e.state == 0) {
            move = dirP * spd;
            if (dP < 40) {
                e.state = 1; e.t = 0;
                Hazard h; h.pos = e.pos; h.r = 46; h.delay = 0.8f * atkMul; h.dmg = dmg;
                addHazard(h);
                e.aux = e.pos;
            }
        } else if (e.t >= 0.8f * atkMul) {
            e.alive = false;   // blew itself up (no kill credit, no gold)
            emit(Ev::Explosion, e.pos, 46, (int)Elem::Fire, 2);
        }
        break;
    }
    case AiKind::Shield: {
        // slowly turns to face the player, advances; a broken shield leaves it stunned
        if (e.guardBreakT > 0) { e.guardBreakT -= DT; move = {0, 0}; break; }
        Vec2 face = e.dir.len2() > 0.01f ? e.dir : dirP;
        float a = std::remainder(dirP.angle() - face.angle(), 2 * PI);
        float turn = clampv(a, -1.6f * DT, 1.6f * DT);
        e.dir = face.rotated(turn);
        move = dirP * spd;
        if (e.cd <= 0 && dP < 60) { e.cd = D.cd; fireEnemyBullet(e.pos, dirP * bs, dmg); }
        break;
    }
    case AiKind::Sniper: {
        if (dP < 200) move = -dirP * spd;
        else if (dP > 280) move = dirP * spd * 0.5f;
        if (e.state == 0 && e.cd <= 0) { e.state = 1; e.t = 0; e.dir = dirP; }
        if (e.state == 1) {
            move = {0, 0};
            if (e.t < 0.7f) e.dir = dirP;   // tracks, then locks for the last 0.3s
            if (e.t >= 1.0f * atkMul) {
                fireEnemyBullet(e.pos, e.dir * bs, dmg, 3.5f, 2.0f);
                e.state = 0;
                e.cd = D.cd;
            }
        }
        break;
    }
    case AiKind::Summoner: {
        if (dP < 150) move = -dirP * spd;
        else if (dP > 230) move = dirP * spd;
        int mini = Content::findEnemy("minislime");
        if (e.cd <= 0 && mini >= 0) {
            e.cd = D.cd;
            if (countAlive(mini) < (e.elite ? 8 : 5))
                for (int k = 0; k < 2; ++k) {
                    Vec2 sp = e.pos + Vec2::fromAngle(rng.range(0, 2 * PI)) * 16;
                    collideCircle(sp, 5);
                    spawnEnemy(mini, false, sp);
                    newEnemies_.back().spawnT = 0.5f;
                }
        }
        break;
    }
    case AiKind::Turret: {
        // stationary spiral; pauses every few seconds
        float cyc = std::fmod(e.t, 4.0f);
        if (cyc < 2.6f && e.cd <= 0) {
            e.cd = D.cd;
            float a = e.t * 2.3f;
            fireEnemyBullet(e.pos, Vec2::fromAngle(a) * bs, dmg);
            if (e.elite) fireEnemyBullet(e.pos, Vec2::fromAngle(a + PI) * bs, dmg);
        }
        break;
    }

    // ------------------------------------------------------------ bosses
    case AiKind::BossSlime: {
        if (e.phase == 0 && e.hp < e.maxHp * 0.5f) { e.phase = 1; emit(Ev::BossPhase, e.pos, 0, e.def); }
        float fast = e.phase ? 1.4f : 1.0f;
        if (e.state == 0) {   // hop toward the player
            float hop = 0.5f + 0.5f * std::sin(e.t * 5.0f);
            move = dirP * spd * fast * hop;
            if (e.cd <= 0) {
                e.state = 1; e.t = 0;
                e.aux = p.pos;   // jump target
                e.vel = e.pos;   // jump start (scratch)
                Hazard h; h.pos = e.aux; h.r = 46; h.delay = 1.0f; h.dmg = dmg * 1.3f;
                addHazard(h);
            }
        } else if (e.state == 1) {   // airborne: move to target over 1s
            float t = std::min(1.0f, e.t / 1.0f);
            e.pos = e.vel + (e.aux - e.vel) * t;
            move = {0, 0};
            if (e.t >= 1.0f) {
                e.pos = e.aux;
                e.state = 2; e.t = 0;
                shake = std::max(shake, 5.0f);
                int mini = Content::findEnemy("minislime");
                if (mini >= 0 && countAlive(mini) < 6)
                    for (int k = 0; k < (e.phase ? 3 : 2); ++k) {
                        Vec2 sp = e.pos + Vec2::fromAngle(rng.range(0, 2 * PI)) * 26;
                        collideCircle(sp, 5);
                        spawnEnemy(mini, false, sp);
                        newEnemies_.back().spawnT = 0.3f;
                    }
                if (e.phase) {
                    for (int k = 0; k < 10; ++k) fireEnemyBullet(e.pos, Vec2::fromAngle(2 * PI * k / 10 + e.t) * 110, dmg * 0.6f);
                }
            }
        } else {
            if (e.t >= 0.7f) { e.state = 0; e.cd = (e.phase ? 2.6f : 3.6f); }
        }
        break;
    }
    case AiKind::BossLich: {
        if (e.phase == 0 && e.hp < e.maxHp * 0.5f) { e.phase = 1; emit(Ev::BossPhase, e.pos, 0, e.def); }
        if (dP < 130) move = -dirP * spd;
        else if (dP > 210) move = dirP * spd;
        else move = perpOf(dirP) * spd * 0.7f;
        if (e.cd <= 0) {
            int pat = e.pattern++ % 3;
            if (pat == 0) {          // ring(s)
                int n = e.phase ? 14 : 14;
                for (int k = 0; k < n; ++k) fireEnemyBullet(e.pos, Vec2::fromAngle(e.t + 2 * PI * k / n) * bs, dmg, 3.5f, 4.5f);
                if (e.phase)
                    for (int k = 0; k < n; ++k) fireEnemyBullet(e.pos, Vec2::fromAngle(e.t + PI / n + 2 * PI * k / n) * bs * 0.7f, dmg, 3.5f, 5.5f);
                e.cd = 1.8f;
            } else if (pat == 1) {   // icicles fall around the player
                int n = e.phase ? 5 : 3;
                for (int k = 0; k < n; ++k) {
                    Hazard h;
                    h.pos = p.pos + (k == 0 ? Vec2{0, 0} : Vec2::fromAngle(rng.range(0, 2 * PI)) * rng.range(20, 70));
                    h.pos.x = clampv(h.pos.x, ARENA_L + 10, ARENA_R - 10);
                    h.pos.y = clampv(h.pos.y, ARENA_T + 10, ARENA_B - 10);
                    h.r = 20; h.delay = 1.1f + 0.12f * k; h.dmg = dmg;
                    addHazard(h);
                }
                e.cd = 2.0f;
            } else {                 // aimed burst
                for (int k = -2; k <= 2; ++k) fireEnemyBullet(e.pos, dirP.rotated(0.12f * k) * bs * 1.4f, dmg);
                e.cd = 1.6f;
            }
            e.cd *= atkMul / e.cdMul;
        }
        break;
    }
    case AiKind::BossGolem: {
        if (e.phase == 0 && e.hp < e.maxHp * 0.6f) {
            e.phase = 1;
            emit(Ev::BossPhase, e.pos, 0, e.def);
            int tur = Content::findEnemy("turret");   // calls two arcane turrets into the arena corners
            if (tur >= 0)
                for (Vec2 c : {Vec2{60, 50}, Vec2{580, 50}}) { spawnEnemy(tur, false, c); newEnemies_.back().spawnT = 1.2f; }
        }
        // adapt: resist the element that hurt it most in the last window
        e.adaptT += DT;
        if (e.adaptT >= 5.0f) {
            e.adaptT = 0;
            int best = 0;
            for (int k = 1; k < (int)Elem::Count; ++k) if (e.adaptDmg[k] > e.adaptDmg[best]) best = k;
            e.adaptResist = (best > 0 && e.adaptDmg[best] > 10) ? (Elem)best : e.adaptResist;
            for (auto& v : e.adaptDmg) v = 0;
        }
        if (e.state == 0) {
            move = dirP * spd * (e.phase ? 1.3f : 1.0f);
            if (e.cd <= 0) {
                int pat = e.pattern++ % 2;
                if (pat == 0) { e.state = 1; e.t = 0; e.dir = dirP; }          // charge
                else {                                                        // lasers
                    e.state = 4; e.t = 0;
                    int n = e.phase ? 8 : 6;
                    float base = rng.range(0, PI / 2);
                    for (int k = 0; k < n; ++k) {
                        Hazard h; h.kind = HazardKind::Line; h.pos = e.pos;
                        h.dir = Vec2::fromAngle(base + 2 * PI * k / n);
                        h.len = 700; h.r = 8; h.delay = 1.0f; h.dmg = dmg; h.linger = 0.3f;
                        addHazard(h);
                    }
                }
            }
        } else if (e.state == 1) {
            e.dir = (e.dir * 0.92f + dirP * 0.08f).norm();
            if (e.t >= 0.8f) { e.state = 2; e.t = 0; }
        } else if (e.state == 2) {
            move = e.dir * bs;
            Vec2 np = e.pos + move * DT;
            if (e.t >= 0.6f || isBlocked(np, e.r)) {
                e.state = 3; e.t = 0; move = {0, 0}; shake = std::max(shake, 4.0f);
                for (int k = 0; k < 12; ++k) fireEnemyBullet(e.pos, Vec2::fromAngle(2 * PI * k / 12 + e.t) * 110, dmg * 0.6f);
            }
        } else if (e.state == 3) {
            if (e.t >= 0.8f) { e.state = 0; e.cd = e.phase ? 1.6f : 2.4f; }
        } else if (e.state == 4) {   // standing still while lasers charge
            if (e.t >= 1.6f) { e.state = 0; e.cd = e.phase ? 1.4f : 2.2f; }
        }
        break;
    }
    }
}
