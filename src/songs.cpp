// Original soundtrack for Space Fighters TURBO, composed for this game.
// Same licence as the rest of the repository (MIT).
//
// Notation: one token per 16th-note step, '|' marks bars.
//   C5, F#4, Bb5  note on      -  hold      .  note off / rest
// Drums: k kick, s snare, h hi-hat. Channels loop independently.

#include "songs.hpp"

namespace {

const char *const title_lead =
    "C5 - - G4 C5 - E5 - G5 - - - E5 - C5 - |"
    "A4 - - E5 A5 - G5 - E5 - - - C5 - E5 - |"
    "F5 - - - A5 - G5 - F5 - E5 - D5 - C5 - |"
    "D5 - - - - - G4 - B4 - D5 - G5 - . . |"
    "A5 - - G5 F5 - - E5 F5 - A5 - C6 - - - |"
    "B5 - - A5 G5 - - F5 G5 - B5 - D6 - - - |"
    "G5 - E5 - B4 - E5 - G5 - B5 - A5 - G5 - |"
    "A5 - - - - - - - G5 - - - - - . . |";

const char *const title_bass =
    "C2 - - . C2 - C3 . C2 - - . C2 - C3 . |"
    "A2 - - . A2 - A3 . A2 - - . A2 - A3 . |"
    "F2 - - . F2 - F3 . F2 - - . F2 - F3 . |"
    "G2 - - . G2 - G3 . G2 - - . G2 - G3 . |"
    "F2 - - . F2 - F3 . F2 - - . F2 - F3 . |"
    "G2 - - . G2 - G3 . G2 - - . G2 - G3 . |"
    "E2 - - . E2 - E3 . E2 - - . E2 - E3 . |"
    "A2 - - . A2 - A3 . G2 - - . G2 - G3 . |";

const char *const title_arp =
    "G4 C5 E5 C5 G4 C5 E5 C5 G4 C5 E5 C5 G4 C5 E5 C5 |"
    "A4 C5 E5 C5 A4 C5 E5 C5 A4 C5 E5 C5 A4 C5 E5 C5 |"
    "A4 C5 F5 C5 A4 C5 F5 C5 A4 C5 F5 C5 A4 C5 F5 C5 |"
    "G4 B4 D5 B4 G4 B4 D5 B4 G4 B4 D5 B4 G4 B4 D5 B4 |"
    "A4 C5 F5 C5 A4 C5 F5 C5 A4 C5 F5 C5 A4 C5 F5 C5 |"
    "G4 B4 D5 B4 G4 B4 D5 B4 G4 B4 D5 B4 G4 B4 D5 B4 |"
    "G4 B4 E5 B4 G4 B4 E5 B4 G4 B4 E5 B4 G4 B4 E5 B4 |"
    "A4 C5 E5 C5 A4 C5 E5 C5 G4 B4 D5 B4 G4 B4 D5 B4 |";

const char *const title_drums =
    "k . . . s . . h k . k . s . h h |"
    "k . . . s . . h k . k . s s s s |";

const char *const game_lead =
    "E5 - - E5 D5 - C5 - D5 - E5 - A4 - - . |"
    "F5 - - E5 D5 - C5 - A4 - C5 - F5 - - . |"
    "G5 - - F5 E5 - D5 - C5 - D5 - E5 - G5 - |"
    "D5 - - - B4 - - - G4 - B4 - D5 - . . |"
    "A5 - - G5 E5 - - D5 E5 - A4 - C5 - E5 - |"
    "F5 - - E5 C5 - - A4 C5 - F5 - A5 - G5 - |"
    "G5 - - F5 D5 - - B4 D5 - G5 - B5 - A5 - |"
    "G#5 - - - E5 - - - B4 - G#4 - E5 - . . |";

const char *const game_bass =
    "A2 . A3 . A2 . A3 . A2 . A3 . A2 A2 A3 . |"
    "F2 . F3 . F2 . F3 . F2 . F3 . F2 F2 F3 . |"
    "C2 . C3 . C2 . C3 . C2 . C3 . C2 C2 C3 . |"
    "G2 . G3 . G2 . G3 . G2 . G3 . G2 G2 G3 . |"
    "A2 . A3 . A2 . A3 . A2 . A3 . A2 A2 A3 . |"
    "F2 . F3 . F2 . F3 . F2 . F3 . F2 F2 F3 . |"
    "G2 . G3 . G2 . G3 . G2 . G3 . G2 G2 G3 . |"
    "E2 . E3 . E2 . E3 . E2 . E3 . E2 E2 E3 . |";

const char *const game_arp =
    "A4 C5 E5 C5 A4 C5 E5 C5 A4 C5 E5 C5 A4 C5 E5 C5 |"
    "F4 A4 C5 A4 F4 A4 C5 A4 F4 A4 C5 A4 F4 A4 C5 A4 |"
    "E4 G4 C5 G4 E4 G4 C5 G4 E4 G4 C5 G4 E4 G4 C5 G4 |"
    "G4 B4 D5 B4 G4 B4 D5 B4 G4 B4 D5 B4 G4 B4 D5 B4 |"
    "A4 C5 E5 C5 A4 C5 E5 C5 A4 C5 E5 C5 A4 C5 E5 C5 |"
    "F4 A4 C5 A4 F4 A4 C5 A4 F4 A4 C5 A4 F4 A4 C5 A4 |"
    "G4 B4 D5 B4 G4 B4 D5 B4 G4 B4 D5 B4 G4 B4 D5 B4 |"
    "E4 G#4 B4 G#4 E4 G#4 B4 G#4 E4 G#4 B4 G#4 E4 G#4 B4 G#4 |";

const char *const game_drums =
    "k . h . s . h . k . k h s . h . |"
    "k . h . s . h k . k k h s . s s |";

const char *const boss_lead =
    "D5 - D5 - F5 - D5 - A5 - G5 - F5 - E5 - |"
    "D5 - D5 - F5 - D5 - Bb5 - A5 - G5 - F5 - |"
    "E5 - E5 - G5 - E5 - C6 - Bb5 - A5 - G5 - |"
    "A5 - - - G#5 - - - A5 - E5 - C#5 - A4 - |"
    "D6 - . D6 C6 - A5 - . A5 F5 - A5 - D6 - |"
    "D6 - . D6 C6 - Bb5 - . Bb5 F5 - Bb5 - D6 - |"
    "D6 - C6 - Bb5 - A5 - G5 - A5 - Bb5 - G5 - |"
    "A5 - - - C#6 - - - E6 - - - A5 - . . |";

const char *const boss_bass =
    "D2 D2 D3 D2 D2 D2 D3 D2 D2 D2 D3 D2 D2 D2 D3 D2 |"
    "Bb1 Bb1 Bb2 Bb1 Bb1 Bb1 Bb2 Bb1 Bb1 Bb1 Bb2 Bb1 Bb1 Bb1 Bb2 Bb1 |"
    "C2 C2 C3 C2 C2 C2 C3 C2 C2 C2 C3 C2 C2 C2 C3 C2 |"
    "A1 A1 A2 A1 A1 A1 A2 A1 A1 A1 A2 A1 A1 A1 A2 A1 |"
    "D2 D2 D3 D2 D2 D2 D3 D2 D2 D2 D3 D2 D2 D2 D3 D2 |"
    "Bb1 Bb1 Bb2 Bb1 Bb1 Bb1 Bb2 Bb1 Bb1 Bb1 Bb2 Bb1 Bb1 Bb1 Bb2 Bb1 |"
    "G1 G1 G2 G1 G1 G1 G2 G1 G1 G1 G2 G1 G1 G1 G2 G1 |"
    "A1 A1 A2 A1 A1 A1 A2 A1 A1 A1 A2 A1 A1 A1 A2 A1 |";

const char *const boss_arp =
    "D5 F5 A5 F5 D5 F5 A5 F5 D5 F5 A5 F5 D5 F5 A5 F5 |"
    "D5 F5 Bb5 F5 D5 F5 Bb5 F5 D5 F5 Bb5 F5 D5 F5 Bb5 F5 |"
    "E5 G5 C6 G5 E5 G5 C6 G5 E5 G5 C6 G5 E5 G5 C6 G5 |"
    "E5 A5 C#6 A5 E5 A5 C#6 A5 E5 A5 C#6 A5 E5 A5 C#6 A5 |"
    "D5 F5 A5 F5 D5 F5 A5 F5 D5 F5 A5 F5 D5 F5 A5 F5 |"
    "D5 F5 Bb5 F5 D5 F5 Bb5 F5 D5 F5 Bb5 F5 D5 F5 Bb5 F5 |"
    "D5 G5 Bb5 G5 D5 G5 Bb5 G5 D5 G5 Bb5 G5 D5 G5 Bb5 G5 |"
    "E5 A5 C#6 A5 E5 A5 C#6 A5 E5 A5 C#6 A5 E5 A5 C#6 A5 |";

const char *const boss_drums =
    "k h s h k h s h k h s h k k s h |"
    "k h s h k h s h k h s h s s s s |";

}

const SongData SONG_TITLE = {120, blit::Waveform::TRIANGLE, title_lead, title_bass, title_arp, title_drums};
const SongData SONG_GAME  = {100, blit::Waveform::TRIANGLE, game_lead,  game_bass,  game_arp,  game_drums};
const SongData SONG_BOSS  = { 90, blit::Waveform::SAW,      boss_lead,  boss_bass,  boss_arp,  boss_drums};
