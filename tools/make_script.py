#!/usr/bin/env python3
"""Turns docs/story/script.md into C++ data for the game (story_script.cpp).

CMake runs this on every build where the script changed, so the script in
docs/story/script.md is the only place the dialogue lives. To run it by hand:

    python3 tools/make_script.py docs/story/script.md story_script.cpp

The format is described in docs/story/index.md (Script format). The tool stops
with a line number when the script has something the game can't show.
"""
import re
import sys

# name in the script -> (portrait row in assets/characters.png, name shown in the box)
SPEAKERS = {
    'BRASK': (0, 'BRASK'),
    'AUX-7': (1, 'AUX-7'),
    'KIT': (1, 'KIT'),
    'ROOK': (2, 'ROOK'),
    'ZIX': (3, 'ZIX'),
    'JUNO': (4, 'JUNO'),
    'WISP': (5, 'WISP'),
    'OSSA': (6, 'OSSA'),
    'LUMO': (7, 'LUMO'),
    'NOVA': (8, 'NOVA'),
    'DOC': (9, 'DOC'),
}
STATIC_ROW = 10
CARD_ROW = -1          # a full screen card (title card, chapter card), no face
CHAPTERS = 6
CHAPTER_SCENES = ['card', 'start', 'radio', 'boss_warning', 'after_boss', 'game_over']
HEADINGS = {
    'radio': 'radio',
    'boss warning': 'boss_warning',
    'after the boss': 'after_boss',
    'game over': 'game_over',
    'game over in free lanes': 'game_over',
}


class ScriptError(Exception):
    pass


def card(lineno, text):
    """'A / B / C. D' -> 'A\\nB\\nC\\nD'."""
    lines = []
    for part in text.split('/'):
        lines += [p.strip().rstrip('.') for p in part.split('. ')]
    lines = [l for l in lines if l]
    if not lines or len(lines) > 5:
        raise ScriptError(f'line {lineno}: a card needs 1 to 5 lines')
    return {'row': CARD_ROW, 'frame': 0, 'name': None, 'text': '\n'.join(lines)}


def parse_block(lines):
    """One ```text block -> list of speeches. lines are (lineno, text)."""
    speeches = []
    speaker = None
    for lineno, line in lines + [(0, '')]:
        line = line.strip()
        m = re.fullmatch(r'\[([A-Z0-9-]+)(?: (\d))?\]', line)
        if m:
            if speaker:
                raise ScriptError(f'line {speaker["lineno"]}: [{speaker["tag"]}] has no text')
            name, frame = m.group(1), m.group(2)
            if name == 'STATIC':
                speaker = {'row': STATIC_ROW, 'frame': 0, 'name': None}
            elif name in SPEAKERS:
                if frame is None or not 0 <= int(frame) <= 4:
                    raise ScriptError(f'line {lineno}: [{name}] needs a frame from 0 to 4')
                row, shown = SPEAKERS[name]
                speaker = {'row': row, 'frame': int(frame), 'name': shown}
            else:
                raise ScriptError(f'line {lineno}: unknown speaker {name}')
            speaker['lineno'], speaker['tag'] = lineno, line[1:-1]
            continue
        if speaker:
            if not line:
                raise ScriptError(f'line {lineno}: [{speaker["tag"]}] has no text')
            # "(the light says) It is the Prism." -> the direction is not shown
            text = re.sub(r'^\([^)]*\)\s*', '', line)
            if not text:
                raise ScriptError(f'line {lineno}: [{speaker["tag"]}] has no text')
            bad = [c for c in text if not 32 <= ord(c) < 127]
            if bad:
                raise ScriptError(f'line {lineno}: the console font has no {bad[0]!r}')
            speeches.append({**speaker, 'text': text})
            speaker = None
            continue
        m = re.fullmatch(r'\(title card: (.*)\)', line)
        if m:
            speeches.append(card(lineno, m.group(1)))
        # other lines in ( ) are directions for people reading the script; blank lines separate speeches
    return speeches


