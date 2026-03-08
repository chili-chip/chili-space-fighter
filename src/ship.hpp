#pragma once

#include "32blit.hpp"
#include "assets.hpp"

using namespace blit;
using namespace std;

struct InputState {
    bool up, down, left, right, a, b, x, y;
};
typedef struct InputState InputState;

class Projectile {
private:
    Vec2 position;
    Vec2 velocity;

    float speed;
    int damage;

    int sprite_index;

public:
    Projectile();
    ~Projectile();

    bool is_off_screen();

    void fire(Vec2 position, float angle);

    void update(uint32_t dt);
    void draw(uint32_t time);
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

    int projectile_index;
    int fire_cooldown;
    bool fired_this_frame;

public:
    virtual InputState get_input() = 0;
    void move(InputState input, uint32_t dt);
    void rotate(float direction);
    void fire(InputState input);

    void update(uint32_t dt);
    void draw(uint32_t time);
};