#ifndef __BOOT_H__
#define __BOOT_H__

/* definitions for SPM control */
#define SPMCR_REG           NVM.CTRLB
#define PAGESIZE            512
#if 0
#define APP_END             0x20000
#define APP_FW_START        0x0
#define APP_FW_END          0x10000
#define APP_BK_START        0x10000     // Backup
#define APP_BK_END          0x20000
#endif

#define APP_BLK_START		0
#define APP_BLK_END			128
#define BACKUP_BLK_START	128
#define BACKUP_BLK_END		256

// BOOT MODE
#define PAYLOAD_BOOT_MODE	0x1
#define CONSOLE_BOOT_MODE	0x2
#define APP_MODE			0x3

// Firmware Type
#define FW_TYPE_BACKUP	0
#define FW_TYPE_APP		1

#define BOOT_PAGE_ADDR		0

#define BOOT_MODE_ADDR		1
#define BOOT_FW_TYPE_ADDR	2

uint8_t boot_get_mode(void);
void  boot_set_mode(uint8_t mode);
void  boot_init_write_flash(void);
void  boot_write_flash(uint8_t *data, uint32_t size);
void  boot_finish_write_flash(uint32_t size);
void  boot_jump_app_section(void);
void  boot_recovery_fw(void);
void  boot_backup_fw(void);

#endif
