#define CLKDIVN  0x4C000014
#define MPLLCON  0x4C000004
#define BWSCON   0x48000000

static void s3c2440_clock_init(void)
{
	// FCLK = FCLK / 1
	// HCLK = FCLK / 4
	// PCLK = FCLK / 8
	*(volatile unsigned int *)CLKDIVN = 0x05;
	__asm__ volatile
	(
		"mrc	p15, 0, r1, c1, c0, 0\r\n"
		"orr	r1, r1, #0xC0000000\r\n"
		"mcr	p15, 0, r1, c1, c0, 0\r\n"
	);
	*(volatile unsigned int *)MPLLCON = (0x5C << 12) | (0x01 << 4) | (0x01);

	// fclk: 400MHz
	// hclk: 100MHz
	// pclk: 50MHz
}

static void sdram_init(void)
{
	static const unsigned int __data[] = {
		0x22011110,
		0x00000700,
		0x00000700,
		0x00000700,
		0x00000700,
		0x00000700,
		0x00000700,
		0x00018005,
		0x00018005,
		0x008C07A3,
		0x000000B1,
		0x00000030,
		0x00000030,
	};
	int i;
	volatile unsigned int *p;

	p = (volatile unsigned int *)BWSCON;
	for (i = 0; i < sizeof(__data) / sizeof(__data[0]); i++) {
		p[i] = __data[i];
	}
}

void pre_init(void)
{
	s3c2440_clock_init();
	sdram_init();
}
