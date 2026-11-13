#pragma once

#include "common.hpp"

enum class EnemyKind : uint8_t { Drone, Bug, Wasp, Crab, Tank, COUNT };
// Drop is only for drones launched by a boss, it is never picked for a wave
enum class MovementPattern : uint8_t { StraightDown, ZigZag, Swoop, Loop, Sweep, Hover, FigureEight, VDive, Strafe, Drop };
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
    Vec2 origin;          // spawn x / slot position, depends on pattern (VDive: y is the place in the V)
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
    float dive_x = 0;     // Hover and VDive: x target when diving
    bool holding = false; // Strafe: true while sliding, it only fires when it stops
};

// What a boss can do. Each boss has its own list and goes round it.
enum class BossAttack : uint8_t { Fan, Burst, Spiral, Ring, Rain, Beam, Drones };

// Big ship at the end of every chapter (every tenth wave).
class Boss {
public:
    static constexpr int32_t BEAM_CHARGE_MS = 750;  // the warning line before the beam
    static constexpr int32_t BEAM_FIRE_MS = 950;
    static constexpr int32_t DEATH_MS = 1800;

    Vec2 position;
    int health = 0, max_health = 1;
    int level = 1;        // 1 for the first boss, 2 for the second...
    uint32_t age = 0;
    int32_t attack_timer = 0;
    int32_t flash_ms = 0;
    int32_t dying_ms = 0;
    int32_t beam_ms = 0;  // counts down through the charge, then the beam; 0 when there is no beam
    uint8_t attack = 0;   // index into this boss's attack list
    uint8_t shots_left = 0;
    uint8_t launches = 0; // drones it has launched that the world has not spawned yet
    float spiral_angle = 0;
    float move_phase = 0; // where it is in its side to side swing; it holds still while it fires a beam
    float pace_scale = 1; // the story mode difficulty, scales its pace
    int32_t dash_ms = 0;  // > 0 during a dash: a short warning, then a fast slide to dash_to
    float dash_from = 0, dash_to = 0;
    bool active = false;
    bool dying = false;

    void spawn(int level);
    void update(uint32_t dt, Vec2 player_pos, bool player_alive);
    void draw(uint32_t time);
    bool take_damage(int amount);  // true when it starts dying
    bool finished() const { return dying && dying_ms <= 0; }
    bool entering() const { return age < 2200; }
    int points() const { return 2500 * level; }
    int sprite_index() const;
    // true while the beam is firing and touches r
    bool beam_hits(const Rect &r) const;
    bool has_attack(BossAttack a) const;

    Rect get_hitbox() const { return Rect(position.x - 7, position.y - 6, 14, 12); }

private:
    bool beam_firing() const { return beam_ms > 0 && beam_ms <= BEAM_FIRE_MS; }
    bool dash_warning() const;
    void start_dash(Vec2 player_pos);
    void update_dash(uint32_t dt);
    void start_attack(BossAttack a, bool enraged);
    void fire(BossAttack a, Vec2 player_pos, bool enraged);
};
