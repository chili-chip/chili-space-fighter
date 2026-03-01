#include "player.hpp"
#include <cstdlib>

#define STARTING_HEALTH 255
#define STARTING_SPEED 100
#define STARTING_MANEUVRABILITY 1.5f
#define STARTING_POSITION_X 64
#define STARTING_POSITION_Y 64
#define STARTING_SHIP_SPRITE_INDEX 1

PlayerBullet::PlayerBullet(Vec2 spawn_position) {
    position_vector = spawn_position;
    movement_vector = Vec2(0, -1);
    speed = 0.3f;
}

PlayerBullet::~PlayerBullet() {

}

bool PlayerBullet::is_off_screen() {
    return position_vector.y < 0;
}

void PlayerBullet::update(uint32_t dt) {
    position_vector += movement_vector * speed * dt;
}

void PlayerBullet::draw(uint32_t time) {
    screen.sprites = Surface::load(asset_projectiles);
    screen.sprite(0, Point(position_vector));
}

Player::Player() {
    position_vector = Vec2(STARTING_POSITION_X, STARTING_POSITION_Y);
    screen_coords = Point(STARTING_POSITION_X, STARTING_POSITION_Y);
    speed = STARTING_SPEED;
    maneuvrability = STARTING_MANEUVRABILITY;
    health = STARTING_HEALTH;
    movement_vector = Vec2(0, 0);
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
    movement_vector = Vec2(0, 0);
    if (buttons & Button::DPAD_UP)    movement_vector.y -= 1.0f;
    if (buttons & Button::DPAD_DOWN)  movement_vector.y += 1.0f;
    if (buttons & Button::DPAD_LEFT)  movement_vector.x -= 1.0f;
    if (buttons & Button::DPAD_RIGHT) movement_vector.x += 1.0f;

    if(movement_vector.x || movement_vector.y)
        movement_vector.normalize();
    position_vector += movement_vector * maneuvrability;

    // keep player inside 128×128 screen area
    if(position_vector.x < 0.0f) position_vector.x = 0.0f;
    if(position_vector.y < 0.0f) position_vector.y = 0.0f;
    if(position_vector.x > 119.0f) position_vector.x = 119.0f;
    if(position_vector.y > 119.0f) position_vector.y = 119.0f;

    screen_coords = Point(position_vector);
}


void Player::fire() {
    if (buttons & Button::A && fire_cooldown == 0 && !fired_this_frame) {
        bullets.push_back(new PlayerBullet(position_vector));
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
    if (movement_vector.x > 0 && screen_coords.x < 119) {
        screen.sprite(ship_sprite_index+1, screen_coords);
    }
    else if (movement_vector.x < 0 && screen_coords.x > 0) {
        screen.sprite(ship_sprite_index-1, screen_coords);
    }
    else {
        screen.sprite(ship_sprite_index, screen_coords);
    }

    draw_cooldown_bar();
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
    particle_gen.update(time, position_vector, movement_vector);

    last_update_time = time;
}
