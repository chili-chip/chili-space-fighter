#include "game.hpp"

uint32_t last_update_time = 0;
Player player;

std::vector<Ship*> enemies;

void init() {
    set_screen_mode(ScreenMode::hires);
    enemies.push_back(new Enemy({1.0f, 0.05f, 100, 7, 0, MovementPattern::Circle}));
}

void render(uint32_t time) {
    screen.pen = Pen(0, 0, 0);
    screen.clear();
    player.draw();
    for (auto &enemy : enemies) {
        enemy->draw();
    }
    ProjectileManager::get_instance().draw();
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
}