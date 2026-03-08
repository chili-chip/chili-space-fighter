#include "ship.hpp"

Surface *ship_spritesheet = Surface::load(asset_ships);
Surface *projectile_spritesheet = Surface::load(asset_projectiles);

Projectile::Projectile() {
    sprite_index = 0;
    position = Vec2(-1, -1);
    velocity = Vec2(0, -1);
    speed = 0.3f;

}

Projectile::Projectile(int sprite_index) {
    this->sprite_index = sprite_index;
    position = Vec2(-1, -1);
    velocity = Vec2(0, -1);
    speed = 0.3f;

}

bool Projectile::is_off_screen() {
    return position.y < 0;
}

void Projectile::update(uint32_t dt) {
    if (is_off_screen()) {
        return;
    }
    position += velocity * speed * dt;
}

void Projectile::fire(Vec2 position, float angle) {
    this->position = position;
    this->velocity = Vec2(0, -1);
    this->velocity.rotate(angle);
}

void Projectile::draw() {
    if (is_off_screen()) {
        return;
    }
    Vec2 projectile1_offset = Vec2(-3, 0);
    Vec2 projectile2_offset = Vec2(4, 0);
    screen.sprites = projectile_spritesheet;
    screen.sprite(sprite_index, Point(position + projectile1_offset));
    screen.sprite(sprite_index, Point(position + projectile2_offset));
}

void Ship::move(Vec2 movement_vector) {
    if (movement_vector.x != 0 || movement_vector.y != 0) {
        movement_vector.normalize();
    }
    velocity = movement_vector * agility * dt;
    position += velocity;

    // keep player inside 128×128 screen area
    if(position.x < 0.0f) position.x = 0.0f;
    if(position.y < 0.0f) position.y = 0.0f;
    if(position.x > 119.0f) position.x = 119.0f;
    if(position.y > 119.0f) position.y = 119.0f;
}

void Ship::rotate(float direction) {
    angle = direction;
}

void Ship::fire() {
    if (fire_cooldown == 0) {
        projectiles[projectile_index].fire(position, angle);
        projectile_index = (projectile_index + 1) % 10;
        fire_cooldown = 255;
    }
}

void Ship::update(uint32_t time) {
    dt = (time - last_update_time);
    ShipControls controls = get_ship_controls();
    move(controls.movement_vector);
    rotate(controls.rotation_direction);

    if (controls.fire) {
        fire();
        controls.fire = false;
    }
    
    for (auto &projectile : projectiles) {
        projectile.update(dt);
    }

    fire_cooldown = max(0, fire_cooldown - (int)dt);

    last_update_time = time;
}

void Ship::draw() {
    for (auto projectile : projectiles) {
        projectile.draw();
    }

    screen.sprites = ship_spritesheet;
    screen.sprite(sprite_index, position);
    // Apply only to player
    // if (velocity.x > 0 && position.x < 119) {
    //     screen.sprite(sprite_index+1, position);
    // }
    // else if (velocity.x < 0 && position.x > 0) {
    //     screen.sprite(sprite_index-1, position);
    // }
    // else {
    //     screen.sprite(sprite_index, position);
    // }
}