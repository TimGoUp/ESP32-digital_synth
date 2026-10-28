#include "midi_parser.h"
#include <cstdio>

static int failures = 0;

static void check(bool ok, const char* name) {
  if (!ok) {
    std::printf("FAIL: %s\n", name);
    failures++;
  }
}

int main() {
  NoteEvent e;

  // Press A1 (note 33), velocity 100
  {
    MidiParser p;
    check(!p.feed(0x90, e), "press: status is not complete");
    check(!p.feed(0x21, e), "press: note is not complete");
    check(p.feed(0x64, e), "press: velocity completes it");
    check(e.on, "press: on");
    check(e.note == 33, "press: note 33");
    check(e.velocity == 100, "press: velocity 100");
  }

  // Release with note off (0x80)
  {
    MidiParser p;
    check(!p.feed(0x80, e), "off: status");
    check(!p.feed(0x21, e), "off: note");
    check(p.feed(0x40, e), "off: complete");
    check(!e.on, "off: not on");
    check(e.note == 33, "off: note 33");
    check(e.velocity == 64, "off: velocity 64");
  }

  // Release with note on, velocity 0
  {
    MidiParser p;
    check(!p.feed(0x90, e), "vel0: status");
    check(!p.feed(0x21, e), "vel0: note");
    check(p.feed(0x00, e), "vel0: complete");
    check(!e.on, "vel0: counts as off");
    check(e.note == 33, "vel0: note 33");
  }

  // Running status: one status byte, two notes
  {
    MidiParser p;
    check(!p.feed(0x90, e), "running: status");
    check(!p.feed(0x3C, e), "running: 1st note");
    check(p.feed(0x64, e), "running: 1st complete");
    check(e.note == 60 && e.velocity == 100, "running: 1st values");
    check(!p.feed(0x3E, e), "running: 2nd note, no status needed");
    check(p.feed(0x5A, e), "running: 2nd complete");
    check(e.on && e.note == 62 && e.velocity == 90, "running: 2nd values");
  }

  // Other channel (0x93 = note on, channel 4)
  {
    MidiParser p;
    check(!p.feed(0x93, e), "channel: status");
    check(!p.feed(0x3C, e), "channel: note");
    check(p.feed(0x64, e), "channel: complete");
    check(e.on && e.note == 60, "channel: values");
  }

  // Control change is ignored, also its running status
  {
    MidiParser p;
    check(!p.feed(0xB0, e), "cc: status");
    check(!p.feed(0x07, e), "cc: number");
    check(!p.feed(0x64, e), "cc: value is not an event");
    check(!p.feed(0x08, e), "cc: running status number");
    check(!p.feed(0x65, e), "cc: running status value");
  }

  // Program change is ignored
  {
    MidiParser p;
    check(!p.feed(0xC0, e), "pc: status");
    check(!p.feed(0x05, e), "pc: program");
    check(!p.feed(0x3C, e), "pc: leftover data is ignored");
    check(!p.feed(0x64, e), "pc: leftover data is ignored (2)");
  }

  // Poly aftertouch is ignored
  {
    MidiParser p;
    check(!p.feed(0xA0, e), "aftertouch: status");
    check(!p.feed(0x3C, e), "aftertouch: note");
    check(!p.feed(0x40, e), "aftertouch: pressure is not an event");
  }

  // A note still works after an ignored message
  {
    MidiParser p;
    p.feed(0xB0, e);
    p.feed(0x07, e);
    p.feed(0x64, e);
    check(!p.feed(0x90, e), "after cc: status");
    check(!p.feed(0x3C, e), "after cc: note");
    check(p.feed(0x64, e), "after cc: complete");
    check(e.note == 60, "after cc: note 60");
  }

  // Realtime byte (clock) in the middle of a message
  {
    MidiParser p;
    check(!p.feed(0x90, e), "clock: status");
    check(!p.feed(0x3C, e), "clock: note");
    check(!p.feed(0xF8, e), "clock: clock byte is not an event");
    check(p.feed(0x64, e), "clock: message still completes");
    check(e.note == 60 && e.velocity == 100, "clock: values");
  }

  // Data bytes with no status before them
  {
    MidiParser p;
    check(!p.feed(0x3C, e), "no status: data ignored");
    check(!p.feed(0x64, e), "no status: data ignored (2)");
  }

  // A new status byte interrupts an unfinished message
  {
    MidiParser p;
    check(!p.feed(0x90, e), "interrupt: status");
    check(!p.feed(0x3C, e), "interrupt: note");
    check(!p.feed(0x80, e), "interrupt: new status");
    check(!p.feed(0x3C, e), "interrupt: note");
    check(p.feed(0x40, e), "interrupt: complete");
    check(!e.on && e.note == 60, "interrupt: note off 60");
  }

  if (failures == 0) std::printf("tests passed\n");
  return failures == 0 ? 0 : 1;
}