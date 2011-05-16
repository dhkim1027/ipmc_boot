#include <avr/io.h>
#include <stdlib.h>
#include <string.h>
#include "clksys.h"
#include "sp_driver.h"
#include "eeprom_driver.h"
#include "boot.h"

uint8_t write_buffer[FLASH_PAGE_SIZE];
uint8_t read_buffer[FLASH_PAGE_SIZE];

static uint32_t page_num = 0;
static uint32_t bytecount = 0;
static uint32_t write_buffer_count = 0;

uint8_t 
boot_get_mode(void)
{
	return EEPROM_ReadByte(0, BOOT_MODE_ADDR);
}

void 
boot_set_mode(uint8_t mode)
{
	EEPROM_WriteByte(0, BOOT_MODE_ADDR ,mode);
}

void  
boot_init_write_flash(void)
{
	uint8_t fw_type ;

	bytecount = 0;
	write_buffer_count = 0;

	fw_type = EEPROM_ReadByte(0, BOOT_FW_TYPE_ADDR);

	memset(write_buffer, 0xff, FLASH_PAGE_SIZE);

	if(fw_type == FW_TYPE_APP){		// Application Firmware
		page_num = APP_BLK_START;
	}else if(fw_type == FW_TYPE_BACKUP){	// Application Firmware Backup
		page_num = BACKUP_BLK_START;
	}else{
		page_num = APP_BLK_START;
	}
}

void  
boot_write_flash(uint8_t *data, uint32_t size)
{
	uint32_t i;

	for(i=0;i<size;i++){

		if(write_buffer_count >= FLASH_PAGE_SIZE){
			SP_LoadFlashPage(write_buffer);
			SP_EraseWriteApplicationPage(page_num * FLASH_PAGE_SIZE);
			SP_WaitForSPM();

			page_num++;
			write_buffer_count = 0;
			memset(write_buffer, 0xff, FLASH_PAGE_SIZE);
		}

		write_buffer[write_buffer_count] = data[i];

		write_buffer_count++;
		bytecount++;
	}
}

void  
boot_finish_write_flash(uint32_t size)
{
	if(write_buffer_count != 0){
		SP_LoadFlashPage(write_buffer);
		SP_EraseWriteApplicationPage(page_num * FLASH_PAGE_SIZE);
		SP_WaitForSPM();

		page_num = 0;
		write_buffer_count = 0;
		memset(write_buffer, 0xff, FLASH_PAGE_SIZE);
	}
}

void  
boot_jump_app_section(void)
{
	uint8_t temp;
	void (*funcptr)( void ) = 0x0000; // Set up function pointer to RESET vector.

	boot_set_mode(APP_MODE);
	SP_WaitForSPM();
	EIND = 0x00;

	// set interrupt vector for Application section
	temp = PMIC_CTRL & ~PMIC_IVSEL_bm;
	CCPWrite( &PMIC.CTRL, temp);

	funcptr();
}

void  
boot_recovery_fw(void)
{
	unsigned long i;

	for(i=APP_BLK_START;i<APP_BLK_END;i++){
		SP_EraseApplicationPage(i * FLASH_PAGE_SIZE);
		SP_WaitForSPM();
	}

	for(i=BACKUP_BLK_START;i<BACKUP_BLK_END;i++){
		SP_ReadFlashPage(read_buffer, i * FLASH_PAGE_SIZE);
		SP_LoadFlashPage(read_buffer);
		SP_WriteApplicationPage( (i - BACKUP_BLK_START) * FLASH_PAGE_SIZE );
		SP_WaitForSPM();

		d_sendchar('#');
	}

	d_sendchar('\r');
	d_sendchar('\n');

	d_sendchar('O');
	d_sendchar('K');
	d_sendchar('.');
	d_sendchar('\r');
	d_sendchar('\n');
}

void  
boot_backup_fw(void)
{
	unsigned long i;

	// Erase
	for(i=BACKUP_BLK_START;i<BACKUP_BLK_END;i++){
		SP_EraseApplicationPage(i * FLASH_PAGE_SIZE);
		SP_WaitForSPM();
	//	d_sendchar('#');
	}

	// Write
	for(i=APP_BLK_START;i<APP_BLK_END;i++){
		SP_ReadFlashPage(read_buffer, i * FLASH_PAGE_SIZE);
		SP_LoadFlashPage(read_buffer);
		SP_WriteApplicationPage( (i + BACKUP_BLK_START) * FLASH_PAGE_SIZE );
		SP_WaitForSPM();
	//	d_sendchar('#');
	}

}
