/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (c) 2013-2020, NVIDIA CORPORATION.
 */

#ifndef _P2771_0000_H
#define _P2771_0000_H

#include <linux/sizes.h>

#include "tegra186-common.h"

/* High-level configuration options */
#define CONFIG_TEGRA_BOARD_STRING	"MIOVISION SMARTSENSE"

/* Environment in eMMC, at the end of 2nd "boot sector" */
#define CONFIG_SYS_MMC_ENV_DEV		0
#define CONFIG_SYS_MMC_ENV_PART		2

#include "tegra-common-post.h"

/* Crystal is 38.4MHz. clk_m runs at half that rate */
#define COUNTER_FREQUENCY	19200000

#endif
