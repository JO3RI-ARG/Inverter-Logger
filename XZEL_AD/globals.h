#ifndef GLOBALS_H
#define GLOBALS_H

#include <Arduino.h>
#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#include <SoftwareSerial.h>

// constants /////////////////////////////////////////////////////////////////

//define pins (Arduino Ethernet R3)
#define PIN_W5100_CS                 10   // Ethernet chip select
#define PIN_SD_CS                    4    // SD card chip select
#define PIN_LED                      9    // LED on the board
#define PIN_INV_RX                   2    // from the MAX3232 TTL output (inverter -> Arduino)
#define PIN_INV_TX                   3    // to the MAX3232 TTL input (Arduino -> inverter)

//define the log
#define LOG_FILE                     "LOG.TXT"   // 8.3 name
#define LOG_INTERVAL_MS              5000UL
#define LOG_LINE_SIZE                48

//define the inverter (MAX3232 on a software serial port, pins 0/1 stay free for the FTDI)
#define INVERTER_BAUD                2400
#define INV_COMMAND                  "QMOD"      // one read-only query: QMOD, QPIGS or QPIWS
#define INV_INTERVAL_MS              10000UL
#define INV_TIMEOUT_MS               2000UL      // the slowest reply (QPIGS) takes about 0.5 s
#define INV_REPLY_SIZE               128
#define TEST_FILE                    "TEST.TXT"  // 8.3 name

//define the DS1307
#define RTC_ADDRESS                  0x68
#define RTC_FORCE_SET                0       // 1 for ONE upload sets the clock to the compile time, then back to 0
#define STAMP_SIZE                   20   // "YYYY-MM-DD HH:MM:SS" + 0

//define the serial port (only with an FTDI cable, pins 0/1 are for RS-232 later)
#define SERIAL_BAUD                  9600

#define FALSE                        0
#define TRUE                         1

// globals ///////////////////////////////////////////////////////////////////

bool sdOk = FALSE;                   // SD card started and writable
bool rtcOk = FALSE;                  // DS1307 found and running
unsigned long lastLog = 0;           // millis() of the last log line
unsigned long lineNr = 0;            // lines written since boot

SoftwareSerial inverter(PIN_INV_RX, PIN_INV_TX);
byte invBuf[INV_REPLY_SIZE];         // the last reply from the inverter
byte invLen = 0;
unsigned long lastQuery = 0;         // millis() of the last query

// the time as read from the DS1307
byte rtcSecond, rtcMinute, rtcHour, rtcDay, rtcMonth;
unsigned int rtcYear;

void blink(byte times, unsigned int ms)
{
  for (byte i = 0; i < times; i++)
  {
    digitalWrite(PIN_LED, HIGH); delay(ms);
    digitalWrite(PIN_LED, LOW);  delay(ms);
  }
}

#endif
