#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "32blit.hpp"
#include "assets.hpp"

#define STARTING_HEALTH 255
#define STARTING_SPEED 100
#define STARTING_MANEUVRABILITY 1.5f
#define STARTING_POSITION_X 64
#define STARTING_POSITION_Y 64

#define STARTING_SHIP_SPRITE_INDEX 1

using namespace blit;

class Player {
private:
    Vec2 position_vector;
    Point screen_coords;
    Vec2 movement_vector;
    uint8_t speed;
    float maneuvrability;
    uint8_t health;
    int ship_sprite_index;

public:
    Player();
    ~Player();

    void move();
    void fire();

    void update(uint32_t time);
    void draw(uint32_t time);


};
#endif // PLAYER_HPP