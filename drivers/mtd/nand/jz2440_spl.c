#include <linux/types.h>
#include <asm/arch/jz2440.h>

#define TACLS   2
#define TWRPH0  6
#define TWRPH1  2

static void nand_wati_idle(void)
{
	int i;

	while(!((*(volatile unsigned int *)NFSTAT) & 1))
	{
		for(i = 0; i < 10; i ++);
	}
}

static void nand_select(int flag)
{
	int i;

	if(flag == 1)
	{
		*(volatile unsigned int *)NFCONT &= ~(1 << 1);
	}
	else if(flag == 0)
	{
		*(volatile unsigned int *)NFCONT |= (1 << 1);
	}
	for(i = 0; i < 10; i ++);
}

static void nand_send_cmd(unsigned char cmd)
{
	*(volatile unsigned int *)NFCMD = cmd;
}

static void nand_send_addr(u32 addr)
{
	volatile unsigned int i, col, page;
	volatile unsigned char *p = (volatile unsigned char *)NFADDR;

	col = addr & (2048 - 1);
	page = addr / 2048;

	*p = col & 0xff; /* Column Address A0~A7 */
	for(i = 0; i < 10; i ++);
	*p = (col >> 8) & 0x0f; /* Column Address A8~A11 */
	for(i = 0; i < 10; i ++);
	*p = page & 0xff; /* Row Address A12~A19 */
	for(i = 0; i < 10; i ++);
	*p = (page >> 8) & 0xff; /* Row Address A20~A27 */
	for(i = 0; i < 10; i ++);
	*p = (page >> 16) & 0x03; /* Row Address A28~A29 */
	for(i = 0; i < 10; i ++);
}

static unsigned char nand_read_byte_raw(void)
{
	return *(volatile unsigned char *)NFDATA;
}

static void nand_reset(void)
{
	nand_select(1);
	nand_send_cmd(0xFF);
	nand_wati_idle();
	nand_select(0);
}

void nand_init(void)
{
	puts("nand_init\n");

	/// clock_enable(CLKSRC_NAND);
	*(volatile unsigned int *)NFCONF = (TACLS << 12) | (TWRPH0 << 8) | (TWRPH1 << 4);
	*(volatile unsigned int *)NFCONT = (1 << 4) | (1 << 1) | (1 << 0);
	nand_reset();
}

/*
 * copy @size bytes from @offs(in nandflash) into @dst(in SDRAM)
 */
int nand_spl_load_image(uint32_t offs, unsigned int size, void *dst)
{
	unsigned int i;
	unsigned char *buf = dst;

	puts("nand_spl_load_image start\n");
	nand_select(1);

	for(i = 0; i < size; i ++)
	{
		nand_send_cmd(0);
		nand_send_addr(offs ++);
		nand_send_cmd(0x30);
		nand_wati_idle();
		*buf = nand_read_byte_raw();
		buf ++;
	}

	puts("nand_spl_load_image end\n");
	nand_select(0);

	return 0;
}

void nand_deselect(void)
{
	puts("nand_deselect\n");
	nand_select(0);
}
