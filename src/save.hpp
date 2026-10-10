#pragma once

#include <cstdint>

// Settings and the high score table, stored with blit::write_save (flash on device, a file on desktop/web).
struct HighScore {
    uint32_t score;
    uint16_t wave;
    char name[4];          // three letters + terminator
};

struct SaveData {
    static constexpr uint32_t MAGIC = 0x53465442;  // "SFTB"
    static constexpr int TOP = 5;

    uint32_t magic;
    uint8_t version;
    uint8_t music;
    uint8_t sound;
    uint8_t ship;          // player ship colour 0..4
    char last_name[4];     // pre-fills the next name entry
    HighScore top[TOP];
};

extern SaveData save_data;

void load_save();
void store_save();

// Rank a score would get (0 = best) or -1 if it doesn't make the table.
int score_rank(uint32_t score);
// Inserts a score with a three-letter name and saves. Returns its rank or -1.
int submit_score(uint32_t score, uint16_t wave, const char *name);
uint32_t best_score();
