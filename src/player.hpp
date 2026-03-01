#pragma once

#include "32blit.hpp"
#include "assets.hpp"
#include "engine_particle_generator.hpp"

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
    blit::EngineParticleGenerator particle_gen;


public:
    Player();
    ~Player();

    void move();
    void fire();

    void update(uint32_t time);
    void draw(uint32_t time);


};
