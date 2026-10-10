#include "world.hpp"
#include "projectile_manager.hpp"
#include "audio.hpp"
#include "fx.hpp"
#include <cstdio>

namespace {
    constexpr int32_t INTRO_MS = 1800;
    constexpr int32_t BOSS_WARNING_MS = 2800;
    constexpr int32_t CLEARED_MS = 1800;
    constexpr int32_t OVER_MS = 2400;
    constexpr uint32_t PERFECT_BONUS = 1000;

    bool overlaps(const Projectile &p, const Rect &r) {
        return p.position.x + p.radius >= r.x && p.position.x - p.radius <= r.x + r.w &&
               p.position.y + p.radius >= r.y && p.position.y - p.radius <= r.y + r.h;
    }

    EnemyKind pick_kind(int wave) {
        int weights[(int)EnemyKind::COUNT] = {
            30,
            25,
            wave >= 2 ? 20 : 0,
            wave >= 4 ? 15 : 0,
            wave >= 6 ? 9 : 0,
        };
        int total = 0;
        for (int w : weights) total += w;
        int roll = irand(0, total - 1);
        for (int i = 0; i < (int)EnemyKind::COUNT; i++) {
            if (roll < weights[i]) return (EnemyKind)i;
            roll -= weights[i];
        }
        return EnemyKind::Drone;
    }

    MovementPattern pick_pattern(int wave) {
        int unlocked = wave >= 4 ? 6 : (wave >= 3 ? 5 : (wave >= 2 ? 3 : 2));
        return (MovementPattern)irand(0, unlocked - 1);
    }
}

void World::start(uint8_t ship_color, Starfield *s) {
    stars = s;
    score = 0;
    wave = 1;
    combo = 0;
    combo_ms = 0;
    next_life_score = EXTRA_LIFE_EVERY;
    player.reset(ship_color);
    boss.active = false;
    for (auto &e : enemies) e.active = false;
    powerups.clear();
    ProjectileManager::get_instance().clear();
    fx::clear();
    set_phase(Phase::Intro, INTRO_MS);
    audio::play_song(audio::Song::Game);
    audio::play(audio::Sfx::WaveStart);
}

void World::set_phase(Phase p, int32_t ms) {
    phase = p;
    phase_ms = ms;
}

void World::spawn_enemy(EnemyKind kind, MovementPattern pattern, Vec2 origin, float dir) {
    float speed = std::min(1.0f + 0.05f * (wave - 1), 1.8f);
    float fire_rate = std::min(1.0f + 0.08f * (wave - 1), 2.5f);
    int bonus_hp = (wave - 1) / 6;
    for (auto &e : enemies) {
        if (e.active) continue;
        e.spawn(kind, pattern, origin, dir, speed, fire_rate, bonus_hp);
        return;
    }
}

void World::build_wave() {
    schedule_count = 0;
    schedule_next = 0;
    wave_time = 0;

    int groups = std::min(3 + wave / 2, 8);
    uint32_t t = 400;
    uint32_t gap = (uint32_t)std::max(700, 2000 - wave * 90);

    for (int g = 0; g < groups && schedule_count < MAX_SPAWNS; g++) {
        EnemyKind kind = pick_kind(wave);
        MovementPattern pattern = pick_pattern(wave);
        float dir = irand(0, 1) ? 1.0f : -1.0f;
        int count = irand(3, 5) + (wave > 6 ? 1 : 0);
        if (kind == EnemyKind::Tank) count = irand(2, 3);
        uint32_t spacing = (uint32_t)irand(280, 380);
        Vec2 origin(frand(16, SCREEN_W - 16), 0);

        switch (pattern) {
            case MovementPattern::Loop:
                origin.x = dir > 0 ? frand(12, 70) : frand(58, 116);
                break;
            case MovementPattern::Swoop:
                origin.x = dir > 0 ? frand(16, 56) : frand(72, 112);
                break;
            case MovementPattern::Sweep:
                origin.y = frand(20, 56);
                break;
            case MovementPattern::Hover:
                count = std::min(count, 5);
                origin.y = 22 + irand(0, 2) * 12;
                spacing = 120;
                break;
            default:
                break;
        }

        for (int i = 0; i < count && schedule_count < MAX_SPAWNS; i++) {
            Spawn &s = schedule[schedule_count++];
            s.at = t + i * spacing;
            s.kind = kind;
            s.pattern = pattern;
            s.origin = origin;
            s.dir = dir;
            if (pattern == MovementPattern::Hover) {
                s.origin.x = SCREEN_W / 2 + (i - (count - 1) / 2.0f) * 20;
            }
        }
        t += count * spacing + gap;
    }
}

