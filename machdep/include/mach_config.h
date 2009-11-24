#ifndef MACH_CONFIG_H
#define MACH_CONFIG_H

#define BITCOUNT		(64)

#if ((BITCOUNT != 64) && (BITCOUNT != 32))
#error Target processor must be 32bit or 64bit!
#endif

#endif
