/*****************************************************************************
 *  SquidMach Bootstrap v0.1                                                 *
 *  Copyright (C)2009 SquidMan (Alex Marshall)       <SquidMan72@gmail.com>  *
 *****************************************************************************/
	.set noreorder
	.equ	SYSCALLS_PRINT_HEX,  1
	.equ	SYSCALLS_PRINT_TEXT, 2

.section .info, "ax", @progbits
about_1:
	.asciiz "SquidMach Bootstrap v0.1.0\n"
about_2:
	.asciiz "Copyright (C)2009 SquidMan (Alex Marshall)\n"
about_3:
	.asciiz "Protected under the GNU GPLv2\n\n"

.section .code, "ax", @progbits
	.org 0x0000			/* Will be at 0xBFC00000 */
					/* Standard SquidMach boot procedure */
_start:
        /* Set the BEV bit. */
        mfc0 $t0, $12
        or   $t0, 0x400000
        mtc0 $t0, $12

        la   $a0, about_1
	lui  $t0, 0x4000		/* Print text. */
	sw   $a0, 0x0004($t0)
	nop
        la   $a0, about_2
	lui  $t0, 0x4000		/* Print text. */
	sw   $a0, 0x0004($t0)
	nop
        la   $a0, about_3
	lui  $t0, 0x4000		/* Print text. */
	sw   $a0, 0x0004($t0)
	nop

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
	
	li   $a0, 0			/* a0 is bootstrap major version. */
	li   $a1, 1			/* a1 is bootstrap minor version. */
	li   $a2, 0			/* a2 is bootstrap revision. */
	lui  $a3, 0x4000		/* a3 is hardware version. */
	lw   $a3, 0x0000($a3)
	lui  $ra, 0x1000		/* Load up the entry point and jump to it. */
	lw   $ra, 0x0000($ra)
	jr   $ra
	nop
hang:					/* Hang if we somehow continue. */
        b    hang
        nop

					/* Standard SquidMach exception handlers */
	.org 0x0200			/* Will be at 0xBFC00200 */
__exception_tlb_handle:
	move $s7, $ra			/* Save the link register into $s7 */
	li   $a0, 1
	lui  $t0, 0x1000		/* Load up the exception handler and jump to it. */
	lw   $t0, 0x0004($t0)
	jalr $t0
	nop
	eret

	.org 0x0280			/* Will be at 0xBFC00280 */
__exception_xtlb_handle:
	move $s7, $ra			/* Save the link register into $s7 */
	li   $a0, 2
	lui  $t0, 0x1000		/* Load up the exception handler and jump to it. */
	lw   $t0, 0x0004($t0)
	jalr $t0
	nop
	eret

	.org 0x0300			/* Will be at 0xBFC00300 */
__exception_cache_handle:
	li   $a0, 3
	lui  $t0, 0x1000		/* Load up the exception handler and jump to it. */
	lw   $t0, 0x0004($t0)
	jalr $t0
	nop
	eret

	.org 0x0380			/* Will be at 0xBFC00380 */
__exception_other_handle:
	move $s7, $ra			/* Save the link register into $s7 */
	li   $t1, 0x20
	mfc0 $t0, $13
	beq  $t0, $t1, syscall_handler
	nop
	li   $a0, 4
	lui  $t0, 0x1000		/* Load up the exception handler and jump to it. */
	lw   $t0, 0x0004($t0)
	jalr $t0
	nop
	eret

	.org 0x0400
print_text:
	lui  $t0, 0x4000		/* Print text. */
	sw   $a0, 0x0004($t0)
	eret

print_hex:
	lui  $t0, 0x4000		/* Print hex. */
	sd   $a0, 0x0008($t0)
	eret

syscall_handler:
	li   $t0, SYSCALLS_PRINT_HEX
	beq  $t0, $v0, print_hex
	nop
	li   $t0, SYSCALLS_PRINT_TEXT
	beq  $t0, $v0, print_text
	nop
	eret


