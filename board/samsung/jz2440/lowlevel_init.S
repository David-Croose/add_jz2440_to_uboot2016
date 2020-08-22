#include <config.h>

#define BWSCON	0x48000000

.globl lowlevel_init
lowlevel_init:
	/* memory control configuration */
	/* make r0 relative the current location so that it */
	/* reads SMRDATA out of FLASH rather than memory ! */
	ldr     r0, =SMRDATA
	/* ldr	r1, =CONFIG_SYS_TEXT_BASE */
	/* sub	r0, r0, r1 */
	ldr	r1, =BWSCON	/* Bus Width Status Controller */
	add     r2, r0, #13*4
0:
	ldr     r3, [r0], #4
	str     r3, [r1], #4
	cmp     r2, r0
	bne     0b

	/**
	 * enable the I,D cache
	 */
	mrc		p15, 0, r0, c1, c0, 0
	orr		r0, r0, #0x0004
	orr		r0, r0, #0x1000
	mcr		p15, 0, r0, c1, c0, 0

	/* everything is fine now */
	mov	pc, lr

	.ltorg
/* the literal pools origin */

SMRDATA:
    .word 0x22011110
    .word 0x00000700
    .word 0x00000700
    .word 0x00000700
    .word 0x00000700
    .word 0x00000700
    .word 0x00000700
    .word 0x00018005
    .word 0x00018005
    .word 0x008C07A3
    .word 0x000000B1
    .word 0x00000030
    .word 0x00000030
