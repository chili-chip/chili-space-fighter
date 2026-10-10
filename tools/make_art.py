#!/usr/bin/env python3
"""Generates the title logo, launcher splash and icon from code.

Run from the repository root:  python3 tools/make_art.py
Needs Pillow. Writes assets/logo.png, assets/splash.png and assets/icon.png.
"""
import random
from PIL import Image

GLYPHS = {
    'T': ["######", "######", "..##..", "..##..", "..##..", "..##..", "..##..", "..##.."],
    'U': ["##..##", "##..##", "##..##", "##..##", "##..##", "##..##", "######", ".####."],
    'R': ["#####.", "######", "##..##", "##..##", "#####.", "####..", "##.##.", "##..##"],
    'B': ["#####.", "##..##", "##..##", "#####.", "##..##", "##..##", "######", "#####."],
    'O': [".####.", "######", "##..##", "##..##", "##..##", "##..##", "######", ".####."],
}
GW, GH = 6, 8
SCALE = 3
SHEAR = 5          # rows per 1px italic shift
OUTLINE = (90, 10, 30, 255)
GRADIENT = [(255, 250, 200), (255, 230, 90), (255, 190, 40), (255, 130, 30), (240, 70, 40), (190, 30, 60)]


def make_logo(text="TURBO"):
    gh, gw = GH * SCALE, GW * SCALE
    gap = SCALE
    w = len(text) * gw + (len(text) - 1) * gap
    h = gh
    shear_max = h // SHEAR
    img_w, img_h = w + shear_max + 2, h + 3   # outline + drop shadow
    mask = [[False] * img_w for _ in range(img_h)]
    for i, ch in enumerate(text):
        g = GLYPHS[ch]
        ox = i * (gw + gap)
        for gy, row in enumerate(g):
            for gx, c in enumerate(row):
                if c != '#':
                    continue
                for sy in range(SCALE):
                    for sx in range(SCALE):
                        y = gy * SCALE + sy
                        x = ox + gx * SCALE + sx + (h - 1 - y) // SHEAR
                        mask[y + 1][x + 1] = True
    img = Image.new('RGBA', (img_w, img_h), (0, 0, 0, 0))
    px = img.load()
    # drop shadow + outline
    for y in range(img_h):
        for x in range(img_w):
            if mask[y][x]:
                continue
            near = any(0 <= y + dy < img_h and 0 <= x + dx < img_w and mask[y + dy][x + dx]
                       for dy in (-1, 0, 1) for dx in (-1, 0, 1))
            if near:
                px[x, y] = OUTLINE
            elif y >= 2 and x >= 1 and mask[y - 2][x - 1]:
                px[x, y] = (30, 0, 40, 255)
    for y in range(img_h):
        for x in range(img_w):
            if mask[y][x]:
                t = (y - 1) / max(1, h - 1)
                c = GRADIENT[min(len(GRADIENT) - 1, int(t * len(GRADIENT)))]
                # highlight the top edge of each stroke
                if y > 0 and not mask[y - 1][x]:
                    c = (255, 255, 235)
                px[x, y] = c + (255,)
    return img


def make_splash(logo):
    random.seed(7)
    img = Image.new('RGBA', (128, 96), (4, 2, 18, 255))
    px = img.load()
    for _ in range(90):
        x, y = random.randrange(128), random.randrange(96)
        b = random.choice([70, 140, 240])
        px[x, y] = (b, b, min(255, b + 40), 255)
    ships = Image.open('assets/ships.png').convert('RGBA')
    misc = Image.open('assets/misc.png').convert('RGBA')

    def cell(sheet, idx, cols, span=1):
        x, y = (idx % cols) * 8, (idx // cols) * 8
        return sheet.crop((x, y, x + 8 * span, y + 8 * span))

    img.alpha_composite(logo, ((128 - logo.width) // 2, 8))
    # boss looming and a few enemies
    img.alpha_composite(cell(ships, 64, 10, 2).resize((32, 32), Image.NEAREST), (88, 40))
    for idx, pos in [(6, (12, 44)), (17, (54, 60)), (39, (62, 38)), (4, (76, 54))]:
        img.alpha_composite(cell(ships, idx, 10), pos)
    # player ship, doubled, with its engine flame
    img.alpha_composite(cell(misc, 19, 13).resize((16, 16), Image.NEAREST), (28, 82))
    img.alpha_composite(cell(ships, 1, 10).resize((16, 16), Image.NEAREST), (28, 68))
    # a couple of shots
    shot = cell(Image.open('assets/projectiles.png').convert('RGBA'), 2, 6)
    for y in (60, 50, 40):
        img.alpha_composite(shot, (30, y))
        img.alpha_composite(shot, (34, y))
    return img


def make_icon():
    ships = Image.open('assets/ships.png').convert('RGBA')
    icon = Image.new('RGBA', (8, 8), (4, 2, 18, 255))
    icon.alpha_composite(ships.crop((8, 0, 16, 8)))
    return icon


if __name__ == '__main__':
    logo = make_logo()
    logo.save('assets/logo.png')
    make_splash(logo).save('assets/splash.png')
    make_icon().save('assets/icon.png')
    print('logo', logo.size)
