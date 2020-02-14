/*
 * Copyright (c) 2016, NVIDIA CORPORATION
 * Copyright (c) 2020, Miovision
 *
 * SPDX-License-Identifier: GPL-2.0
 */

#include <common.h>
#include <i2c.h>
#include "../../nvidia/p2571/max77620_init.h"
#include <asm-generic/gpio.h>
#include <dm/device.h>

typedef enum {
	TXSLOT_INVALID,
	TXSLOT_A,
	TXSLOT_B,
} txslot_t;

static txslot_t txslot = TXSLOT_INVALID;

int tegra_board_init(void)
{
	struct udevice *dev;
	uchar val;
	int ret;
	int err = 0;
	char *name = "x6"; // port x, offset 6 aka u-boot gpio #158 aka linux gpio #478
	struct gpio_desc desc;

	/*
	 * This should be doable via DTS but there's a missing driver; there
	 * needs to be a UCLASS_GPIO_ACTUAL_PINS and then a driver that will
	 * handle a DTS entry that is just the pins. Current gpio drivers are
	 * all for GPIO controller chips.
	 */
	ret = dm_gpio_lookup_name(name, &desc);
	if (ret) {
		error("dm_gpio_lookup_name() failed (%d), assuming Slot A", ret);
		txslot = TXSLOT_A;
	} else {
		ret = dm_gpio_request(&desc, "txslot-gpios");
		if (ret) {
			error("dm_gpio_request() failed (%d), assuming Slot A", ret);
			txslot = TXSLOT_A;
		} else {
			ret = dm_gpio_get_value(&desc);
			if (ret) {
				txslot = TXSLOT_A;
			} else {
				txslot = TXSLOT_B;
			}
		}
	}

	/*
	   NOTE: I2C bus numbering is not the same here as it is for the kernel;
	   Bus 1 here is i2c-0 for the kernel.
	   Is this due to differences between the u-boot and kernel DTSs?
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

	if (txslot == TXSLOT_A) {
		// can this be handled by the DTS?
		ret = i2c_get_chip_for_busnum(1, 0x4c, 1, &dev);
		if (ret) {
			printf("%s: Cannot find LM96063 I2C chip\n", __func__);
			err = ret;
		} else {
			/* Turn on the tachometer function, it's off by default */
			val = 0x04;
			ret = dm_i2c_write(dev, 0x03, &val, 1);
			if (ret) {
				printf("i2c_write 0 0x4c 0x03 failed: %d\n", ret);
				err = ret;
			}
		}
	}

	return err;
}

#ifndef CONFIG_OF_BOARD_SETUP
#error "Smartsense requires CONFIG_OF_BOARD_SETUP, please enable"
#else
#include <fdt_support.h>
#include <linux/ctype.h>

static int match_slot(txslot_t txslot, char *slots)
{
	char asc_txslot;
	char slot;

	if (txslot == TXSLOT_INVALID || !slots)
		return 0;

	asc_txslot = (char)(txslot - 1 + 'a');

	for (slot = *slots++; slot; slot = *slots++) {
		if (slot == ',' || slot == ' ' || slot == '\t')
			continue;
		if (tolower(slot) == asc_txslot)
			return 1;
	}

	return 0;
}

int ft_board_setup(void *blob, bd_t *bd)
{
	int subnode;
	const char *name;
	int len;
	const void *value;
	int ret;

	if (txslot == TXSLOT_INVALID) {
		error("txslot is invalid, skipping board setup");
		return 0;
	}

	fdt_for_each_subnode(blob, subnode, 0) {
		name = fdt_get_name(blob, subnode, &len);
		if (!name)
			name = "(unnamed)";

		value = fdt_getprop(blob, subnode, "smartsense,txslots", &len);
		if (!value) {
			if (len != -FDT_ERR_NOTFOUND)
				error("fdt_getprop() failed on subnode '%s', (%d)", name, len);
			continue;
		}

		if (!match_slot(txslot, (char *)value)) {
			/* NOTE: fdt_del_node() says it may renumber the nodes. This doesn't seem
			 * to be an issue for the foreach loop, yet. */
			ret = fdt_del_node(blob, subnode);
			if (ret < 0) {
				error("Couldn't delete node '%s', (%d)", name, ret);
			}
		}
	}

	return 0;
}
#endif