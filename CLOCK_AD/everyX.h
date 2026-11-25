#ifndef EVERYX_H
#define EVERYX_H

#include "globals.h"

// Like arduboy.everyXFrames(), but based on millis().
//
//   if (everyXsec(5))   { ... }   // runs once every 5 seconds
//   if (everyXmin(10))  { ... }   // runs once every 10 minutes
//   if (everyXhours(1)) { ... }   // runs once every hour
//
// Every place in the code where you write one of these gets its OWN timer
// (a hidden static variable), so two everyXsec(5) lines do not disturb each other.
// The first time it fires is X after start-up.
//
// Notes:
// - millis() rolls over after ~49.7 days; the subtraction below handles that.
// - If the loop was blocked for longer than the interval (delay, SD write ...)
//   it fires ONCE and carries on from now (no burst of catch-up calls).
// - millis() is not as exact as the DS1307; over days it drifts a few seconds.
//   Use the RTC time for the timestamps, this only for "do something every X".
// - Use it in a fixed place (loop), not in a function that is called only sometimes:
//   the timer only counts from the last time THAT line fired.

// the real work: 'last' is the timer of one call site
bool everyXms(unsigned long interval, unsigned long &last)
{
  unsigned long now = millis();
  unsigned long passed = now - last;
  if (passed < interval) return FALSE;
  // keep the rhythm (no drift) unless we are more than one interval behind
  last = (passed < 2 * interval) ? last + interval : now;
  return TRUE;
}

// each use gets its own 'static' timer ({ } with a value is a GCC extension, fine on AVR)
#define everyXsec(n)    ({ static unsigned long _last = 0; everyXms((unsigned long)(n) * 1000UL, _last); })
#define everyXmin(n)    ({ static unsigned long _last = 0; everyXms((unsigned long)(n) * 60000UL, _last); })
#define everyXhours(n)  ({ static unsigned long _last = 0; everyXms((unsigned long)(n) * 3600000UL, _last); })

#endif
