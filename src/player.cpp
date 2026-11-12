#include "player.hpp"
#include "projectile_manager.hpp"
#include "audio.hpp"
#include "fx.hpp"

namespace {
    constexpr float SPEED = 0.075f;            // pixels per ms
    constexpr float BULLET_SPEED = 0.20f;
    constexpr int32_t FIRE_MS = 150;
    constexpr int32_t RAPID_FIRE_MS = 80;
    constexpr int BULLET_SPRITE = 2;
    const int SHIP_SPRITE[5] = {1, 11, 21, 31, 41};  // centre frame, tilt frames are -1 / +1
    constexpr float WING_GAP = 14;                   // px between the player and the wingman
    const int FLAME_FRAMES[4] = {44, 45, 46, 47};    // fire on a burning ship, in assets/misc.png
}

void draw_flame(Vec2 ship, float tilt, bool thrust, uint32_t time) {
    static const Pen FLAME[4] = {Pen(255, 243, 146), Pen(255, 200, 40), Pen(255, 140, 0), Pen(227, 81, 0)};
    // the engine moves 1 px with the tilted frames
    Point p = Point(ship) + Point(tilt < -0.01f ? -2 : (tilt > 0.01f ? 0 : -1), 3) + fx::offset();
    int length = (thrust ? 4 : 2) + (int)((time / 60) % 2);
    for (int i = 0; i < length; i++) {
        screen.pen = FLAME[std::min(3, i * 4 / length + (i == length - 1 ? 1 : 0))];
        screen.h_span(p + Point(0, i), 2);
    }
}

void Player::reset(uint8_t ship_color) {
    color = ship_color;
    lives = 3;
    weapon = 1;
    rapid_ms = 0;
    shield_ms = 0;
    shields = 0;
    repair_ms = 0;
    controls_enabled = true;
    position = Vec2(SCREEN_W / 2, SCREEN_H - 14);
    velocity = Vec2(0, 0);
    hull = MAX_HULL;
    invuln_ms = 2000;
    fire_cooldown = 300;
    alive = true;
}

void Player::add_shield() {
    shields = std::min(shields + 1, MAX_SHIELDS);
    shield_ms = 10000;
}

bool Player::hit() {
    if (shielded()) {
        // the outer shield layer soaks the hit and pops
        shields--;
        invuln_ms = 600;
        audio::play(audio::Sfx::Shield);
        fx::particles(position, Pen(120, 230, 255), 16, 0.06f);
        return false;
    }

    hull--;
    fx::shake(3);
    if (hull > 0) {
        invuln_ms = 1400;
        audio::play(audio::Sfx::PlayerHit);
        fx::explosion(position, fx::Boom::Spark);
        fx::particles(position, Pen(255, 120, 60), 10, 0.05f);
        return false;
    }

    lives--;
    weapon = std::max(1, weapon - 1);
    rapid_ms = 0;
    if (lives > 0) {
        // the ship catches fire while the ship computer repairs it
        repair_ms = REPAIR_MS;
        audio::play(audio::Sfx::PlayerDie);
        fx::explosion(position, fx::Boom::Spark);
        fx::particles(position, Pen(255, 160, 60), 20, 0.07f, 600);
        return false;
    }

    // the last life: the ship is destroyed
    alive = false;
    audio::play(audio::Sfx::PlayerDie);
    fx::shake(6);
    fx::explosion(position, fx::Boom::Big);
    fx::explosion(position + Vec2(-5, 3), fx::Boom::Small, 150);
    fx::explosion(position + Vec2(5, -2), fx::Boom::Small, 300);
    fx::particles(position, Pen(255, 200, 80), 30, 0.08f, 700);
    fx::particles(position, Pen(255, 255, 255), 12, 0.10f, 500);
    return true;
}

