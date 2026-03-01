#include "player.hpp"
#include <cstdlib>

#define STARTING_HEALTH 255
#define STARTING_SPEED 100
#define STARTING_MANEUVRABILITY 1.5f
#define STARTING_POSITION_X 64
#define STARTING_POSITION_Y 64
#define STARTING_SHIP_SPRITE_INDEX 1

Player::Player() {
    screen.sprites = Surface::load(asset_ships);
    position_vector = Vec2(STARTING_POSITION_X, STARTING_POSITION_Y);
    screen_coords = Point(STARTING_POSITION_X, STARTING_POSITION_Y);
    speed = STARTING_SPEED;
    maneuvrability = STARTING_MANEUVRABILITY;
    health = STARTING_HEALTH;
    movement_vector = Vec2(0, 0);
    ship_sprite_index = STARTING_SHIP_SPRITE_INDEX;
}

Player::~Player() {

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
    
}

void Player::draw(uint32_t time) {
    // draw particles first (background)
    for (auto p : particle_gen.particles) {
        screen.pen = Pen(255, 200, 100);
        screen.pixel(Point(p->pos));
    }

    // then draw the ship
    if (movement_vector.x > 0 && screen_coords.x < 119) {
        screen.sprite(ship_sprite_index+1, screen_coords);
    }
    else if (movement_vector.x < 0 && screen_coords.x > 0) {
        screen.sprite(ship_sprite_index-1, screen_coords);
    }
    else {
        screen.sprite(ship_sprite_index, screen_coords);
    }
}

void Player::update(uint32_t time) {
    move();
    
    particle_gen.update(time, position_vector);
}
