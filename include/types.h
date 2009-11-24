#ifndef _TYPES
#define _TYPES

#define BITCOUNT		(64)
//#define BITCOUNT		(32)

#if ((BITCOUNT != 64) && (BITCOUNT != 32))
#error Target processor must be 32bit or 64bit!
#endif

typedef unsigned char		u8;
typedef unsigned short		u16;
typedef unsigned int		u32;
typedef unsigned long long	u64;
typedef signed char		s8;
typedef signed short		s16;
typedef signed int		s32;
typedef signed long long	s64;

#if BITCOUNT == 64
typedef s64			mipsRegister_s;
typedef u64			mipsRegister_u;
#elif BITCOUNT == 32
typedef s32			mipsRegister_s;
typedef u32			mipsRegister_u;
#endif
typedef mipsRegister_s		mipsRegister;

typedef u8			mipsReg;

typedef u32			mipsJumpAddress_u;
typedef s32			mipsJumpAddress_s;
typedef mipsJumpAddress_u	mipsJumpAddress;

typedef u8			mipsShift_u;
typedef s8			mipsShift_s;
typedef mipsShift_u		mipsShift;

typedef s16			mipsSignedImmediate;
typedef u16			mipsUnsignedImmediate;

typedef void			mipsInstruction;

#endif