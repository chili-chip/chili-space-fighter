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
