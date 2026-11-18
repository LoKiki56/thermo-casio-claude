# Génère src/font5.h : police bitmap 5x7 (cellule 6x8) pour écran monochrome 128x64.
# Glyphes ASCII : table classique 5x7 en colonnes (bit0 = ligne du haut).
# Lettres accentuées : lettre de base + accent dans les lignes 0-1 (libres pour les minuscules).
import sys
from PIL import Image

ASCII = """
00 00 00 00 00|00 00 5F 00 00|00 07 00 07 00|14 7F 14 7F 14|24 2A 7F 2A 12|23 13 08 64 62|36 49 56 20 50|00 05 03 00 00
00 1C 22 41 00|00 41 22 1C 00|14 08 3E 08 14|08 08 3E 08 08|00 50 30 00 00|08 08 08 08 08|00 60 60 00 00|20 10 08 04 02
3E 51 49 45 3E|00 42 7F 40 00|42 61 51 49 46|21 41 45 4B 31|18 14 12 7F 10|27 45 45 45 39|3C 4A 49 49 30|01 71 09 05 03
36 49 49 49 36|06 49 49 29 1E|00 36 36 00 00|00 56 36 00 00|08 14 22 41 00|14 14 14 14 14|00 41 22 14 08|02 01 51 09 06
32 49 79 41 3E|7E 11 11 11 7E|7F 49 49 49 36|3E 41 41 41 22|7F 41 41 22 1C|7F 49 49 49 41|7F 09 09 01 01|3E 41 41 51 32
7F 08 08 08 7F|00 41 7F 41 00|20 40 41 3F 01|7F 08 14 22 41|7F 40 40 40 40|7F 02 04 02 7F|7F 04 08 10 7F|3E 41 41 41 3E
7F 09 09 09 06|3E 41 51 21 5E|7F 09 19 29 46|46 49 49 49 31|01 01 7F 01 01|3F 40 40 40 3F|1F 20 40 20 1F|3F 40 38 40 3F
63 14 08 14 63|07 08 70 08 07|61 51 49 45 43|00 7F 41 41 00|02 04 08 10 20|00 41 41 7F 00|04 02 01 02 04|40 40 40 40 40
00 01 02 04 00|20 54 54 54 78|7F 48 44 44 38|38 44 44 44 20|38 44 44 48 7F|38 54 54 54 18|08 7E 09 01 02|08 14 54 54 3C
7F 08 04 04 78|00 44 7D 40 00|20 40 44 3D 00|00 7F 10 28 44|00 41 7F 40 00|7C 04 18 04 78|7C 08 04 04 78|38 44 44 44 38
7C 14 14 14 08|08 14 14 18 7C|7C 08 04 04 08|48 54 54 54 20|04 3F 44 40 20|3C 40 40 20 7C|1C 20 40 20 1C|3C 40 30 40 3C
44 28 10 28 44|0C 50 50 50 3C|44 64 54 4C 44|00 08 36 41 00|00 00 7F 00 00|00 41 36 08 00
"""
cols = [bytes(int(h, 16) for h in g.split()) for g in "|".join(l for l in ASCII.split("\n") if l.strip()).split("|")]
cols = [c for c in cols if len(c) == 5]
assert len(cols) == 94, len(cols)       # 0x20..0x7D ; '~' ajouté plus bas
G = {32 + i: list(c) for i, c in enumerate(cols)}

def art(rows, top=0):
    out = [0] * 5
    for r, line in enumerate(rows):
        assert len(line) == 5, line
        for c, ch in enumerate(line):
            if ch == '#': out[c] |= 1 << (r + top)
    return out

