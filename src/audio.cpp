#include "audio.hpp"
#include "songs.hpp"
#include <vector>
#include <algorithm>

namespace audio {

namespace {

// ---------------------------------------------------------------------------
// Music
// ---------------------------------------------------------------------------

constexpr uint8_t EV_HOLD = 0, EV_OFF = 1, EV_KICK = 2, EV_SNARE = 3, EV_HAT = 4;
constexpr int MUSIC_CHANNELS = 4;
constexpr int CH_LEAD = 0, CH_BASS = 1, CH_ARP = 2, CH_DRUMS = 3;

struct ParsedSong {
    const SongData *data = nullptr;
    std::vector<uint8_t> events[MUSIC_CHANNELS];
};

ParsedSong songs[3];  // Title, Game, Boss
Song current = Song::None;
uint32_t step_acc = 0;
size_t step_pos[MUSIC_CHANNELS];
bool music_on = true;
bool sound_on = true;

// drum hit currently sounding on the drum channel (for the kick pitch drop)
uint8_t drum_kind = 0;
uint32_t drum_age = 0;

float midi_to_hz(int note) {
    return 440.0f * powf(2.0f, (note - 69) / 12.0f);
}

std::vector<uint8_t> parse_track(const char *s) {
    static const int8_t SEMITONE[7] = {9, 11, 0, 2, 4, 5, 7};  // A..G
    std::vector<uint8_t> out;
    while (*s) {
        char c = *s;
        if (c == ' ' || c == '|' || c == '\n') { s++; continue; }
        if (c == '-') out.push_back(EV_HOLD);
        else if (c == '.') out.push_back(EV_OFF);
        else if (c == 'k') out.push_back(EV_KICK);
        else if (c == 's') out.push_back(EV_SNARE);
        else if (c == 'h') out.push_back(EV_HAT);
        else if (c >= 'A' && c <= 'G') {
            int semi = SEMITONE[c - 'A'];
            const char *p = s + 1;
            if (*p == '#') { semi++; p++; }
            else if (*p == 'b') { semi--; p++; }
            int octave = (*p >= '0' && *p <= '9') ? *p - '0' : 4;
            out.push_back((uint8_t)((octave + 1) * 12 + semi));
        }
        // skip to the end of this token
        while (*s && *s != ' ' && *s != '|') s++;
    }
    return out;
}

void setup_instruments(const SongData *song) {
    AudioChannel &lead = channels[CH_LEAD];
    lead.waveforms = Waveform::SQUARE;
    lead.pulse_width = 0x5000;
    lead.volume = 0x2c00;
    lead.attack_ms = 4; lead.decay_ms = 160; lead.sustain = 0x9000; lead.release_ms = 50;

    AudioChannel &bass = channels[CH_BASS];
    bass.waveforms = song->bass_wave;
    bass.volume = song->bass_wave == Waveform::SAW ? 0x3000 : 0x6000;
    bass.attack_ms = 2; bass.decay_ms = 90; bass.sustain = 0xb000; bass.release_ms = 25;

    AudioChannel &arp = channels[CH_ARP];
    arp.waveforms = Waveform::SQUARE;
    arp.pulse_width = 0x2000;
    arp.volume = 0x1000;
    arp.attack_ms = 1; arp.decay_ms = 60; arp.sustain = 0x5000; arp.release_ms = 15;
}

void play_drum(uint8_t kind) {
    AudioChannel &ch = channels[CH_DRUMS];
    ch.off();
    drum_kind = kind;
    drum_age = 0;
    switch (kind) {
        case EV_KICK:
            ch.waveforms = Waveform::SINE;
            ch.frequency = 160;
            ch.volume = 0x7000;
            ch.attack_ms = 1; ch.decay_ms = 110; ch.sustain = 0; ch.release_ms = 1;
            break;
        case EV_SNARE:
            ch.waveforms = Waveform::NOISE;
            ch.frequency = 5000;
            ch.volume = 0x3000;
            ch.attack_ms = 1; ch.decay_ms = 120; ch.sustain = 0; ch.release_ms = 1;
            break;
        case EV_HAT:
            ch.waveforms = Waveform::NOISE;
            ch.frequency = 14000;
            ch.volume = 0x1400;
            ch.attack_ms = 1; ch.decay_ms = 35; ch.sustain = 0; ch.release_ms = 1;
            break;
    }
    ch.trigger_attack();
}

void music_step() {
    ParsedSong &song = songs[(int)current - 1];
    for (int c = 0; c < MUSIC_CHANNELS; c++) {
        auto &ev = song.events[c];
        if (ev.empty()) continue;
        uint8_t e = ev[step_pos[c]];
        step_pos[c] = (step_pos[c] + 1) % ev.size();

        if (c == CH_DRUMS) {
            if (e >= EV_KICK && e <= EV_HAT) play_drum(e);
            continue;
        }
        if (e == EV_HOLD) continue;
        if (e == EV_OFF) { channels[c].trigger_release(); continue; }
        channels[c].frequency = (uint16_t)midi_to_hz(e);
        channels[c].trigger_attack();
    }
}

void silence_music() {
    for (int c = 0; c < MUSIC_CHANNELS; c++) channels[c].off();
}

void start_song(Song song) {
    current = song;
    step_acc = 0;
    for (auto &p : step_pos) p = 0;
    silence_music();
    if (song == Song::None || !music_on) return;
    setup_instruments(songs[(int)song - 1].data);
    music_step();
}

// ---------------------------------------------------------------------------
// Sound effects
// ---------------------------------------------------------------------------

struct Step {
    uint16_t f0, f1;     // frequency sweep start / end (Hz)
    uint16_t ms;
    uint8_t wave;
    uint16_t volume;
};

struct SfxDef {
    const Step *steps;
    uint8_t count;
    int8_t channel;      // fixed channel, or -1 for round robin
};

#define SFX(name, ch, ...) \
    const Step name##_steps[] = {__VA_ARGS__}; \
    const SfxDef name = {name##_steps, sizeof(name##_steps) / sizeof(Step), ch};

constexpr uint8_t SQ = Waveform::SQUARE, TR = Waveform::TRIANGLE, SW = Waveform::SAW,
                  NZ = Waveform::NOISE, SN = Waveform::SINE;

SFX(sfx_shoot, 4, {1600, 600, 40, SQ, 0x1400})
SFX(sfx_hit, -1, {520, 260, 35, SQ, 0x2000})
SFX(sfx_explode, -1, {3000, 400, 240, NZ, 0x4800})
SFX(sfx_big_explode, -1, {2400, 150, 450, NZ, 0x6000}, {140, 40, 450, SQ, 0x3000})
SFX(sfx_player_hit, 7, {420, 110, 140, SW, 0x4000}, {2000, 300, 120, NZ, 0x3800})
SFX(sfx_player_die, 7, {700, 60, 520, SW, 0x4800}, {1500, 100, 500, NZ, 0x4800})
SFX(sfx_power_up, 7, {523, 523, 45, SQ, 0x2800}, {659, 659, 45, SQ, 0x2800},
    {784, 784, 45, SQ, 0x2800}, {1047, 1568, 120, SQ, 0x2800})
SFX(sfx_extra_life, 7, {784, 784, 60, TR, 0x5000}, {1047, 1047, 60, TR, 0x5000},
    {1319, 1319, 60, TR, 0x5000}, {1568, 1568, 60, TR, 0x5000}, {2093, 2093, 220, TR, 0x5000})
SFX(sfx_shield, 7, {300, 1400, 180, SN, 0x5000})
SFX(sfx_coin, 7, {988, 988, 50, SQ, 0x2400}, {1319, 1319, 140, SQ, 0x2400})
SFX(sfx_wave_start, 7, {440, 440, 80, SQ, 0x2400}, {659, 659, 80, SQ, 0x2400}, {880, 880, 200, SQ, 0x2400})
SFX(sfx_wave_clear, 7, {659, 659, 70, TR, 0x5000}, {784, 784, 70, TR, 0x5000},
    {988, 988, 70, TR, 0x5000}, {1319, 1319, 240, TR, 0x5000})
SFX(sfx_boss_warn, 7, {880, 880, 160, SW, 0x3000}, {660, 660, 160, SW, 0x3000},
    {880, 880, 160, SW, 0x3000}, {660, 660, 160, SW, 0x3000},
    {880, 880, 160, SW, 0x3000}, {660, 660, 160, SW, 0x3000})
SFX(sfx_menu_move, 7, {900, 1200, 25, SQ, 0x1800})
SFX(sfx_menu_select, 7, {660, 660, 45, SQ, 0x2400}, {990, 990, 90, SQ, 0x2400})
SFX(sfx_menu_back, 7, {700, 380, 70, SQ, 0x2000})
SFX(sfx_pause, 7, {523, 523, 60, TR, 0x4000}, {392, 392, 90, TR, 0x4000})
SFX(sfx_game_over, 7, {392, 392, 220, TR, 0x5000}, {330, 330, 220, TR, 0x5000},
    {262, 262, 220, TR, 0x5000}, {131, 120, 600, TR, 0x5000})
SFX(sfx_new_best, 7, {523, 523, 80, SQ, 0x2400}, {659, 659, 80, SQ, 0x2400}, {784, 784, 80, SQ, 0x2400},
    {1047, 1047, 160, SQ, 0x2400}, {784, 784, 80, SQ, 0x2400}, {1047, 1047, 80, SQ, 0x2400},
    {1319, 1319, 80, SQ, 0x2400}, {1568, 1568, 360, SQ, 0x2400})

const SfxDef *const SFX_TABLE[(int)Sfx::COUNT] = {
    &sfx_shoot, &sfx_hit, &sfx_explode, &sfx_big_explode, &sfx_player_hit, &sfx_player_die,
    &sfx_power_up, &sfx_extra_life, &sfx_shield, &sfx_coin, &sfx_wave_start, &sfx_wave_clear, &sfx_boss_warn,
    &sfx_menu_move, &sfx_menu_select, &sfx_menu_back, &sfx_pause, &sfx_game_over, &sfx_new_best,
};

struct Voice {
    const SfxDef *def = nullptr;
    uint8_t step = 0;
    uint32_t age = 0;
};

constexpr int SFX_FIRST = 4, SFX_LAST = 7;
Voice voices[SFX_LAST - SFX_FIRST + 1];
int round_robin = 0;

void start_step(int channel, Voice &v) {
    const Step &st = v.def->steps[v.step];
    AudioChannel &ch = channels[channel];
    ch.waveforms = st.wave;
    ch.frequency = st.f0;
    ch.volume = st.volume;
    ch.pulse_width = 0x7fff;
    ch.attack_ms = 2; ch.decay_ms = 4; ch.sustain = 0xffff; ch.release_ms = 30;
    ch.trigger_attack();
    v.age = 0;
}

}  // namespace

void init() {
    songs[0].data = &SONG_TITLE;
    songs[1].data = &SONG_GAME;
    songs[2].data = &SONG_BOSS;
    for (auto &s : songs) {
        s.events[CH_LEAD] = parse_track(s.data->lead);
        s.events[CH_BASS] = parse_track(s.data->bass);
        s.events[CH_ARP] = parse_track(s.data->arp);
        s.events[CH_DRUMS] = parse_track(s.data->drums);
    }
    for (int c = 0; c < CHANNEL_COUNT; c++) channels[c].off();
}

void update(uint32_t dt) {
    // music sequencer
    if (current != Song::None && music_on) {
        uint32_t step_ms = songs[(int)current - 1].data->step_ms;
        step_acc += dt;
        while (step_acc >= step_ms) {
            step_acc -= step_ms;
            music_step();
        }
        if (drum_kind == EV_KICK) {
            drum_age += dt;
            channels[CH_DRUMS].frequency = (uint16_t)std::max(40.0f, 160.0f - drum_age * 1.6f);
        }
    }

    // effect sweeps
    for (int i = 0; i <= SFX_LAST - SFX_FIRST; i++) {
        Voice &v = voices[i];
        if (!v.def) continue;
        int channel = SFX_FIRST + i;
        v.age += dt;
        const Step &st = v.def->steps[v.step];
        if (v.age >= st.ms) {
            v.step++;
            if (v.step >= v.def->count) {
                channels[channel].trigger_release();
                v.def = nullptr;
                continue;
            }
            start_step(channel, v);
            continue;
        }
        float t = (float)v.age / st.ms;
        channels[channel].frequency = (uint16_t)(st.f0 + (st.f1 - st.f0) * t);
    }
}

void play(Sfx sfx) {
    if (!sound_on) return;
    const SfxDef *def = SFX_TABLE[(int)sfx];
    int channel = def->channel;
    if (channel < 0) {
        // explosions and hits share channels 5 and 6
        channel = 5 + round_robin;
        round_robin = (round_robin + 1) % 2;
    }
    Voice &v = voices[channel - SFX_FIRST];
    v.def = def;
    v.step = 0;
    start_step(channel, v);
}

void play_song(Song song) {
    if (song == current) return;
    start_song(song);
}

void stop_music() {
    start_song(Song::None);
}

void set_music_enabled(bool on) {
    if (on == music_on) return;
    music_on = on;
    Song s = current;
    current = Song::None;
    start_song(s);
}

void set_sound_enabled(bool on) {
    sound_on = on;
    if (!on) {
        for (int c = SFX_FIRST; c <= SFX_LAST; c++) channels[c].off();
        for (auto &v : voices) v.def = nullptr;
    }
}

}
