#include "enemy.hpp"

Enemy::Enemy(const EnemyPreset& preset) {
    speed = preset.speed;
    agility = preset.agility;
    health = preset.health;
    sprite_index = preset.sprite_index;
    projectile_index = preset.projectile_index;
    position = Vec2(64, 10);
    velocity = Vec2(0, 0);
    last_update_time = 0;
    movement_pattern = preset.movement_pattern;

    for (int i = 0; i < 10; ++i) {
        projectiles[i].set_sprite_index(projectile_index);
    }
}

ShipControls Enemy::get_ship_controls() {
    ShipControls controls;
    controls.movement_vector = Vec2(0, 0);
    controls.rotation_direction = M_PI;
    controls.fire = true;

    switch (movement_pattern) {
        case MovementPattern::StraightDown:
            controls.movement_vector = Vec2(0, 1);
            break;
        case MovementPattern::ZigZag:
            controls.movement_vector = Vec2(sinf(last_update_time / 1000.0f) * 0.5f, 1);
            break;
        case MovementPattern::Circle:
            controls.movement_vector = Vec2(cosf(last_update_time / 1000.0f), sinf(last_update_time / 1000.0f));
            break;
    }
    return controls;
}