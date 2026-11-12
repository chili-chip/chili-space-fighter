#pragma once

#include "common.hpp"

class Player {
public:
    static constexpr int MAX_HULL = 3;
    static constexpr int MAX_WEAPON = 4;
    static constexpr uint32_t RESPAWN_MS = 1800;

    Vec2 position;
    Vec2 velocity;
    int hull = MAX_HULL;
    int lives = 3;
    int weapon = 1;
    int32_t rapid_ms = 0;
    int32_t shield_ms = 0;
    int32_t invuln_ms = 0;
    int32_t respawn_ms = 0;  // > 0 while waiting to respawn after losing a life
    int32_t fire_cooldown = 0;
    uint8_t color = 0;       // ship colour variant 0..4
    bool alive = true;
    bool controls_enabled = true;
    bool autopilot = false;  // flies itself to autopilot_to and holds fire, while a scene plays
    Vec2 autopilot_to = Vec2(SCREEN_W / 2, SCREEN_H / 2);

    void reset(uint8_t ship_color);
    void update(uint32_t dt);
    void draw(uint32_t time);

    // returns true if the hit cost the last life
    bool hit();
    bool can_be_hit() const { return alive && invuln_ms <= 0; }
    bool shielded() const { return shield_ms > 0; }
    Rect get_hitbox() const { return Rect(position.x - 2, position.y - 2, 5, 5); }

private:
    void fire();
    void spawn();
    uint32_t anim_time = 0;
};

// A friendly fighter on the player's wing (Juno's Sunset in chapter 2). It can't be hit, and fires
// when the player fires.
class Wingman {
public:
    Vec2 position;
    bool active = false;

    void join();   // flies in from below
    void leave();  // flies off the top of the screen
    void update(uint32_t dt, const Player &player);
    void draw(uint32_t time);

private:
    Vec2 velocity;
    float side = 1;      // which wing, -1 left or 1 right of the player
    bool leaving = false;
    int32_t fire_cooldown = 0;
    uint32_t anim_time = 0;
};

// The engine flame under a ship: 2 px wide, flickering, longer when thrusting forward
void draw_flame(Vec2 ship, float tilt, bool thrust, uint32_t time);
