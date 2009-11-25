/*****************************************************************************
 *  SquidMach Bootstrap v0.1                                                 *
 *  Copyright (C)2009 SquidMan (Alex Marshall)       <SquidMan72@gmail.com>  *
 *****************************************************************************/
	.set	noreorder
	.set	nomove

	.include "syscall.inc"
	
	.equ	BOOTSUPPORT_ENTRY,			0x10000000
	.equ	BOOTSUPPORT_EXCEPTION,			0x10000004

	.equ	HW_REG_HWVERSION,			0x40000000
	.equ	HW_REG_PRINT_TEXT,			0x40000004
	.equ	HW_REG_PRINT_HEX,			0x40000008
	.equ	HW_REG_PRINT_INT,			0x40000010
	
	.equ	BOOTSTRAP_MAJOR,			0
	.equ	BOOTSTRAP_MINOR,			1
	.equ	BOOTSTRAP_REV,				0


.section .info, "ax", @progbits
about_1:
	.asciiz "SquidMach Bootstrap v"
about_2:
	.asciiz "\nCopyright (C)2009 SquidMan (Alex Marshall)\n"
about_3:
	.asciiz "Protected under the GNU GPLv2\n\n"
period:
	.asciiz "."

.section .code, "ax", @progbits
	.org 0x0000			/* Will be at 0xBFC00000 */
					/* Standard SquidMach boot procedure */
_start:
        /* Set the BEV bit. */
        mfc0 $t0, $12
        or   $t0, 0x400000
        mtc0 $t0, $12

        la   $a0, about_1
	sw   $a0, HW_REG_PRINT_TEXT($zero)	/* Print text. */
	li   $a0, BOOTSTRAP_MAJOR		/* Bootstrap major version. */
	sd   $a0, HW_REG_PRINT_INT($zero)	/* Print integer. */
        la   $a0, period
	sw   $a0, HW_REG_PRINT_TEXT($zero)	/* Print text. */
	li   $a0, BOOTSTRAP_MINOR		/* Bootstrap minor version. */
	sd   $a0, HW_REG_PRINT_INT($zero)	/* Print integer. */
        la   $a0, period
	sw   $a0, HW_REG_PRINT_TEXT($zero)	/* Print text. */
	li   $a0, BOOTSTRAP_REV			/* Bootstrap revision. */
	sd   $a0, HW_REG_PRINT_INT($zero)	/* Print integer. */
        la   $a0, about_2
	sw   $a0, HW_REG_PRINT_TEXT($zero)	/* Print text. */
        la   $a0, about_3
	sw   $a0, HW_REG_PRINT_TEXT($zero)	/* Print text. */

	move $v0, $zero
	move $v1, $zero
	move $t0, $zero
	move $t1, $zero
	move $t2, $zero
	move $t3, $zero
	move $t4, $zero
	move $t5, $zero
	move $t6, $zero
	move $t7, $zero
	move $s0, $zero
	move $s1, $zero
	move $s2, $zero
	move $s3, $zero
	move $s4, $zero
	move $s5, $zero
	move $s6, $zero
	move $s7, $zero
	move $t8, $zero
	move $t9, $zero
	move $k0, $zero
	move $k1, $zero
	move $ra, $zero
	
	li   $a0, BOOTSTRAP_MAJOR		/* a0 is bootstrap major version. */
	li   $a1, BOOTSTRAP_MINOR		/* a1 is bootstrap minor version. */
	li   $a2, BOOTSTRAP_REV			/* a2 is bootstrap revision. */
	lw   $a3, HW_REG_HWVERSION($zero)	/* a3 is hardware version. */
	lw   $ra, BOOTSUPPORT_ENTRY($zero)	/* Load up the entry point and jump to it. */
	jr   $ra
	nop
hang:						/* Hang if we somehow continue. */
        b    hang
        nop

						/* Standard SquidMach exception handlers */
	.org 0x0200				/* Will be at 0xBFC00200 */
__exception_tlb_handle:
	mfc0 $t0, $13
	srl  $t0, $t0, 2
	lw   $t0, BOOTSUPPORT_EXCEPTION($zero)	/* Load up the exception handler and jump to it. */
	jalr $t0
	nop
	eret

	.org 0x0280				/* Will be at 0xBFC00280 */
__exception_xtlb_handle:
	mfc0 $a0, $13
	srl  $a0, $a0, 2
	lw   $t0, BOOTSUPPORT_EXCEPTION($zero)	/* Load up the exception handler and jump to it. */
	jalr $t0
	nop
	eret

	.org 0x0300				/* Will be at 0xBFC00300 */
__exception_cache_handle:
	mfc0 $t0, $13
	srl  $t0, $t0, 2
	lw   $t0, BOOTSUPPORT_EXCEPTION($zero)	/* Load up the exception handler and jump to it. */
	jalr $t0
	nop
	eret

	.org 0x0380				/* Will be at 0xBFC00380 */
__exception_other_handle:
	li   $t1, 0x8
	mfc0 $t0, $13
	srl  $t0, $t0, 2
	beq  $t0, $t1, syscall_handler
	nop
	lw   $t0, BOOTSUPPORT_EXCEPTION($zero)	/* Load up the exception handler and jump to it. */
	jalr $t0
	nop
	eret

	.org 0x0400
print_text:
	sw   $a0, HW_REG_PRINT_TEXT($zero)	/* Print text. */
	li   $a0, 1
	eret

print_hex:
	sd   $a0, HW_REG_PRINT_HEX($zero)	/* Print hex. */
	li   $a0, 1
	eret

print_int:
	sd   $a0, HW_REG_PRINT_INT($zero)	/* Print integer. */
	li   $a0, 1
	eret

get_hardware_version:
	lw   $a0, HW_REG_HWVERSION($zero)	/* Load v0 with hardware version. */
	eret

get_bootstrap_version:
	li   $a0, BOOTSTRAP_MAJOR		/* v0 is bootstrap major version. */
	li   $a1, BOOTSTRAP_MINOR		/* v1 is bootstrap minor version. */
	li   $a2, BOOTSTRAP_REV			/* v2 is bootstrap revision. */
	eret

syscall_handler:
	li   $t0, SYSCALLS_PRINT_HEX
	beq  $t0, $v0, print_hex
	nop
	li   $t0, SYSCALLS_PRINT_INT
	beq  $t0, $v0, print_int
	nop
	li   $t0, SYSCALLS_PRINT_TEXT
	beq  $t0, $v0, print_text
	nop
	li   $t0, SYSCALLS_GET_HW_VERSION
	beq  $t0, $v0, get_hardware_version
	nop
	li   $t0, SYSCALLS_GET_BOOTSTRAP_VERSION
	beq  $t0, $v0, get_bootstrap_version
	nop
	eret


