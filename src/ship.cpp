#include "ship.hpp"

Surface *ship_spritesheet = Surface::load(asset_ships);
Surface *projectile_spritesheet = Surface::load(asset_projectiles);

Projectile::Projectile() {
    position = Vec2(-1, -1);
    velocity = Vec2(0, -1);
    speed = 0.3f;
}

Projectile::~Projectile() {

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

void Projectile::draw(uint32_t time) {
    if (is_off_screen()) {
        return;
    }
    Vec2 projectile1_offset = Vec2(-3, 0);
    Vec2 projectile2_offset = Vec2(4, 0);
    screen.sprites = projectile_spritesheet;
    screen.sprite(2, Point(position + projectile1_offset));
    screen.sprite(2, Point(position + projectile2_offset));
}

void Ship::move(InputState input, uint32_t dt) {
    velocity = Vec2(0, 0);
    if (input.up){
        Vec2 movement_vector = Vec2(0, -1);
        movement_vector.rotate(angle);
        velocity += movement_vector;
    }
    if (input.down) {
        Vec2 movement_vector = Vec2(0, 1);
        movement_vector.rotate(angle);
        velocity += movement_vector;
    }
    if (input.left){
        Vec2 movement_vector = Vec2(-1, 0);
        movement_vector.rotate(angle);
        velocity += movement_vector;
    }
    if (input.right) {
        Vec2 movement_vector = Vec2(1, 0);
        movement_vector.rotate(angle);
        velocity += movement_vector;
    }

    if(velocity.x || velocity.y) {
        velocity.normalize();
    }

    position += velocity * agility * dt;

    // keep player inside 128×128 screen area
    if(position.x < 0.0f) position.x = 0.0f;
    if(position.y < 0.0f) position.y = 0.0f;
    if(position.x > 119.0f) position.x = 119.0f;
    if(position.y > 119.0f) position.y = 119.0f;
}

void Ship::rotate(float direction) {
    angle = direction;
}

void Ship::fire(InputState input) {
    if (input.a && fire_cooldown == 0 && !fired_this_frame) {
        projectiles[projectile_index].fire(position, angle);
        projectile_index = (projectile_index + 1) % 10;
        fire_cooldown = 255;
        fired_this_frame = true;
    }

    if (!input.a) {
        fired_this_frame = false;
    }
}

void Ship::update(uint32_t time) {
    uint32_t dt = (time - last_update_time);
    InputState input = get_input();
    move(input, dt);
    fire(input);
    
    for (auto &projectile : projectiles) {
        projectile.update(dt);
    }

    fire_cooldown = max(0, fire_cooldown - (int)dt);

    last_update_time = time;
}

void Ship::draw(uint32_t time) {
    // draw ship sprite at position with rotation
    // blit::sprite(sprite_index, position, SpriteFlags::rotate(angle));

    for (auto projectile : projectiles) {
        projectile.draw(time);
    }

    screen.sprites = ship_spritesheet;
    if (velocity.x > 0 && position.x < 119) {
        screen.sprite(sprite_index+1, position);
    }
    else if (velocity.x < 0 && position.x > 0) {
        screen.sprite(sprite_index-1, position);
    }
    else {
        screen.sprite(sprite_index, position);
    }
}