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
        {39,  5,  250, FireStyle::Spread, 2400, 0.85f, 12},  // Crab
        {47,  10, 400, FireStyle::Burst,  2800, 0.70f, 20},  // Tank
    };

    constexpr float LOOP_RADIUS = 16.0f;
    constexpr float LOOP_SPEED = 3.0f;    // radians per second
    constexpr float HOVER_ENTER_S = 1.2f;
    constexpr float HOVER_STAY_S = 5.0f;

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
            pm.fire(muzzle, bullet_speed, PI_F, 0, Faction::ENEMY);
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

    if (burst_left > 0) {
        if (can_fire) shoot(player_pos);
        burst_left--;
        fire_timer = 140;
        return;
    }
    if (can_fire) {
        shoot(player_pos);
        if (preset.fire == FireStyle::Burst) {
            burst_left = 2;
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
    const int BOSS_SPRITES[6] = {64, 66, 68, 84, 86, 88};
    constexpr float BOSS_Y = 26;
    constexpr uint32_t BOSS_DEATH_MS = 1800;
}

int Boss::sprite_index() const {
    return BOSS_SPRITES[(level - 1) % 6];
}

void Boss::spawn(int lvl) {
    level = lvl;
    max_health = 60 + 40 * (level - 1);
    health = max_health;
    position = Vec2(SCREEN_W / 2, -16);
    age = 0;
    attack_timer = 600;
    attack = 0;
    shots_left = 0;
    flash_ms = 0;
    spiral_angle = 0;
    dying = false;
    dying_ms = 0;
    active = true;
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
    float pace = 1.0f + 0.12f * (level - 1) + (enraged ? 0.35f : 0.0f);
    float tm = t - 2.2f;
    position = Vec2(SCREEN_W / 2 + sinf(tm * 0.7f * pace) * 38, BOSS_Y + sinf(tm * 1.3f) * 5);

    if (!player_alive) return;
    attack_timer -= (int32_t)(dt * pace);
    if (attack_timer > 0) return;

    auto &pm = ProjectileManager::get_instance();
    Vec2 muzzle = position + Vec2(0, 7);
    float bullet_speed = 0.045f * (1.0f + 0.08f * (level - 1));

    if (shots_left == 0) {
        // start the next attack in the cycle
        attack = (attack + 1) % 3;
        switch (attack) {
            case 0: shots_left = enraged ? 3 : 2; break;
            case 1: shots_left = (enraged ? 6 : 4) + std::min(level, 3); break;
            case 2: shots_left = (enraged ? 24 : 16) + 4 * std::min(level, 3); break;
        }
    }

    switch (attack) {
        case 0: {  // fan
            int n = std::min(5 + level + (enraged ? 2 : 0), 11);
            float offset = (shots_left % 2) ? 0.1f : -0.1f;
            for (int i = 0; i < n; i++) {
                float a = PI_F - 0.9f + 1.8f * i / (n - 1) + offset;
                pm.fire(muzzle, bullet_speed, a, 9, Faction::ENEMY, 1, 2.5f);
            }
            attack_timer = 420;
            break;
        }
        case 1:  // aimed burst from alternating cannons
            pm.fire_at(position + Vec2((shots_left % 2) ? -6 : 6, 5), player_pos, bullet_speed * 1.5f, 12);
            attack_timer = 110;
            break;
        case 2:  // twin spiral
            spiral_angle += 0.45f;
            pm.fire(muzzle, bullet_speed, spiral_angle, 21, Faction::ENEMY);
            pm.fire(muzzle, bullet_speed, spiral_angle + PI_F, 21, Faction::ENEMY);
            attack_timer = 80;
            break;
    }

    shots_left--;
    if (shots_left == 0) attack_timer = enraged ? 650 : 1000;
}

bool Boss::take_damage(int amount) {
    if (dying || !active) return false;
    health -= amount;
    flash_ms = 60;
    if (health <= 0) {
        health = 0;
        dying = true;
        dying_ms = BOSS_DEATH_MS;
        return true;
    }
    return false;
}

void Boss::draw() {
    if (!active) return;
    if (dying && (dying_ms / 70) % 2 == 0) {
        draw_sprite_flash(sheets.ships, sprite_index(), position, Size(2, 2), Pen(255, 220, 120));
        return;
    }
    if (flash_ms > 0) draw_sprite_flash(sheets.ships, sprite_index(), position, Size(2, 2));
    else draw_sprite(sheets.ships, sprite_index(), position, Size(2, 2));
}
