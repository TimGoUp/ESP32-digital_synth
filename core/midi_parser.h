#pragma once
#include <cstdint>

inline bool isStatusByte(uint8_t b) {
  return b >= 0x80; 
}

struct NoteEvent {
  bool on;           // true = note on, false = note off
  uint8_t note;      // 0..127
  uint8_t velocity;  // 0..127
};

class MidiParser {
 public:
  // Feed sprejema en bajt naenkrat. Vrne true, ko je eno MIDI sporočilo popolno. 
  // b je bajt, ki ga trenutno obdelujemo, out pa struct, ki se spremeni, ko je sporočilo popolno
  bool feed(uint8_t b, NoteEvent& out) {
    if (b >= 0xF8) return false;        // realtime byte: ignore

    if (isStatusByte(b)) {
      uint8_t type = b & 0xF0;            // odstrani zadnji del (kanal 1-16) 
      tracking_ = (type == 0x80 || type == 0x90);
      isOn_ = (type == 0x90);
      count_ = 0;
      return false;                   // status bajt ni zadnji 
    }
    if (!tracking_) return false;       // data of a message we don't care about
    if (count_ == 0) {
      note_ = b;
      count_ = 1;
      return false;
    }
    count_ = 0;                         // keep tracking_: running status
    out.on = isOn_ && b != 0;           // note on with velocity 0 = note off
    out.note = note_;
    out.velocity = b;
    return true;
  }

 private:
  bool tracking_ = false;
  bool isOn_ = false;
  uint8_t count_ = 0;
  uint8_t note_ = 0;
};