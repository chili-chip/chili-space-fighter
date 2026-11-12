#pragma once

#include "common.hpp"

// Three-layer parallax starfield scrolling down the screen, with a look for each story chapter:
// lane beacons at Halo, gate rings at the Tollgate, rocks in the Rust Ring, gas in the Drift,
// crystals over Glimmer and alarm lights at Halo in lockdown. See docs/story/backgrounds.md.
class Starfield {
public:
    void init();
    void update(uint32_t dt);
    void draw();
    // 0 for endless mode and the menus, 1 to 6 for the story chapters
    void set_theme(int chapter);

    // 1.0 = normal cruising speed, higher values streak the stars (warp)
    float speed = 1.0f;

private:
    static constexpr int STAR_COUNT = 56;
    static constexpr int PROP_COUNT = 7;
    struct Star { float x, y; uint8_t layer; };
    // something bigger than a star drifting past: a rock, a ring, a crystal or a gas cloud
    struct Prop { float x, y, speed; uint8_t kind; uint8_t seed; };

    Star stars[STAR_COUNT];
    Prop props[PROP_COUNT];
    int theme = 0;
    uint32_t time = 0;
    float beacon_y = 0;

    void place(Prop &p, bool anywhere);
    void draw_prop(const Prop &p);
    void draw_beacons(Pen on, Pen off, int gap, bool alarm);
};
