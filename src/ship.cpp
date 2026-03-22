#include "ship.hpp"

Surface *ship_spritesheet = Surface::load(asset_ships);

void Ship::move(Vec2 movement_vector, uint32_t dt) {
    if (movement_vector.x != 0 || movement_vector.y != 0) {
        movement_vector.normalize();
    }
    velocity = movement_vector * agility * dt;
    position += velocity;

    // Screen Bounds
    position.x = std::clamp(position.x, 0.0f, 119.0f);
    position.y = std::clamp(position.y, 0.0f, 119.0f);
}

void Ship::rotate(float direction) {
    angle = direction;
}

void Ship::fire() {
    if (fire_cooldown <= 0) {
        // Request a projectile from the manager
        ProjectileManager::get_instance().fire(position, angle, projectile_sprite_idx);
        fire_cooldown = COOLDOWN_MAX;
    }
}

void Ship::update(uint32_t dt) {
    ShipControls controls = get_ship_controls();
    
    move(controls.movement_vector, dt);
    rotate(controls.rotation_direction);

    if (controls.fire) fire();

    if (fire_cooldown > 0) fire_cooldown -= dt;
}

void Ship::draw() {
    screen.sprites = ship_spritesheet;
    screen.sprite(sprite_index, position);
}