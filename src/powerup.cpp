#include "powerup.hpp"

namespace {
    const int SPRITES[(int)PowerUpKind::COUNT] = {15, 16, 39, 52, 2, 0};
    const char *NAMES[(int)PowerUpKind::COUNT] = {"POWER UP", "RAPID FIRE", "SHIELD", "REPAIR", "1UP", "BONUS"};
    const int WEIGHTS[(int)PowerUpKind::COUNT] = {26, 16, 16, 14, 4, 24};
    constexpr float FALL_SPEED = 0.018f;
}

int powerup_sprite(PowerUpKind kind) { return SPRITES[(int)kind]; }
const char *powerup_name(PowerUpKind kind) { return NAMES[(int)kind]; }

void PowerUpManager::clear() {
    for (auto &p : pool) p.active = false;
}

void PowerUpManager::spawn(Vec2 pos, PowerUpKind kind) {
    for (auto &p : pool) {
        if (p.active) continue;
        p.kind = kind;
        p.position = pos;
        p.age = 0;
        p.active = true;
        return;
    }
}

void PowerUpManager::spawn_random(Vec2 pos, int weapon_level, int hull, int max_hull) {
    int weights[(int)PowerUpKind::COUNT];
    int total = 0;
    for (int i = 0; i < (int)PowerUpKind::COUNT; i++) {
        weights[i] = WEIGHTS[i];
        if (i == (int)PowerUpKind::Weapon && weapon_level >= 4) weights[i] = 0;
        if (i == (int)PowerUpKind::Repair && hull >= max_hull) weights[i] = 0;
        total += weights[i];
    }
    int roll = irand(0, total - 1);
    for (int i = 0; i < (int)PowerUpKind::COUNT; i++) {
        if (roll < weights[i]) {
            spawn(pos, (PowerUpKind)i);
            return;
        }
        roll -= weights[i];
    }
}

void PowerUpManager::update(uint32_t dt) {
    for (auto &p : pool) {
        if (!p.active) continue;
        p.age += dt;
        p.position.y += FALL_SPEED * dt;
        p.position.x += sinf(p.age * 0.004f) * 0.01f * dt;
        if (p.age > PowerUp::LIFETIME_MS || p.position.y > SCREEN_H + 6) p.active = false;
    }
}

void PowerUpManager::draw(uint32_t time) {
    for (auto &p : pool) {
        if (!p.active) continue;
        // blink when about to disappear
        if (p.age > PowerUp::LIFETIME_MS - 2000 && (p.age / 100) % 2 == 0) continue;
        // soft glow behind the pickup
        Point c = Point(p.position);
        int r = 5 + (int)((time / 150) % 2);
        screen.pen = Pen(255, 255, 255, 60);
        screen.circle(c, r);
        draw_sprite(sheets.misc, powerup_sprite(p.kind), p.position);
    }
}
