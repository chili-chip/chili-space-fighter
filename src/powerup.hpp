#pragma once

#include "common.hpp"

enum class PowerUpKind : uint8_t { Weapon, Rapid, Shield, Repair, Life, Coin, COUNT };

struct PowerUp {
    PowerUpKind kind;
    Vec2 position;
    uint32_t age = 0;
    bool active = false;

    static constexpr uint32_t LIFETIME_MS = 9000;

    Rect get_hitbox() const { return Rect(position.x - 4, position.y - 4, 9, 9); }
};

int powerup_sprite(PowerUpKind kind);
const char *powerup_name(PowerUpKind kind);

class PowerUpManager {
public:
    static constexpr int MAX_POWERUPS = 8;
    PowerUp pool[MAX_POWERUPS];

    void clear();
    void spawn(Vec2 pos, PowerUpKind kind);
    // picks a kind using drop weights, avoiding useless ones
    void spawn_random(Vec2 pos, int weapon_level, int hull, int max_hull);
    void update(uint32_t dt);
    void draw(uint32_t time);
};