void World::add_score(uint32_t points, Vec2 where, bool combo_counts) {
    if (combo_counts) {
        combo++;
        combo_ms = COMBO_WINDOW_MS;
    }
    uint32_t gained = points * (combo_counts ? multiplier() : 1);
    score += gained;

    char text[12];
    snprintf(text, sizeof(text), "%lu", (unsigned long)gained);
    fx::popup(where, text, multiplier() > 1 ? Pen(255, 230, 90) : Pen(255, 255, 255));

    while (score >= next_life_score) {
        next_life_score += EXTRA_LIFE_EVERY;
        if (player.lives < 9) player.lives++;
        audio::play(audio::Sfx::ExtraLife);
        fx::popup(player.position + Vec2(0, -10), "1UP!", Pen(120, 255, 140));
    }
}

void World::kill_enemy(Enemy &e) {
    const EnemyPreset &preset = enemy_preset(e.kind);
    bool big = e.kind == EnemyKind::Tank || e.kind == EnemyKind::Crab;
    fx::explosion(e.position, e.kind == EnemyKind::Crab ? fx::Boom::Blue : fx::Boom::Small);
    fx::particles(e.position, Pen(255, 190, 70), big ? 18 : 10, 0.06f);
    if (big) fx::shake(1.5f);
    audio::play(audio::Sfx::Explode);
    add_score(preset.points, e.position, true);
    if (irand(1, 100) <= preset.drop_chance) {
        powerups.spawn_random(e.position, player.weapon, player.hull, Player::MAX_HULL);
    }
}

void World::player_hit() {
    hit_this_wave = true;
    combo = 0;
    combo_ms = 0;
    bool last_life = player.hit();
    if (!player.alive) {
        // clear the screen of bullets so the respawn is fair
        ProjectileManager::get_instance().clear_enemy_bullets(true);
    }
    if (last_life) {
        set_phase(Phase::Over, OVER_MS);
        audio::stop_music();
    }
}

void World::collect(PowerUp &p) {
    p.active = false;
    const char *label = powerup_name(p.kind);
    switch (p.kind) {
        case PowerUpKind::Weapon:
            player.weapon = std::min(Player::MAX_WEAPON, player.weapon + 1);
            audio::play(audio::Sfx::PowerUp);
            if (player.weapon == Player::MAX_WEAPON) label = "MAX POWER";
            break;
        case PowerUpKind::Rapid:
            player.rapid_ms = 10000;
            audio::play(audio::Sfx::PowerUp);
            break;
        case PowerUpKind::Shield:
            player.shield_ms = 10000;
            audio::play(audio::Sfx::Shield);
            break;
        case PowerUpKind::Repair:
            player.hull = Player::MAX_HULL;
            audio::play(audio::Sfx::PowerUp);
            break;
        case PowerUpKind::Life:
            if (player.lives < 9) player.lives++;
            audio::play(audio::Sfx::ExtraLife);
            break;
        case PowerUpKind::Coin:
            audio::play(audio::Sfx::Coin);
            add_score(500, p.position + Vec2(0, 8), false);
            break;
        default:
            break;
    }
    fx::popup(p.position, label, Pen(120, 255, 200));
    fx::particles(p.position, Pen(180, 255, 220), 12, 0.05f);
}

