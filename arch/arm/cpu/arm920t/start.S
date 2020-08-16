/*
 *  armboot - Startup Code for ARM920 CPU-core
 *
 *  Copyright (c) 2001	Marius Gröger <mag@sysgo.de>
 *  Copyright (c) 2002	Alex Züpke <azu@sysgo.de>
 *  Copyright (c) 2002	Gary Jennejohn <garyj@denx.de>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <asm-offsets.h>
#include <common.h>
#include <config.h>

/*
 *************************************************************************
 *
 * Startup Code (called from the ARM reset exception vector)
 *
 * do important init only if we don't start from memory!
 * relocate armboot to ram
 * setup stack
 * jump to second stage
 *
 *************************************************************************
 */

/**
 * clock registers
 */
#define  CLKDIV         0x4C000014
#define  MPLLCON        0x4C000004

	.globl	reset

reset:
	/**
	 * disable the watch dog timer
	 */
	ldr		r0, =0x53000000
	mov		r1, #0
	str		r1, [r0]

#if 0
	/**
	 * make the clock run in 400MHz
	 */
	ldr		r0, =CLKDIV
	mov		r1, #5
	str		r1, [r0]
	/***************************/
	mrc		p15, 0, r1, c1, c0, 0
	orr		r1, r1, #0xC0000000
	mcr		p15, 0, r1, c1, c0, 0
	/***************************/
	ldr		r0, =MPLLCON
	ldr		r1, =0x5c012
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
	ldr		r0, =0x48000000
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
#endif

	bl	_main

/*------------------------------------------------------------------------------*/

	.globl	c_runtime_cpu_setup
c_runtime_cpu_setup:

	mov	pc, lr

/*
 *************************************************************************
 *
 * CPU_init_critical registers
 *
 * setup important registers
 * setup memory timing
 *
 *************************************************************************
 */


#ifndef CONFIG_SKIP_LOWLEVEL_INIT
cpu_init_crit:
#if 0
	/*
	 * flush v4 I/D caches
	 */
	mov	r0, #0
	mcr	p15, 0, r0, c7, c7, 0	/* flush v3/v4 cache */
	mcr	p15, 0, r0, c8, c7, 0	/* flush v4 TLB */

	/*
	 * disable MMU stuff and caches
	 */
	mrc	p15, 0, r0, c1, c0, 0
	bic	r0, r0, #0x00002300	@ clear bits 13, 9:8 (--V- --RS)
	bic	r0, r0, #0x00000087	@ clear bits 7, 2:0 (B--- -CAM)
	orr	r0, r0, #0x00000002	@ set bit 2 (A) Align
	orr	r0, r0, #0x00001000	@ set bit 12 (I) I-Cache
	mcr	p15, 0, r0, c1, c0, 0
#endif

	/*
	 * before relocating, we have to setup RAM timing
	 * because memory timing is board-dependend, you will
	 * find a lowlevel_init.S in your board directory.
	 */
	mov	ip, lr

	bl	lowlevel_init

	mov	lr, ip
	mov	pc, lr
#endif /* CONFIG_SKIP_LOWLEVEL_INIT */
