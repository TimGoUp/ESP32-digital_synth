// Live MIDI monitor: shows what the keyboard sends and what the parser makes of it.
#include "midi_parser.h"
#include "RtMidi.h"
#include <cstdio>
#include <iostream>
#include <vector>

static void onMessage(double /*timestamp*/, std::vector<unsigned char>* message, void* userData) {
  MidiParser* parser = static_cast<MidiParser*>(userData);

  std::printf("raw:");
  for (unsigned char b : *message) std::printf(" %02X", b);
  std::printf("\n");

  for (unsigned char b : *message) {
    NoteEvent e;
    if (parser->feed(b, e)) {
      std::printf("  -> %s  note %3d  velocity %3d\n", e.on ? "ON " : "OFF", e.note, e.velocity);
    }
  }
}

int main() {
  try {
    RtMidiIn midiIn;
    unsigned int ports = midiIn.getPortCount();
    if (ports == 0) {
      std::printf("No MIDI input found. Is the keyboard plugged in?\n");
      return 1;
    }

    std::printf("MIDI inputs:\n");
    for (unsigned int i = 0; i < ports; i++) {
      std::printf("  %u: %s\n", i, midiIn.getPortName(i).c_str());
    }

    unsigned int chosen = 0;
    if (ports > 1) {
      std::printf("Choose port number: ");
      std::cin >> chosen;
      std::cin.ignore();
      if (chosen >= ports) {
        std::printf("No such port.\n");
        return 1;
      }
    }

    MidiParser parser;
    midiIn.openPort(chosen);
    midiIn.setCallback(&onMessage, &parser);
    std::printf("Listening on port %u. Play something. Press Enter to quit.\n", chosen);
    std::cin.get();
  } catch (RtMidiError& error) {
    error.printMessage();
    return 1;
  }
  return 0;
}