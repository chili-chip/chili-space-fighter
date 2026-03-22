#include "effect_manager.hpp"

Surface *misc_sprites = Surface::load(asset_misc);

void Effect::update(uint32_t dt) {
    if (!active) return;
    frame += anim_speed * (dt / 20.0f);
    if (frame >= total_frames) {
        active = false;
        parent_position = nullptr;
    }
}

void Effect::draw() {
    if (!active) return;
    Vec2 draw_pos = parent_position ? *parent_position : static_position;
    screen.sprites = misc_sprites;
    screen.sprite(start_sprite + (int)frame, draw_pos);
}

EffectManager::EffectManager() {}

EffectManager& EffectManager::get_instance() {
    static EffectManager instance;
    return instance;
}

void EffectManager::spawn_explosion(const Vec2* pos_ptr, int sprite_idx, int frames, float speed) {
    for (int i = 0; i < MAX_EFFECTS; i++) {
        if (!pool[i].active) {
            pool[i].parent_position = pos_ptr;
            pool[i].start_sprite = sprite_idx;
            pool[i].total_frames = frames;
            pool[i].anim_speed = speed;
            pool[i].frame = 0;
            pool[i].active = true;
            break;
        }
    }
}

void EffectManager::update(uint32_t dt) {
    for (int i = 0; i < MAX_EFFECTS; i++) {
        pool[i].update(dt);
    }
}

void EffectManager::draw() {
    for (int i = 0; i < MAX_EFFECTS; i++) {
        pool[i].draw();
    }
}

bool EffectManager::is_effect_active_on(const Vec2* pos_ptr) {
    for (int i = 0; i < MAX_EFFECTS; i++) {
        if (pool[i].active && pool[i].parent_position == pos_ptr) return true;
    }
    return false;
}