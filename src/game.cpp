#include "game.hpp"

using namespace blit;

Player player;

void init() {
    set_screen_mode(ScreenMode::hires);
    player = Player();
}

void render(uint32_t time) {
    screen.pen = Pen(0, 0, 0);
    screen.clear();
    player.draw(time);
}

void update(uint32_t time) {
    player.update(time);
}