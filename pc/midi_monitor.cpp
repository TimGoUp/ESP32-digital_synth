#include <cstdio>
#include <iostream>
#include <vector>

#include "RtMidi.h"

// 1. Vključimo miniaudio (brez kakršnekoli inštalacije na Windowsih!)
#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

// 2. Tvoje zvočne datoteke
#include "midi_parser.h"
#include "voice.h"

constexpr float SAMPLE_RATE = 44100.0f;

struct SynthApp {
  MidiParser parser;
  Voice voice;
};

// 3. ZVOČNA ZANKA: miniaudio kliče to funkcijo vsakič, ko zvočniki potrebujejo nove vzorce
void audioCallback(ma_device* pDevice, void* pOutput, const void* /*pInput*/, ma_uint32 frameCount) {
  Voice* voice = static_cast<Voice*>(pDevice->pUserData);
  float* buffer = static_cast<float*>(pOutput);

  for (ma_uint32 i = 0; i < frameCount; ++i) {
    float sample = voice->nextSample(); // <--- TVOJ VOICE IZRAČUNA ZVOK!
    
    // Stereo: levi in desni zvočnik
    *buffer++ = sample;
    *buffer++ = sample;
  }
}

// 4. MIDI ZANKA: Ko pritisneš tipko na klaviaturi
static void onMessage(double /*timestamp*/, std::vector<unsigned char>* message, void* userData) {
  SynthApp* app = static_cast<SynthApp*>(userData);

  for (unsigned char b : *message) {
    NoteEvent e;
    if (app->parser.feed(b, e)) {
      std::printf("  -> %s  note %3d  velocity %3d\n", e.on ? "ON " : "OFF", e.note, e.velocity);
      
      if (e.on) {
        app->voice.noteOn(e.note, e.velocity); // Zaigraj noto!
      } else {
        app->voice.noteOff(e.note);            // Ugasni noto!
      }
    }
  }
}

int main() {
  SynthApp app;
  app.voice.init(SAMPLE_RATE);

  // Nastavimo privzet analogni zvok: ŽAGA s hitrim napadom
  app.voice.getOsc().setWaveform(Waveform::SQUARE);
  app.voice.getEnv().setAttack(0.01f);  // 10 ms
  app.voice.getEnv().setDecay(0.2f);   // 200 ms
  app.voice.getEnv().setSustain(0.6f); // 60 % glasnosti
  app.voice.getEnv().setRelease(0.3f); // 300 ms izzvenevanje

  try {
    // A) NASTAVITEV MIDI VHODA
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
    }

    midiIn.openPort(chosen);
    midiIn.setCallback(&onMessage, &app);

    // B) NASTAVITEV ZVOČNEGA IZHODA (miniaudio)
    ma_device_config config = ma_device_config_init(ma_device_type_playback);
    config.playback.format   = ma_format_f32; // 32-bit float avdio
    config.playback.channels = 2;             // Stereo
    config.sampleRate        = static_cast<ma_uint32>(SAMPLE_RATE);
    config.dataCallback      = audioCallback;
    config.pUserData         = &app.voice;

    ma_device device;
    if (ma_device_init(nullptr, &config, &device) != MA_SUCCESS) {
      std::printf("Failed to initialize audio output!\n");
      return 1;
    }

    ma_device_start(&device);

    std::printf("\n==================================================\n");
    std::printf("   SINTETIZATOR IGRA V ŽIVO (REALTIME)!           \n");
    std::printf("   Igraj na MIDI klaviaturo. Pritisni ENTER za izhod.\n");
    std::printf("==================================================\n");

    std::cin.get();

    // Zaustavitev
    ma_device_uninit(&device);

  } catch (const std::exception& error) {
    std::printf("Napaka: %s\n", error.what());
    return 1;
  }

  return 0;
}