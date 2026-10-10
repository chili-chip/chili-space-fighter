#pragma once

#include "common.hpp"

// Three-layer parallax starfield scrolling down the screen.
class Starfield {
public:
    void init();
    void update(uint32_t dt);
    void draw();

    // 1.0 = normal cruising speed, higher values streak the stars (warp)
    float speed = 1.0f;

private:
    static constexpr int STAR_COUNT = 56;
    struct Star { float x, y; uint8_t layer; };
    Star stars[STAR_COUNT];
};
