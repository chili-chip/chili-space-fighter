#include "fx.hpp"
#include <cstring>

namespace fx {

namespace {
    struct Explosion {
        Vec2 pos;
        Boom kind;
        int32_t delay = 0;
        float frame = 0;
        bool active = false;
    };

    struct Particle {
        Vec2 pos, vel;
        Pen color;
        uint32_t age = 0, life = 0;
        bool active = false;
    };

    struct Popup {
        Vec2 pos;
        char text[12];
        Pen color;
        uint32_t age = 0;
        bool active = false;
    };

    constexpr int MAX_EXPLOSIONS = 24;
    constexpr int MAX_PARTICLES = 160;
    constexpr int MAX_POPUPS = 8;
    constexpr uint32_t POPUP_MS = 800;

    Explosion explosions[MAX_EXPLOSIONS];
    Particle parts[MAX_PARTICLES];
    Popup popups[MAX_POPUPS];
    int next_particle = 0;
    float shake_amount = 0;
    Point shake_offset;
}

void clear() {
    for (auto &e : explosions) e.active = false;
    for (auto &p : parts) p.active = false;
    for (auto &p : popups) p.active = false;
    shake_amount = 0;
    shake_offset = Point(0, 0);
}

void explosion(Vec2 pos, Boom kind, uint32_t delay_ms) {
    for (auto &e : explosions) {
        if (!e.active) {
            e = {pos, kind, (int32_t)delay_ms, 0, true};
            return;
        }
    }
}

void particles(Vec2 pos, Pen color, int count, float speed, uint32_t life_ms) {
    for (int i = 0; i < count; i++) {
        Particle &p = parts[next_particle];
        next_particle = (next_particle + 1) % MAX_PARTICLES;
        float a = frand(0, 2 * PI_F);
        float s = frand(0.3f, 1.0f) * speed;
        p.pos = pos;
        p.vel = Vec2(cosf(a) * s, sinf(a) * s);
        p.color = color;
        p.age = 0;
        p.life = life_ms / 2 + (uint32_t)irand(0, (int)life_ms / 2);
        p.active = true;
    }
}

void popup(Vec2 pos, const char *text, Pen color) {
    // reuse the oldest slot if all are busy
    Popup *slot = &popups[0];
    for (auto &p : popups) {
        if (!p.active) { slot = &p; break; }
        if (p.age > slot->age) slot = &p;
    }
    slot->pos = pos;
    strncpy(slot->text, text, sizeof(slot->text) - 1);
    slot->text[sizeof(slot->text) - 1] = 0;
    slot->color = color;
    slot->age = 0;
    slot->active = true;
}

void shake(float amount) {
    if (amount > shake_amount) shake_amount = amount;
}

Point offset() { return shake_offset; }

void update(uint32_t dt) {
    for (auto &e : explosions) {
        if (!e.active) continue;
        if (e.delay > 0) {
            e.delay -= dt;
            if (e.delay <= 0) {
                // the moment a delayed blast goes off
                particles(e.pos, e.kind == Boom::Blue ? Pen(120, 220, 255) : Pen(255, 180, 60), 6, 0.05f);
            }
            continue;
        }
        float speed = e.kind == Boom::Big ? 0.010f : 0.014f;
        e.frame += speed * dt;
        if (e.frame >= 4) e.active = false;
    }

    for (auto &p : parts) {
        if (!p.active) continue;
        p.age += dt;
        if (p.age >= p.life) { p.active = false; continue; }
        p.pos += p.vel * (float)dt;
        p.vel *= 0.985f;
    }

    for (auto &p : popups) {
        if (!p.active) continue;
        p.age += dt;
        p.pos.y -= 0.012f * dt;
        if (p.age >= POPUP_MS) p.active = false;
    }

    if (shake_amount > 0.1f) {
        shake_offset = Point((int)roundf(frand(-1, 1) * shake_amount), (int)roundf(frand(-1, 1) * shake_amount));
        shake_amount *= powf(0.992f, (float)dt);
    } else {
        shake_amount = 0;
        shake_offset = Point(0, 0);
    }
}

void draw() {
    for (auto &p : parts) {
        if (!p.active) continue;
        float t = (float)p.age / p.life;
        Pen c = p.color;
        c.a = (uint8_t)(255 * (1.0f - t));
        screen.pen = c;
        screen.pixel(Point(p.pos) + shake_offset);
    }

    for (auto &e : explosions) {
        if (!e.active || e.delay > 0) continue;
        int f = (int)e.frame;
        switch (e.kind) {
            case Boom::Small: draw_sprite(sheets.misc, 87 + f, e.pos); break;
            case Boom::Blue:  draw_sprite(sheets.misc, 100 + f, e.pos); break;
            case Boom::Spark: draw_sprite(sheets.misc, 83 + f, e.pos); break;
            case Boom::Big: {
                // 2x scaled orange blast
                screen.sprites = sheets.misc;
                Point p = Point(e.pos) - Point(8, 8) + shake_offset;
                screen.sprite(87 + f, p, Point(0, 0), 2.0f);
                break;
            }
        }
    }

    for (auto &p : popups) {
        if (!p.active) continue;
        Pen c = p.color;
        if (p.age > POPUP_MS / 2) c.a = (uint8_t)(255 * (1.0f - (float)(p.age - POPUP_MS / 2) / (POPUP_MS / 2)));
        screen.pen = c;
        screen.text(p.text, minimal_font, Point(p.pos) + shake_offset, true, TextAlign::center_center);
    }
}

}
