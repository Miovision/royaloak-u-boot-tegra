/*
 * Copyright (c) 2016, NVIDIA CORPORATION
 * Copyright (c) 2020, Miovision
 *
 * SPDX-License-Identifier: GPL-2.0
 */

#include <common.h>
#include <i2c.h>
#include "../../nvidia/p2571/max77620_init.h"

int tegra_board_init(void)
{
	struct udevice *dev;
	uchar val;
	int ret;
	int err = 0;

	/*
	   NOTE: I2C bus numbering is not the same here as it is for the kernel;
	   Bus 1 here is i2c-0 for the kernel.
    */

	/* Turn on MAX77620 LDO3 to 3.3V for SD card power */
	debug("%s: Set LDO3 for VDDIO_SDMMC_AP power to 3.3V\n", __func__);
	ret = i2c_get_chip_for_busnum(0, MAX77620_I2C_ADDR_7BIT, 1, &dev);
	if (ret) {
		printf("%s: Cannot find MAX77620 I2C chip\n", __func__);
		err = ret;
	} else {
		/* 0xF2 for 3.3v, enabled: bit7:6 = 11 = enable, bit5:0 = voltage */
		val = 0xF2;
		ret = dm_i2c_write(dev, MAX77620_CNFG1_L3_REG, &val, 1);
		if (ret) {
			printf("i2c_write 0 0x3c 0x27 failed: %d\n", ret);
			err = ret;
		}
	}

	ret = i2c_get_chip_for_busnum(1, 0x4c, 1, &dev);
	if (ret) {
		printf("%s: Cannot find LM96063 I2C chip\n", __func__);
	} else {
		/* Turn on the tachometer function, it's off by default */
		val = 0x04;
		ret = dm_i2c_write(dev, 0x03, &val, 1);
		if (ret) {
			printf("i2c_write 0 0x4c 0x03 failed: %d\n", ret);
		}
	}

	return err;
}

