#pragma once

#include "32blit.hpp"
#include "assets.hpp"
#include "engine_particle_generator.hpp"
#include <vector>

using namespace blit;

class PlayerBullet {
private:
    Point position_coords;
    Vec2 movement_vector;
    float speed;

public:
    PlayerBullet(Point position_coords);
    ~PlayerBullet();

    bool is_off_screen();

    void update(uint32_t dt);
    void draw(uint32_t time);
};

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
    std::vector<PlayerBullet*> bullets;
    uint8_t fire_cooldown;
    uint32_t timer;
    uint32_t last_update_time;
    bool fired_this_frame;


public:
    Player();
    ~Player();

    void move();
    void fire();
    void clear_bullets();
    void cooldown(uint32_t time);

    void update(uint32_t time);
    
    void draw_cooldown_bar();
    void draw(uint32_t time);


};
