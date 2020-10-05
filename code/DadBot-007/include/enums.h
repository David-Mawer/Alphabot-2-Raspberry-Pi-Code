#ifndef ENUMS_H_INCLUDED
#define ENUMS_H_INCLUDED

enum TDirection {forward, back, left, right, none};

// Some colours nicely pre-coded.
enum TLedColour : uint32_t {
  Black=0x000000,
  White=0x808080,
  Red=0x800000,      // red
  Orange=0x804000,   // orange
  Yellow=0x808000,   // yellow
  Green=0x008000,    // green
  LightBlue=0x008080,// lightblue
  Blue=0x000080,     // blue
  Purple=0x400040,   // purple
  Pink=0x800040      // pink
};

#endif // ENUMS_H_INCLUDED
