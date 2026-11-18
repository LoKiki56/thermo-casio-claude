# Assemble les captures PGM d'un dossier en une planche PNG (agrandies x3).
import sys, glob, os
from PIL import Image, ImageDraw
d, out = sys.argv[1], sys.argv[2]
cols = int(sys.argv[3]) if len(sys.argv) > 3 else 4
files = sorted(glob.glob(os.path.join(d, "*.pgm")))
S = 3; W, H = 128 * S, 64 * S; pad = 10; lab = 14
rows = (len(files) + cols - 1) // cols
sheet = Image.new("RGB", (cols * (W + pad) + pad, rows * (H + lab + pad) + pad), (200, 205, 215))
dr = ImageDraw.Draw(sheet)
for i, f in enumerate(files):
    im = Image.open(f).convert("L").resize((W, H), Image.NEAREST)
    # teinte façon LCD Casio
    lcd = Image.merge("RGB", [im.point(lambda v: 40 + v * 175 // 255), im.point(lambda v: 50 + v * 185 // 255), im.point(lambda v: 40 + v * 150 // 255)])
    x = pad + (i % cols) * (W + pad); y = pad + (i // cols) * (H + lab + pad)
    sheet.paste(lcd, (x, y + lab))
    dr.text((x, y), os.path.basename(f)[:-4], fill=(20, 20, 30))
sheet.save(out)
print(len(files), "captures ->", out)
