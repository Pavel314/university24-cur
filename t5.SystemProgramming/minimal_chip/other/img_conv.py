from PIL import Image
import numpy as np
import struct

def convert_to_chip8_bmp(input_path, output_path, max_width=128, max_height=64):
    img = Image.open(input_path).convert("1", dither = Image.Dither.NONE)
    scale = min(max_width / img.width, max_height / img.height)
    new_w = int(img.width * scale)
    new_h = int(img.height * scale)
    img = img.resize((new_w, new_h))
    final_img = Image.new("1", (max_width, max_height), 0) 
    final_img.paste(img, ((max_width - new_w) // 2, (max_height - new_h) // 2))
    final_img.save(output_path, format="BMP")
    final_img.show()
    return final_img;


def mono_to_tile(img: Image.Image):
    if img.size != (128, 64): raise ValueError(f"Expected 128x64, got {img.size}")
    a = (np.asarray(img, dtype=np.uint8) > 0).astype(np.uint16)
    tiles = a.reshape(4, 16, 8, 16)
    weights = (1 << np.arange(15, -1, -1, dtype=np.uint16))
    masks = (tiles * weights).sum(axis=3)
    masks = masks.transpose(2, 0, 1).reshape(-1, 16)
    return [tuple(map(int, row)) for row in masks]

SCHIP_PROG = bytes.fromhex(
"""
0x00 0xE0 0x00 0xFF 0x60 0x00 0x61 0x00 0x62 0x00 0x63 0x10 0x64 0x20 0x65 0x30 0x67 0x20 0xA2 0x42 0xD1 0x20 0xF7 0x1E 0xD1 0x30 0xF7 0x1E 0xD1 0x40 0xF7 0x1E 0xD1 0x50 0xF7 0x1E 0x71 0x10 0x70 0x01 0x30 0x08 0x12 0x14 0x69 0x00 0xF8 0x0A 0x39 0x00 0x12 0x3A 0x00 0xFE 0x69 0x01 0x12 0x3E 0x00 0xFF 0x69 0x00 0x00 0xFE 0x12 0x2E
"""

.replace("0x", "") 
)


def write_tiles(tuples_list, filename: str):
    with open(filename, 'wb') as f:
        f.write(SCHIP_PROG)
        for tup in tuples_list:
            for value in tup:
                f.write(struct.pack('>H', value))

img = convert_to_chip8_bmp(r"input_img.jpg", "output.bmp")
tiles = mono_to_tile(img)
write_tiles(tiles, "schip_output")

"""
:alias x v1
:alias y1 v2
:alias y2 v3
:alias y3 v4
:alias y4 v5
:alias step v7
:alias k v8
:alias cm v9

: main
  clear
	hires
  v0 := 0
	x := 0
  y1 := 0
	y2 := 16
	y3 := 32
	y4 := 48
	step := 32
	i := image
	loop
	sprite x y1 0
	i += step
	sprite x y2 0
	i += step
	sprite x y3 0
	i += step
	sprite x y4 0
	i += step
	x += 16
	v0 += 1
	
	if v0 != 8 then
	again
	
	cm := 0
	loop
	k := key
	if cm == 0  begin
		lores
		cm := 1 
	else
		hires
		cm := 0 	
	end
	lores
	again
	

: image
  0b00000001 0b00000010
	0b00000011 0b00000100
	0b00000101 0b00000110
	0b00000111 0b00001000
"""
