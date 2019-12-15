Support
=======

The easiest way to get help with the project is to join the `Brew Bubbles discussion on Homebrewtalk.com`_.  You may also open an issue on Github_.

.. _Brew Bubbles discussion on Homebrewtalk.com: https://support.brewbubbles.com
.. _Github: https://github.com/lbussy/brew-bubbles/issues

FAQ
---

No FAQ yet, I'll capture that as time goes on.

LED Flashing

//////////////////////////////////////////////////////////////////////////
//
// Frequency with which to blink LED when in Access Point blocking loop
//
#ifndef APBLINK
#define APBLINK 1000UL // 1000ms each state = 2000ms per cycle = 0.5Hz
#endif
//
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//
// Frequency with which to blink LED when in Station Connect blocking loop
//
#ifndef STABLINK
#define STABLINK 333UL // 333ms each state = 666ms per cycle = ~1.5Hz
#endif
//
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//
// Frequency with which to blink LED when in NTP blocking loop
//
#ifndef NTPBLINK
#define NTPBLINK 50UL // 50ms each state = 100ms per cycle = 10Hz
#endif
//
//////////////////////////////////////////////////////////////////////////

Serial Debug

