#include "game.hpp"

using namespace blit;

Player player;

Enemy enemies[1] = {
    Enemy({1.0f, 0.05f, 100, 7, 0, MovementPattern::Circle}),
};

void init() {
    set_screen_mode(ScreenMode::hires);
    player = Player();
}

void render(uint32_t time) {
    screen.pen = Pen(0, 0, 0);
    screen.clear();
    player.draw();
    for (auto &enemy : enemies) {
        enemy.draw();
    }
}

void update(uint32_t time) {
    player.update(time);
    for (auto &enemy : enemies) {
        enemy.update(time);
    }
}