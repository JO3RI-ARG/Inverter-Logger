/*
  XZEL_AD
  Xunzel inverter logger, version 0.1.0
  Arduino Ethernet R3 (W5100 + SD) + DS1307 RTC

  Step 1: SD card write test with a timestamp from the DS1307.
  Read-only project: this sketch never sends anything to the inverter.

  CONTINUED by
  2026 - JO3RI

  License: MIT : https://opensource.org/licenses/MIT

*/

#define VERSION "0.1.0"

#include "globals.h"
#include "W5100.h"
#include "DS1307.h"
#include "SDcard.h"


void setup()
{
  pinMode(PIN_LED, OUTPUT);
  w5100Disable();                    // the W5100 shares the SPI bus with the SD card: keep it quiet
  Serial.begin(SERIAL_BAUD);
  Serial.print(F("XZEL_AD v"));
  Serial.println(F(VERSION));

  rtcBegin();
  sdBegin();
}

void loop()
{
  if (!sdOk)
  {
    blink(1, 600);                   // slow blink = SD problem
    sdBegin();                       // keep trying
    return;
  }

  if (millis() - lastLog >= LOG_INTERVAL_MS)
  {
    lastLog = millis();
    stateLogTest();
  }
}
