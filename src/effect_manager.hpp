#pragma once
#include "32blit.hpp"
#include "assets.hpp"

using namespace blit;

struct Effect {
    const Vec2* parent_position = nullptr; // Pointer to the ship's position
    Vec2 static_position;                  // Fallback if no parent
    float frame = 0;
    float anim_speed = 0.1f;
    int start_sprite = 0;
    int total_frames = 5;
    bool active = false;

    void update(uint32_t dt);
    void draw();
};

class EffectManager {
private:
    EffectManager(); // Private constructor for Singleton
    static const int MAX_EFFECTS = 20;
    Effect pool[MAX_EFFECTS];

public:
    static EffectManager& get_instance();

    // Prevent copying
    EffectManager(const EffectManager&) = delete;
    void operator=(const EffectManager&) = delete;

    void spawn_explosion(const Vec2* pos_ptr, int sprite_idx, int frames, float speed = 0.2f);
    void update(uint32_t dt);
    void draw();
    bool is_effect_active_on(const Vec2* pos_ptr);
};