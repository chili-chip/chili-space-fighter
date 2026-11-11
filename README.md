# Ultra Hyper Space Super Fighters TURBO

[![Build](https://github.com/chili-chip/chili-space-fighter/actions/workflows/build.yml/badge.svg)](https://github.com/chili-chip/chili-space-fighter/actions/workflows/build.yml)

The galaxy is in danger! The evil aliens are invading and it's up to you to stop them.

A vertical arcade shooter for the [VGC Zero](https://chilichip.eu) (128x128), built on the
[chili-chip fork of the 32blit SDK](https://github.com/chili-chip/32blit-sdk). It also runs on
desktop (SDL) and in the browser: **[play it on the web](https://space-fighter-game.chilichip.eu/)**.

## Features

- A story told between waves: six chapters with a boss each, a cast of ten, then endless Free Lanes
- Endless numbered waves built from enemy formations: dives, zig-zags, swoops, loops, side sweeps
  and squadrons that hover in formation before diving at you
- Five enemy types with their own fire patterns, getting faster and tougher every wave
- A boss every fifth wave, with fan, burst and spiral bullet patterns and an enraged second phase
- Power-ups: weapon level (twin, spread, quad, max), rapid fire, shield, repair, extra life and bonus coins
- Combo multiplier for chaining kills, no-hit wave bonus, extra life every 10,000 points
- Top-5 high score table with three-letter names, saved on the device
- Original chiptune soundtrack (title, in-game and boss themes) and synthesised sound effects
- Five ship colours to pick from in Options; music and sound can be switched off separately

## Story

The game tells the story **Open Sky** in scenes between waves: six chapters of five waves each,
then the endless Free Lanes. A shows the next page of a speech and B skips a scene.

The world, the cast from `assets/characters.png`, the ships and the full script are in
[docs/story](docs/story/index.md). The dialogue lives only in
[docs/story/script.md](docs/story/script.md); the build turns it into code, so to change a line,
edit the script and build. See [Changing the dialogue](docs/story/index.md#changing-the-dialogue).

![Dialogue box](docs/story/img/game-dialogue.png)

## Controls

| Button | Action |
| --- | --- |
| D-pad | Fly |
| A (hold) | Fire |
| MENU | Pause |
| A / B | Confirm / back in menus |
| A / B in a story scene | Next page / skip the scene |

On desktop the keyboard maps to: arrows or WASD for the D-pad, `Z` for A, `X` for B and `Esc` or `2` for MENU.

## Building

You need the [chili-chip 32blit SDK](https://github.com/chili-chip/32blit-sdk) checked out next to this repository.

Desktop (SDL):

```
mkdir build
cd build
cmake -D32BLIT_DIR=../../32blit-sdk ..
make
./game.elf
```

VGC Zero:

```
mkdir build.vgc
cd build.vgc
cmake .. \
  -D32BLIT_DIR=../../32blit-sdk \
  -DPICO_SDK_PATH=../../pico-sdk \
  -DPICO_BOARD=chilichip_vgc \
  -DPICO_PLATFORM=rp2350-arm-s \
  -DCMAKE_TOOLCHAIN_FILE=../../32blit-sdk/pico2.toolchain
make
```
To build an installable `.blit` for the VGC Zero loader instead of a standalone `.uf2`, add
`-DBLIT_EXECUTABLE_PICO_BLIT=1` to the vgc cmake command (or use the `vgc-blit` preset), then install it over USB with
`32blit install game.blit`. The console needs the loader flashed first; see the
[loader instructions](https://github.com/chili-chip/32blit-sdk/blob/master/docs/vgc.md#loader-and-blit-games-os-preview).

[detailed instructions for vgc](https://github.com/chili-chip/32blit-sdk/blob/master/docs/vgc.md)

See the [VGC build docs](https://github.com/chili-chip/32blit-sdk/blob/master/docs/vgc.md) for flashing.
The web build is produced by CI with Emscripten and deployed from `main`.

## Code layout

| File | What it does |
| --- | --- |
| `src/game.cpp` | 32blit entry points, title screen, menus, pause, game over and name entry |
| `src/world.cpp` | One run: wave schedule, collisions, scoring, HUD, banners and when story scenes play |
| `src/story.cpp` | Story text box: wrapping, pages, portraits and title cards |
| `src/player.cpp` | Player ship, weapons, hull, shield and respawn |
| `src/enemy.cpp` | Enemy types, movement patterns and the boss |
| `src/powerup.cpp` | Power-up drops |
| `src/projectile_manager.cpp` | Bullet pool |
| `src/fx.cpp` | Explosions, particles, score popups and screen shake |
| `src/starfield.cpp` | Parallax starfield |
| `src/audio.cpp`, `src/songs.cpp` | Sequencer, sound effects and the soundtrack |
| `src/save.cpp` | Settings and high scores |
| `tools/make_art.py` | Regenerates the TURBO logo, launcher splash and icon |
| `tools/make_script.py` | Turns `docs/story/script.md` into `story_script.cpp` at build time |

## Credits

Game by teriyakigod / [Chili Chip](https://chilichip.eu). Music and sound effects are original and
released under the same MIT licence as the code.

The sprites in `assets/ships.png`, `characters.png`, `projectiles.png`, `ui.png` and `misc.png`
come from the [8x8 Space Shooter Asset Pack](https://gvituri.itch.io/space-shooter) by
Gustavo Vituri. They are not under the MIT licence. The pack allows use and changes in personal
and commercial games, but not passing the files on as an asset pack, so do not reuse them
outside this game. The logo, splash and icon are made by `tools/make_art.py`.