def parse(md):
    """script.md -> {'intro': [...], 'chapters': [{scene: [...]}], 'ending': [...], 'free_lanes': [...], ...}"""
    story = {'chapters': [{} for _ in range(CHAPTERS)]}
    section = sub = None
    block = None
    for lineno, line in enumerate(md.splitlines(), 1):
        if block is not None:
            if line.startswith('```'):
                store(story, section, sub, lineno, parse_block(block), block)
                block = None
            else:
                block.append((lineno, line))
            continue
        if line.startswith('```text'):
            block = []
        elif line.startswith('## '):
            title = line[3:].strip().lower()
            m = re.match(r'chapter (\d)', title)
            section = int(m.group(1)) if m else title
            sub = None
        elif line.startswith('### '):
            sub = line[4:].strip().lower()
    # every scene is optional (the game skips a missing one) except the chapter cards
    for i, ch in enumerate(story['chapters'], 1):
        if 'card' not in ch:
            raise ScriptError(f'chapter {i} has no chapter card')
    return story


def store(story, section, sub, lineno, speeches, raw):
    where = f'block ending on line {lineno}'
    if section == 'intro':
        story['intro'] = speeches
    elif section == 'ending':
        story['ending'] = speeches
    elif section == 'free lanes':
        story['free_lanes_game_over' if sub else 'free_lanes'] = speeches
    elif isinstance(section, int) and 1 <= section <= CHAPTERS:
        ch = story['chapters'][section - 1]
        if sub == 'chapter card':
            if 'card' not in ch:
                text = [l.strip() for _, l in raw if l.strip()]
                if len(text) != 3:
                    raise ScriptError(f'{where}: a chapter card is 3 lines (chapter, title, place)')
                ch['card'] = text
            else:
                ch['start'] = speeches
        elif sub in HEADINGS:
            ch[HEADINGS[sub]] = speeches
        else:
            raise ScriptError(f'{where}: no scene called "{sub}" in chapter {section}')
    else:
        raise ScriptError(f'{where}: not in a scene the game knows')


def c_str(s):
    if s is None:
        return 'nullptr'
    return '"' + s.replace('\\', '\\\\').replace('"', '\\"').replace('\n', '\\n') + '"'


def emit(story):
    out = ['// Generated by tools/make_script.py from docs/story/script.md. Edit the script, not this file.',
           '#include "story.hpp"', '', 'namespace story {', '', 'namespace {']
    arrays = {}

    def array(name, speeches):
        out.append(f'const Speech {name}[] = {{')
        for s in speeches:
            out.append(f'    {{{c_str(s["name"])}, {s["row"]}, {s["frame"]}, {c_str(s["text"])}}},')
        out.append('};')
        arrays[name] = len(speeches)

    def scene(name):
        return f'{{{name}, {arrays[name]}}}' if name in arrays else '{nullptr, 0}'

    for key in ('intro', 'ending', 'free_lanes', 'free_lanes_game_over'):
        if story.get(key):
            array(f'{key}_lines', story[key])
    for i, ch in enumerate(story['chapters'], 1):
        for key in CHAPTER_SCENES[1:]:
            if ch.get(key):
                array(f'ch{i}_{key}', ch[key])
    out += ['}  // namespace', '']
    for key in ('intro', 'ending', 'free_lanes', 'free_lanes_game_over'):
        out.append(f'const Scene {key} = {scene(f"{key}_lines")};')
    out += ['', f'const Chapter chapters[{CHAPTERS}] = {{']
    for i, ch in enumerate(story['chapters'], 1):
        out.append('    {')
        out.append('        {' + ', '.join(c_str(t) for t in ch['card']) + '},')
        for key in CHAPTER_SCENES[1:]:
            out.append(f'        {scene(f"ch{i}_{key}")},')
        out.append('    },')
    out += ['};', '', '}  // namespace story', '']
    return '\n'.join(out)


def main(argv):
    if len(argv) != 3:
        print(__doc__)
        return 2
    with open(argv[1], encoding='utf-8') as f:
        md = f.read()
    try:
        cpp = emit(parse(md))
    except ScriptError as e:
        print(f'{argv[1]}: {e}', file=sys.stderr)
        return 1
    with open(argv[2], 'w', encoding='utf-8') as f:
        f.write(cpp)
    return 0


if __name__ == '__main__':
    sys.exit(main(sys.argv))
