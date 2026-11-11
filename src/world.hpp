#pragma once

#include "common.hpp"
#include "player.hpp"
#include "enemy.hpp"
#include "powerup.hpp"
#include "starfield.hpp"
#include "story.hpp"

// One run of the game: waves, enemies, the boss, scoring, the in-game HUD and the story scenes
// between waves (chapter 1 is waves 1 to 5, and so on; after chapter 6 come the endless Free Lanes).
class World {
public:
    enum class Phase { Intro, Fighting, BossWarning, BossFight, Cleared, Story, Over };

    Player player;
    Boss boss;
    PowerUpManager powerups;

    uint32_t score = 0;
    int wave = 1;
    Phase phase = Phase::Intro;

    void start(uint8_t ship_color, Starfield *stars);
    void update(uint32_t dt);
    void draw(uint32_t time);

    bool is_over() const { return phase == Phase::Over && phase_ms <= 0 && !dialogue.active(); }
    bool is_boss_wave() const { return wave % 5 == 0; }
    // 1 to 6 while the story runs, 0 in the Free Lanes
    int chapter() const { return wave <= story::CHAPTERS * story::WAVES_PER_CHAPTER ? (wave - 1) / story::WAVES_PER_CHAPTER + 1 : 0; }

private:
    static constexpr int MAX_ENEMIES = 40;
    static constexpr int MAX_SPAWNS = 64;
    static constexpr uint32_t EXTRA_LIFE_EVERY = 10000;
    static constexpr int32_t COMBO_WINDOW_MS = 1500;

    struct Spawn {
        uint32_t at;
        EnemyKind kind;
        MovementPattern pattern;
        Vec2 origin;
        float dir;
    };

    Enemy enemies[MAX_ENEMIES];
    Spawn schedule[MAX_SPAWNS];
    int schedule_count = 0;
    int schedule_next = 0;
    uint32_t wave_time = 0;
    int32_t phase_ms = 0;
    int32_t escort_ms = 0;
    uint32_t next_life_score = EXTRA_LIFE_EVERY;
    int combo = 0;
    int32_t combo_ms = 0;
    bool hit_this_wave = false;
    bool perfect_bonus = false;
    Starfield *stars = nullptr;

    // what happens when the scene in the Story phase ends
    enum class After { Card, Wave, Boss, Ending, NextWave };
    story::Dialogue dialogue;
    After after_story = After::Wave;
    bool game_over_said = false;

    void play(const story::Scene &scene, After then);
    void continue_after(After then);
    void begin_wave();
    void spawn_boss();
    void next_wave();
    bool chapter_first_wave() const { return chapter() > 0 && (wave - 1) % story::WAVES_PER_CHAPTER == 0; }

    void set_phase(Phase p, int32_t ms);
    void build_wave();
    void spawn_enemy(EnemyKind kind, MovementPattern pattern, Vec2 origin, float dir);
    void handle_collisions();
    void kill_enemy(Enemy &e);
    void collect(PowerUp &p);
    void player_hit();
    void add_score(uint32_t points, Vec2 where, bool combo_counts);
    int multiplier() const { return 1 + std::min(combo, 28) / 4; }

    void draw_hud(uint32_t time);
    void draw_banner(uint32_t time);
};
