#include "enemy.hpp"
#include "projectile_manager.hpp"
#include "fx.hpp"
#include "audio.hpp"

namespace {
    const EnemyPreset PRESETS[(int)EnemyKind::COUNT] = {
        // sprite, hp, points, fire style,      fire ms, speed, drop %
        { 4,  1,  50,  FireStyle::None,   0,    1.10f, 4},   // Drone
        { 6,  2,  100, FireStyle::Down,   1900, 1.00f, 6},   // Bug
        {17,  3,  150, FireStyle::Aimed,  2100, 0.95f, 8},   // Wasp
        {39,  5,  250, FireStyle::Spread, 3000, 0.85f, 12},  // Crab
        {47,  10, 400, FireStyle::Burst,  4000, 0.70f, 20},  // Tank
    };

    constexpr float LOOP_RADIUS = 16.0f;
    constexpr float LOOP_SPEED = 3.0f;    // radians per second
    constexpr float HOVER_ENTER_S = 1.2f;
    constexpr float HOVER_STAY_S = 5.0f;
    constexpr float EIGHT_Y = 42;         // FigureEight: centre of the eight
    constexpr float V_ENTER_S = 1.3f;     // VDive: fly in as a V, wait, then dive
    constexpr float V_WAIT_S = 0.6f;
    constexpr float STRAFE_SLIDE_S = 0.8f;
    constexpr float STRAFE_STOP_S = 0.7f;
    constexpr int STRAFE_STOPS = 4;
    constexpr int BURST_SHOTS = 2;        // Tank: shots per volley

    float ease_out(float t) { return 1.0f - (1.0f - t) * (1.0f - t); }
}

const EnemyPreset &enemy_preset(EnemyKind kind) {
    return PRESETS[(int)kind];
}

void Enemy::spawn(EnemyKind k, MovementPattern p, Vec2 o, float d, float spd, float rate, int bonus_hp) {
    const EnemyPreset &preset = enemy_preset(k);
    kind = k;
    pattern = p;
    origin = o;
    dir = d;
    speed = spd * preset.speed;
    fire_rate = rate;
    health = preset.health + bonus_hp;
    age = 0;
    flash_ms = 0;
    burst_left = 0;
    holding = false;
    fire_timer = (int32_t)(preset.fire_ms * frand(0.3f, 0.9f));
    entered = false;
    active = true;
    position = Vec2(o.x, -8);
    if (pattern == MovementPattern::Sweep) position = Vec2(dir > 0 ? -8 : SCREEN_W + 8, o.y);
    move(0, Vec2(SCREEN_W / 2, SCREEN_H));
}

