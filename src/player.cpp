#include "player.hpp"

#define STARTING_HEALTH 255
#define STARTING_SPEED 100
#define STARTING_AGILITY 0.2f
#define STARTING_POSITION_X 64
#define STARTING_POSITION_Y 64
#define STARTING_SHIP_SPRITE_INDEX 1


Player::Player() {
    position = Vec2(STARTING_POSITION_X, STARTING_POSITION_Y);
    speed = STARTING_SPEED;
    agility = STARTING_AGILITY;
    health = STARTING_HEALTH;
    velocity = Vec2(0, 0);
    sprite_index = STARTING_SHIP_SPRITE_INDEX;
    last_update_time = 0;
}

InputState Player::get_input() {
    InputState input;
    input.up = buttons & Button::DPAD_UP;
    input.down = buttons & Button::DPAD_DOWN;
    input.left = buttons & Button::DPAD_LEFT;
    input.right = buttons & Button::DPAD_RIGHT;
    input.a = buttons & Button::A;
    input.b = buttons & Button::B;
    input.x = buttons & Button::X;
    input.y = buttons & Button::Y;

    return input;
}


// void Player::fire() {
//     if (buttons & Button::A && fire_cooldown == 0 && !fired_this_frame) {
//         bullets.push_back(new Projectile(position));
//         fire_cooldown = 255;
//         fired_this_frame = true;
//     }
    
//     if (!(buttons & Button::A)) {
//         fired_this_frame = false;
//     }
// }

// void Player::clear_bullets() {
//     for (int i = (int)bullets.size() - 1; i >= 0; --i) {
//         if (bullets[i]->is_off_screen()) {
//             delete bullets[i];
//             bullets.erase(bullets.begin() + i);
//         }
//     }
// }

// void Player::cooldown(uint32_t time) {
//     if (fire_cooldown > 0) {
//         if (time - timer > 5) {
//             fire_cooldown -= 17;
//             timer = time;
//         }

//     }
// }

// void Player::draw(uint32_t time) {
//     // draw bullets
//     for (auto b : bullets) {
//         b->draw(time);
//     }

//     // draw particles
//     for (auto p : particle_gen.particles) {
//         screen.pen = Pen(255, 200, 100);
//         screen.pixel(Point(p->pos));
//     }

//     // draw the ship
//     screen.sprites = Surface::load(asset_ships);
//     if (velocity.x > 0 && position.x < 119) {
//         screen.sprite(ship_sprite_index+1, position);
//     }
//     else if (velocity.x < 0 && position.x > 0) {
//         screen.sprite(ship_sprite_index-1, position);
//     }
//     else {
//         screen.sprite(ship_sprite_index, position);
//     }
// }

// void Player::update(uint32_t time) {
//     uint32_t dt = time - last_update_time;
//     move();
//     fire();
//     cooldown(time);
//     clear_bullets();
    
//     for (auto b : bullets) {
//         b->update(dt);
//     }
//     particle_gen.update(time, position, velocity);

//     last_update_time = time;
// }
