/*
 * Memory Setup stuff - taken from blob memsetup.S
 *
 * Copyright (C) 1999 2000 2001 Erik Mouw (J.A.K.Mouw@its.tudelft.nl) and
 *                     Jan-Derk Bakker (J.D.Bakker@its.tudelft.nl)
 *
 * Modified for the Samsung SMDK2410 by
 * (C) Copyright 2002
 * David Mueller, ELSOFT AG, <d.mueller@elsoft.ch>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */


#include <config.h>

#define CLKDIVN  0x4C000014
#define MPLLCON  0x4C000004
#define BWSCON   0x48000000

.globl lowlevel_init
lowlevel_init:
	/**
	 * make the clock run in 400MHz
	 */
	ldr		r0, =CLKDIVN
	mov		r1, #5
	str		r1, [r0]
	/***************************/
	mrc		p15, 0, r1, c1, c0, 0
	orr		r1, r1, #0xC0000000
	mcr		p15, 0, r1, c1, c0, 0
	/***************************/
	ldr		r0, =MPLLCON
	ldr		r1, =0x5c011
	str		r1, [r0]

	/**
	 * enable the I,D cache
	 */
	mrc		p15, 0, r0, c1, c0, 0
	orr		r0, r0, #0x0004
	orr		r0, r0, #0x1000
	mcr		p15, 0, r0, c1, c0, 0

	/**
	 * setup the SDRAM
	 */
	ldr		r0, =BWSCON
	ldr		r1, =0x22011110
	str		r1, [r0]
	add		r0, r0, #4
	ldr		r1, =0x00000700
	str		r1, [r0]
	add		r0, r0, #4
	ldr		r1, =0x00000700
	str		r1, [r0]
	add		r0, r0, #4
	ldr		r1, =0x00000700
	str		r1, [r0]
	add		r0, r0, #4
	ldr		r1, =0x00000700
	str		r1, [r0]
	add		r0, r0, #4
	ldr		r1, =0x00000700
	str		r1, [r0]
	add		r0, r0, #4
	ldr		r1, =0x00000700
	str		r1, [r0]
	add		r0, r0, #4
	ldr		r1, =0x00018005
	str		r1, [r0]
	add		r0, r0, #4
	ldr		r1, =0x00018005
	str		r1, [r0]
	add		r0, r0, #4
	ldr		r1, =0x008C07A3
	str		r1, [r0]
	add		r0, r0, #4
	ldr		r1, =0x000000B1
	str		r1, [r0]
	add		r0, r0, #4
	ldr		r1, =0x00000030
	str		r1, [r0]
	add		r0, r0, #4
	ldr		r1, =0x00000030
	str		r1, [r0]
	add		r0, r0, #4
	mov	pc, lr

