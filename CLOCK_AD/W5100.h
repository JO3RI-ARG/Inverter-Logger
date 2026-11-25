#ifndef W5100_H
#define W5100_H

#include "globals.h"

// The W5100 and the SD card share the SPI bus.
// Pin 10 HIGH = W5100 not selected, so the SD card has the bus to itself.
// (when the web server comes: select one device at a time)

void w5100Disable()
{
  pinMode(PIN_W5100_CS, OUTPUT);
  digitalWrite(PIN_W5100_CS, HIGH);
}

#endif
