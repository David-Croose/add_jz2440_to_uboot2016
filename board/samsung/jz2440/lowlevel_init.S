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

.globl lowlevel_init
lowlevel_init:
	/**
	 * enable the I,D cache
	 */
	mrc	p15, 0, r0, c1, c0, 0
	orr	r0, r0, #0x0004
	orr	r0, r0, #0x1000
	mcr	p15, 0, r0, c1, c0, 0

	ldr	sp, =4096
	b	pre_init
