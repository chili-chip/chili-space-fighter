#include "starfield.hpp"

namespace {
    // pixels per ms for each layer, far to near
    const float LAYER_SPEED[3] = {0.008f, 0.020f, 0.045f};

    enum Kind : uint8_t { NONE, ROCK, RING, CRYSTAL, CLOUD };
    enum Beacons : uint8_t { NO_BEACONS, LANE, GATE, ALARM };

    struct Theme {
        Pen sky;
        Pen star[3];      // far to near
        Kind prop;
        uint8_t props;    // how many, up to PROP_COUNT
        Beacons beacons;
    };

    const Theme THEMES[7] = {
        // endless mode and the menus
        {Pen(4, 2, 18), {Pen(70, 70, 110), Pen(140, 140, 190), Pen(240, 240, 255)}, NONE, 0, NO_BEACONS},
        // 1 Halo Station: a clean blue sky and the lights of the Lanes
        {Pen(2, 6, 26), {Pen(60, 80, 130), Pen(130, 160, 220), Pen(230, 240, 255)}, NONE, 0, LANE},
        // 2 the Tollgate: warm haze, gate rings, amber warning lights
        {Pen(16, 8, 6), {Pen(100, 70, 50), Pen(180, 140, 100), Pen(255, 230, 200)}, RING, 2, GATE},
        // 3 the Rust Ring: dust and rocks
        {Pen(16, 7, 4), {Pen(90, 60, 40), Pen(160, 110, 80), Pen(240, 210, 180)}, ROCK, 5, NO_BEACONS},
        // 4 the Drift: purple gas clouds
        {Pen(10, 4, 20), {Pen(80, 60, 120), Pen(150, 120, 200), Pen(240, 220, 255)}, CLOUD, 4, NO_BEACONS},
        // 5 Glimmer: pink sky and crystals
        {Pen(14, 3, 20), {Pen(120, 60, 140), Pen(255, 130, 220), Pen(200, 255, 250)}, CRYSTAL, 6, NO_BEACONS},
        // 6 Halo Station in lockdown: red alarm lights and the shut gates (the sky stays dark so red bullets show)
        {Pen(10, 6, 18), {Pen(90, 70, 90), Pen(170, 140, 160), Pen(255, 235, 240)}, RING, 2, ALARM},
    };

    // sprites in assets/misc.png
    constexpr int ROCK_SMALL = 40, ROCK_BIG = 28, RING_OPEN = 63, RING_SHUT = 61;
    const int CRYSTAL_FRAMES[3] = {84, 85, 86};
}

void Starfield::init() {
    for (int i = 0; i < STAR_COUNT; i++) {
        stars[i].x = frand(0, SCREEN_W);
        stars[i].y = frand(0, SCREEN_H);
        // more far stars than near ones
        stars[i].layer = i < STAR_COUNT / 2 ? 0 : (i < STAR_COUNT * 5 / 6 ? 1 : 2);
    }
    set_theme(theme);
}

void Starfield::set_theme(int chapter) {
    theme = std::max(0, std::min(chapter, 6));
    for (auto &p : props) place(p, true);
}

void Starfield::place(Prop &p, bool anywhere) {
    p.kind = THEMES[theme].prop;
    p.seed = (uint8_t)irand(0, 255);
    p.x = frand(8, SCREEN_W - 8);
    p.y = anywhere ? frand(-20, SCREEN_H) : -20 - frand(0, 60);
    switch (p.kind) {
        case ROCK: p.speed = p.seed % 3 == 0 ? 0.022f : frand(0.03f, 0.06f); break;
        case RING: p.speed = 0.028f; p.y = anywhere ? frand(-20, SCREEN_H) : -20 - frand(0, 120); break;
        case CLOUD: p.speed = frand(0.010f, 0.016f); break;
        default: p.speed = frand(0.02f, 0.04f); break;
    }
}

void Starfield::update(uint32_t dt) {
    time += dt;
    for (auto &s : stars) {
        s.y += LAYER_SPEED[s.layer] * speed * dt;
        if (s.y >= SCREEN_H) {
            s.y -= SCREEN_H + 4;
            s.x = frand(0, SCREEN_W);
        }
    }
    for (int i = 0; i < THEMES[theme].props; i++) {
        Prop &p = props[i];
        p.y += p.speed * speed * dt;
        if (p.y > SCREEN_H + 24) place(p, false);
    }
    beacon_y += LAYER_SPEED[2] * speed * dt;
}

void Starfield::draw_prop(const Prop &p) {
    Vec2 pos(p.x, p.y);
    switch (p.kind) {
        case ROCK:
            if (p.seed % 3 == 0) draw_sprite(sheets.misc, ROCK_BIG, pos, Size(2, 2), p.seed & 3);
            else draw_sprite(sheets.misc, ROCK_SMALL, pos, Size(1, 1), p.seed & 3);
            break;
        case RING:
            draw_sprite(sheets.misc, theme == 6 ? RING_SHUT : RING_OPEN, pos, Size(2, 2));
            break;
        case CRYSTAL:
            draw_sprite(sheets.misc, CRYSTAL_FRAMES[(time / 160 + p.seed) % 3], pos);
            break;
        case CLOUD:
            // a few soft overlapping blobs
            for (int i = 0; i < 3; i++) {
                int r = 9 + (p.seed >> (i * 2) & 3) * 2;
                Point c = Point(pos) + Point((i - 1) * 9, ((p.seed >> i) & 1) * 5 - i * 2);
                screen.pen = i == 1 ? Pen(120, 50, 150, 34) : Pen(90, 40, 140, 30);
                screen.circle(c, r);
            }
            break;
        default:
            break;
    }
}

void Starfield::draw_beacons(Pen on, Pen off, int gap, bool alarm) {
    int scroll = (int)beacon_y % gap;
    for (int y = scroll - gap, i = 0; y < SCREEN_H; y += gap, i++) {
        bool lit = alarm ? (time / 250) % 2 : ((i + (int)(beacon_y / gap) + time / 400) % 3) == 0;
        screen.pen = lit ? on : off;
        screen.rectangle(Rect(1, y, 2, 2));
        screen.rectangle(Rect(SCREEN_W - 3, y, 2, 2));
    }
}

void Starfield::draw() {
    const Theme &t = THEMES[theme];
    screen.pen = t.sky;
    screen.clear();

    for (auto &s : stars) {
        screen.pen = t.star[s.layer];
        Point p(s.x, s.y);
        int streak = (int)(LAYER_SPEED[s.layer] * speed * 40.0f);
        if (streak > 1) {
            screen.line(p, p - Point(0, streak));
        } else {
            screen.pixel(p);
        }
    }

    // props are scenery, dimmed so they never read as something to shoot or dodge
    screen.alpha = 150;
    for (int i = 0; i < t.props; i++) draw_prop(props[i]);
    screen.alpha = 255;

    switch (t.beacons) {
        case LANE: draw_beacons(Pen(120, 220, 255), Pen(30, 50, 90), 24, false); break;
        case GATE: draw_beacons(Pen(255, 170, 40), Pen(70, 40, 10), 20, false); break;
        case ALARM: draw_beacons(Pen(255, 40, 40), Pen(80, 10, 15), 16, true); break;
        default: break;
    }
}
