# Ultra Hyper Space Super Fighters TURBO

![Build](https://github.com/chili-chip/chili-space-fighter/actions/workflows/build.yml/badge.svg)

A 32blit space shooter for [ChiliChip VGC Zero](https://github.com/chili-chip/32blit-sdk/blob/master/docs/vgc.md).

**Metadata title:** `space-game`

The galaxy is in danger! The evil aliens are invading and it's up to you to stop them!

[Play in the browser](https://space-fighter-game.chilichip.eu/)

## Controls

Mapped in `Player::get_ship_controls()` (`src/player.cpp`): fly with the D-pad, fire with **A**.

| Action | VGC Zero | Desktop / web |
| ------ | -------- | ------------- |
| Fly    | D-pad    | Arrow keys or WASD |
| Fire   | **A**    | **Z** or **U** |

Desktop and web keys follow the 32blit SDL mapping (`Button::DPAD_*` and `Button::A`).

## Play on VGC Zero

CI packages a UF2 as `bin/game.uf2` inside the **VGC** release zip (`chili-space-fighter-<tag>-VGC.zip`).

1. Connect the console over USB.
2. Power it off, then hold the top **X** button and press **Power**.
3. The device appears as `RPI-RP2`.
4. Copy `game.uf2` onto the `RPI-RP2` volume. The console reboots into the game.

Hardware and build details: [VGC Zero docs](https://github.com/chili-chip/32blit-sdk/blob/master/docs/vgc.md)

## Build from source

Desktop (SDL):

```bash
mkdir build && cd build
cmake -D32BLIT_DIR=/path/to/32blit-sdk ..
cmake --build .
```

VGC Zero (produces `game.uf2`):

```bash
mkdir build.vgc && cd build.vgc
cmake .. \
  -D32BLIT_DIR=../../32blit-sdk \
  -DPICO_SDK_PATH=../../pico-sdk \
  -DPICO_BOARD=chilichip_vgc \
  -DPICO_PLATFORM=rp2350-arm-s \
  -DCMAKE_TOOLCHAIN_FILE=../../32blit-sdk/pico2.toolchain
cmake --build .
```

GitHub Actions builds **VGC** (UF2) and desktop (Linux, macOS, Windows), plus STM32 and web. Publishing a GitHub release attaches those artifacts, including VGC.
