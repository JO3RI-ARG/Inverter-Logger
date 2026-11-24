#ifndef DS1307_H
#define DS1307_H

#include "globals.h"

PROGMEM const char monthNames[] = "JanFebMarAprMayJunJulAugSepOctNovDec";

byte bcdToDec(byte v) { return (v >> 4) * 10 + (v & 0x0F); }
byte decToBcd(byte v) { return ((v / 10) << 4) | (v % 10); }

// reads the time into rtcSecond..rtcYear. Returns FALSE if the chip does not answer.
// The result is also FALSE (but the time is filled) when the clock is halted.
bool rtcRead()
{
  Wire.beginTransmission(RTC_ADDRESS);
  Wire.write((byte)0);
  if (Wire.endTransmission() != 0) return FALSE;
  if (Wire.requestFrom((byte)RTC_ADDRESS, (byte)7) != 7) return FALSE;

  byte s = Wire.read();
  rtcMinute = bcdToDec(Wire.read() & 0x7F);
  rtcHour   = bcdToDec(Wire.read() & 0x3F);      // 24 hour mode
  Wire.read();                                   // day of week: not used
  rtcDay    = bcdToDec(Wire.read() & 0x3F);
  rtcMonth  = bcdToDec(Wire.read() & 0x1F);
  rtcYear   = 2000 + bcdToDec(Wire.read());
  rtcSecond = bcdToDec(s & 0x7F);
  return !(s & 0x80);                            // bit 7 = clock halt
}

// sets the clock to the time this sketch was compiled (treated as UTC) and starts it
void rtcSetFromCompileTime()
{
  byte month = 1;
  for (byte i = 0; i < 12; i++)
    if (strncmp_P(__DATE__, monthNames + 3 * i, 3) == 0) month = i + 1;

  Wire.beginTransmission(RTC_ADDRESS);
  Wire.write((byte)0);
  Wire.write(decToBcd(atoi(__TIME__ + 6)));      // seconds, bit 7 = 0 starts the clock
  Wire.write(decToBcd(atoi(__TIME__ + 3)));      // minutes
  Wire.write(decToBcd(atoi(__TIME__)));          // hours
  Wire.write((byte)1);                           // day of week: not used
  Wire.write(decToBcd(atoi(__DATE__ + 4)));      // day
  Wire.write(decToBcd(month));
  Wire.write(decToBcd(atoi(__DATE__ + 9)));      // year (last 2 digits)
  Wire.endTransmission();
}

void rtcBegin()
{
  Wire.begin();
  Wire.beginTransmission(RTC_ADDRESS);
  if (Wire.endTransmission() != 0)
  {
    Serial.println(F("RTC not found"));
    rtcOk = FALSE;
    return;
  }
  if (RTC_FORCE_SET || !rtcRead())               // forced, or halted (new chip or battery was out)
  {
    Serial.println(F("RTC set from compile time"));
    rtcSetFromCompileTime();
  }
  rtcOk = rtcRead();
}

// "YYYY-MM-DD HH:MM:SS", or "millis=12345" when there is no clock
void makeStamp(char *buf)
{
  if (rtcOk && rtcRead())
    snprintf_P(buf, STAMP_SIZE, PSTR("%04u-%02u-%02u %02u:%02u:%02u"),
               rtcYear, rtcMonth, rtcDay, rtcHour, rtcMinute, rtcSecond);
  else
    snprintf_P(buf, STAMP_SIZE, PSTR("ms=%lu"), millis());
}

#endif
