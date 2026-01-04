# Green SHIP8 Emulator

A minimalist **CHIP-8/SCHIP** emulator built with **SDL3** and **Nuklear**.

## Features

- **Broad game compatibility** thanks to a flexible core(`src/chip8.h`) and support for (almost) all **quirk sets**.
- **ROM capability / quirk selection via database metadata**, solving the common problem of not knowing which features a random ROM expects.
- **Retro display effects**: pixel fade/persistence (CRT-like afterglow), a red neon look.
  > It's best seen in action but screenshots will be added.
- **Bonus tool:** `other/img_conv.py` converts arbitrary images into a **hi-res ROM** and can be used to test the **hi-res→lo-res** transition (waits for a key press after launch).

## Toolchain

- **C99/11**
- **SDL3**
- **Nuklear**
- **CMake**

## ROM DB integration (chip-8-database)

One of the biggest CHIP-8 ecosystem issues is that for an arbitrary ROM it’s often unclear **which quirks/features** are required for correct behavior.  
To address this, the project integrates the community-maintained **chip-8-database**:

- https://github.com/chip-8/chip-8-database

Integration is implemented via: `src/romdb/romdb_gen.py`. The script generates `romdb.h` and `romdb.c`, these files make database access straightforward and keep runtime overhead effectively negligible (most of the work is done ahead of time).

## Resources and rc89

- `src/res/resource_compiler.exe`

This repository currently includes my own resource compiler called **rc89**. I plan to publish its source code separately soon.

## Project status

This project was built in about **two weeks** and it was a lot of fun.

## ROMs and licensing

ROMs inside `roms/` belong to their respective authors and may have different licenses/terms.  
They are included for testing/demo purposes only. If you are a rights holder and would like any item removed, please open an issue and I will take it down.

## Building

1. `git clone https://github.com/Pavel314/university24-cur.git`
1. `cd university24-cur/t5.SystemProgramming/minimal_chip`
1. `cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Releasee`
1. `cmake --build build -j`

---