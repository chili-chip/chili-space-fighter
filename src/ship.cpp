#include "ship.hpp"

Surface *ship_spritesheet = Surface::load(asset_ships);

void Ship::move(Vec2 movement_vector, uint32_t dt) {
    if (movement_vector.x != 0 || movement_vector.y != 0) {
        movement_vector.normalize();
    }
    velocity = movement_vector * agility * dt;
    position += velocity;
}

void Ship::rotate(float direction) {
    angle = direction;
}

void Ship::fire() {
    if (fire_cooldown <= 0) {
        ProjectileManager::get_instance().fire(position, projectile_speed, angle, projectile_sprite_idx, faction);
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

    //DEBUG DRAW
    // screen.pen = Pen(255,0,0);
    // screen.rectangle(Rect(position.x,position.y, 8, 8));
}

void Ship:: take_damage(int amount) {
    health -= amount;
    if (health <= 0) {
        health = 0;
        alive = false;
        EffectManager::get_instance().spawn_explosion(&position, 87, 4, 0.1f);
    }
}