void Enemy::move(uint32_t dt, Vec2 player_pos) {
    float t = age / 1000.0f * speed;
    switch (pattern) {
        case MovementPattern::StraightDown:
            position = Vec2(origin.x, -6 + 38 * t);
            break;
        case MovementPattern::ZigZag:
            position = Vec2(origin.x + sinf(t * 3.2f) * 22 * dir, -6 + 30 * t);
            break;
        case MovementPattern::Swoop:
            position = Vec2(origin.x + dir * 26 * t * t, -6 + 52 * t);
            break;
        case MovementPattern::Loop: {
            const float loop_y = 44;
            const float descend_s = (loop_y + 6) / 55.0f;
            const float loop_s = 2 * PI_F / LOOP_SPEED;
            if (t < descend_s) {
                position = Vec2(origin.x, -6 + 55 * t);
            } else if (t < descend_s + loop_s) {
                float a = (t - descend_s) * LOOP_SPEED;
                position = Vec2(origin.x + dir * LOOP_RADIUS * (1 - cosf(a)), loop_y + LOOP_RADIUS * sinf(a));
            } else {
                position = Vec2(origin.x, loop_y + 55 * (t - descend_s - loop_s));
            }
            break;
        }
        case MovementPattern::Sweep: {
            float x = dir > 0 ? -8 + 42 * t : SCREEN_W + 8 - 42 * t;
            position = Vec2(x, origin.y + sinf(t * 2.6f) * 8 + 6 * t);
            break;
        }
        case MovementPattern::FigureEight: {
            // down to the eight, twice round it, then away down the screen
            const float descend_s = (EIGHT_Y + 6) / 50.0f;
            const float eight_s = 2 * 2 * PI_F / 2.2f;
            if (t < descend_s) {
                position = Vec2(origin.x, -6 + 50 * t);
            } else if (t < descend_s + eight_s) {
                float a = (t - descend_s) * 2.2f;
                position = Vec2(origin.x + dir * 20 * sinf(a), EIGHT_Y + 9 * sinf(2 * a));
            } else {
                position = Vec2(origin.x, EIGHT_Y + 50 * (t - descend_s - eight_s));
            }
            break;
        }
        case MovementPattern::VDive: {
            float ts = age / 1000.0f;
            float top = 30 - origin.y;  // the leader flies lowest, the wings trail behind it
            if (ts < V_ENTER_S) {
                position = Vec2(origin.x, lerpf(-8 - origin.y, top, ease_out(ts / V_ENTER_S)));
                dive_x = player_pos.x + (origin.x - SCREEN_W / 2);  // keep the V's shape in the dive
            } else if (ts > V_ENTER_S + V_WAIT_S) {
                float step = dt / 1000.0f;
                position.y += 80 * speed * step;
                position.x += clampf(dive_x - position.x, -35 * step, 35 * step);
            }
            break;
        }
        case MovementPattern::Strafe: {
            // in to its row, then slides left and right, stopping to fire, and leaves down the screen
            float ts = age / 1000.0f;
            const float cycle = STRAFE_SLIDE_S + STRAFE_STOP_S;
            holding = true;
            if (ts < HOVER_ENTER_S) {
                position = Vec2(origin.x, lerpf(-8, origin.y, ease_out(ts / HOVER_ENTER_S)));
            } else if (ts < HOVER_ENTER_S + cycle * STRAFE_STOPS) {
                float s = ts - HOVER_ENTER_S;
                int k = (int)(s / cycle);
                float in = s - k * cycle;
                auto stop_x = [&](int n) { return n < 0 ? origin.x : origin.x + dir * (n % 2 ? -16.0f : 16.0f); };
                holding = in < STRAFE_SLIDE_S;
                float f = holding ? ease_out(in / STRAFE_SLIDE_S) : 1.0f;
                position = Vec2(lerpf(stop_x(k - 1), stop_x(k), f), origin.y);
            } else {
                holding = false;
                position.y += 45 * speed * dt / 1000.0f;
            }
            break;
        }
        case MovementPattern::Hover: {
            float ts = age / 1000.0f;  // hovering ignores the speed multiplier for timing
            if (ts < HOVER_ENTER_S) {
                position = Vec2(origin.x, lerpf(-8, origin.y, ease_out(ts / HOVER_ENTER_S)));
            } else if (ts < HOVER_ENTER_S + HOVER_STAY_S / speed) {
                float h = ts - HOVER_ENTER_S;
                position = Vec2(origin.x + sinf(h * 1.7f + origin.x) * 6, origin.y + sinf(h * 2.3f) * 2);
                dive_x = player_pos.x;
            } else {
                // dive towards where the player was
                float step = dt / 1000.0f;
                position.y += 70 * speed * step;
                float dx = dive_x - position.x;
                position.x += clampf(dx, -30 * step, 30 * step);
            }
            break;
        }
    }
}

void Enemy::shoot(Vec2 player_pos) {
    auto &pm = ProjectileManager::get_instance();
    float bullet_speed = 0.05f * (0.9f + 0.1f * fire_rate);
    Vec2 muzzle = position + Vec2(0, 3);
    switch (enemy_preset(kind).fire) {
        case FireStyle::None:
            break;
        case FireStyle::Down:
            pm.fire(muzzle, bullet_speed, PI_F, 14, Faction::ENEMY);
            break;
        case FireStyle::Aimed:
            pm.fire_at(muzzle, player_pos, bullet_speed * 1.1f, 14);
            break;
        case FireStyle::Spread:
            for (int i = -1; i <= 1; i++) pm.fire(muzzle, bullet_speed, PI_F + i * 0.35f, 21, Faction::ENEMY);
            break;
        case FireStyle::Burst:
            pm.fire_at(muzzle, player_pos, bullet_speed * 1.2f, 12);
            break;
    }
}

void Enemy::update(uint32_t dt, Vec2 player_pos, bool player_alive) {
    if (!active) return;
    age += dt;
    if (flash_ms > 0) flash_ms -= dt;
    move(dt, player_pos);

    bool on_screen = position.x > -4 && position.x < SCREEN_W + 4 && position.y > -4 && position.y < SCREEN_H + 4;
    if (on_screen) entered = true;
    if ((entered && !on_screen) || age > 25000) {
        active = false;
        return;
    }

    const EnemyPreset &preset = enemy_preset(kind);
    if (preset.fire == FireStyle::None || !player_alive) return;
    bool can_fire = position.y > HUD_H + 2 && position.y < SCREEN_H * 3 / 4 && position.y < player_pos.y - 12;
    fire_timer -= (int32_t)(dt * fire_rate);
    if (fire_timer > 0) return;
    if (holding && burst_left == 0) return;  // a strafer saves its shot for the next stop

    if (burst_left > 0) {
        if (can_fire) shoot(player_pos);
        burst_left--;
        fire_timer = 140;
        return;
    }
    if (can_fire) {
        shoot(player_pos);
        if (preset.fire == FireStyle::Burst) {
            burst_left = BURST_SHOTS - 1;
            fire_timer = 140;
            return;
        }
    }
    fire_timer = (int32_t)(preset.fire_ms * frand(0.8f, 1.3f));
}

