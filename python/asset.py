from pathlib import Path
import sys
import traceback

from PIL import Image

def rgb_to_rgb565(r, g, b):
    return ((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3)


def format_c_array(values, per_line=12):
    lines = []
    for i in range(0, len(values), per_line):
        chunk = values[i : i + per_line]
        lines.append("    " + ", ".join(f"0x{v:04X}" for v in chunk))
    return ",\n".join(lines)


def make_header(outname, size):
    guard = outname.upper() + "_H"
    return f"""#ifndef {guard}
#define {guard}

#include "eadk.h"

extern const eadk_color_t {outname}_data[{size}];
extern const uint16_t {outname}_width;
extern const uint16_t {outname}_height;

#endif
"""


def make_source(outname, width, height, values):
    size = len(values)
    arr = format_c_array(values)
    return f"""#include \"{outname}.h\"\n\nconst uint16_t {outname}_width = {width};\nconst uint16_t {outname}_height = {height};\n\nconst eadk_color_t {outname}_data[{size}] = {{\n{arr}\n}};\n"""


def main():
    try:
        cwd = Path.cwd()
        inp = cwd / "input.png"
        print(f"Reading: {inp}")
        if not inp.exists():
            print("input.png not found in current directory:", cwd)
            return

        img = Image.open(inp).convert("RGBA")
        if img.mode == "RGBA":
            bg = Image.new("RGB", img.size, (255, 255, 255))
            bg.paste(img, mask=img.split()[3])
            img = bg
        else:
            img = img.convert("RGB")

        width, height = img.size
        pixels = list(img.getdata())
        values = [rgb_to_rgb565(r, g, b) for (r, g, b) in pixels]

        outname = "image_data"
        hpath = cwd / (outname + ".h")
        cpath = cwd / (outname + ".c")
        hpath.write_text(make_header(outname, len(values)))
        cpath.write_text(make_source(outname, width, height, values))

        print(f"Wrote {hpath} and {cpath} ({width}x{height}, {len(values)} pixels)")
    except Exception:
        traceback.print_exc()


if __name__ == "__main__":
    main()