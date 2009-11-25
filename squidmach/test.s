/*****************************************************************************
 *  µChecksum 0.1                                                            *
 *  Quick'n'simple checksum alghoritm made in MIPS asm just for exercise.    *
 *  (C) 2009 - The Lemon Man                                                 *
 *  (C) 2009 - SquidMan                                                      *
 *****************************************************************************/
	.set	noreorder
	.set	nomove

	.include "syscall.inc"

.section .info, "wa", @progbits
info:
	.asciiz "µChecksum 0.1 - (C) 2009 - The Lemon Man & SquidMan\nRunning bootstrap v"
info2:
	.asciiz "\nHardware version "
newline:
	.asciiz "\n"
bootstrap_dot:
	.asciiz "."

.section .code, "ax", @progbits
_main:
	/* Print out my text. */
	la   $a0, info
	li   $v0, SYSCALLS_PRINT_TEXT
	syscall
	li   $v0, SYSCALLS_GET_BOOTSTRAP_VERSION
	syscall
	li   $v0, SYSCALLS_PRINT_INT
	syscall
	la   $a0, bootstrap_dot
	li   $v0, SYSCALLS_PRINT_TEXT
	syscall
	move $a0, $a1
	li   $v0, SYSCALLS_PRINT_INT
	syscall
	la   $a0, bootstrap_dot
	li   $v0, SYSCALLS_PRINT_TEXT
	syscall
	move $a0, $a2
	li   $v0, SYSCALLS_PRINT_INT
	syscall
	la   $a0, info2
	li   $v0, SYSCALLS_PRINT_TEXT
	syscall
	li   $v0, SYSCALLS_GET_HW_VERSION
	syscall
	li   $v0, SYSCALLS_PRINT_INT
	syscall
	la   $a0, newline
	li   $v0, SYSCALLS_PRINT_TEXT
	syscall
	/* Point the streameater to 0xBFC00000 and 0x20 as size. */
	la   $a0, 0xBFC00000
	li   $a1, 0x20
	jal  eatStream
	nop
	move $a0, $v0
	li   $v0, SYSCALLS_PRINT_HEX
	syscall
hang:
	j    hang
	nop

eatStream:
	/* Save the link register... */
	move $t6, $ra
	/* and our arguments. */
	move $t0, $a0
	move $t1, $a1
	add  $t1, $t1, $t0
	move $v0, $zero
eatLoop:
	/* Load 2 bytes... */
	lb   $t2, 0($t0)
	lb   $t3, 1($t0)
	/* ...get odd bits from first and even from the other... */
	andi $t2, $t2, 0xAA
	andi $t3, $t3, 0x55
	/* ...xor em up... */
	xor  $t2, $t2, $t3
	/* ...and add to the checksum! */
	add  $v0, $v0, $t2
	/* Go ahead till we reach the end. */
	addi $t0, $t0, 1
	bne  $t0, $t1, eatLoop
	nop
	/* Restore link register and go back. */
	move $ra, $t6
	jr   $ra
	nop

_exception:			/* Ignore any exceptions */
	move $ra, $s7
	jr   $ra
	nop

.section .bootsupport, "a", @progbits
	.word	_main
	.word	_exception
