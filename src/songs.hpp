#pragma once

#include "32blit.hpp"

struct SongData {
    uint16_t step_ms;      // length of one 16th-note step
    uint8_t bass_wave;     // blit::Waveform for the bass channel
    const char *lead;
    const char *bass;
    const char *arp;
    const char *drums;
};

extern const SongData SONG_TITLE;
extern const SongData SONG_GAME;
extern const SongData SONG_BOSS;
