#ifndef __BOOT_H__
#define __BOOT_H__

/* definitions for SPM control */
#define SPMCR_REG           NVM.CTRLB
#define PAGESIZE            512
#define APP_END             0x20000
#define APP_FW_START        0x0
#define APP_FW_END          0x10000
#define APP_BK_START        0x10000     // Backup
#define APP_BK_END          0x20000
#define APP_FW              0
#define APP_BK              1

#define PAYLOAD_BOOT_MODE	0x0
#define CONSOLE_BOOT_MODE	0x1
#define APP_MODE			0x2

#define BOOT_PAGE_ADDR	0
#define BOOT_MODE_ADDR	0

void  boot_init(void);
uint8_t boot_get_mode(void);
void  boot_set_mode(uint8_t mode);
void  boot_init_write_flash(uint8_t section);
void  boot_write_flash(uint8_t *data, uint32_t size);
void  boot_finish_write_flash(uint32_t size);
void  boot_jump_app_section(void);
void  boot_reset_ipmc(void);



#endif
