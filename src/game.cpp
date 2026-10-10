// Ultra Hyper Space Super Fighters TURBO
// Title screen, menus and the state machine around a run of the game (see world.cpp).

#include "game.hpp"
#include "common.hpp"
#include "audio.hpp"
#include "fx.hpp"
#include "save.hpp"
#include "starfield.hpp"
#include "world.hpp"
#include "powerup.hpp"
#include "projectile_manager.hpp"
#include <cstdio>
#include <cstring>

namespace {

enum class State { Title, Options, HighScores, HowTo, Credits, Playing, Paused, GameOver };

const char *const VERSION = "v1.0.0";
const char *const SHIP_NAMES[5] = {"SILVER", "SUNSET", "VIPER", "NEBULA", "CRIMSON"};
const int SHIP_SPRITES[5] = {1, 11, 21, 31, 41};

Starfield stars;
World world;
State state = State::Title;
uint32_t last_time = 0;
uint32_t state_time = 0;     // ms since entering the current state
int32_t fade_ms = 0;
int cursor = 0;

// game over results
int final_rank = -1;
bool entering_name = false;
char entry_name[4] = "AAA";
int entry_pos = 0;
uint32_t final_score = 0;
int final_wave = 0;

constexpr int32_t FADE_MS = 220;

// true only on the update the button went down (blit::pressed() is true for as long as it is held)
bool just_pressed(uint32_t b) { return buttons.pressed & b; }

void go(State s, int new_cursor = 0) {
    state = s;
    state_time = 0;
    cursor = new_cursor;
    fade_ms = FADE_MS;
}


void apply_settings() {
    audio::set_music_enabled(save_data.music);
    audio::set_sound_enabled(save_data.sound);
}

// --------------------------------------------------------------------------
// Menu helpers
// --------------------------------------------------------------------------

// Up/down navigation with wrap; returns true when the cursor moved
bool menu_nav(int count) {
    int old = cursor;
    if (just_pressed(Button::DPAD_UP)) cursor = (cursor + count - 1) % count;
    if (just_pressed(Button::DPAD_DOWN)) cursor = (cursor + 1) % count;
    if (cursor != old) {
        audio::play(audio::Sfx::MenuMove);
        return true;
    }
    return false;
}

void draw_menu(const char *const *items, int count, int y, uint32_t time) {
    for (int i = 0; i < count; i++) {
        int iy = y + i * 11;
        bool sel = i == cursor;
        Pen pen = sel ? Pen(255, 230, 120) : Pen(150, 150, 190);
        int x = SCREEN_W / 2 + (sel ? 4 : 0);
        shadow_text(items[i], minimal_font, Point(x, iy), pen, TextAlign::top_center);
        if (sel) {
            // the player's ship is the cursor, wobbling with its engine lit
            int w = screen.measure_text(items[i], minimal_font).w;
            Vec2 ship(x - w / 2 - 9 + sinf(time * 0.008f) * 1.5f, iy + 3);
            draw_sprite(sheets.misc, 5 + (time / 60) % 4, ship + Vec2(0, 7));
            draw_sprite(sheets.ships, SHIP_SPRITES[save_data.ship], ship);
        }
    }
}

void draw_panel(Rect r) {
    screen.pen = Pen(6, 4, 30, 215);
    screen.rectangle(r);
    screen.pen = Pen(90, 80, 200);
    screen.h_span(r.tl(), r.w);
    screen.h_span(Point(r.x, r.y + r.h - 1), r.w);
    screen.v_span(r.tl(), r.h);
    screen.v_span(Point(r.x + r.w - 1, r.y), r.h);
}

void draw_heading(const char *text, uint32_t time) {
    shadow_text(text, outline_font, Point(SCREEN_W / 2, 6), rainbow(time * 0.004f), TextAlign::top_center);
}

void draw_hint(const char *text, uint32_t time) {
    if ((time / 500) % 2 == 0) return;
    shadow_text(text, minimal_font, Point(SCREEN_W / 2, SCREEN_H - 10), Pen(120, 120, 160), TextAlign::top_center);
}

// --------------------------------------------------------------------------
// Title
// --------------------------------------------------------------------------

const char *const TITLE_ITEMS[] = {"PLAY", "HIGH SCORES", "OPTIONS", "HOW TO PLAY", "CREDITS"};
constexpr int TITLE_COUNT = 5;

void start_game() {
    world.start(save_data.ship, &stars);
    go(State::Playing);
}

void update_title() {
    stars.speed = lerpf(stars.speed, 1.5f, 0.02f);
    if (state_time < 400) return;
    menu_nav(TITLE_COUNT);
    if (just_pressed(Button::A)) {
        audio::play(audio::Sfx::MenuSelect);
        switch (cursor) {
            case 0: start_game(); break;
            case 1: go(State::HighScores); break;
            case 2: go(State::Options); break;
            case 3: go(State::HowTo); break;
            case 4: go(State::Credits); break;
        }
    }
}

void draw_logo(uint32_t time) {
    // drop in from the top when the title first appears
    float k = clampf(state_time / 700.0f, 0, 1);
    float drop = (1.0f - k) * (1.0f - k) * -60.0f;
    int y = (int)drop + (int)roundf(sinf(time * 0.003f) * 1.5f);

    shadow_text("ULTRA HYPER SPACE", minimal_font, Point(SCREEN_W / 2, 4 + y), Pen(130, 200, 255), TextAlign::top_center);
    shadow_text("SUPER FIGHTERS", outline_font, Point(SCREEN_W / 2, 13 + y), Pen(255, 255, 255), TextAlign::top_center);

    Surface *logo = sheets.logo;
    Point lp((SCREEN_W - logo->bounds.w) / 2, 24 + y);
    screen.blit(logo, Rect(Point(0, 0), logo->bounds), lp);

    // a glint sweeping across the logo every few seconds
    int sweep = (int)((time / 12) % 300) - 40;
    if (sweep >= 0 && sweep < logo->bounds.w + 10) {
        screen.pen = Pen(255, 255, 255, 120);
        for (int i = 0; i < logo->bounds.h; i++) {
            int gx = lp.x + sweep - i / 2;
            if (gx >= lp.x && gx < lp.x + logo->bounds.w && logo->get_pixel(Point(gx - lp.x, i)).a > 0) {
                screen.pixel(Point(gx, lp.y + i));
                if (gx + 1 < lp.x + logo->bounds.w && logo->get_pixel(Point(gx + 1 - lp.x, i)).a > 0) screen.pixel(Point(gx + 1, lp.y + i));
            }
        }
    }
}

void draw_title(uint32_t time) {
    draw_logo(time);
    if (state_time > 400) draw_menu(TITLE_ITEMS, TITLE_COUNT, 58, time);

    char buf[24];
    snprintf(buf, sizeof(buf), "HI %06lu", (unsigned long)best_score());
    shadow_text(buf, minimal_font, Point(2, SCREEN_H - 9), Pen(255, 200, 90));
    shadow_text(VERSION, minimal_font, Point(SCREEN_W - 2, SCREEN_H - 9), Pen(90, 90, 130), TextAlign::top_right);
}

// --------------------------------------------------------------------------
// Options
// --------------------------------------------------------------------------

void update_options() {
    menu_nav(4);
    bool left = just_pressed(Button::DPAD_LEFT), right = just_pressed(Button::DPAD_RIGHT), a = just_pressed(Button::A);
    bool changed = false;
    switch (cursor) {
        case 0:
            if (a || left || right) { save_data.music ^= 1; changed = true; }
            break;
        case 1:
            if (a || left || right) { save_data.sound ^= 1; changed = true; }
            break;
        case 2:
            if (left) { save_data.ship = (save_data.ship + 4) % 5; changed = true; }
            if (right || a) { save_data.ship = (save_data.ship + 1) % 5; changed = true; }
            break;
        case 3:
            if (a) {
                store_save();
                audio::play(audio::Sfx::MenuBack);
                go(State::Title, 2);
                return;
            }
            break;
    }
    if (changed) {
        apply_settings();
        audio::play(audio::Sfx::MenuSelect);
    }
    if (just_pressed(Button::B)) {
        store_save();
        audio::play(audio::Sfx::MenuBack);
        go(State::Title, 2);
    }
}

void draw_options(uint32_t time) {
    draw_heading("OPTIONS", time);
    char music[16], sound[16], ship[20];
    snprintf(music, sizeof(music), "MUSIC  %s", save_data.music ? "ON" : "OFF");
    snprintf(sound, sizeof(sound), "SOUND  %s", save_data.sound ? "ON" : "OFF");
    snprintf(ship, sizeof(ship), "< %s >", SHIP_NAMES[save_data.ship]);
    const char *items[] = {music, sound, ship, "BACK"};
    draw_menu(items, 4, 30, time);

    // ship preview
    Vec2 p(SCREEN_W / 2, 92 + sinf(time * 0.004f) * 2);
    draw_sprite(sheets.misc, 18 + (time / 60) % 4, p + Vec2(0, 7));
    int tilt = (int)((time / 700) % 4);
    int frame = SHIP_SPRITES[save_data.ship] + (tilt == 1 ? -1 : (tilt == 3 ? 1 : 0));
    draw_sprite(sheets.ships, frame, p);
    draw_hint("B BACK", time);
}

// --------------------------------------------------------------------------
// High scores, how to play, credits
// --------------------------------------------------------------------------

void draw_scores(uint32_t time, int highlight) {
    char buf[32];
    for (int i = 0; i < SaveData::TOP; i++) {
        const HighScore &h = save_data.top[i];
        int y = 30 + i * 13;
        Pen pen = i == 0 ? Pen(255, 210, 80) : Pen(200, 200, 230);
        if (i == highlight) pen = rainbow(time * 0.01f);
        snprintf(buf, sizeof(buf), "%d.", i + 1);
        shadow_text(buf, minimal_font, Point(14, y), pen);
        shadow_text(h.name, outline_font, Point(26, y - 1), pen);
        snprintf(buf, sizeof(buf), "%06lu", (unsigned long)h.score);
        shadow_text(buf, outline_font, Point(52, y - 1), pen);
        snprintf(buf, sizeof(buf), "W%d", h.wave);
        shadow_text(buf, minimal_font, Point(SCREEN_W - 14, y), Pen(140, 160, 255), TextAlign::top_right);
    }
}

void update_back_only(int title_cursor) {
    if (just_pressed(Button::A) || just_pressed(Button::B)) {
        audio::play(audio::Sfx::MenuBack);
        go(State::Title, title_cursor);
    }
}

void draw_howto(uint32_t time) {
    draw_heading("HOW TO PLAY", time);
    struct Line { const char *key, *what; };
    const Line lines[] = {{"D-PAD", "FLY"}, {"A", "FIRE (HOLD)"}, {"MENU", "PAUSE"}};
    for (int i = 0; i < 3; i++) {
        shadow_text(lines[i].key, minimal_font, Point(42, 20 + i * 9), Pen(255, 210, 90), TextAlign::top_right);
        shadow_text(lines[i].what, minimal_font, Point(48, 20 + i * 9), Pen(220, 220, 240));
    }

    shadow_text("PICK UPS", minimal_font, Point(SCREEN_W / 2, 50), Pen(140, 160, 255), TextAlign::top_center);
    for (int i = 0; i < (int)PowerUpKind::COUNT; i++) {
        int col = i % 2, row = i / 2;
        Vec2 icon(10 + col * 62, 66 + row * 13);
        draw_sprite(sheets.misc, powerup_sprite((PowerUpKind)i), icon);
        shadow_text(powerup_name((PowerUpKind)i), minimal_font, Point(icon) + Point(8, -3), Pen(220, 220, 240));
    }
    shadow_text("CHAIN KILLS FOR COMBOS", minimal_font, Point(SCREEN_W / 2, 105), Pen(255, 230, 90), TextAlign::top_center);
    draw_hint("B BACK", time);
}

void draw_credits(uint32_t time) {
    draw_heading("CREDITS", time);
    struct Line { const char *a; Pen pen; };
    const Line lines[] = {
        {"GAME", Pen(140, 160, 255)},
        {"TERIYAKIGOD", Pen(255, 255, 255)},
        {"CHILICHIP", Pen(255, 120, 90)},
        {"", Pen(0, 0, 0)},
        {"MUSIC + SOUND", Pen(140, 160, 255)},
        {"ORIGINAL CHIPTUNE", Pen(255, 255, 255)},
        {"", Pen(0, 0, 0)},
        {"BUILT WITH 32BLIT SDK", Pen(140, 160, 255)},
        {"FOR THE VGC ZERO", Pen(255, 255, 255)},
    };
    for (int i = 0; i < 9; i++) {
        shadow_text(lines[i].a, minimal_font, Point(SCREEN_W / 2, 22 + i * 9), lines[i].pen, TextAlign::top_center);
    }
    shadow_text("CHILICHIP.EU", minimal_font, Point(SCREEN_W / 2, 108), rainbow(time * 0.006f), TextAlign::top_center);
    draw_hint("B BACK", time);
}

// --------------------------------------------------------------------------
// Playing, pause, game over
// --------------------------------------------------------------------------

const char *const PAUSE_ITEMS[] = {"RESUME", "QUIT TO TITLE"};

void update_playing(uint32_t dt) {
    world.update(dt);
    if (world.phase != World::Phase::Over && (just_pressed(Button::MENU) || just_pressed(Button::Y))) {
        audio::play(audio::Sfx::Pause);
        go(State::Paused);
        fade_ms = 0;
        return;
    }
    if (world.is_over()) {
        final_score = world.score;
        final_wave = world.wave;
        final_rank = score_rank(final_score);
        entering_name = final_rank >= 0;
        entry_pos = 0;
        memcpy(entry_name, save_data.last_name, 4);
        audio::play(final_rank == 0 ? audio::Sfx::NewBest : audio::Sfx::GameOver);
        go(State::GameOver);
    }
}

void update_paused() {
    menu_nav(2);
    if (just_pressed(Button::MENU) || just_pressed(Button::Y) || just_pressed(Button::B) || (just_pressed(Button::A) && cursor == 0)) {
        audio::play(audio::Sfx::Pause);
        state = State::Playing;
        return;
    }
    if (just_pressed(Button::A) && cursor == 1) {
        audio::play(audio::Sfx::MenuBack);
        ProjectileManager::get_instance().clear();
        fx::clear();
        audio::play_song(audio::Song::Title);
        go(State::Title);
    }
}

void draw_paused(uint32_t time) {
    screen.pen = Pen(0, 0, 0, 150);
    screen.rectangle(Rect(0, 0, SCREEN_W, SCREEN_H));
    draw_panel(Rect(20, 40, 88, 48));
    shadow_text("PAUSED", outline_font, Point(SCREEN_W / 2, 46), Pen(255, 255, 255), TextAlign::top_center);
    draw_menu(PAUSE_ITEMS, 2, 62, time);
}

void update_name_entry() {
    char &c = entry_name[entry_pos];
    if (just_pressed(Button::DPAD_UP)) { c = c >= 'Z' ? 'A' : c + 1; audio::play(audio::Sfx::MenuMove); }
    if (just_pressed(Button::DPAD_DOWN)) { c = c <= 'A' ? 'Z' : c - 1; audio::play(audio::Sfx::MenuMove); }
    if (just_pressed(Button::DPAD_LEFT) || just_pressed(Button::B)) {
        if (entry_pos > 0) { entry_pos--; audio::play(audio::Sfx::MenuBack); }
    }
    if (just_pressed(Button::DPAD_RIGHT) || just_pressed(Button::A)) {
        if (entry_pos < 2) {
            entry_pos++;
            audio::play(audio::Sfx::MenuSelect);
        } else if (just_pressed(Button::A)) {
            final_rank = submit_score(final_score, (uint16_t)final_wave, entry_name);
            entering_name = false;
            state_time = 0;
            audio::play(audio::Sfx::MenuSelect);
        }
    }
}

void update_game_over() {
    if (state_time > 1800) audio::play_song(audio::Song::Title);
    if (state_time < 900) return;
    if (entering_name) {
        update_name_entry();
        return;
    }
    if (just_pressed(Button::A)) {
        audio::play(audio::Sfx::MenuSelect);
        start_game();
    } else if (just_pressed(Button::B)) {
        audio::play(audio::Sfx::MenuBack);
        go(State::Title);
    }
}

void draw_game_over(uint32_t time) {
    screen.pen = Pen(0, 0, 0, 140);
    screen.rectangle(Rect(0, 0, SCREEN_W, SCREEN_H));
    draw_panel(Rect(10, 14, 108, 100));

    shadow_text("GAME OVER", outline_font, Point(SCREEN_W / 2, 20), Pen(255, 80, 80), TextAlign::top_center);

    char buf[32];
    snprintf(buf, sizeof(buf), "%06lu", (unsigned long)final_score);
    shadow_text("SCORE", minimal_font, Point(SCREEN_W / 2, 34), Pen(140, 160, 255), TextAlign::top_center);
    shadow_text(buf, outline_font, Point(SCREEN_W / 2, 43), Pen(255, 255, 255), TextAlign::top_center);
    snprintf(buf, sizeof(buf), "REACHED WAVE %d", final_wave);
    shadow_text(buf, minimal_font, Point(SCREEN_W / 2, 55), Pen(200, 200, 230), TextAlign::top_center);

    if (final_rank == 0) {
        int bounce = (int)fabsf(sinf(time * 0.008f) * 3);
        shadow_text("NEW BEST!", outline_font, Point(SCREEN_W / 2, 68 - bounce), rainbow(time * 0.012f), TextAlign::top_center);
    } else if (final_rank > 0) {
        snprintf(buf, sizeof(buf), "HIGH SCORE #%d", final_rank + 1);
        shadow_text(buf, outline_font, Point(SCREEN_W / 2, 67), Pen(255, 210, 90), TextAlign::top_center);
    } else {
        snprintf(buf, sizeof(buf), "BEST %06lu", (unsigned long)best_score());
        shadow_text(buf, minimal_font, Point(SCREEN_W / 2, 68), Pen(150, 150, 190), TextAlign::top_center);
    }

    if (entering_name) {
        if (state_time < 900) return;
        shadow_text("ENTER YOUR NAME", minimal_font, Point(SCREEN_W / 2, 80), Pen(200, 200, 230), TextAlign::top_center);
        for (int i = 0; i < 3; i++) {
            char letter[2] = {entry_name[i], 0};
            int x = SCREEN_W / 2 - 14 + i * 14;
            bool sel = i == entry_pos;
            Pen pen = sel ? ((time / 200) % 2 ? Pen(255, 230, 120) : Pen(255, 255, 255)) : Pen(160, 160, 200);
            shadow_text(letter, outline_font, Point(x, 92), pen, TextAlign::top_center);
            screen.pen = sel ? Pen(255, 230, 120) : Pen(80, 80, 120);
            screen.h_span(Point(x - 3, 101), 7);
            if (sel) {
                // little up/down arrows around the selected letter
                screen.pixel(Point(x, 89)); screen.h_span(Point(x - 1, 90), 3);
                screen.pixel(Point(x, 105)); screen.h_span(Point(x - 1, 104), 3);
            }
        }
        return;
    }

    if (state_time > 900 && (time / 400) % 2) {
        shadow_text("A  PLAY AGAIN", minimal_font, Point(SCREEN_W / 2, 88), Pen(255, 230, 120), TextAlign::top_center);
        shadow_text("B  TITLE", minimal_font, Point(SCREEN_W / 2, 98), Pen(180, 180, 210), TextAlign::top_center);
    }
}

}  // namespace

