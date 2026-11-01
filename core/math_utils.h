#pragma once
#include <cmath>
#include <cstdint>
#include <algorithm>

constexpr float PI = 3.14159265358979323846f; 

// Pretvorba MIDI note (0 - 127) v frekvenco v Hz
inline float midiToFreq(uint8_t note) {
  //if (note > 127) note = 127;
  return 440.0f * std::pow(2.0f, (static_cast<float>(note) - 69.0f) / 12.0f);
}

// Linearna interpolacija (uporabno kasneje za filtre in valovne tabele)
inline float lerp(float a, float b, float t) {
  return a + t * (b - a);
}
