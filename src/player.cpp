#include "player.hpp"
#include <cstdlib>

#define STARTING_HEALTH 255
#define STARTING_SPEED 100
#define STARTING_MANEUVRABILITY 1.5f
#define STARTING_POSITION_X 64
#define STARTING_POSITION_Y 64
#define STARTING_SHIP_SPRITE_INDEX 1

Projectile::Projectile(Point spawn_position) {
    position_coords = spawn_position;
    velocity = Vec2(0, -1);
    speed = 0.5f;
}

Projectile::~Projectile() {

}

bool Projectile::is_off_screen() {
    return position_coords.y < 0;
}

void Projectile::update(uint32_t dt) {
    position_coords += velocity * speed * dt;
}

void Projectile::draw(uint32_t time) {
    Point projectile1_offset = Point(-3, 0);
    Point projectile2_offset = Point(4, 0);
    screen.sprites = Surface::load(asset_projectiles);
    screen.sprite(2, Point(position_coords + projectile1_offset));
    screen.sprite(2, Point(position_coords + projectile2_offset));
}

Player::Player() {
    position = Vec2(STARTING_POSITION_X, STARTING_POSITION_Y);
    speed = STARTING_SPEED;
    maneuvrability = STARTING_MANEUVRABILITY;
    health = STARTING_HEALTH;
    velocity = Vec2(0, 0);
    ship_sprite_index = STARTING_SHIP_SPRITE_INDEX;
    fire_cooldown = 0;
    timer = 0;
    last_update_time = 0;
    fired_this_frame = false;
}

Player::~Player() {
    for (auto b : bullets) {
        delete b;
    }
    bullets.clear();
}

void Player::move() {
    velocity = Vec2(0, 0);
    if (buttons & Button::DPAD_UP)    velocity.y -= 1.0f;
    if (buttons & Button::DPAD_DOWN)  velocity.y += 1.0f;
    if (buttons & Button::DPAD_LEFT)  velocity.x -= 1.0f;
    if (buttons & Button::DPAD_RIGHT) velocity.x += 1.0f;

    if(velocity.x || velocity.y)
        velocity.normalize();
    position += velocity * maneuvrability;

    // keep player inside 128×128 screen area
    if(position.x < 0.0f) position.x = 0.0f;
    if(position.y < 0.0f) position.y = 0.0f;
    if(position.x > 119.0f) position.x = 119.0f;
    if(position.y > 119.0f) position.y = 119.0f;
}


void Player::fire() {
    if (buttons & Button::A && fire_cooldown == 0 && !fired_this_frame) {
        bullets.push_back(new Projectile(position));
        fire_cooldown = 255;
        fired_this_frame = true;
    }
    
    if (!(buttons & Button::A)) {
        fired_this_frame = false;
    }
}

void Player::clear_bullets() {
    for (int i = (int)bullets.size() - 1; i >= 0; --i) {
        if (bullets[i]->is_off_screen()) {
            delete bullets[i];
            bullets.erase(bullets.begin() + i);
        }
    }
}

void Player::cooldown(uint32_t time) {
    if (fire_cooldown > 0) {
        if (time - timer > 5) {
            fire_cooldown -= 17;
            timer = time;
        }

    }
}

void Player::draw_cooldown_bar() {
    screen.pen = Pen(255, 0, 0);
    screen.rectangle(Rect(0, 120, fire_cooldown / 255.0f * 128, 8));
}

void Player::draw(uint32_t time) {
    // draw bullets
    for (auto b : bullets) {
        b->draw(time);
    }

    // draw particles
    for (auto p : particle_gen.particles) {
        screen.pen = Pen(255, 200, 100);
        screen.pixel(Point(p->pos));
    }

    // draw the ship
    screen.sprites = Surface::load(asset_ships);
    if (velocity.x > 0 && position.x < 119) {
        screen.sprite(ship_sprite_index+1, position);
    }
    else if (velocity.x < 0 && position.x > 0) {
        screen.sprite(ship_sprite_index-1, position);
    }
    else {
        screen.sprite(ship_sprite_index, position);
    }
}

void Player::update(uint32_t time) {
    uint32_t dt = time - last_update_time;
    move();
    fire();
    cooldown(time);
    clear_bullets();
    
    for (auto b : bullets) {
        b->update(dt);
    }
    particle_gen.update(time, position, velocity);

    last_update_time = time;
}
