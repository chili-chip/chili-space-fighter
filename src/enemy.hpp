#pragma once

#include "32blit.hpp"
#include "ship.hpp"
#include "math.h"

enum class MovementPattern {
    StraightDown,
    ZigZag,
    Circle
};

struct EnemyPreset {
    float speed;
    float agility;
    int health;
    int sprite_index;
    int projectile_index;
    MovementPattern movement_pattern;
};

class Enemy : public Ship {
private:
    MovementPattern movement_pattern;

    ShipControls get_ship_controls() override;

public:
    Enemy(const EnemyPreset& preset);
};
