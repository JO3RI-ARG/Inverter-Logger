/*
  XZEL_AD
  Xunzel inverter logger, version 0.2.0
  Arduino Ethernet R3 (W5100 + SD) + MAX3232 on pins 2/3 (+ DS1307 RTC, optional for now)

  Step 2: one read-only query (INV_COMMAND in globals.h) every 10 seconds,
  the raw reply is appended to TEST.TXT on the SD card.
  Read-only project: only the fixed Q queries in INVERTER.h are ever sent.

  CREATED by
  2026 - JO3RI

  License: MIT : https://opensource.org/licenses/MIT

*/

#define VERSION "0.2.0"

#include "globals.h"
#include "W5100.h"
#include "DS1307.h"
#include "SDcard.h"
#include "INVERTER.h"


void setup()
{
  pinMode(PIN_LED, OUTPUT);
  w5100Disable();                    // the W5100 shares the SPI bus with the SD card: keep it quiet
  Serial.begin(SERIAL_BAUD);         // FTDI only: the inverter is on pins 2/3
  Serial.print(F("XZEL_AD v"));
  Serial.println(F(VERSION));

  rtcBegin();
  sdBegin();
  inverterBegin();
  if (sdOk) stateQueryInverter();    // first query right away
}

void loop()
{
  if (!sdOk)
  {
    blink(1, 600);                   // slow blink = SD problem
    sdBegin();                       // keep trying
    return;
  }

  if (millis() - lastQuery >= INV_INTERVAL_MS)
  {
    lastQuery = millis();
    stateQueryInverter();
  }
}
