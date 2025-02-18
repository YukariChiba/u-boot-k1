#include <common.h>
#include <dm.h>
#include <env.h>
#include <image.h>
#include <splash.h>
#include <mmc.h>
#include <fb_spacemit.h>


#if defined(CONFIG_SPLASH_SCREEN) && defined(CONFIG_CMD_BMP)

int set_emmc_splash_location(struct splash_location *locations) {
	int dev_index = mmc_get_env_dev();
	int part_index;
	char devpart_str[16];
	int err;

	err = get_partition_index_by_name(BOOTFS_NAME, &part_index);
	if (err) {
		pr_err("Failed to get partition index for %s\n", BOOTFS_NAME);
		return -1;
	}

	snprintf(devpart_str, sizeof(devpart_str), "%d:%d", dev_index, part_index);

	locations[0].name = "emmc_fs";
	locations[0].storage = SPLASH_STORAGE_MMC;
	locations[0].flags = SPLASH_STORAGE_FS;
	locations[0].devpart = strdup(devpart_str);
	return 0;
}

int set_mmc_splash_location(struct splash_location *locations) {
	int dev_index = mmc_get_env_dev();
	int part_index;
	char devpart_str[16];
	int err;

	err = get_partition_index_by_name(BOOTFS_NAME, &part_index);
	if (err) {
		pr_err("Failed to get partition index for %s\n", BOOTFS_NAME);
		return -1;
	}

	snprintf(devpart_str, sizeof(devpart_str), "%d:%d", dev_index, part_index);

	locations[0].name = "mmc_fs";
	locations[0].storage = SPLASH_STORAGE_MMC;
	locations[0].flags = SPLASH_STORAGE_FS;
	locations[0].devpart = strdup(devpart_str);
	return 0;
}

int set_nor_splash_location(struct splash_location *locations) {
	int part, blk_index;
	char *blk_name;
	char devpart_str[16];

	if (get_available_boot_blk_dev(&blk_name, &blk_index)){
		printf("can not get available blk dev\n");
		return -1;
	}
	part = detect_blk_dev_or_partition_exist(blk_name, blk_index, BOOTFS_NAME);
	if (part < 0)
		return -1;

	snprintf(devpart_str, sizeof(devpart_str), "%d:%d", blk_index, part);

	if (!strcmp("mmc", blk_name))
		locations[0].name = "emmc_fs";
	else if (!strcmp("nvme", blk_name))
		locations[0].name = "nvme_fs";
	else
		return -1;

	locations[0].storage = SPLASH_STORAGE_NVME;
	locations[0].flags = SPLASH_STORAGE_FS;
	locations[0].devpart = strdup(devpart_str);
	return 0;
}

int set_nand_splash_location(struct splash_location *locations)
{
	char *nand_part = parse_mtdparts_and_find_bootfs();
	if (nand_part) {
		locations[0].name = "nand_fs";
		locations[0].storage = SPLASH_STORAGE_NAND;
		locations[0].flags = SPLASH_STORAGE_FS;
		locations[0].mtdpart = strdup(nand_part);
		locations[0].ubivol = strdup(BOOTFS_NAME);
		return 0;
	} else {
		pr_err("Failed to find bootfs on NAND\n");
		return -1;
	}
}

int load_splash_screen(void) {
	int ret;
	enum board_boot_mode boot_mode = get_boot_mode();
	struct splash_location splash_locations[1];

	memset(splash_locations, 0, sizeof(splash_locations));

	switch (boot_mode) {
		case BOOT_MODE_EMMC:
			ret = set_emmc_splash_location(splash_locations);
			break;
		case BOOT_MODE_SD:
			ret = set_mmc_splash_location(splash_locations);
			break;
		case BOOT_MODE_NAND:
			ret = set_nand_splash_location(splash_locations);
			break;
		case BOOT_MODE_NOR:
			ret = set_nor_splash_location(splash_locations);
			break;
		default:
			pr_err("Unsupported boot mode for splash screen\n");
			break;
	}
	if(ret)
		return -1;

	if (CONFIG_IS_ENABLED(SPLASH_SOURCE))
		return splash_source_load(splash_locations, ARRAY_SIZE(splash_locations));

	return splash_video_logo_load();
}

int splash_screen_prepare(void)
{
	enum board_boot_mode boot_mode = get_boot_mode();
	switch (boot_mode) {
	case BOOT_MODE_EMMC:
		env_set("splashsource", "emmc_fs");
		break;
	case BOOT_MODE_SD:
		env_set("splashsource", "mmc_fs");
		break;
	case BOOT_MODE_NAND:
		env_set("splashsource", "nand_fs");
		break;
	case BOOT_MODE_NOR:
		int blk_index;
		char *blk_name;

		if (get_available_boot_blk_dev(&blk_name, &blk_index)){
			printf("can not get available blk dev\n");
			return -1;
		}

		if (!strcmp("mmc", blk_name))
			env_set("splashsource", "emmc_fs");
		else if (!strcmp("nvme", blk_name))
			env_set("splashsource", "nvme_fs");
		else
			printf("not defind blk dev while prepare for splash screen\n");
		break;
	case BOOT_MODE_SHELL:
	case BOOT_MODE_USB:
	default:
		pr_err("Cannot support showing bootlogo in this boot mode!\n");
		break;
	}

	return load_splash_screen();
}

#endif
