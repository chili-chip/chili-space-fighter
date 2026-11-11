#!/usr/bin/env python3
"""Converts public domain ProTracker songs into note sheets for the game's chiptune player.

The game does not play music files. Its sequencer (src/audio.cpp) plays four text tracks per song:
lead, bass, arp and drums, one token per step. This tool reads a .mod file, picks which instrument
of the original plays which part, and writes those tracks into src/songs.cpp.

Run from the repository root (downloads the songs from The Mod Archive):

    python3 tools/mod_to_song.py

Each song in SONGS says which samples (instruments) of the .mod become which part. A sample can be
limited to one channel with "sample@channel". Samples not listed are left out. Notes are moved by
whole octaves so each part sits in a range the console's square waves play well.
"""
import math
import os
import struct
import sys
import urllib.request

# role targets: median MIDI note each part is moved to (by octaves)
TARGET = {'lead': 74, 'arp': 67, 'bass': 43}

# Songs used in the game. All are public domain on The Mod Archive (modarchive.org).
SONGS = [
    dict(
        name='TITLE', id=205968, title='XLR8', author='Jam',
        bass_wave='TRIANGLE',
        lead=['6@2', '6@3', '8@3'], arp=['8@2', '5', '7'], bass=['4'],
        kick=['1'], snare=['2'], hat=['3'],
    ),
    dict(
        name='STAGE_A', id=211660, title='Highway Encounter', author='Jam',
        bass_wave='TRIANGLE',
        lead=['2', '3', '4', '9', '12', '13'], arp=['5@2'], bass=['5@1'],
        kick=['7'], snare=['6'], hat=['8'],
    ),
    dict(
        name='STAGE_B', id=195782, title='8-bit Castle', author='Jam',
        bass_wave='SQUARE',
        lead=['1@1', '1@3'], arp=['18', '6', '2'], bass=['1@0', '1@2'],
        kick=['7'], snare=['10', '3'], hat=['9'],
    ),
    dict(
        name='BOSS', id=197296, title='May is 4 Her', author='Drozerix',
        bass_wave='SAW',
        lead=['2'], arp=['7', '8'], bass=['1'],
        kick=['3', '4'], snare=['6'], hat=['5'],
    ),
    dict(
        name='STORY', id=208535, title='heatwave cruise', author='Jam',
        bass_wave='TRIANGLE',
        lead=['2', '7@3'], arp=['10', '7@2'], bass=['3'],
        kick=['6'], snare=['5'], hat=[],
    ),
    dict(
        name='ENDING', id=208532, title='late night coffee', author='Jam',
        bass_wave='TRIANGLE',
        lead=['7'], arp=['6', '10', '20', '21'], bass=['3'],
        kick=['18'], snare=['16', '14'], hat=['12', '13'],
    ),
]

URL = 'https://api.modarchive.org/downloads.php?moduleid={}'
CACHE = os.path.join(os.path.dirname(__file__), '.mod-cache')
NOTE_NAMES = ['C', 'C#', 'D', 'D#', 'E', 'F', 'F#', 'G', 'G#', 'A', 'A#', 'B']
AMIGA_C3 = 428      # ProTracker period of C-3, played here as MIDI 60
SAMPLE_RATE_C3 = 8287


def load_mod(data):
    samples = []
    for i in range(31):
        o = 20 + i * 30
        length, finetune, volume, loop_start, loop_len = struct.unpack('>HbBHH', data[o + 22:o + 30])
        samples.append(dict(length=length * 2, loops=loop_len > 1))
    song_len = data[950]
    order = list(data[952:952 + 128])[:song_len]
    if data[1080:1084] not in (b'M.K.', b'M!K!', b'FLT4', b'4CHN'):
        raise SystemExit('only 4 channel ProTracker songs are supported')
    patterns = []
    o = 1084
    for _ in range(max(order) + 1):
        rows = []
        for _ in range(64):
            row = []
            for _ in range(4):
                b = data[o:o + 4]
                o += 4
                row.append(((b[0] & 0xF0) | (b[2] >> 4), ((b[0] & 0x0F) << 8) | b[1], b[2] & 0x0F, b[3]))
            rows.append(row)
        patterns.append(rows)
    return samples, order, patterns


def walk(order, patterns):
    """Rows in play order, following pattern breaks (Dxx) and jumps (Bxx) until the song loops."""
    speed, bpm = 6, 125
    pos, start, seen = 0, 0, set()
    while pos < len(order) and (pos, start) not in seen:
        seen.add((pos, start))
        jump = None
        for r in range(start, 64):
            row = patterns[order[pos]][r]
            for _, _, fx, param in row:
                if fx == 0xF and param:
                    if param < 32:
                        speed = param
                    else:
                        bpm = param
            yield row, speed, bpm
            for _, _, fx, param in row:
                if fx == 0xD:
                    jump = (pos + 1, (param >> 4) * 10 + (param & 15))
                if fx == 0xB:
                    jump = (param, 0) if param > pos else (len(order), 0)
            if jump:
                break
        pos, start = jump if jump else (pos + 1, 0)


def midi(period):
    return round(12 * math.log2(AMIGA_C3 / period)) + 60


