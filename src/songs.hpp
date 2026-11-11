#pragma once

#include "32blit.hpp"

// One song for the sequencer in audio.cpp: four text tracks, one token per step.
// The note sheets in songs.cpp are made by tools/mod_to_song.py.
struct SongData {
    uint16_t step_ms;      // length of one step
    uint8_t bass_wave;     // blit::Waveform for the bass channel
    const char *lead;
    const char *bass;
    const char *arp;
    const char *drums;
};

extern const SongData SONG_TITLE;
extern const SongData SONG_STAGE_A;
extern const SongData SONG_STAGE_B;
extern const SongData SONG_BOSS;
extern const SongData SONG_STORY;
extern const SongData SONG_ENDING;
