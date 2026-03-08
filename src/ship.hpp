#pragma once

#include "32blit.hpp"
#include "assets.hpp"

using namespace blit;
using namespace std;

struct ShipControls{
    Vec2 movement_vector;
    float rotation_direction;
    bool fire;
};
typedef ShipControls ShipControls;

class Projectile {
private:
    Vec2 position;
    Vec2 velocity;

    float speed;
    int damage;

    int sprite_index;

public:
    Projectile();
    Projectile(int sprite_index);

    bool is_off_screen();

    void fire(Vec2 position, float angle);

    void update(uint32_t dt);
    void draw();

    void set_sprite_index(int index) {
        sprite_index = index;
    }
};

class Ship {
protected:
    Vec2 position;
    Vec2 velocity;
    float angle;

    float speed;
    float agility;
    int health;

    Projectile projectiles[10];

    int sprite_index;

    uint32_t last_update_time;
    uint32_t dt;

    int projectile_index;
    int fire_cooldown;

public:
    virtual ShipControls get_ship_controls() = 0;
    void move(Vec2 movement_vector);
    void rotate(float direction);
    void fire();

    void update(uint32_t time);
    void draw();
};