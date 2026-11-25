#ifndef INVERTER_H
#define INVERTER_H

// READ-ONLY: only the fixed queries listed in invAllowed() can ever be sent.
// There is no path from the network or the serial monitor to the inverter.

#include "globals.h"
#include "DS1307.h"
#include "SDcard.h"

// CRC16-XMODEM; a CRC byte equal to '(' CR or LF is incremented by 1 (as in the manual)
void invCrc(const byte *d, byte n, byte *hi, byte *lo)
{
  unsigned int crc = 0;
  for (byte i = 0; i < n; i++)
  {
    crc ^= (unsigned int)d[i] << 8;
    for (byte b = 0; b < 8; b++)
      crc = (crc & 0x8000) ? (crc << 1) ^ 0x1021 : (crc << 1);
  }
  *hi = crc >> 8;
  *lo = crc & 0xFF;
  if (*hi == 0x28 || *hi == 0x0D || *hi == 0x0A) (*hi)++;
  if (*lo == 0x28 || *lo == 0x0D || *lo == 0x0A) (*lo)++;
}

// the whitelist: read-only queries that are known to work on this inverter
bool invAllowed(const char *cmd)
{
  return !strcmp_P(cmd, PSTR("QMOD")) || !strcmp_P(cmd, PSTR("QPIGS")) || !strcmp_P(cmd, PSTR("QPIWS"));
}

bool invSend(const char *cmd)
{
  if (cmd[0] != 'Q' || !invAllowed(cmd)) return FALSE;
  byte n = strlen(cmd);
  byte hi, lo;
  invCrc((const byte *)cmd, n, &hi, &lo);
  while (inverter.available()) inverter.read();  // drop anything old
  inverter.write((const byte *)cmd, n);
  inverter.write(hi);
  inverter.write(lo);
  inverter.write((byte)0x0D);
  return TRUE;
}

// collects the reply in invBuf until the CR (CRC bytes are never 0x0D). FALSE on timeout.
bool invReceive()
{
  invLen = 0;
  unsigned long t0 = millis();
  while (millis() - t0 < INV_TIMEOUT_MS)
  {
    while (inverter.available())
    {
      byte b = inverter.read();
      if (invLen < INV_REPLY_SIZE) invBuf[invLen++] = b;
      if (b == 0x0D) return TRUE;
    }
  }
  return FALSE;
}

// a good reply is:  '('  payload  CRC-hi  CRC-lo  CR
bool invReplyOk()
{
  if (invLen < 4 || invBuf[0] != '(' || invBuf[invLen - 1] != 0x0D) return FALSE;
  byte hi, lo;
  invCrc(invBuf, invLen - 3, &hi, &lo);
  return invBuf[invLen - 3] == hi && invBuf[invLen - 2] == lo;
}

// one line:  stamp,COMMAND,payload   or   stamp,COMMAND,ERR:...
void invPrintResult(Print &p, const char *stamp, bool sent, bool ok)
{
  p.print(stamp);
  p.print(',');
  p.print(INV_COMMAND);
  p.print(',');
  if (ok)
    p.write(invBuf + 1, invLen - 4);             // the payload without '(' CRC CR
  else if (!sent)
    p.print(F("ERR:notallowed"));
  else if (invLen == 0)
    p.print(F("ERR:noreply"));                   // nothing came back: swap the two RS-232 data wires?
  else
  {
    p.print(F("ERR:bad,"));                      // garbage or wrong CRC: show the first bytes in hex
    for (byte i = 0; i < invLen && i < 24; i++)
    {
      if (invBuf[i] < 16) p.print('0');
      p.print(invBuf[i], HEX);
    }
  }
  p.println();
}

void stateQueryInverter()
{
  char stamp[STAMP_SIZE];
  makeStamp(stamp);
  bool sent = invSend(INV_COMMAND);
  invLen = 0;
  bool got = sent && invReceive();
  bool ok = got && invReplyOk();

  File f = sdOpen(TEST_FILE);
  if (!f)
  {
    Serial.println(F("write FAILED"));
    sdOk = FALSE;                                // loop() will restart the card
    return;
  }
  invPrintResult(f, stamp, sent, ok);
  f.close();
  invPrintResult(Serial, stamp, sent, ok);
  if (ok) blink(1, 50); else blink(2, 250);      // 1 short blink = good reply, 2 slow blinks = problem
}

void inverterBegin()
{
  inverter.begin(INVERTER_BAUD);                 // 2400 baud, 8N1
}

#endif
