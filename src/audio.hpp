#pragma once

#include "common.hpp"

// Sound effects and the chiptune soundtrack, all generated with the 32blit synth.
// Channels 0-3 play music (lead, bass, arpeggio, drums), channels 4-7 play effects.
namespace audio {
    enum class Sfx {
        Shoot, Hit, Explode, BigExplode, PlayerHit, PlayerDie,
        PowerUp, ExtraLife, Shield, Coin, WaveStart, WaveClear, BossWarn,
        MenuMove, MenuSelect, MenuBack, Pause, GameOver, NewBest,
        Static, BeamCharge, Beam,
        COUNT
    };

    // Title screen, the two stage themes, boss fights, story scenes, and the ending and credits
    enum class Song { None, Title, StageA, StageB, Boss, Story, Ending };

    void init();
    void update(uint32_t dt);

    void play(Sfx sfx);
    void talk(int voice);        // one short blip of a character's voice while their text appears
    void play_song(Song song);   // restarts only if a different song is requested
    void stop_music();

    void set_music_enabled(bool on);
    void set_sound_enabled(bool on);
}
