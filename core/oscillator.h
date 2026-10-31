#pragma once
#include <cmath>

// 4 osnovne oblike valov
enum class Waveform {
  SINE,
  SAW,
  SQUARE,
  TRIANGLE
};

class Oscillator {
 public:

  void init(float sampleRate) {
    sampleRate_ = sampleRate;
    phase_ = 0.0f;
    waveform_ = Waveform::SAW;  // Privzeta oblika vala
    pulseWidth_ = 0.5f;         // Privzeta širina pulza
    setFrequency(440.0f);       // Privzeta nota
  }

  // nastavitev frekvence in izračun koraka faze
  void setFrequency(float freq) {
    frequency_ = freq;
    phaseIncrement_ = frequency_ / sampleRate_;
  }

  // Izbira oblike vala
  void setWaveform(Waveform wave) {
    waveform_ = wave;
  }

  // širina pulza kvadrata z varnosnim pogojem
  void setPulseWidth(float pw) {
    if (pw < 0.05f) pw = 0.05f;
    if (pw > 0.95f) pw = 0.95f;
    pulseWidth_ = pw;
  }

  // Ponastavitev faze ob novem udarcu tipke (prepreči poke)
  void resetPhase() {
    phase_ = 0.0f;
  }

  // Glavna funkcija: izračun naslednjega zvočnega vzorca (-1.0 do +1.0)
  float nextSample() {
    float out = 0.0f;

    switch (waveform_) {
      case Waveform::SAW:
        out = 2.0f * phase_ - 1.0f;
        break;

      case Waveform::SQUARE:
        out = (phase_ < pulseWidth_) ? 1.0f : -1.0f;
        break;

      case Waveform::TRIANGLE:
        out = 4.0f * std::abs(phase_ - 0.5f) - 1.0f;
        break;

      case Waveform::SINE:
        out = std::sin(phase_ * 6.28318530718f); //pi je napisan kot float, ker esp nima double števil
        break;
    }

    // Premakni fazo za naslednji klic
    phase_ += phaseIncrement_;

    // Ko faza pride do konca jo ponastavimo
    if (phase_ >= 1.0f) {
      phase_ -= 1.0f;
    }

    return out;
  }

 private:
  float sampleRate_ = 44100.0f; // vzorci / sec
  float frequency_ = 440.0f;  // frekvenca tona 
  float phase_ = 0.0f;  // faza 0f-1f
  float phaseIncrement_ = 0.0f;  // velikost premika faze
  
  Waveform waveform_ = Waveform::SAW;  // oblika vala
  float pulseWidth_ = 0.5f;  //  koliko časa je signal high ( duty cycle )
};