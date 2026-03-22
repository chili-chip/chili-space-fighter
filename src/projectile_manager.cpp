#include "projectile_manager.hpp"
#include "ship.hpp"

Surface *projectile_spritesheet = Surface::load(asset_projectiles);

void Projectile::update(uint32_t dt) {
    if (!active) return;
    position += velocity * dt;
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

    //DEBUG DRAW
    // screen.pen = Pen(255,0,0);
    // screen.rectangle(Rect(position.x+1, position.y+4, 8, 1));

}

void ProjectileManager::fire(Vec2 pos, float projectile_speed, float angle, int sprite_idx, Faction faction) {
    for (int i = 0; i < MAX_PROJECTILES; i++) {
        if (!pool[i].active) {
            pool[i].position = pos;
            pool[i].velocity = Vec2(0, -1) * projectile_speed;
            pool[i].velocity.rotate(angle);
            pool[i].sprite_index = sprite_idx;
            pool[i].owner = faction; // Track the source
            pool[i].active = true;
            return;
        }
    }
}

void ProjectileManager::check_collisions(Ship& player, std::vector<Ship*>& enemies) {
    for (int i = 0; i < MAX_PROJECTILES; i++) {
        if (!pool[i].active) continue;

        Rect bullet_rect = pool[i].get_hitbox();

        if (pool[i].owner == PLAYER) {
            // Check player bullets against all enemies
            for (auto enemy : enemies) {
                if (bullet_rect.intersects(enemy->get_hitbox())) {
                    enemy->take_damage(10); // Assume a take_damage method exists
                    pool[i].active = false; // Destroy bullet
                    break; 
                }
            }
        } else {
            // Check enemy bullets against the player
            if (bullet_rect.intersects(player.get_hitbox())) {
                player.take_damage(10);
                pool[i].active = false;
            }
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