# Digital MIDI synthesizer (ESP32-S3)

Digital synthesizer on an ESP32-S3 with MIDI input and physical controls
(pots, encoder, buttons, OLED). The sound engine in `core/` is plain C++ and is
tested on a PC before it runs on the board.

## Layout
- `core/`  portable engine (no hardware code)
- `pc/`    PC tests and test app
- `esp/`   ESP-IDF project (added later)
- `docs/`  wiring diagram and parts list

## Build and test on the PC
    cmake -S . -B build
    cmake --build build
    ctest --test-dir build --output-on-failure
