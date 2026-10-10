#pragma once

#include "common.hpp"

// Sound effects and the chiptune soundtrack, all generated with the 32blit synth.
// Channels 0-3 play music (lead, bass, arpeggio, drums), channels 4-7 play effects.
namespace audio {
    enum class Sfx {
        Shoot, Hit, Explode, BigExplode, PlayerHit, PlayerDie,
        PowerUp, ExtraLife, Shield, Coin, WaveStart, WaveClear, BossWarn,
        MenuMove, MenuSelect, MenuBack, Pause, GameOver, NewBest,
        COUNT
    };

    enum class Song { None, Title, Game, Boss };

    void init();
    void update(uint32_t dt);

    void play(Sfx sfx);
    void play_song(Song song);   // restarts only if a different song is requested
    void stop_music();

    void set_music_enabled(bool on);
    void set_sound_enabled(bool on);
}
