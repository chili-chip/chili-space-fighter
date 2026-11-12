#include "common.hpp"
#include "fx.hpp"

Sheets sheets;

void draw_sprite(Surface *sheet, int index, Vec2 pos, Size span, uint8_t transform) {
    screen.sprites = sheet;
    int cols = sheet->bounds.w / 8;
    Rect src((index % cols), (index / cols), span.w, span.h);
    Point p = Point(pos) - Point(span.w * 4, span.h * 4) + fx::offset();
    screen.sprite(src, p, transform);
}

void draw_sprite_flash(Surface *sheet, int index, Vec2 pos, Size span, Pen pen) {
    int cols = sheet->bounds.w / 8;
    Point src((index % cols) * 8, (index / cols) * 8);
    Point p = Point(pos) - Point(span.w * 4, span.h * 4) + fx::offset();
    screen.pen = pen;
    for (int y = 0; y < span.h * 8; y++) {
        for (int x = 0; x < span.w * 8; x++) {
            if (sheet->get_pixel(src + Point(x, y)).a > 64) {
                screen.pixel(p + Point(x, y));
            }
        }
    }
}

void draw_sprite_ramp(Surface *sheet, int index, Vec2 pos, const Pen ramp[3]) {
    int cols = sheet->bounds.w / 8;
    Point src((index % cols) * 8, (index / cols) * 8);
    Point p = Point(pos) - Point(4, 4) + fx::offset();
    for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 8; x++) {
            Pen c = sheet->get_pixel(src + Point(x, y));
            if (c.a <= 64) continue;
            int light = c.r + c.g + c.b;
            screen.pen = ramp[light > 600 ? 2 : (light > 380 ? 1 : 0)];
            screen.pixel(p + Point(x, y));
        }
    }
}

void shadow_text(std::string_view s, const Font &font, Point p, Pen pen, TextAlign align) {
    screen.pen = Pen(0, 0, 0, 200);
    screen.text(s, font, p + Point(1, 1), true, align);
    screen.pen = pen;
    screen.text(s, font, p, true, align);
}

Pen rainbow(float t) {
    float r = 0.5f + 0.5f * sinf(t);
    float g = 0.5f + 0.5f * sinf(t + 2.094f);
    float b = 0.5f + 0.5f * sinf(t + 4.188f);
    return Pen((int)(80 + 175 * r), (int)(80 + 175 * g), (int)(80 + 175 * b));
}
