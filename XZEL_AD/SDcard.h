#ifndef SDCARD_H
#define SDCARD_H

// (named SDcard.h, not sd.h: on a Mac "#include <SD.h>" would pick up a local sd.h)

#include "globals.h"
#include "DS1307.h"

bool sdBegin()
{
  digitalWrite(PIN_W5100_CS, HIGH);
  pinMode(PIN_SD_CS, OUTPUT);
  sdOk = SD.begin(PIN_SD_CS);
  Serial.println(sdOk ? F("SD ok") : F("SD FAILED (card in? FAT16/FAT32?)"));
  if (sdOk) blink(3, 100);                       // 3 quick blinks = SD ok
  return sdOk;
}

// open, write, close every time: nothing is lost when the power drops
bool sdWriteLine(const char *line)
{
  File f = SD.open(LOG_FILE, FILE_WRITE);
  if (!f) return FALSE;
  f.println(line);
  f.close();
  return TRUE;
}

// one test line:  UTC,TEST,<line number>
void stateLogTest()
{
  char stamp[STAMP_SIZE];
  char line[LOG_LINE_SIZE];
  makeStamp(stamp);
  snprintf_P(line, LOG_LINE_SIZE, PSTR("%s,TEST,%lu"), stamp, ++lineNr);

  if (sdWriteLine(line))
  {
    Serial.println(line);
    blink(1, 50);                                // short blink = written
  }
  else
  {
    Serial.println(F("write FAILED"));
    sdOk = FALSE;                                // loop() will restart the card
  }
}

#endif
