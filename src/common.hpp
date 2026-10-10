#pragma once

#include "32blit.hpp"
#include "assets.hpp"
#include <cstdint>
#include <cmath>
#include <algorithm>

using namespace blit;

// Screen and playfield
constexpr int SCREEN_W = 128;
constexpr int SCREEN_H = 128;
constexpr int HUD_H = 8;              // top status bar
constexpr float PI_F = 3.14159265f;

// Sprite sheets, loaded once in init()
struct Sheets {
    Surface *ships = nullptr;        // 10x10 grid of 8x8 ships, bosses are 16x16
    Surface *projectiles = nullptr;  // 6x10 grid of 8x8 bullets
    Surface *misc = nullptr;         // 13x8 grid: pickups, flames, explosions
    Surface *logo = nullptr;         // title logo
};
extern Sheets sheets;

// Small helpers
inline float frand() { return (float)(blit::random() & 0xffff) / 65535.0f; }
inline float frand(float lo, float hi) { return lo + (hi - lo) * frand(); }
inline int irand(int lo, int hi) { return lo + (int)(blit::random() % (uint32_t)(hi - lo + 1)); }
inline float clampf(float v, float lo, float hi) { return v < lo ? lo : (v > hi ? hi : v); }
inline float lerpf(float a, float b, float t) { return a + (b - a) * t; }

// Draw an 8x8 (or larger) sprite centred on pos, offset by the current screen shake
void draw_sprite(Surface *sheet, int index, Vec2 pos, Size span = Size(1, 1), uint8_t transform = 0);
// Same, but as a solid white silhouette (hit flash)
void draw_sprite_flash(Surface *sheet, int index, Vec2 pos, Size span = Size(1, 1), Pen pen = Pen(255, 255, 255));
// Text with a 1px dark shadow so it reads on top of the starfield
void shadow_text(std::string_view s, const Font &font, Point p, Pen pen, TextAlign align = TextAlign::top_left);
// HSV-ish rainbow colour for animated text
Pen rainbow(float t);
