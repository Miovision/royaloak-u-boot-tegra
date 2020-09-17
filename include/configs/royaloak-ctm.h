/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * (C) Copyright 2020
 * Miovision Technologies Inc <www.miovision.com>
 *
 * SPDX-License-Identifier:     GPL-2.0+
 */

#ifndef _CTM_ROYALOAK_H
#define _CTM_ROYALOAK_H

#include <linux/sizes.h>

#include "tegra210-common.h"

/* Board-specific serial config */
#define CONFIG_TEGRA_ENABLE_UARTA

#ifdef CONFIG_P3450_EMMC
#define CONFIG_TEGRA_BOARD_STRING	"Royal Oak CTM"
#define BOOT_TARGET_DEVICES(func) \
	func(MMC, mmc, 0) \
	func(PXE, pxe, na) \
	func(DHCP, dhcp, na)
#define CONFIG_SYS_MMC_ENV_DEV		0
#define CONFIG_SYS_MMC_ENV_PART		2
#define DEFAULT_MMC_DEV			"0"
#else
/* High-level configuration options */
#define CONFIG_TEGRA_BOARD_STRING	"Royal Oak CTM"

/* Only MMC/PXE/DHCP for now, add USB back in later when supported */
#define BOOT_TARGET_DEVICES(func) \
	func(MMC, mmc, 1) \
	func(PXE, pxe, na) \
	func(DHCP, dhcp, na)

/* Environment at end of QSPI, in the VER partition */
#define CONFIG_ENV_SPI_MAX_HZ		48000000
#define CONFIG_ENV_SPI_MODE		SPI_MODE_0
#define CONFIG_SPI_FLASH_SIZE		(4 << 20)
#define DEFAULT_MMC_DEV			"1"
#endif

#define CONFIG_PREBOOT

#define BOARD_EXTRA_ENV_SETTINGS \
	"preboot=if test -e mmc " DEFAULT_MMC_DEV ":1 /u-boot-preboot.scr; then " \
		"load mmc " DEFAULT_MMC_DEV ":1 ${scriptaddr} /u-boot-preboot.scr; " \
		"source ${scriptaddr}; " \
	"fi\0"

/* General networking support */
#include "tegra-common-usb-gadget.h"
#include "tegra-common-post.h"

/* Crystal is 38.4MHz. clk_m runs at half that rate */
#define COUNTER_FREQUENCY	19200000

#endif /* _CTM_ROYALOAK_H */