// --------------------------------------------------------------------------
// 32blit entry points
// --------------------------------------------------------------------------

void init() {
    set_screen_mode(ScreenMode::hires);

    sheets.ships = Surface::load(asset_ships);
    sheets.projectiles = Surface::load(asset_projectiles);
    sheets.misc = Surface::load(asset_misc);
    sheets.logo = Surface::load(asset_logo);

    load_save();
    audio::init();
    apply_settings();
    stars.init();
    stars.speed = 8.0f;  // warp in

    go(State::Title);
    fade_ms = 600;
    audio::play_song(audio::Song::Title);
}

void update(uint32_t time) {
    uint32_t dt = time - last_time;
    last_time = time;
    if (dt > 50) dt = 50;  // after a hitch, don't jump

    state_time += dt;
    if (fade_ms > 0) fade_ms -= dt;
    audio::update(dt);

    if (state != State::Paused && state != State::GameOver) {
        stars.update(dt);
        fx::update(dt);
    } else if (state == State::GameOver) {
        fx::update(dt);
    }

    switch (state) {
        case State::Title:      update_title(); break;
        case State::Options:    update_options(); break;
        case State::HighScores: update_back_only(1); break;
        case State::HowTo:      update_back_only(3); break;
        case State::Credits:    update_back_only(4); break;
        case State::Playing:    update_playing(dt); break;
        case State::Paused:     update_paused(); break;
        case State::GameOver:   update_game_over(); break;
    }
}

void render(uint32_t time) {
    screen.alpha = 255;
    screen.mask = nullptr;
    stars.draw();

    switch (state) {
        case State::Title:      draw_title(time); break;
        case State::Options:    draw_options(time); break;
        case State::HighScores:
            draw_heading("HIGH SCORES", time);
            draw_scores(time, -1);
            draw_hint("B BACK", time);
            break;
        case State::HowTo:      draw_howto(time); break;
        case State::Credits:    draw_credits(time); break;
        case State::Playing:    world.draw(time); break;
        case State::Paused:     world.draw(time); draw_paused(time); break;
        case State::GameOver:   world.draw(time); draw_game_over(time); break;
    }

    if (fade_ms > 0) {
        screen.pen = Pen(0, 0, 0, (uint8_t)clampf(255.0f * fade_ms / FADE_MS, 0, 255));
        screen.rectangle(Rect(0, 0, SCREEN_W, SCREEN_H));
    }
}
