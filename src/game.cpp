#include "game.hpp"

using namespace blit;

uint32_t last_update_time = 0;
Player player;

Enemy enemies[1] = {
    Enemy({1.0f, 0.05f, 100, 7, 0, MovementPattern::Circle}),
};

void init() {
    set_screen_mode(ScreenMode::hires);
}

void render(uint32_t time) {
    screen.pen = Pen(0, 0, 0);
    screen.clear();
    ProjectileManager::get_instance().draw();
    player.draw();
    for (auto &enemy : enemies) {
        enemy.draw();
    }
}

void update(uint32_t time) {
    uint32_t dt = (time - last_update_time);
    last_update_time = time;
    ProjectileManager::get_instance().update(dt);
    player.update(dt);
    for (auto &enemy : enemies) {
        enemy.update(dt);
    }
}