#pragma once
#include "32blit.hpp"
#include <vector>
#include "assets.hpp"

using namespace blit;

class Projectile {
public:
    Vec2 position;
    Vec2 velocity;
    float speed = 0.3f;
    int sprite_index = 0;
    bool active = false;

    void update(uint32_t dt);
    void draw();
    bool is_off_screen();
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

    void fire(Vec2 pos, float angle, int sprite_idx);

    void update(uint32_t dt);
    void draw();
};