G[ord('~')] = art([".##.#", "#..#."], 3)
G[ord('γ')] = art(["#...#", ".#.#.", "..#..", "..#..", "..#..", "..#.."], 2)
G[ord('Δ')] = art(["..#..", ".#.#.", ".#.#.", "#...#", "#...#", "#...#", "#####"], 0)
G[ord('η')] = art(["####.", "#...#", "#...#", "#...#", "#...#", "....#"], 2)
G[ord('ρ')] = art([".###.", "#...#", "#...#", "#...#", "####.", "#...."], 2)
G[ord('→')] = art(["..#..", "...#.", "#####", "...#.", "..#.."], 1)
G[ord('←')] = art(["..#..", ".#...", "#####", ".#...", "..#.."], 1)
G[ord('↑')] = art(["..#..", ".###.", "#.#.#", "..#..", "..#..", "..#..", "..#.."], 0)
G[ord('↓')] = art(["..#..", "..#..", "..#..", "..#..", "#.#.#", ".###.", "..#.."], 0)
G[ord('×')] = art(["#...#", ".#.#.", "..#..", ".#.#.", "#...#"], 1)
G[ord('·')] = art(["..#.."], 3)
G[ord('…')] = art(["#.#.#"], 6)
G[ord('°')] = art([".##..", "#..#.", "#..#.", ".##.."], 0)
G[ord('²')] = art([".##..", "...#.", "..#..", ".###."], 0)
G[ord('³')] = art([".##..", "..#..", "...#.", ".##.."], 0)
G[ord('₁')] = art(["..#..", ".##..", "..#..", ".###."], 3)
G[ord('₂')] = art([".##..", "...#.", "..#..", ".###."], 3)
G[ord('≠')] = art(["....#", "#####", "...#.", "#####", ".#..."], 1)
G[ord('≈')] = art([".#.#.", "#.#..", ".....", ".#.#.", "#.#.."], 1)


# Descendantes propres (ligne 7 disponible)
G[ord('g')] = art([".####", "#...#", "#...#", ".####", "....#", ".###."], 2)
G[ord('p')] = art(["####.", "#...#", "#...#", "####.", "#....", "#...."], 2)
G[ord('q')] = art([".####", "#...#", "#...#", ".####", "....#", "....#"], 2)
G[ord('y')] = art(["#...#", "#...#", "#...#", ".####", "....#", ".###."], 2)
G[ord('j')] = art(["...#.", ".....", "...#.", "...#.", "...#.", "...#.", "...#.", "..#.."], 0)

def accent(base, kind):
    g = list(G[ord(base)])
    if base == 'i': g[2] &= ~0x01          # i sans point
    if kind == 'acute': g[3] |= 1; g[2] |= 2
    elif kind == 'grave': g[1] |= 1; g[2] |= 2
    elif kind == 'circ': g[1] |= 2; g[2] |= 1; g[3] |= 2
    elif kind == 'diaer': g[1] |= 1; g[3] |= 1
    elif kind == 'cedil': g[2] |= 0x80
    return g
for ch, base, kind in [('é','e','acute'),('è','e','grave'),('ê','e','circ'),('ë','e','diaer'),('à','a','grave'),('â','a','circ'),
                       ('ù','u','grave'),('û','u','circ'),('ü','u','diaer'),('ô','o','circ'),('î','i','circ'),('ï','i','diaer'),('ç','c','cedil')]:
    G[ord(ch)] = accent(base, kind)

cps = sorted(G)
with open(sys.argv[1], "w") as f:
    f.write("/* Généré par tools/genfont5.py - police 5x7 (cellule 6x8), colonnes, bit0 = haut. */\n")
    f.write("#define FONT_N %d\n" % len(cps))
    f.write("static const unsigned short font_cp[FONT_N] = {%s};\n" % ",".join(str(c) for c in cps))
    f.write("static const unsigned char font_col[FONT_N][5] = {\n")
    for c in cps: f.write("{" + ",".join("0x%02X" % b for b in G[c]) + "},\n")
    f.write("};\n")

S = 5
per = 32
sheet = Image.new("RGB", (per * 6 * S, ((len(cps) + per - 1) // per) * 9 * S), "white")
for i, c in enumerate(cps):
    for x, b in enumerate(G[c]):
        for y in range(8):
            if b >> y & 1:
                for dy in range(S):
                    for dx in range(S):
                        sheet.putpixel((((i % per) * 6 + x) * S + dx, ((i // per) * 9 + y) * S + dy), (0, 0, 0))
sheet.save(sys.argv[2])
print(len(cps), "glyphes")