def matcher(spec):
    out = []
    for s in spec:
        sample, _, ch = s.partition('@')
        out.append((int(sample), int(ch) if ch else None))
    return lambda sample, ch: any(sample == s and (c is None or c == ch) for s, c in out)


def name(n):
    return f'{NOTE_NAMES[n % 12]}{n // 12 - 1}'


def convert(song, data):
    samples, order, patterns = load_mod(data)
    rows = list(walk(order, patterns))
    speed, bpm = rows[0][1], rows[0][2]
    step_ms = round(2500 * speed / bpm)
    roles = {r: matcher(song.get(r, [])) for r in ('lead', 'arp', 'bass', 'kick', 'snare', 'hat')}

    melodic = {r: [] for r in ('lead', 'arp', 'bass')}   # per row: note, or None, or 'off'
    drums = []
    for row, _, _ in rows:
        picked = {r: [] for r in melodic}
        drum = '.'
        for ch, (sample, period, fx, param) in enumerate(row):
            cut = (fx == 0xC and param == 0) or (fx == 0xE and param >> 4 == 0xC)
            if not period:
                for r in melodic:
                    if cut and roles[r](sample or -1, ch):
                        picked[r].append('off')
                continue
            for r in ('kick', 'snare', 'hat'):
                if roles[r](sample, ch) and (drum == '.' or 'ksh'.index(r[0]) < 'ksh'.index(drum)):
                    drum = r[0]
            for r in melodic:
                if roles[r](sample, ch):
                    picked[r].append((midi(period), samples[sample - 1], period))
        drums.append(drum)
        for r in melodic:
            notes = [p for p in picked[r] if p != 'off']
            if notes:
                # the melody is the top voice, the bass the bottom one
                melodic[r].append(min(notes, key=lambda n: n[0]) if r == 'bass' else max(notes, key=lambda n: n[0]))
            else:
                melodic[r].append('off' if picked[r] else None)

    tracks = {}
    for r, events in melodic.items():
        notes = sorted(e[0] for e in events if isinstance(e, tuple))
        shift = 12 * round((TARGET[r] - notes[len(notes) // 2]) / 12) if notes else 0
        tokens, sounding_until = [], -1
        for i, e in enumerate(events):
            if isinstance(e, tuple):
                n, smp, period = e
                tokens.append(name(min(max(n + shift, 24), 96)))
                # a one shot sample stops on its own: release when it would have ended
                if smp['loops']:
                    sounding_until = None
                else:
                    seconds = smp['length'] / (SAMPLE_RATE_C3 * AMIGA_C3 / period)
                    sounding_until = i + max(1, round(seconds * 1000 / step_ms))
            elif e == 'off' or (sounding_until is not None and i == sounding_until):
                tokens.append('.')
                sounding_until = -1
            else:
                tokens.append('-' if sounding_until != -1 else '.')
        tracks[r] = tokens
    tracks['drums'] = drums
    return step_ms, tracks


def bars(tokens):
    lines = []
    for i in range(0, len(tokens), 16):
        lines.append('    "' + ' '.join(tokens[i:i + 16]) + ' |"')
    return '\n'.join(lines)


def fetch(song_id):
    os.makedirs(CACHE, exist_ok=True)
    path = os.path.join(CACHE, f'{song_id}.mod')
    if not os.path.exists(path):
        req = urllib.request.Request(URL.format(song_id), headers={'User-Agent': 'mod_to_song'})
        with urllib.request.urlopen(req) as r, open(path, 'wb') as f:
            f.write(r.read())
    with open(path, 'rb') as f:
        return f.read()


def main():
    out = [
        '// Generated by tools/mod_to_song.py. Do not edit by hand: change the tool and run it again.',
        '//',
        '// Note sheets converted from public domain ProTracker songs on The Mod Archive:',
    ]
    for s in SONGS:
        out.append(f'//   {s["title"]} by {s["author"]}, https://modarchive.org/module.php?{s["id"]}')
    out += [
        '//',
        '// Notation: one token per step, \'|\' marks 16 steps.',
        '//   C5, F#4  note on      -  hold      .  note off / rest',
        '// Drums: k kick, s snare, h hi-hat.',
        '',
        '#include "songs.hpp"',
        '',
        'namespace {',
    ]
    defs = []
    for s in SONGS:
        step_ms, tracks = convert(s, fetch(s['id']))
        key = s['name'].lower()
        for part in ('lead', 'bass', 'arp', 'drums'):
            out.append(f'\nconst char *const {key}_{part} =\n{bars(tracks[part])};')
        defs.append(f'const SongData SONG_{s["name"]} = {{{step_ms}, blit::Waveform::{s["bass_wave"]}, '
                    f'{key}_lead, {key}_bass, {key}_arp, {key}_drums}};')
        print(f'{s["name"]:8s} {s["title"]}: {len(tracks["lead"])} steps of {step_ms} ms', file=sys.stderr)
    out += ['', '}  // namespace', ''] + defs + ['']
    path = os.path.join(os.path.dirname(__file__), '..', 'src', 'songs.cpp')
    with open(path, 'w') as f:
        f.write('\n'.join(out))


if __name__ == '__main__':
    main()
