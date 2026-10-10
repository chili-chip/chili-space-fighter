#include "save.hpp"
#include "32blit.hpp"
#include <cstring>

SaveData save_data;

namespace {
    void reset_defaults() {
        save_data = {};
        save_data.magic = SaveData::MAGIC;
        save_data.version = 2;
        save_data.music = 1;
        save_data.sound = 1;
        save_data.ship = 0;
        strcpy(save_data.last_name, "AAA");
        // a few scores to beat on a fresh install
        static const HighScore defaults[SaveData::TOP] = {
            {10000, 6, "CHI"}, {6000, 4, "LLI"}, {3000, 3, "VGC"}, {1500, 2, "ZRO"}, {500, 1, "ACE"}
        };
        for (int i = 0; i < SaveData::TOP; i++) save_data.top[i] = defaults[i];
    }
}

void load_save() {
    SaveData loaded;
    if (blit::read_save(loaded) && loaded.magic == SaveData::MAGIC && loaded.version == 2) {
        save_data = loaded;
        if (save_data.ship > 4) save_data.ship = 0;
        save_data.last_name[3] = 0;
        for (auto &h : save_data.top) h.name[3] = 0;
    } else {
        reset_defaults();
    }
}

void store_save() {
    blit::write_save(save_data);
}

int score_rank(uint32_t score) {
    for (int i = 0; i < SaveData::TOP; i++) {
        if (score > save_data.top[i].score) return i;
    }
    return -1;
}

int submit_score(uint32_t score, uint16_t wave, const char *name) {
    int rank = score_rank(score);
    if (rank < 0) return -1;
    for (int j = SaveData::TOP - 1; j > rank; j--) save_data.top[j] = save_data.top[j - 1];
    HighScore &h = save_data.top[rank];
    h.score = score;
    h.wave = wave;
    memcpy(h.name, name, 3);
    h.name[3] = 0;
    memcpy(save_data.last_name, name, 3);
    save_data.last_name[3] = 0;
    store_save();
    return rank;
}

uint32_t best_score() {
    return save_data.top[0].score;
}
