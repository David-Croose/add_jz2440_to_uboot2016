#include <common.h>
#include <spl.h>
#include <asm/arch/jz2440.h>

static void s3c2440_clock_init(void)
{
	// FCLK = FCLK / 1
	// HCLK = FCLK / 4
	// PCLK = FCLK / 8
	*(volatile unsigned int *)CLKDIVN = 0x05;
	__asm__
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

void uart0_putc(unsigned char sendbyte)
{
	while(!((*(volatile unsigned int *)UTRSTAT0) & (1 << 2)));
	*(volatile unsigned char *)UTXH0 = sendbyte;
}

void uart0_puts(const char *p)
{
	while(*p) {
		if (*p == '\n')
			uart0_putc('\r');
		uart0_putc(*p);
		p++;
	}
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
	uart0_puts("====> hang\n");
	for (;;)
		;
}

int	serial_init(void)
{
	uart0_init();
	return 0;
}

void led_init(void)
{
	int tmp;

	clock_enable(CLKSRC_GPIO);

	// GPF4 set output
	tmp = *(volatile unsigned int *)GPFCON;
	tmp &= ~((0x3 & 0x3) << (4 * 2));
	tmp |= (0x1 & 0x3) << (4 * 2);
	*(volatile unsigned int *)GPFCON = tmp;

	// GPF5 set output
	tmp = *(volatile unsigned int *)GPFCON;
	tmp &= ~((0x3 & 0x3) << (5 * 2));
	tmp |= (0x1 & 0x3) << (5 * 2);
	*(volatile unsigned int *)GPFCON = tmp;

	// GPF6 set output
	tmp = *(volatile unsigned int *)GPFCON;
	tmp &= ~((0x3 & 0x3) << (6 * 2));
	tmp |= (0x1 & 0x3) << (6 * 2);
	*(volatile unsigned int *)GPFCON = tmp;

	// GPF4, GPF5, GPF6 set to be high
	*(volatile unsigned int *)GPFDAT |= 1 << 4;
	*(volatile unsigned int *)GPFDAT |= 1 << 5;
	*(volatile unsigned int *)GPFDAT |= 1 << 6;
}

void led_ctrl(int select, int light)
{

	int i;

	// there are 3 leds in total
	for(i = 0; i < 3; i ++)
	{
		if((select >> i) & 1)
		{
			switch(i)
			{
			case 0:
				if(light)
				{
					*(volatile unsigned int *)GPFDAT &= ~(1 << 4);
				}
				else
				{
					*(volatile unsigned int *)GPFDAT |= 1 << 4;
				}
				break;

			case 1:
				if(light)
				{
					*(volatile unsigned int *)GPFDAT &= ~(1 << 5);
				}
				else
				{
					*(volatile unsigned int *)GPFDAT |= 1 << 5;
				}
				break;

			case 2:
				if(light)
				{
					*(volatile unsigned int *)GPFDAT &= ~(1 << 6);
				}
				else
				{
					*(volatile unsigned int *)GPFDAT |= 1 << 6;
				}
				break;
			}
		}
	}

}

void board_init_f(ulong dummy)
{
	unsigned int __data[] = {
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
	s3c2440_clock_init();

	preloader_console_init();

	////////////////////////////////////////////////
#if 1
	p = (volatile unsigned int *)0x48000000;
	for (i = 0; i < 13; i++) {
		*p = (volatile unsigned int *)&__data[i];
		p++;
	}
#endif
	////////////////////////////////////////////////

	spl_init();

	led_init();
	led_ctrl(1, 1);

	uart0_puts("\nboard_init_f\n");

	uart0_puts("SDRAM testing...\n");
	for (i = 0; i < 1000; i++) {
		*(volatile unsigned char *)(0x30000000 + i) = i;
		if (*(volatile unsigned char *)(0x30000000 + i) != i) {
			uart0_puts("SDRAM test error\n");
			led_ctrl(7, 1);
			while(1);
		}
	}

	uart0_puts("SDRAM test done\n");

}
