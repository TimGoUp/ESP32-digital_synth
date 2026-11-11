#pragma once
#include <cstdint>
#include "math_utils.h"


enum class EnvStatus { IDLE, ATTACK, DECAY, SUSTAIN, RELEASE };

class Envelope {
 public:
  void init(float sampleRate) {
    sampleRate_ = sampleRate;
    status_ = EnvStatus::IDLE;
    currentLevel_ = 0.0f;

    // default
    setAttack(0.01f); 
    setDecay(0.1f);      
    setSustain(0.7f);    
    setRelease(0.2f);    
  }

  // Nastavitvene funkcije
  void setAttack(float seconds) {
    seconds = clamp(seconds, 0.001f, 10.0f);
    attackRate_ = 1.0f / (seconds * sampleRate_); // koliko vzorcev potrebujemo da pridemo do vrha
  }

  void setDecay(float seconds) {
    seconds = clamp(seconds, 0.001f, 10.0f);
    decayRate_ = 1.0f / (seconds * sampleRate_);
  }

  void setSustain(float level) {
    sustainLevel_ = clamp(level, 0.0f, 1.0f);
  }

  void setRelease(float seconds) {
    seconds = clamp(seconds, 0.001f, 10.0f);
    releaseRate_ = 1.0f / (seconds * sampleRate_);
  }

  // note on/off
  void noteOn() {
    status_ = EnvStatus::ATTACK;
  }

  void noteOff() {
    if (status_ != EnvStatus::IDLE) {
      status_ = EnvStatus::RELEASE;
    }
  }

  // ali env proizvaja zvok
  bool isActive() const {
    return status_ != EnvStatus::IDLE;
  }

  // nasledni vzorec (0.0f - 1.0f)
  float nextSample() {
    switch (status_) {
      case EnvStatus::IDLE:
        currentLevel_ = 0.0f;
        break;

      case EnvStatus::ATTACK:
        currentLevel_ += attackRate_;
        if (currentLevel_ >= 1.0f) {
          currentLevel_ = 1.0f;
          status_ = EnvStatus::DECAY; // Ko doseže vrh, začne padati
        }
        break;

      case EnvStatus::DECAY:
        currentLevel_ -= decayRate_;
        if (currentLevel_ <= sustainLevel_) {
          currentLevel_ = sustainLevel_;
          status_ = EnvStatus::SUSTAIN; // Ko pade do sustain ravni, tam ostane
        }
        break;

      case EnvStatus::SUSTAIN:
        currentLevel_ = sustainLevel_;
        break;

      case EnvStatus::RELEASE:
        currentLevel_ -= releaseRate_;
        if (currentLevel_ <= 0.0f) {
          currentLevel_ = 0.0f;
          status_ = EnvStatus::IDLE; // Zvok je ugasnil
        }
        break;
    }

    return currentLevel_;
  }

 private:
  float sampleRate_ = 44100.0f;
  EnvStatus status_ = EnvStatus::IDLE;
  float currentLevel_ = 0.0f;

  float sustainLevel_ = 0.7f;
  float attackRate_ = 0.0f;
  float decayRate_ = 0.0f;
  float releaseRate_ = 0.0f;
};