void Player::fire() {
    auto &pm = ProjectileManager::get_instance();
    Vec2 nose = position + Vec2(0, -4);
    switch (weapon) {
        case 1:
            pm.fire(nose + Vec2(-2, 0), BULLET_SPEED, 0, BULLET_SPRITE, Faction::PLAYER);
            pm.fire(nose + Vec2(2, 0), BULLET_SPEED, 0, BULLET_SPRITE, Faction::PLAYER);
            break;
        case 2:
            pm.fire(nose, BULLET_SPEED, 0, BULLET_SPRITE, Faction::PLAYER);
            pm.fire(nose + Vec2(-2, 1), BULLET_SPEED, -0.18f, BULLET_SPRITE, Faction::PLAYER);
            pm.fire(nose + Vec2(2, 1), BULLET_SPEED, 0.18f, BULLET_SPRITE, Faction::PLAYER);
            break;
        case 3:
            pm.fire(nose + Vec2(-2, 0), BULLET_SPEED, 0, BULLET_SPRITE, Faction::PLAYER);
            pm.fire(nose + Vec2(2, 0), BULLET_SPEED, 0, BULLET_SPRITE, Faction::PLAYER);
            pm.fire(nose + Vec2(-3, 1), BULLET_SPEED, -0.25f, BULLET_SPRITE, Faction::PLAYER);
            pm.fire(nose + Vec2(3, 1), BULLET_SPEED, 0.25f, BULLET_SPRITE, Faction::PLAYER);
            break;
        default:
            pm.fire(nose, BULLET_SPEED, 0, 3, Faction::PLAYER, 2, 2.0f);
            pm.fire(nose + Vec2(-3, 0), BULLET_SPEED, 0, BULLET_SPRITE, Faction::PLAYER);
            pm.fire(nose + Vec2(3, 0), BULLET_SPEED, 0, BULLET_SPRITE, Faction::PLAYER);
            pm.fire(nose + Vec2(-3, 1), BULLET_SPEED, -0.30f, BULLET_SPRITE, Faction::PLAYER);
            pm.fire(nose + Vec2(3, 1), BULLET_SPEED, 0.30f, BULLET_SPRITE, Faction::PLAYER);
            break;
    }
    audio::play(audio::Sfx::Shoot);
}

void Player::update(uint32_t dt) {
    anim_time += dt;
    if (rapid_ms > 0) rapid_ms -= dt;
    if (shield_ms > 0) shield_ms -= dt;
    if (shield_ms <= 0) shields = 0;
    if (invuln_ms > 0) invuln_ms -= dt;
    if (fire_cooldown > 0) fire_cooldown -= dt;
    if (!alive) return;

    if (repair_ms > 0) {
        repair_ms -= dt;
        // the hull pips fill up as the repair goes
        hull = std::min(MAX_HULL, (int)((REPAIR_MS - repair_ms) * (MAX_HULL + 1) / REPAIR_MS));
        if ((anim_time / 90) != ((anim_time - dt) / 90)) {
            fx::particles(position + Vec2(frand(-2, 2), 0), Pen(110, 100, 110), 1, 0.02f, 500);  // smoke
        }
        if (repair_ms <= 0) {
            repair_ms = 0;
            hull = MAX_HULL;
            invuln_ms = 2000;
        }
    }

    Vec2 move(0, 0);
    if (autopilot) {
        // full speed, slowing down over the last few pixels
        Vec2 d = autopilot_to - position;
        float dist = d.length();
        if (dist > 0.5f) move = d / dist * std::min(1.0f, dist / 6);
    } else if (controls_enabled) {
        if (buttons & Button::DPAD_UP) move.y -= 1;
        if (buttons & Button::DPAD_DOWN) move.y += 1;
        if (buttons & Button::DPAD_LEFT) move.x -= 1;
        if (buttons & Button::DPAD_RIGHT) move.x += 1;
        // analogue stick where the hardware has one
        if (fabsf(joystick.x) > 0.25f) move.x = joystick.x;
        if (fabsf(joystick.y) > 0.25f) move.y = joystick.y;
    }
    if (move.length() > 1.0f) move.normalize();

    velocity = move * (repairing() ? SPEED / 2 : SPEED);
    position += velocity * (float)dt;
    position.x = clampf(position.x, 4, SCREEN_W - 4);
    position.y = clampf(position.y, HUD_H + 6, SCREEN_H - 5);

    if (controls_enabled && !autopilot && !repairing() && (buttons & Button::A) && fire_cooldown <= 0) {
        fire();
        fire_cooldown = rapid_ms > 0 ? RAPID_FIRE_MS : FIRE_MS;
    }
}

