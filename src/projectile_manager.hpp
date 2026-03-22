#pragma once
#include "32blit.hpp"
#include <vector>
#include "assets.hpp"

using namespace blit;

class Ship; 
enum Faction { PLAYER, ENEMY };

class Projectile {
public:
    Vec2 position;
    Vec2 velocity;
    int sprite_index = 0;
    bool active = false;
    Faction owner;

    void update(uint32_t dt);
    void draw();
    bool is_off_screen();
    Rect get_hitbox() {
        return Rect(position.x+1, position.y+4, 8, 1);
    }
};

class ProjectileManager {
private:
    ProjectileManager() {}
    static const int MAX_PROJECTILES = 100;
    Projectile pool[MAX_PROJECTILES];

public:
    static ProjectileManager& get_instance() {
        static ProjectileManager instance;
        return instance;
    }

    ProjectileManager(const ProjectileManager&) = delete;
    void operator=(const ProjectileManager&) = delete;

    void fire(Vec2 pos, float projectile_speed, float angle, int sprite_idx, Faction faction);

    void update(uint32_t dt);
    void draw();
    void check_collisions(Ship& player, std::vector<Ship*>& enemies);
};