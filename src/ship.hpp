#pragma once
#include "32blit.hpp"
#include "projectile_manager.hpp"
#include "assets.hpp"

using namespace blit;

struct ShipControls {
    Vec2 movement_vector;
    float rotation_direction;
    bool fire;
};

class Ship {
protected:
    Vec2 position;
    Vec2 velocity;
    float angle = 0.0f;
    float speed = 0.0f;
    float agility = 0.2f;
    int health = 100;
    int sprite_index = 0;
    int projectile_sprite_idx = 0;
    int fire_cooldown = 0;
    const int COOLDOWN_MAX = 250;

public:
    virtual ShipControls get_ship_controls() = 0;
    virtual void move(Vec2 movement_vector, uint32_t dt);
    virtual void rotate(float direction);
    virtual void fire();

    virtual void update(uint32_t dt);
    virtual void draw();
};