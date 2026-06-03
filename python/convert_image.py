import os
import sys
from pathlib import Path
from PIL import Image


def rgb_to_palette_index(rgb):
    r, g, b = rgb
    intensity = (r + g + b) / 3.0
    idx = int(round(intensity / 255.0 * 15.0))
    if idx < 0:
        return 0
    if idx > 15:
        return 15
    return idx


def pack_indices_to_bytes(indices):
    out = bytearray()
    i = 0
    n = len(indices)
    while i < n:
        a = indices[i]
        b = indices[i + 1] if (i + 1) < n else 0
        out.append(((a & 0x0F) << 4) | (b & 0x0F))
        i += 2
    return out


def rle_encode(indices):
    out = bytearray()
    if not indices:
        return out

    cur = indices[0]
    run = 1
    for v in indices[1:]:
        if v == cur and run < 16:
            run += 1
            continue
        out.append(((run - 1) << 4) | (cur & 0x0F))
        cur = v
        run = 1

    if run > 0:
        out.append(((run - 1) << 4) | (cur & 0x0F))

    return out

def main():
    script_dir = Path(__file__).resolve().parent
    img_path = script_dir / 'image.png'
    if not img_path.exists():
        print(f"Fichier introuvable: {img_path}")
        sys.exit(1)

    im = Image.open(img_path).convert('RGB')
    width, height = im.size

    if width % 320 != 0 or height % 240 != 0:
        print(f"Image size must be a multiple of 320x240 (got {width}x{height})")
        sys.exit(1)

    pixels = list(im.getdata())

    out = bytearray()
    for y in range(height):
        row_start = y * width
        for x_chunk in range(0, width, 320):
            x_end = min(x_chunk + 320, width)
            line_indices = []
            for x in range(x_chunk, x_end):
                line_indices.append(rgb_to_palette_index(pixels[row_start + x]))
            out.extend(rle_encode(line_indices))

    data_bytes = bytes(out)

    bin_path = script_dir / 'input.bin'
    with open(bin_path, 'wb') as bf:
        bf.write(data_bytes)

    print(f'Wrote {bin_path} ({len(data_bytes)} bytes)')


if __name__ == '__main__':
    main()
