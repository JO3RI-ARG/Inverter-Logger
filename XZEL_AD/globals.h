#ifndef GLOBALS_H
#define GLOBALS_H

#include <Arduino.h>
#include <SPI.h>
#include <SD.h>
#include <Wire.h>

// constants /////////////////////////////////////////////////////////////////

//define pins (Arduino Ethernet R3)
#define PIN_W5100_CS                 10   // Ethernet chip select
#define PIN_SD_CS                    4    // SD card chip select
#define PIN_LED                      9    // LED on the board

//define the log
#define LOG_FILE                     "LOG.TXT"   // 8.3 name
#define LOG_INTERVAL_MS              5000UL
#define LOG_LINE_SIZE                48

//define the DS1307
#define RTC_ADDRESS                  0x68
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
