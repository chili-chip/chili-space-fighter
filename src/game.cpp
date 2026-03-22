#include "game.hpp"

uint32_t last_update_time = 0;
Player player;

std::vector<Ship*> enemies;

void init() {
    set_screen_mode(ScreenMode::hires);
    enemies.push_back(new Enemy({0.05f, 20, 7, 0, MovementPattern::Circle}));
}

void render(uint32_t time) {
    screen.pen = Pen(0, 0, 0);
    screen.clear();
    player.draw();
    for (auto &enemy : enemies) {
        enemy->draw();
    }
    ProjectileManager::get_instance().draw();
    EffectManager::get_instance().draw();
}

void update(uint32_t time) {
    uint32_t dt = (time - last_update_time);
    last_update_time = time;
    player.update(dt);
    for (auto &enemy : enemies) {
        enemy->update(dt);
    }
    ProjectileManager::get_instance().update(dt);
    ProjectileManager::get_instance().check_collisions(player, enemies);

    // Erase enemies only when they are dead AND their explosion is done
    enemies.erase(
        std::remove_if(enemies.begin(), enemies.end(),
            [](Ship* s) {
                return !s->is_alive() && !EffectManager::get_instance().is_effect_active_on(s->get_pos_ptr()); 
            }),
        enemies.end()
    );

    EffectManager::get_instance().update(dt);
}