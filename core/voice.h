#pragma once
#include <cstdint>
#include "math_utils.h"
#include "oscillator.h"
#include "envelope.h"

class Voice {
    private:
    Oscillator osc_;
    Envelope env_;

    uint8_t currentNote_ = 0;
    float velocity_ = 0.0f;

    public:
    void init(float sampleRate) {
        osc_.init(sampleRate);
        env_.init(sampleRate);
    }

    // Pritisk tipke
    void noteOn(uint8_t note, uint8_t velocity) {
        currentNote_ = note;
        velocity_ = static_cast<float>(velocity) / 127.0f;

        osc_.setFrequency(midiToFreq(note));
        
        env_.noteOn();
    }

    // Spust tipke
    void noteOff(uint8_t note) {
        if (note == currentNote_) {
        env_.noteOff();
        }
    }

    bool isActive() const {
        return env_.isActive();
    }

    Oscillator& getOsc() { return osc_; }
    Envelope& getEnv() { return env_; }

    float nextSample() {
        if (!env_.isActive()) {
        return 0.0f;
        }
        return osc_.nextSample() * env_.nextSample() *0.2; // velocity_; // ZMNOŽEK VREDNOSTI OSCILATORJA IN OVOJNICE TER GLASNOSTI TONA 
    }

 
};