bool Enemy::take_damage(int amount) {
    health -= amount;
    flash_ms = 70;
    if (health <= 0) {
        active = false;
        return true;
    }
    return false;
}

void Enemy::draw() {
    if (!active) return;
    int sprite = enemy_preset(kind).sprite_index;
    if (flash_ms > 0) draw_sprite_flash(sheets.ships, sprite, position);
    else draw_sprite(sheets.ships, sprite, position);
}

// ---------------------------------------------------------------------------
// Boss
// ---------------------------------------------------------------------------

namespace {
    constexpr float BOSS_Y = 26;
    constexpr uint32_t BOSS_DEATH_MS = 1800;
    constexpr int BEAM_HALF_W = 2;  // the beam is 5 px wide
    constexpr float SWING = 38;     // px either side of the middle
    constexpr int32_t DASH_WARN_MS = 300, DASH_MOVE_MS = 450;

    struct BossPreset {
        int sprite;
        BossAttack attacks[4];
        uint8_t attack_count;
        int8_t beams[2];     // beam x offsets from the centre
        uint8_t beam_count;
        Pen beam;
    };

    // in story order, see docs/story/ships.md
    const BossPreset BOSSES[6] = {
        // the Herald, a show ship: big pretty fans and bursts
        {64, {BossAttack::Fan, BossAttack::Burst}, 2, {0}, 0, Pen()},
        // the Tollgate: the gate closing (fan), papers checks (aimed bursts), and its scanner eye
        {66, {BossAttack::Fan, BossAttack::Beam, BossAttack::Burst}, 3, {0}, 1, Pen(255, 160, 40)},
        // the Cradle drops its eggs, and rings of bullets from the hangar
        {68, {BossAttack::Rain, BossAttack::Ring, BossAttack::Rain, BossAttack::Burst}, 4, {0}, 0, Pen()},
        // the Paragon, Rook's fighter: fast aimed bursts and a long beam
        {84, {BossAttack::Burst, BossAttack::Beam, BossAttack::Fan}, 3, {0}, 1, Pen(120, 220, 255)},
        // the Prism, the jammer: spirals and rings of signal
        {86, {BossAttack::Spiral, BossAttack::Ring, BossAttack::Spiral, BossAttack::Beam}, 4, {0}, 1, Pen(255, 110, 230)},
        // the Bulwark: everything, with a beam from each hull
        {88, {BossAttack::Beam, BossAttack::Fan, BossAttack::Spiral, BossAttack::Rain}, 4, {-9, 9}, 2, Pen(255, 70, 40)},
    };

    const BossPreset &preset(int level) { return BOSSES[(level - 1) % 6]; }
}

int Boss::sprite_index() const {
    return preset(level).sprite;
}

void Boss::spawn(int lvl) {
    level = lvl;
    max_health = 60 + 40 * (level - 1);
    health = max_health;
    position = Vec2(SCREEN_W / 2, -16);
    age = 0;
    attack_timer = 600;
    attack = preset(level).attack_count - 1;  // the first attack is the first in the list
    shots_left = 0;
    flash_ms = 0;
    beam_ms = 0;
    spiral_angle = 0;
    move_phase = 0;
    dash_ms = 0;
    dying = false;
    dying_ms = 0;
    active = true;
}

void Boss::start_attack(BossAttack a, bool enraged) {
    int l = std::min(level, 3);
    switch (a) {
        case BossAttack::Fan: shots_left = enraged ? 3 : 2; break;
        case BossAttack::Burst: shots_left = (enraged ? 6 : 4) + l; break;
        case BossAttack::Spiral: shots_left = (enraged ? 24 : 16) + 4 * l; break;
        case BossAttack::Ring: shots_left = enraged ? 4 : 3; break;
        case BossAttack::Rain: shots_left = (enraged ? 16 : 11) + 2 * l; break;
        case BossAttack::Beam:
            shots_left = 1;
            beam_ms = BEAM_CHARGE_MS + BEAM_FIRE_MS;
            audio::play(audio::Sfx::BeamCharge);
            break;
    }
}

