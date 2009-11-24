/*****************************************************************************
 *  SquidMach Bootstrap v0.1                                                 *
 *  Copyright (C)2009 SquidMan (Alex Marshall)       <SquidMan72@gmail.com>  *
 *****************************************************************************/
	.set noreorder
	.equ	SYSCALLS_PRINT,	4

.section .info, "ax", @progbits
about_1:
	.asciiz "SquidMach Bootstrap v0.1.0"
about_2:
	.asciiz "Copyright (C)2009 SquidMan (Alex Marshall)"
about_3:
	.asciiz "Protected under the GNU GPLv2"

.section .code, "ax", @progbits
	.org 0x0000			/* Will be at 0xBFC00000 */
					/* Standard SquidMach boot procedure */
_start:
        /* Set the BEV bit. */
        mfc0 $t0, $12
        or   $t0, 0x400000
        mtc0 $t0, $12

        la   $a0, about_1
        jal  print
	nop
        la   $a0, about_2
        jal  print
	nop
        la   $a0, about_3
        jal  print
	nop

	move $v0, $zero
	move $v1, $zero
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
	lui  $t0, 0x1000		/* Load up the entry point and jump to it. */
	lw   $t0, 0x0000($t0)
	jr   $t0
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
	jr   $t0
	nop
	b    hang
	nop

	.org 0x0280			/* Will be at 0xBFC00280 */
__exception_xtlb_handle:
	move $s7, $ra			/* Save the link register into $s7 */
	li   $a0, 2
	lui  $t0, 0x1000		/* Load up the exception handler and jump to it. */
	lw   $t0, 0x0004($t0)
	jr   $t0
	nop
	b    hang
	nop

	.org 0x0300			/* Will be at 0xBFC00300 */
__exception_cache_handle:
	move $s7, $ra			/* Save the link register into $s7 */
	li   $a0, 3
	lui  $t0, 0x1000		/* Load up the exception handler and jump to it. */
	lw   $t0, 0x0004($t0)
	jr   $t0
	nop
	b    hang
	nop

	.org 0x0380			/* Will be at 0xBFC00380 */
__exception_other_handle:
	move $s7, $ra			/* Save the link register into $s7 */
	li   $a0, 4
	lui  $t0, 0x1000		/* Load up the exception handler and jump to it. */
	lw   $t0, 0x0004($t0)
	jr   $t0
	nop
	b    hang
	nop

	.org 0x0400
print:
	jr   $ra
	nop
	b    hang
	nop
