GAS LISTING bootstrap.s 			page 1


   1              	/*****************************************************************************
   2              	 *  SquidMach Bootstrap v0.1                                                 *
   3              	 *  Copyright (C)2009 SquidMan (Alex Marshall)       <SquidMan72@gmail.com>  *
   4              	 *****************************************************************************/
   5              		.set noreorder
   6              		.equ	SYSCALLS_PRINT,	4
   7              	
   8              	.section .info, "ax", @progbits
   9              	about_1:
  10 0000 53717569 		.asciiz "SquidMach Bootstrap v0.1.0"
  10      644D6163 
  10      6820426F 
  10      6F747374 
  10      72617020 
  11              	about_2:
  12 001b 436F7079 		.asciiz "Copyright (C)2009 SquidMan (Alex Marshall)"
  12      72696768 
  12      74202843 
  12      29323030 
  12      39205371 
  13              	about_3:
  14 0046 50726F74 		.asciiz "Protected under the GNU GPLv2"
  14      65637465 
  14      6420756E 
  14      64657220 
  14      74686520 
  15              	
  16              	.section .code, "ax", @progbits
  17              		.org 0x0000			/* Will be at 0xBFC00000 */
  18              						/* Standard SquidMach boot procedure */
  19              	_start:
  20              	        /* Set the BEV bit. */
  21 0000 00600840 	        mfc0 $t0, $12
  22 0004 4000013C 	        or   $t0, 0x400000
  22      25400101 
  23 000c 00608840 	        mtc0 $t0, $12
  24              	
  25 0010 0000043C 	        la   $a0, about_1
  25      00008424 
  26 0018 0001000C 	        jal  print
  27 001c 00000000 		nop
  28 0020 0000043C 	        la   $a0, about_2
  28      1B008424 
  29 0028 0001000C 	        jal  print
  30 002c 00000000 		nop
  31 0030 0000043C 	        la   $a0, about_3
  31      46008424 
  32 0038 0001000C 	        jal  print
  33 003c 00000000 		nop
  34              	
  35 0040 2D100000 		move $v0, $zero
  36 0044 2D180000 		move $v1, $zero
  37 0048 2D480000 		move $t1, $zero
  38 004c 2D500000 		move $t2, $zero
  39 0050 2D580000 		move $t3, $zero
  40 0054 2D600000 		move $t4, $zero
  41 0058 2D680000 		move $t5, $zero
GAS LISTING bootstrap.s 			page 2


  42 005c 2D700000 		move $t6, $zero
  43 0060 2D780000 		move $t7, $zero
  44 0064 2D800000 		move $s0, $zero
  45 0068 2D880000 		move $s1, $zero
  46 006c 2D900000 		move $s2, $zero
  47 0070 2D980000 		move $s3, $zero
  48 0074 2DA00000 		move $s4, $zero
  49 0078 2DA80000 		move $s5, $zero
  50 007c 2DB00000 		move $s6, $zero
  51 0080 2DB80000 		move $s7, $zero
  52 0084 2DC00000 		move $t8, $zero
  53 0088 2DC80000 		move $t9, $zero
  54 008c 2DD00000 		move $k0, $zero
  55 0090 2DD80000 		move $k1, $zero
  56 0094 2DF80000 		move $ra, $zero
  57              		
  58 0098 00000424 		li   $a0, 0			/* a0 is bootstrap major version. */
  59 009c 01000524 		li   $a1, 1			/* a1 is bootstrap minor version. */
  60 00a0 00000624 		li   $a2, 0			/* a2 is bootstrap revision. */
  61 00a4 0040073C 		lui  $a3, 0x4000		/* a3 is hardware version. */
  62 00a8 0000E78C 		lw   $a3, 0x0000($a3)
  63 00ac FFFF083C 		lui  $t0, 0xFFFF		/* Load up the entry point and jump to it. */
  64 00b0 0100013C 		lw   $t0, 0xFF00($t0)
  64      21082800 
  64      00FF288C 
  65 00bc 08000001 		jr   $t0
  66 00c0 00000000 		nop
  67              	hang:					/* Hang if we somehow continue. */
  68 00c4 FFFF0010 	        b    hang
  69 00c8 00000000 	        nop
  70              	
  71              						/* Standard SquidMach exception handlers */
  72 00cc 00000000 		.org 0x0200			/* Will be at 0xBFC00200 */
  72      00000000 
  72      00000000 
  72      00000000 
  72      00000000 
  73              	__exception_tlb_handle:
  74 0200 2DB8E003 		move $s7, $ra			/* Save the link register into $s7 */
  75 0204 01000424 		li   $a0, 1
  76 0208 FFFF083C 		lui  $t0, 0xFFFF		/* Load up the exception handler and jump to it. */
  77 020c 0100013C 		lw   $t0, 0xFF04($t0)
  77      21082800 
  77      04FF288C 
  78 0218 08000001 		jr   $t0
  79 021c 00000000 		nop
  80 0220 A8FF0010 		b    hang
  81 0224 00000000 		nop
  82              	
  83 0228 00000000 		.org 0x0280			/* Will be at 0xBFC00280 */
  83      00000000 
  83      00000000 
  83      00000000 
  83      00000000 
  84              	__exception_xtlb_handle:
  85 0280 2DB8E003 		move $s7, $ra			/* Save the link register into $s7 */
  86 0284 02000424 		li   $a0, 2
GAS LISTING bootstrap.s 			page 3


  87 0288 FFFF083C 		lui  $t0, 0xFFFF		/* Load up the exception handler and jump to it. */
  88 028c 0100013C 		lw   $t0, 0xFF04($t0)
  88      21082800 
  88      04FF288C 
  89 0298 08000001 		jr   $t0
  90 029c 00000000 		nop
  91 02a0 88FF0010 		b    hang
  92 02a4 00000000 		nop
  93              	
  94 02a8 00000000 		.org 0x0300			/* Will be at 0xBFC00300 */
  94      00000000 
  94      00000000 
  94      00000000 
  94      00000000 
  95              	__exception_cache_handle:
  96 0300 2DB8E003 		move $s7, $ra			/* Save the link register into $s7 */
  97 0304 03000424 		li   $a0, 3
  98 0308 FFFF083C 		lui  $t0, 0xFFFF		/* Load up the exception handler and jump to it. */
  99 030c 0100013C 		lw   $t0, 0xFF04($t0)
  99      21082800 
  99      04FF288C 
 100 0318 08000001 		jr   $t0
 101 031c 00000000 		nop
 102 0320 68FF0010 		b    hang
 103 0324 00000000 		nop
 104              	
 105 0328 00000000 		.org 0x0380			/* Will be at 0xBFC00380 */
 105      00000000 
 105      00000000 
 105      00000000 
 105      00000000 
 106              	__exception_other_handle:
 107 0380 2DB8E003 		move $s7, $ra			/* Save the link register into $s7 */
 108 0384 04000424 		li   $a0, 4
 109 0388 FFFF083C 		lui  $t0, 0xFFFF		/* Load up the exception handler and jump to it. */
 110 038c 0100013C 		lw   $t0, 0xFF04($t0)
 110      21082800 
 110      04FF288C 
 111 0398 08000001 		jr   $t0
 112 039c 00000000 		nop
 113 03a0 48FF0010 		b    hang
 114 03a4 00000000 		nop
 115              	
 116 03a8 00000000 		.org 0x0400
 116      00000000 
 116      00000000 
 116      00000000 
 116      00000000 
 117              	print:
 118 0400 0800E003 		jr   $ra
 119 0404 00000000 		nop
 120 0408 2EFF0010 		b    hang
 121 040c 00000000 		nop
