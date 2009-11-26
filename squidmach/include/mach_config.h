#ifndef MACH_CONFIG_H
#define MACH_CONFIG_H

#define SCREEN_X		(320)
#define SCREEN_Y		(240)
#define SCREEN_FPS		(60)

#define BITCOUNT		(64)
#define CLOCK_SPEED		(100 * 1000 * 1000)

#if ((BITCOUNT != 64) && (BITCOUNT != 32))
#error Target processor must be 32bit or 64bit!
#endif

#endif