void Player::draw(uint32_t time) {
    if (!alive) return;
    // blink while invulnerable
    if (invuln_ms > 0 && (invuln_ms / 80) % 2 == 0) return;

    int sprite = SHIP_SPRITE[color];
    if (velocity.x < -0.01f) sprite -= 1;
    else if (velocity.x > 0.01f) sprite += 1;

    draw_flame(position, velocity.x, velocity.y < -0.01f, anim_time);
    draw_sprite(sheets.ships, sprite, position);
    if (repairing()) {
        // on fire: a flickering flame on the hull
        draw_sprite(sheets.misc, FLAME_FRAMES[(anim_time / 70) % 4], position + Vec2(1, -2));
    }

    if (shielded()) {
        // one pulsing bubble per layer, flickers when about to run out
        if (shield_ms > 1500 || (shield_ms / 100) % 2 == 0) {
            Point c = Point(position) + fx::offset();
            for (int layer = 0; layer < shields; layer++) {
                float r = 7.5f + layer * 2.5f + sinf(time * 0.012f + layer) * 0.8f;
                int dots = 28 + layer * 8;
                for (int i = 0; i < dots; i++) {
                    float a = i * (2 * PI_F / dots);
                    screen.pen = (i + time / 60) % 4 == 0 ? Pen(255, 255, 255) : Pen(100, 220, 255, 200);
                    screen.pixel(c + Point((int)roundf(cosf(a) * r), (int)roundf(sinf(a) * r)));
                }
            }
        }
    }
}

void Wingman::join(int ship) {
    sprite = ship;
    active = true;
    leaving = false;
    side = 1;
    fire_cooldown = 0;
    position = Vec2(SCREEN_W / 2 + WING_GAP, SCREEN_H + 10);
    velocity = Vec2(0, 0);
}

void Wingman::leave() {
    leaving = true;
}

void Wingman::update(uint32_t dt, const Player &player) {
    if (!active) return;
    anim_time += dt;
    if (fire_cooldown > 0) fire_cooldown -= dt;
    Vec2 before = position;

    if (leaving) {
        position.y -= SPEED * 1.5f * dt;
        if (position.y < -10) active = false;
    } else {
        // keep to the side with room, swapping only near the edge so it doesn't cross back and forth
        if (side > 0 && player.position.x > SCREEN_W - WING_GAP - 6) side = -1;
        if (side < 0 && player.position.x < WING_GAP + 6) side = 1;
        Vec2 to = player.alive ? player.position + Vec2(side * WING_GAP, 3) : Vec2(SCREEN_W / 2 + side * WING_GAP, SCREEN_H - 11);
        position += (to - position) * std::min(1.0f, dt * 0.006f);

        if (player.alive && !player.autopilot && player.controls_enabled && (buttons & Button::A) && fire_cooldown <= 0) {
            ProjectileManager::get_instance().fire(position + Vec2(0, -4), BULLET_SPEED, 0, BULLET_SPRITE, Faction::PLAYER);
            fire_cooldown = FIRE_MS;
        }
    }
    velocity = (position - before) / (float)std::max<uint32_t>(dt, 1);
}

void Wingman::draw(uint32_t time) {
    if (!active) return;
    float tilt = std::abs(velocity.x) > 0.02f ? velocity.x : 0;
    int frame = sprite + (tilt < 0 ? -1 : (tilt > 0 ? 1 : 0));
    draw_flame(position, tilt, velocity.y < -0.01f, anim_time);
    draw_sprite(sheets.ships, frame, position);
}
