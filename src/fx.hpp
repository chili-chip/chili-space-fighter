#pragma once

#include "common.hpp"

// Visual effects: explosions, particles, floating score text, screen shake.
namespace fx {
    enum class Boom { Small, Big, Blue, Spark };

    void clear();
    void update(uint32_t dt);
    void draw();

    // delay_ms lets a big death chain go off over time
    void explosion(Vec2 pos, Boom kind = Boom::Small, uint32_t delay_ms = 0);
    void particles(Vec2 pos, Pen color, int count, float speed, uint32_t life_ms = 400);
    // one puff of a burning hull: flames that cool from white to red and smoke, trailing down the screen
    void fire(Vec2 pos);
    void popup(Vec2 pos, const char *text, Pen color);
    void shake(float amount);

    Point offset();  // current screen shake offset
}
