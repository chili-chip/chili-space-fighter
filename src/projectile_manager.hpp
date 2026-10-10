#pragma once

#include "common.hpp"

enum class Faction : uint8_t { PLAYER, ENEMY };

struct Projectile {
    Vec2 position;
    Vec2 velocity;       // pixels per ms
    uint8_t sprite_index = 0;
    uint8_t damage = 1;
    float radius = 1.5f;
    Faction owner = Faction::PLAYER;
    bool active = false;

    bool is_off_screen() const;
};

// Fixed pool of bullets for the player and the enemies.
class ProjectileManager {
public:
    static constexpr int MAX_PROJECTILES = 160;
    Projectile pool[MAX_PROJECTILES];

    static ProjectileManager &get_instance() {
        static ProjectileManager instance;
        return instance;
    }
    ProjectileManager(const ProjectileManager &) = delete;
    void operator=(const ProjectileManager &) = delete;

    // angle 0 = straight up, positive = clockwise
    void fire(Vec2 pos, float speed, float angle, int sprite_idx, Faction faction, uint8_t damage = 1, float radius = 1.5f);
    // fire towards a target point
    void fire_at(Vec2 pos, Vec2 target, float speed, int sprite_idx, float spread = 0.0f);

    void clear();
    void clear_enemy_bullets(bool sparkle);
    void update(uint32_t dt);
    void draw();

private:
    ProjectileManager() {}
};
