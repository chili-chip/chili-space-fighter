#include "projectile_manager.hpp"

Surface *projectile_spritesheet = Surface::load(asset_projectiles);

void Projectile::update(uint32_t dt) {
    if (!active) return;
    position += velocity * speed * dt;
    if (is_off_screen()) active = false;
}

bool Projectile::is_off_screen() {
    return (position.y < 0 || position.y > 120 || position.x < 0 || position.x > 120);
}

void Projectile::draw() {
    if (!active) return;
    screen.sprites = projectile_spritesheet;
    // Galaga-style double cannons
    screen.sprite(sprite_index, Point(position + Vec2(-3, 0)));
    screen.sprite(sprite_index, Point(position + Vec2(4, 0)));
}

void ProjectileManager::fire(Vec2 pos, float angle, int sprite_idx) {
    for (int i = 0; i < MAX_PROJECTILES; i++) {
        if (!pool[i].active) {
            pool[i].position = pos;
            pool[i].velocity = Vec2(0, -1);
            pool[i].velocity.rotate(angle);
            pool[i].sprite_index = sprite_idx;
            pool[i].active = true;
            return;
        }
    }
}

void ProjectileManager::update(uint32_t dt) {
    for (int i = 0; i < MAX_PROJECTILES; i++) {
        pool[i].update(dt);
    }
}

void ProjectileManager::draw() {
    for (int i = 0; i < MAX_PROJECTILES; i++) {
        pool[i].draw();
    }
}