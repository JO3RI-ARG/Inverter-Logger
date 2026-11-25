/*
  CLOCK_AD

  CREATED by
  2026 - JO3RI

  License: MIT : https://opensource.org/licenses/MIT

*/

#define VERSION "0.1.0"

#include "globals.h"
#include "W5100.h"
#include "DS1307.h"
#include "everyX.h"



void setup()
{
  pinMode(PIN_LED, OUTPUT);


  rtcBegin();
  sdBegin();
}

void loop()
{

}
