#include <common.h>
#include <spl.h>

u32 spl_boot_device(void)
{
	return BOOT_DEVICE_NAND;
}

inline void hang(void)
{
	for (;;)
		;
}


void board_init_f(ulong dummy)
{
	preloader_console_init();

}
