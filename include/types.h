#ifndef _TYPES
#define _TYPES

#include "mach_config.h"

#ifdef __APPLE__
#ifndef __MACOSX__
#define __MACOSX__	1
#endif
#endif

#define INLINE		__attribute__((always_inline)) inline

#define NORMAL_INST_COUNT	0x3F
#define SPECIAL_INST_COUNT	0x3F
#define REGIMM_INST_COUNT	0x1F
#define COPROC_INST_COUNT	0x1F
#define COPROC_BC_INST_COUNT	0x1F
#define COPROC_COP0_INST_COUNT	0x1F

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

#define ENDIANNESS_BE		(0x02)
#define ENDIANNESS_LE		(0x01)

#define DEFAULT_INSTRUCTION_PC	4
#define DEFAULT_CYCLES		1

#define REGISTER_HI 		32
#define REGISTER_LO		33

#define INSTRUCTION(opcode)	(opcode >> 26)
#define FUNCT(opcode)		(opcode & 0x3F)

#define RS(opcode)		((opcode >> 21) & 31)
#define RT(opcode)		((opcode >> 16) & 31)
#define RD(opcode)		((opcode >> 11) & 31)
#define SHIFT(opcode)		((opcode >> 6) & 31)
#define IMMEDIATE(opcode)	(opcode & 0xFFFF)
#define JUMP(opcode)		(opcode & 0x3FFFFFF)

enum {
	COP0_REG_CONTEXT 	= 4,
	COP0_REG_BADVADDR 	= 8,
	COP0_REG_COUNT 		= 9,
	COP0_REG_COMPARE 	= 11,
	COP0_REG_STATUS 	= 12,
	COP0_REG_CAUSE 		= 13,
	COP0_REG_EPC 		= 14,
	COP0_REG_WATCHLO 	= 18,
	COP0_REG_WATCHHI 	= 19,
	COP0_REG_XCONTEXT 	= 20,
	COP0_REG_ECC 		= 26,
	COP0_REG_CACHEERR 	= 27,
	COP0_REG_ERROREPC 	= 30
} COP0_REGISTERS;

enum {
	EXCEPTION_INTERRUPT	= 0,
	EXCEPTION_TLBMOD	= 1,
	EXCEPTION_TLBLOAD	= 2,
	EXCEPTION_TLBSTORE	= 3,
	EXCEPTION_BADADDR_LOAD  = 4,
	EXCEPTION_BADADDR_STORE = 5,
	EXCEPTION_BUSERR_FETCH	= 6,
	EXCEPTION_BUSERR_DATA	= 7,
	EXCEPTION_SYSCALL	= 8,
	EXCEPTION_BREAKPOINT	= 9,
	EXCEPTION_RESERVEDINSTR = 10,
	EXCEPTION_UNUSABLECOP	= 11,
	EXCEPTION_OVERFLOW	= 12,
	EXCEPTION_TRAP		= 13,
	EXCEPTION_VC_INSTR	= 14,
	EXCEPTION_FPU		= 15,
	EXCEPTION_WATCH		= 23,
	EXCEPTION_VC_DATA 	= 31
} EXCEPTION_CODES;

typedef struct _mipsDasm {
	u8 delay;
	u8 instruction;
	s8 rs;
	s8 rt;
	s8 rd;
	s32 shift;
	s16 immediate;
	s32 jump;
	s8 funct;
} mipsDasm;

typedef struct _mipsMappedMemory {
	u32 addrStart;
	u32 addrEnd;
	u32 size;
	u8  flags;
	u64* memory;
	struct _mipsMappedMemory *next;
} mipsMappedMemory;

typedef struct _mipsCpu {
	/* Backupped opcode for 1-instr-delay. */
	mipsRegister bOpcode;
	/* Program counters. */
	mipsRegister pc;
	mipsRegister nPc;
	/* Cycle counter */
	int cycles;
	/* Registers */
	mipsRegister r[34];
	mipsRegister cr[32][3];
	/* Ram accessors. */
	u8 endian;
	
	struct _mipsMappedMemory *rootBank;
	struct _mipsMappedMemory *mappedBanks;
	
	u8  (*readByte)   (struct _mipsCpu* cpu, u32 address);
	u16 (*readHword)  (struct _mipsCpu* cpu, u32 address);
	u32 (*readWord)   (struct _mipsCpu* cpu, u32 address);
	u64 (*readDword)  (struct _mipsCpu* cpu, u32 address);
	u32 (*readOpcode) (struct _mipsCpu* cpu, u32 address);
	
	void (*writeByte) (struct _mipsCpu* cpu, u32 address, u8 value);
	void (*writeHword)(struct _mipsCpu* cpu, u32 address, u16 value);
	void (*writeWord) (struct _mipsCpu* cpu, u32 address, u32 value);
	void (*writeDword)(struct _mipsCpu* cpu, u32 address, u64 value);
} mipsCpu;

typedef struct _mipsInstrTbl {
	mipsInstruction (*execute)(struct _mipsCpu* cpu, mipsDasm *dasm);
	char textDisasm[20];
	u8 delay;
	u8 cycles;
	u8 pc;
} mipsInstrTbl;

typedef struct _mipsCopInstrTbl {
	mipsInstruction (*execute)(struct _mipsCpu* cpu, mipsDasm *dasm, int cop);
	char textDisasm[20];
	u8 delay;
	u8 cycles;
	u8 pc;
} mipsCopInstrTbl;

#endif