void World::handle_collisions() {
    auto &pm = ProjectileManager::get_instance();

    for (auto &p : pm.pool) {
        if (!p.active) continue;

        if (p.owner == Faction::PLAYER) {
            for (auto &e : enemies) {
                if (!e.active || !overlaps(p, e.get_hitbox())) continue;
                p.active = false;
                if (e.take_damage(p.damage)) {
                    kill_enemy(e);
                } else {
                    audio::play(audio::Sfx::Hit);
                    fx::particles(p.position, Pen(255, 255, 200), 3, 0.04f, 200);
                }
                break;
            }
            if (p.active && boss.active && !boss.dying && overlaps(p, boss.get_hitbox())) {
                p.active = false;
                fx::particles(p.position, Pen(255, 255, 200), 3, 0.04f, 200);
                if (boss.take_damage(p.damage)) {
                    audio::play(audio::Sfx::BigExplode);
                    pm.clear_enemy_bullets(true);
                    fx::shake(4);
                } else {
                    audio::play(audio::Sfx::Hit);
                }
            }
        } else if (player.can_be_hit()) {
            if (overlaps(p, player.get_hitbox())) {
                p.active = false;
                player_hit();
            }
        }
    }

    if (!player.alive) return;
    Rect ph = player.get_hitbox();

    // ramming
    for (auto &e : enemies) {
        if (!e.active || !e.get_hitbox().intersects(ph)) continue;
        if (!player.can_be_hit()) continue;
        e.take_damage(999);
        kill_enemy(e);
        player_hit();
        if (!player.alive) return;
    }
    if (boss.active && !boss.dying && player.can_be_hit() && boss.get_hitbox().intersects(ph)) {
        player_hit();
        if (!player.alive) return;
    }

    Rect pickup = Rect(ph.x - 2, ph.y - 2, ph.w + 4, ph.h + 4);
    for (auto &p : powerups.pool) {
        if (p.active && p.get_hitbox().intersects(pickup)) collect(p);
    }
}

void World::update(uint32_t dt) {
    if (phase_ms > 0) phase_ms -= dt;
    if (combo_ms > 0) {
        combo_ms -= dt;
        if (combo_ms <= 0) combo = 0;
    }

    switch (phase) {
        case Phase::Intro:
            stars->speed = lerpf(stars->speed, 1.0f, 0.02f);
            if (phase_ms <= 0) {
                hit_this_wave = false;
                if (is_boss_wave()) {
                    set_phase(Phase::BossWarning, BOSS_WARNING_MS);
                    audio::stop_music();
                    audio::play(audio::Sfx::BossWarn);
                } else {
                    build_wave();
                    set_phase(Phase::Fighting, 0);
                }
            }
            break;

        case Phase::BossWarning:
            if (phase_ms <= 0) {
                boss.spawn(wave / 5);
                escort_ms = 5000;
                set_phase(Phase::BossFight, 0);
                audio::play_song(audio::Song::Boss);
            }
            break;

        case Phase::Fighting: {
            wave_time += dt;
            while (schedule_next < schedule_count && schedule[schedule_next].at <= wave_time) {
                Spawn &s = schedule[schedule_next++];
                spawn_enemy(s.kind, s.pattern, s.origin, s.dir);
            }
            bool any_alive = false;
            for (auto &e : enemies) any_alive |= e.active;
            if (schedule_next >= schedule_count && !any_alive && player.alive) {
                perfect_bonus = !hit_this_wave;
                if (perfect_bonus) add_score(PERFECT_BONUS, Vec2(SCREEN_W / 2, 80), false);
                audio::play(audio::Sfx::WaveClear);
                set_phase(Phase::Cleared, CLEARED_MS);
            }
            break;
        }

        case Phase::BossFight:
            // later bosses bring escorts
            if (boss.level >= 2 && !boss.dying && !boss.entering()) {
                escort_ms -= dt;
                if (escort_ms <= 0) {
                    escort_ms = 4500;
                    float dir = irand(0, 1) ? 1.0f : -1.0f;
                    for (int i = 0; i < 3; i++) {
                        spawn_enemy(EnemyKind::Drone, MovementPattern::Swoop,
                                    Vec2((dir > 0 ? 14.0f : 114.0f) + dir * i * 12, 0), dir);
                    }
                }
            }
            if (boss.finished() || (!boss.active && boss.dying)) {
                fx::explosion(boss.position, fx::Boom::Big);
                fx::explosion(boss.position + Vec2(-8, 4), fx::Boom::Big, 120);
                fx::explosion(boss.position + Vec2(8, -3), fx::Boom::Big, 240);
                fx::particles(boss.position, Pen(255, 220, 120), 50, 0.10f, 900);
                fx::particles(boss.position, Pen(120, 220, 255), 30, 0.07f, 900);
                fx::shake(8);
                audio::play(audio::Sfx::BigExplode);
                add_score(boss.points(), boss.position, false);
                powerups.spawn(boss.position, player.weapon < Player::MAX_WEAPON ? PowerUpKind::Weapon : PowerUpKind::Life);
                boss.active = false;
                boss.dying = false;
                perfect_bonus = !hit_this_wave;
                if (perfect_bonus) add_score(PERFECT_BONUS, Vec2(SCREEN_W / 2, 80), false);
                set_phase(Phase::Cleared, CLEARED_MS + 1200);
                audio::play_song(audio::Song::Game);
            }
            break;

        case Phase::Cleared:
            if (phase_ms < 600) stars->speed = lerpf(stars->speed, 4.0f, 0.05f);
            if (phase_ms <= 0) {
                wave++;
                set_phase(Phase::Intro, INTRO_MS);
                audio::play(audio::Sfx::WaveStart);
            }
            break;

        case Phase::Over:
            break;
    }

    Vec2 target = player.alive ? player.position : Vec2(SCREEN_W / 2, SCREEN_H);
    player.update(dt);
    for (auto &e : enemies) e.update(dt, target, player.alive);
    boss.update(dt, target, player.alive);
    powerups.update(dt);
    ProjectileManager::get_instance().update(dt);
    if (phase != Phase::Over) handle_collisions();
}

