#pragma once

#include "common.hpp"

enum class EnemyKind : uint8_t { Drone, Bug, Wasp, Crab, Tank, COUNT };
enum class MovementPattern : uint8_t { StraightDown, ZigZag, Swoop, Loop, Sweep, Hover };
enum class FireStyle : uint8_t { None, Down, Aimed, Spread, Burst };

struct EnemyPreset {
    int sprite_index;
    int health;
    int points;
    FireStyle fire;
    uint32_t fire_ms;     // time between volleys
    float speed;          // base speed multiplier
    int drop_chance;      // % chance of dropping a power-up
};

const EnemyPreset &enemy_preset(EnemyKind kind);

// Small enemy following a scripted path down the screen.
class Enemy {
public:
    EnemyKind kind = EnemyKind::Drone;
    MovementPattern pattern = MovementPattern::StraightDown;
    Vec2 position;
    Vec2 origin;          // spawn x / slot position, depends on pattern
    float dir = 1;        // -1 or 1, mirrors the path
    float speed = 1;      // difficulty multiplier
    float fire_rate = 1;  // difficulty multiplier
    int health = 1;
    uint32_t age = 0;
    int32_t fire_timer = 0;
    int32_t flash_ms = 0;
    uint8_t burst_left = 0;
    bool active = false;
    bool entered = false; // has been on screen at least once

    void spawn(EnemyKind kind, MovementPattern pattern, Vec2 origin, float dir, float speed, float fire_rate, int bonus_hp);
    void update(uint32_t dt, Vec2 player_pos, bool player_alive);
    void draw();
    // returns true when this hit destroyed it
    bool take_damage(int amount);

    Rect get_hitbox() const { return Rect(position.x - 3, position.y - 3, 7, 7); }

private:
    void move(uint32_t dt, Vec2 player_pos);
    void shoot(Vec2 player_pos);
    float dive_x = 0;     // Hover: x target when diving
};

// Big ship at the end of every fifth wave.
class Boss {
public:
    Vec2 position;
    int health = 0, max_health = 1;
    int level = 1;        // 1 for the first boss, 2 for the second...
    uint32_t age = 0;
    int32_t attack_timer = 0;
    int32_t flash_ms = 0;
    int32_t dying_ms = 0;
    uint8_t attack = 0;
    uint8_t shots_left = 0;
    float spiral_angle = 0;
    bool active = false;
    bool dying = false;

    void spawn(int level);
    void update(uint32_t dt, Vec2 player_pos, bool player_alive);
    void draw();
    bool take_damage(int amount);  // true when it starts dying
    bool finished() const { return dying && dying_ms <= 0; }
    bool entering() const { return age < 2200; }
    int points() const { return 2500 * level; }
    int sprite_index() const;

    Rect get_hitbox() const { return Rect(position.x - 7, position.y - 6, 14, 12); }
};