void Boss::fire(BossAttack a, Vec2 player_pos, bool enraged) {
    auto &pm = ProjectileManager::get_instance();
    Vec2 muzzle = position + Vec2(0, 7);
    float bullet_speed = 0.045f * (1.0f + 0.08f * (level - 1));
    switch (a) {
        case BossAttack::Fan: {
            int n = std::min(5 + level + (enraged ? 2 : 0), 11);
            float offset = (shots_left % 2) ? 0.1f : -0.1f;
            for (int i = 0; i < n; i++) {
                float angle = PI_F - 0.9f + 1.8f * i / (n - 1) + offset;
                pm.fire(muzzle, bullet_speed, angle, 9, Faction::ENEMY, 1, 2.5f);
            }
            attack_timer = 420;
            break;
        }
        case BossAttack::Burst:  // aimed, from alternating cannons
            pm.fire_at(position + Vec2((shots_left % 2) ? -6 : 6, 5), player_pos, bullet_speed * 1.5f, 12);
            attack_timer = 110;
            break;
        case BossAttack::Spiral:  // twin spiral
            spiral_angle += 0.45f;
            pm.fire(muzzle, bullet_speed, spiral_angle, 21, Faction::ENEMY);
            pm.fire(muzzle, bullet_speed, spiral_angle + PI_F, 21, Faction::ENEMY);
            attack_timer = 80;
            break;
        case BossAttack::Ring: {  // a full circle, every other one turned half a gap
            int n = enraged ? 16 : 12;
            float turn = (shots_left % 2) ? PI_F / n : 0;
            for (int i = 0; i < n; i++) pm.fire(position, bullet_speed * 0.9f, turn + 2 * PI_F * i / n, 14, Faction::ENEMY);
            attack_timer = 520;
            break;
        }
        case BossAttack::Rain:  // eggs dropped from under the wings
            pm.fire(position + Vec2(frand(-10, 10), 6), bullet_speed * frand(0.7f, 1.0f), PI_F + frand(-0.25f, 0.25f), 9,
                    Faction::ENEMY, 1, 2.5f);
            attack_timer = enraged ? 90 : 130;
            break;
        case BossAttack::Beam:
            break;  // handled in update
    }
}

void Boss::update(uint32_t dt, Vec2 player_pos, bool player_alive) {
    if (!active) return;
    age += dt;
    if (flash_ms > 0) flash_ms -= dt;

    if (dying) {
        dying_ms -= dt;
        // chain of explosions across the hull
        if ((dying_ms / 120) != ((dying_ms + (int32_t)dt) / 120)) {
            fx::explosion(position + Vec2(frand(-8, 8), frand(-7, 7)), irand(0, 2) ? fx::Boom::Small : fx::Boom::Blue);
            audio::play(audio::Sfx::Explode);
            fx::shake(2);
        }
        if (dying_ms <= 0) active = false;
        return;
    }

    float t = age / 1000.0f;
    if (entering()) {
        float k = t / 2.2f;
        position = Vec2(SCREEN_W / 2, lerpf(-16, BOSS_Y, 1.0f - (1.0f - k) * (1.0f - k)));
        return;
    }

    bool enraged = health * 2 < max_health;
    float pace = (1.0f + 0.12f * (level - 1) + (enraged ? 0.35f : 0.0f)) * pace_scale;

    if (beam_ms > 0) {
        // hold still and fire
        bool was_charging = !beam_firing();
        beam_ms -= dt;
        if (was_charging && beam_firing()) {
            audio::play(audio::Sfx::Beam);
            fx::shake(2);
        }
        if (beam_ms <= 0) {
            beam_ms = 0;
            shots_left = 0;
            attack_timer = enraged ? 650 : 1000;
        }
        return;
    }

    float bob = BOSS_Y + sinf(t * 1.3f) * 5;
    if (dash_ms > 0) {
        update_dash(dt);
        position.y = bob;
        return;
    }
    // the swing's phase adds up, so a change of pace never makes the boss jump
    move_phase += dt / 1000.0f * 0.7f * pace;
    position = Vec2(SCREEN_W / 2 + sinf(move_phase) * SWING, bob);

    if (!player_alive) return;
    attack_timer -= (int32_t)(dt * pace);
    if (attack_timer > 0) return;

    const BossPreset &p = preset(level);
    if (shots_left == 0) {
        // start the next attack in this boss's list
        attack = (attack + 1) % p.attack_count;
        start_attack(p.attacks[attack], enraged);
        if (beam_ms > 0) return;
    }

    fire(p.attacks[attack], player_pos, enraged);
    shots_left--;
    if (shots_left == 0) {
        attack_timer = enraged ? 650 : 1000;
        // between attacks it sometimes dashes over the player's column, more often when enraged
        if (irand(1, 100) <= (enraged ? 55 : 25) && fabsf(player_pos.x - position.x) > 16) start_dash(player_pos);
    }
}