void World::draw(uint32_t time) {
    powerups.draw(time);
    for (auto &e : enemies) e.draw();
    boss.draw();
    player.draw(time);
    ProjectileManager::get_instance().draw();
    fx::draw();
    draw_banner(time);
    draw_hud(time);
}

void World::draw_hud(uint32_t time) {
    char buf[24];

    // top bar
    screen.pen = Pen(0, 0, 0, 170);
    screen.rectangle(Rect(0, 0, SCREEN_W, HUD_H));
    screen.pen = Pen(60, 60, 120);
    screen.h_span(Point(0, HUD_H), SCREEN_W);

    snprintf(buf, sizeof(buf), "%06lu", (unsigned long)score);
    screen.pen = Pen(255, 255, 255);
    screen.text(buf, minimal_font, Point(2, 1));

    snprintf(buf, sizeof(buf), "WAVE %d", wave);
    screen.pen = Pen(150, 170, 255);
    screen.text(buf, minimal_font, Point(SCREEN_W / 2 + 6, 1), true, TextAlign::top_center);

    // lives as small hearts
    snprintf(buf, sizeof(buf), "%d", std::max(0, player.lives));
    screen.sprites = sheets.misc;
    screen.sprite(52, Point(SCREEN_W - 17, 0));
    screen.pen = Pen(255, 160, 220);
    screen.text(buf, minimal_font, Point(SCREEN_W - 7, 1));

    // combo multiplier under the score
    int mult = multiplier();
    if (mult > 1 && combo_ms > 0) {
        snprintf(buf, sizeof(buf), "x%d", mult);
        shadow_text(buf, minimal_font, Point(2, HUD_H + 2), rainbow(time * 0.01f));
        // combo timer bar
        screen.pen = Pen(255, 230, 90, 160);
        screen.h_span(Point(2, HUD_H + 10), (int)(16 * combo_ms / COMBO_WINDOW_MS));
    }

    if (phase == Phase::Over) return;

    // hull pips, bottom left
    for (int i = 0; i < Player::MAX_HULL; i++) {
        bool full = player.alive && i < player.hull;
        Pen pen = Pen(40, 40, 60);
        if (full) {
            pen = player.hull == 1 ? ((time / 200) % 2 ? Pen(255, 60, 60) : Pen(140, 20, 20)) : Pen(90, 230, 110);
        }
        screen.pen = Pen(0, 0, 0, 160);
        screen.rectangle(Rect(1 + i * 7, SCREEN_H - 5, 7, 5));
        screen.pen = pen;
        screen.rectangle(Rect(2 + i * 7, SCREEN_H - 4, 5, 3));
    }

    // weapon level, bottom right
    for (int i = 0; i < Player::MAX_WEAPON; i++) {
        screen.pen = i < player.weapon ? Pen(255, 170, 50) : Pen(50, 40, 40);
        screen.rectangle(Rect(SCREEN_W - 5 - i * 4, SCREEN_H - 2 - i, 3, 1 + i));
    }

    // timed power-ups
    int x = SCREEN_W - 20;
    if (player.rapid_ms > 0) {
        screen.pen = (player.rapid_ms > 2000 || (time / 120) % 2) ? Pen(255, 140, 60) : Pen(120, 60, 30);
        screen.text("R", minimal_font, Point(x - 6, SCREEN_H - 7));
        x -= 7;
    }
    if (player.shield_ms > 0) {
        screen.pen = (player.shield_ms > 2000 || (time / 120) % 2) ? Pen(120, 220, 255) : Pen(40, 90, 120);
        screen.text("S", minimal_font, Point(x - 6, SCREEN_H - 7));
    }

    // boss health
    if (boss.active && !boss.entering()) {
        int w = 96;
        int fill = w * boss.health / boss.max_health;
        screen.pen = Pen(0, 0, 0, 180);
        screen.rectangle(Rect(15, HUD_H + 2, w + 2, 4));
        screen.pen = (boss.flash_ms > 0) ? Pen(255, 255, 255) : Pen(230, 50, 70);
        screen.rectangle(Rect(16, HUD_H + 3, fill, 2));
    }
}

