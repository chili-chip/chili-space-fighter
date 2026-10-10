#include "starfield.hpp"

namespace {
    // pixels per ms for each layer, far to near
    const float LAYER_SPEED[3] = {0.008f, 0.020f, 0.045f};
    const Pen LAYER_PEN[3] = {Pen(70, 70, 110), Pen(140, 140, 190), Pen(240, 240, 255)};
}

void Starfield::init() {
    for (int i = 0; i < STAR_COUNT; i++) {
        stars[i].x = frand(0, SCREEN_W);
        stars[i].y = frand(0, SCREEN_H);
        // more far stars than near ones
        stars[i].layer = i < STAR_COUNT / 2 ? 0 : (i < STAR_COUNT * 5 / 6 ? 1 : 2);
    }
}

void Starfield::update(uint32_t dt) {
    for (auto &s : stars) {
        s.y += LAYER_SPEED[s.layer] * speed * dt;
        if (s.y >= SCREEN_H) {
            s.y -= SCREEN_H + 4;
            s.x = frand(0, SCREEN_W);
        }
    }
}

void Starfield::draw() {
    screen.pen = Pen(4, 2, 18);
    screen.clear();

    for (auto &s : stars) {
        screen.pen = LAYER_PEN[s.layer];
        Point p(s.x, s.y);
        int streak = (int)(LAYER_SPEED[s.layer] * speed * 40.0f);
        if (streak > 1) {
            screen.line(p, p - Point(0, streak));
        } else {
            screen.pixel(p);
        }
    }
}
