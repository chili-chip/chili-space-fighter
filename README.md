# Ultra Hyper Space Super Fighters TURBO

![Build](https://github.com/chili-chip/chili-space-fighter/workflows/Build/badge.svg)

The galaxy is in danger! The evil aliens are invading and it's up to you to stop them.

A vertical arcade shooter for the [VGC Zero](https://chilichip.eu) (128x128), built on the
[chili-chip fork of the 32blit SDK](https://github.com/chili-chip/32blit-sdk). It also runs on
desktop (SDL) and in the browser: **[play it on the web](https://space-fighter-game.chilichip.eu/)**.

## Features

- Endless numbered waves built from enemy formations: dives, zig-zags, swoops, loops, side sweeps
  and squadrons that hover in formation before diving at you
- Five enemy types with their own fire patterns, getting faster and tougher every wave
- A boss every fifth wave, with fan, burst and spiral bullet patterns and an enraged second phase
- Power-ups: weapon level (twin, spread, quad, max), rapid fire, shield, repair, extra life and bonus coins
- Combo multiplier for chaining kills, no-hit wave bonus, extra life every 10,000 points
- Top-5 high score table with three-letter names, saved on the device
- Original chiptune soundtrack (title, in-game and boss themes) and synthesised sound effects
- Five ship colours to pick from in Options; music and sound can be switched off separately

## Controls

| Button | Action |
| --- | --- |
| D-pad | Fly |
| A (hold) | Fire |
| MENU | Pause |
| A / B | Confirm / back in menus |

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

See the [VGC build docs](https://github.com/chili-chip/32blit-sdk/blob/master/docs/vgc.md) for flashing.
The web build is produced by CI with Emscripten and deployed from `main`.

## Code layout

| File | What it does |
| --- | --- |
| `src/game.cpp` | 32blit entry points, title screen, menus, pause, game over and name entry |
| `src/world.cpp` | One run: wave schedule, collisions, scoring, HUD and banners |
| `src/player.cpp` | Player ship, weapons, hull, shield and respawn |
| `src/enemy.cpp` | Enemy types, movement patterns and the boss |
| `src/powerup.cpp` | Power-up drops |
| `src/projectile_manager.cpp` | Bullet pool |
| `src/fx.cpp` | Explosions, particles, score popups and screen shake |
| `src/starfield.cpp` | Parallax starfield |
| `src/audio.cpp`, `src/songs.cpp` | Sequencer, sound effects and the soundtrack |
| `src/save.cpp` | Settings and high scores |
| `tools/make_art.py` | Regenerates the TURBO logo, launcher splash and icon |

## Credits

Game by teriyakigod / [Chili Chip](https://chilichip.eu). Music and sound effects are original and
released under the same MIT licence as the code.
