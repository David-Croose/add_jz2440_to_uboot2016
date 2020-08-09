/// #include <asm/spl.h>
/// #include <linux/types.h>
/// #include <asm/types.h>

#include <common.h>
/// #include <asm/io.h>
/// #include <asm/errno.h>
#include <spl.h>

/// void spl_board_init(void)
/// {
/// 	// nand init
/// 	preloader_console_init();
/// }

u32 spl_boot_device(void)
{
	return BOOT_DEVICE_NAND;
}

/// int timer_init(void)
/// {
/// 	/* spl doesn't need timer */
/// 	return 0;
/// }

inline void hang(void)
{
	for (;;)
		;
}


void board_init_f(ulong dummy)
{
	/// preloader_console_init();

}
