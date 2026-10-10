#include "projectile_manager.hpp"
#include "fx.hpp"

bool Projectile::is_off_screen() const {
    return position.y < -8 || position.y > SCREEN_H + 8 || position.x < -8 || position.x > SCREEN_W + 8;
}

void ProjectileManager::fire(Vec2 pos, float speed, float angle, int sprite_idx, Faction faction, uint8_t damage, float radius) {
    for (auto &p : pool) {
        if (p.active) continue;
        p.position = pos;
        p.velocity = Vec2(sinf(angle), -cosf(angle)) * speed;
        p.sprite_index = sprite_idx;
        p.owner = faction;
        p.damage = damage;
        p.radius = radius;
        p.active = true;
        return;
    }
}

void ProjectileManager::fire_at(Vec2 pos, Vec2 target, float speed, int sprite_idx, float spread) {
    Vec2 d = target - pos;
    // angle measured from "up", clockwise
    float angle = atan2f(d.x, -d.y) + spread;
    fire(pos, speed, angle, sprite_idx, Faction::ENEMY);
}

void ProjectileManager::clear() {
    for (auto &p : pool) p.active = false;
}

void ProjectileManager::clear_enemy_bullets(bool sparkle) {
    for (auto &p : pool) {
        if (p.active && p.owner == Faction::ENEMY) {
            if (sparkle) fx::particles(p.position, Pen(140, 220, 255), 2, 0.03f, 300);
            p.active = false;
        }
    }
}

void ProjectileManager::update(uint32_t dt) {
    for (auto &p : pool) {
        if (!p.active) continue;
        p.position += p.velocity * (float)dt;
        if (p.is_off_screen()) p.active = false;
    }
}

void ProjectileManager::draw() {
    for (auto &p : pool) {
        if (p.active) draw_sprite(sheets.projectiles, p.sprite_index, p.position);
    }
}