void World::draw_banner(uint32_t time) {
    char buf[24];
    auto band = [](int y, int h, Pen pen) {
        screen.pen = pen;
        screen.rectangle(Rect(0, y, SCREEN_W, h));
    };

    switch (phase) {
        case Phase::Intro: {
            // slide in, hold, slide out
            int32_t elapsed = INTRO_MS - phase_ms;
            float in = clampf(elapsed / 250.0f, 0, 1);
            float out = clampf(phase_ms / 250.0f, 0, 1);
            int x = (int)((1.0f - in) * -SCREEN_W + (1.0f - out) * SCREEN_W);
            band(50, 26, Pen(0, 0, 0, 150));
            snprintf(buf, sizeof(buf), "WAVE %d", wave);
            shadow_text(buf, outline_font, Point(SCREEN_W / 2 + x, 55), rainbow(time * 0.008f), TextAlign::top_center);
            shadow_text(is_boss_wave() ? "SOMETHING BIG IS COMING" : "GET READY", minimal_font,
                        Point(SCREEN_W / 2 - x, 66), Pen(200, 200, 230), TextAlign::top_center);
            break;
        }
        case Phase::BossWarning: {
            bool on = (time / 250) % 2;
            band(46, 32, on ? Pen(160, 0, 20, 170) : Pen(60, 0, 10, 170));
            // hazard stripes
            screen.pen = Pen(255, 200, 0);
            for (int i = -2; i < SCREEN_W / 8 + 2; i++) {
                int sx = i * 8 + (int)((time / 30) % 8);
                screen.line(Point(sx, 46), Point(sx + 3, 49));
                screen.line(Point(sx, 75), Point(sx + 3, 78));
            }
            shadow_text("WARNING", outline_font, Point(SCREEN_W / 2, 53), on ? Pen(255, 255, 255) : Pen(255, 80, 80), TextAlign::top_center);
            shadow_text("BOSS APPROACHING", minimal_font, Point(SCREEN_W / 2, 64), Pen(255, 220, 120), TextAlign::top_center);
            break;
        }
        case Phase::Cleared: {
            if (boss.active) break;
            band(50, perfect_bonus ? 28 : 18, Pen(0, 0, 0, 150));
            shadow_text("WAVE CLEAR!", outline_font, Point(SCREEN_W / 2, 54), Pen(120, 255, 160), TextAlign::top_center);
            if (perfect_bonus) {
                shadow_text("NO HIT BONUS +1000", minimal_font, Point(SCREEN_W / 2, 66), rainbow(time * 0.01f), TextAlign::top_center);
            }
            break;
        }
        case Phase::Over: {
            int32_t elapsed = OVER_MS - phase_ms;
            if (elapsed > 600) {
                band(54, 18, Pen(0, 0, 0, 160));
                shadow_text("GAME OVER", outline_font, Point(SCREEN_W / 2, 59), Pen(255, 80, 80), TextAlign::top_center);
            }
            break;
        }
        default:
            break;
    }
}
