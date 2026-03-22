#include "enemy.hpp"

Enemy::Enemy(const EnemyPreset& preset) {
    speed = preset.speed;
    agility = preset.agility;
    health = preset.health;
    sprite_index = preset.sprite_index;
    projectile_sprite_idx = preset.projectile_index;
    projectile_speed = 0.2f;
    COOLDOWN_MAX = 800;
    position = Vec2(64, 10);
    velocity = Vec2(0, 0);
    movement_pattern = preset.movement_pattern;
    faction = Faction::ENEMY;
}

ShipControls Enemy::get_ship_controls() {
    ShipControls controls;
    controls.movement_vector = Vec2(0, 0);
    controls.rotation_direction = 3.14f;
    controls.fire = true;

    switch (movement_pattern) {
        case MovementPattern::StraightDown:
            controls.movement_vector = Vec2(0, 1);
            break;
        case MovementPattern::ZigZag:
            controls.movement_vector = Vec2(sinf(blit::now() / 1000.0f) * 0.5f, 1);
            break;
        case MovementPattern::Circle:
            controls.movement_vector = Vec2(cosf(blit::now() / 1000.0f), sinf(blit::now() / 1000.0f));
            break;
    }
    return controls;
}