bool Boss::dash_warning() const {
    return dash_ms > DASH_MOVE_MS;
}

void Boss::start_dash(Vec2 player_pos) {
    dash_ms = DASH_WARN_MS + DASH_MOVE_MS;
    dash_from = position.x;
    dash_to = clampf(player_pos.x, SCREEN_W / 2 - SWING, SCREEN_W / 2 + SWING);
    audio::play(audio::Sfx::BeamCharge);
}

void Boss::update_dash(uint32_t dt) {
    dash_ms -= dt;
    if (dash_warning()) return;  // shakes in place, see draw
    float k = 1.0f - std::max<int32_t>(0, dash_ms) / (float)DASH_MOVE_MS;
    k = k * k * (3 - 2 * k);  // ease in and out
    position.x = lerpf(dash_from, dash_to, k);
    if (dash_ms <= 0) {
        dash_ms = 0;
        // carry on swinging from where the dash ended
        move_phase = asinf(clampf((position.x - SCREEN_W / 2) / SWING, -1, 1));
        fx::shake(1.5f);
    }
}

bool Boss::beam_hits(const Rect &r) const {
    if (!active || dying || !beam_firing()) return false;
    const BossPreset &p = preset(level);
    for (int i = 0; i < p.beam_count; i++) {
        float x = position.x + p.beams[i];
        if (r.x <= x + BEAM_HALF_W && r.x + r.w >= x - BEAM_HALF_W && r.y + r.h >= position.y) return true;
    }
    return false;
}

bool Boss::take_damage(int amount) {
    if (dying || !active) return false;
    health -= amount;
    flash_ms = 60;
    if (health <= 0) {
        health = 0;
        dying = true;
        dying_ms = BOSS_DEATH_MS;
        beam_ms = 0;
        dash_ms = 0;
        return true;
    }
    return false;
}

void Boss::draw(uint32_t time) {
    if (!active) return;
    if (dying && (dying_ms / 70) % 2 == 0) {
        draw_sprite_flash(sheets.ships, sprite_index(), position, Size(2, 2), Pen(255, 220, 120));
        return;
    }

    if (beam_ms > 0 && !dying) {
        const BossPreset &p = preset(level);
        Point o = fx::offset();
        for (int i = 0; i < p.beam_count; i++) {
            int x = (int)position.x + p.beams[i] + o.x;
            int top = (int)position.y + 6 + o.y;
            if (!beam_firing()) {
                // charging: a thin flickering line and a growing glow at the muzzle
                float k = 1.0f - (float)(beam_ms - BEAM_FIRE_MS) / BEAM_CHARGE_MS;
                if ((time / 50) % 2) {
                    screen.pen = Pen(p.beam.r, p.beam.g, p.beam.b, 90 + (int)(100 * k));
                    screen.v_span(Point(x, top), SCREEN_H - top);
                }
                screen.pen = Pen(255, 255, 255, 120 + (int)(120 * k));
                screen.circle(Point(x, top), 1 + (int)(2 * k));
            } else {
                int w = BEAM_HALF_W + (int)((time / 40) % 2);
                screen.pen = Pen(p.beam.r, p.beam.g, p.beam.b, 170);
                screen.rectangle(Rect(x - w, top, w * 2 + 1, SCREEN_H - top));
                screen.pen = Pen(255, 255, 255);
                screen.rectangle(Rect(x - 1, top, 3, SCREEN_H - top));
                screen.circle(Point(x, top), 3);
            }
        }
    }

    if (dash_warning()) {
        // about to dash: it shakes and flashes
        Vec2 shake(((time / 30) % 2) ? 1 : -1, 0);
        if ((time / 60) % 2) draw_sprite_flash(sheets.ships, sprite_index(), position + shake, Size(2, 2), Pen(255, 200, 80));
        else draw_sprite(sheets.ships, sprite_index(), position + shake, Size(2, 2));
        return;
    }
    if (flash_ms > 0) draw_sprite_flash(sheets.ships, sprite_index(), position, Size(2, 2));
    else draw_sprite(sheets.ships, sprite_index(), position, Size(2, 2));
}
