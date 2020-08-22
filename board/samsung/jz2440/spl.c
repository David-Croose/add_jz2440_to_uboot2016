#include <common.h>
#include <spl.h>
#include <asm/arch/jz2440.h>

static void uart0_init(void)
{
#define PCLK 50000000
#define BAUDRATE 115200
	unsigned int tmp;

	clock_enable(CLKSRC_UART0);

	// set GPH2 multiplexed as TX0
	tmp = *(volatile unsigned int *)GPHCON;
	tmp &= ~((0x3 & 0x3) << (2 * 2));
	tmp |= (0x2 & 0x3) << (2 * 2);
	*(volatile unsigned int *)GPHCON = tmp;

	// set GPH3 multiplexed as RX0
	tmp = *(volatile unsigned int *)GPHCON;
	tmp &= ~((0x3 & 0x3) << (3 * 2));
	tmp |= (0x2 & 0x3) << (3 * 2);
	*(volatile unsigned int *)GPHCON = tmp;

	// set GPH2 pull up
	tmp = *(volatile unsigned int *)GPHUP;
	tmp &= ~(1 << 2);
	*(volatile unsigned int *)GPHUP = tmp;

	// set GPH3 pull up
	tmp = *(volatile unsigned int *)GPHUP;
	tmp &= ~(1 << 3);
	*(volatile unsigned int *)GPHUP = tmp;

	tmp = 0;
	tmp |= (3 & 0x3) << 0;    // Normal mode operation
	tmp |= (0 & 0x1) << 2;    // No parity
	tmp |= (0 & 0x7) << 3;    // One stop bit per frame
	tmp |= (0 & 0x1) << 6;    // 8-bits data
	*(volatile unsigned int *)ULCON0 = tmp;

	tmp = 0;
	tmp |= (1 & 0x3) << 0;    // Rx Interrupt request or polling mode
	tmp |= (1 & 0x3) << 2;    // Tx Interrupt request or polling mode
	tmp |= (0 & 0x1) << 4;    // Don't send break signal while transmitting
	tmp |= (0 & 0x1) << 5;    // Don't use loopback mode
	tmp |= (1 & 0x1) << 6;    // Generate receive error status interrupt
	tmp |= (1 & 0x1) << 7;    // Disable Rx time out interrupt when UART FIFO is enabled. The interrupt is a receive interrupt
	tmp |= (0 & 0x1) << 8;    // Interrupt is requested the instant Rx buffer receivesthe data in Non-FIFO mode or reaches Rx FIFO Trigger Level inFIFO mode
	tmp |= (0 & 0x1) << 9;    // Interrupt is requested as soon as the Tx bufferbecomes empty in Non-FIFO mode or reaches Tx FIFO TriggerLevel in FIFO mode
	tmp |= (0 & 0x3) << 10;   // Select PCLK as the source clock of UART0
	*(volatile unsigned int *)UCON0 = tmp;

	// UBRDIVn = (int)( UART clock / ( buad rate x 16) ) –1
	*(volatile unsigned int *)UBRDIV0 = (PCLK * 10 / BAUDRATE / 16 % 10) >= 5
	                   ? (PCLK / BAUDRATE / 16 + 1 - 1)
					   : (PCLK / BAUDRATE / 16 - 1);
#if 0
	*(volatile unsigned int *)SRCPND |= (1 << 28);    // clear uart0's irq request pending bit, whether it is masked or not
	*(volatile unsigned int *)INTPND |= (1 << 28);    // clear uart0's irq request pending bit
	*(volatile unsigned int *)SUBSRCPND |= 1;         // clear uart0's rx irq pending flag

	*(volatile unsigned int *)INTMSK &= ~(1 << 28);   // make INT_UART0 Irq available
#endif
}

void clock_enable(unsigned int clknum)
{
	unsigned int tmp;

	tmp = *(volatile unsigned int *)CLKCON;
	tmp |= clknum;
	*(volatile unsigned int *)CLKCON = tmp;
}

unsigned int spl_boot_device(void)
{
	return BOOT_DEVICE_NAND;
}

inline void hang(void)
{
	puts("====> hang\n");
	for (;;)
		;
}

void putc(char c)
{
	while(!((*(volatile unsigned int *)UTRSTAT0) & (1 << 2)));
	*(volatile unsigned char *)UTXH0 = c;
}

void puts(const char *p)
{
	while(*p) {
		if (*p == '\n')
			putc('\r');
		putc(*p);
		p++;
	}
}

void board_init_f(ulong dummy)
{
	uart0_init();
	preloader_console_init();	/* TODO  need it? */
	spl_init();

	puts("board_init_f\n");
}
