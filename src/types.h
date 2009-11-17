#ifndef _TYPES
#define _TYPES

typedef unsigned char		u8;
typedef unsigned short		u16;
typedef unsigned int		u32;
typedef unsigned long long	u64;
typedef signed char		s8;
typedef signed short		s16;
typedef signed int		s32;
typedef signed long long	s64;

typedef s64			mipsRegister_s;
typedef u64			mipsRegister_u;
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