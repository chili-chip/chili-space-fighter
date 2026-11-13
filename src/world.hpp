#pragma once

#include "common.hpp"
#include "player.hpp"
#include "enemy.hpp"
#include "powerup.hpp"
#include "starfield.hpp"
#include "story.hpp"
#include "audio.hpp"

// One run of the game: waves, enemies, the boss, scoring, the in-game HUD and the story scenes
// between waves (chapter 1 is waves 1 to 10, and so on; every tenth wave is a boss).
class World {
public:
    enum class Phase { Intro, Fighting, BossWarning, BossFight, Cleared, Story, Over, Complete };
    // Story: chapters 1 to 6 with scenes between waves, ending after wave 60. Endless: waves forever, high scores.
    enum class Mode { Story, Endless };

    Player player;
    Wingman wingmen[2];  // friends on the wing, see wing_sprite()
    Boss boss;
    PowerUpManager powerups;

    uint32_t score = 0;
    int wave = 1;
    Phase phase = Phase::Intro;
    Mode mode = Mode::Endless;
    bool god = false;  // cheat: nothing hurts the player (kept between runs)

    // chapter: where a story run begins (1 to 6); retry: the chapter is played again after a game over
    void start(uint8_t ship_color, Starfield *stars, Mode mode, int chapter = 1, bool retry = false);
    void update(uint32_t dt);
    void draw(uint32_t time);

    // cheats, from the pause menu
    void skip_wave();   // destroys every enemy and the boss, as if the player had
    void max_power();   // full weapon, three shields, rapid fire, full hull, 9 lives

    bool is_over() const { return phase == Phase::Over && phase_ms <= 0 && !dialogue.active(); }
    bool is_complete() const { return phase == Phase::Complete; }
    bool is_boss_wave() const { return wave % story::WAVES_PER_CHAPTER == 0; }
    // 1 to 6 in story mode, 0 in endless mode
    int chapter() const { return mode == Mode::Story ? std::min((wave - 1) / story::WAVES_PER_CHAPTER + 1, story::CHAPTERS) : 0; }

private:
    static constexpr int MAX_ENEMIES = 40;
    static constexpr int MAX_SPAWNS = 64;
    static constexpr uint32_t EXTRA_LIFE_EVERY = 10000;
    static constexpr int32_t COMBO_WINDOW_MS = 1500;
    static constexpr int CONVOY_SHIPS = 42;  // the families' ships that go through the Tollgate

    // story mode only: a rubber band on enemy fire rate and speed and on the boss's pace
    static constexpr float DIFFICULTY_MIN = 0.7f, DIFFICULTY_MAX = 1.15f;
    static constexpr float DIFFICULTY_DOWN = 0.1f;  // a life lost, or a retried chapter
    static constexpr float DIFFICULTY_UP = 0.05f;   // a wave cleared without a hit

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
    float difficulty = 1;
    Starfield *stars = nullptr;

    struct ConvoyShip { float x, y, speed; uint8_t sprite; };
    ConvoyShip convoy[CONVOY_SHIPS];
    int convoy_count = 0;  // ships launched so far
    int32_t convoy_ms = 0; // until the next one launches

    // what happens when the scene in the Story phase ends
    enum class After { Card, Wave, Boss, Ending, NextWave, Complete };
    story::Dialogue dialogue;
    story::Comms comms;    // the ship computer while the ship burns
    int repair_line = 0;
    After after_story = After::Wave;
    bool game_over_said = false;

    void play(const story::Scene &scene, After then);
    void continue_after(After then);
    void begin_wave();
    void spawn_boss();
    void next_wave();
    int wing_sprite(int slot) const;
    void draw_rescue(uint32_t time);
    void adjust_difficulty(float step);
    void update_convoy(uint32_t dt);
    void draw_convoy(uint32_t time);
    audio::Song stage_song() const;
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
