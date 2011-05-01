#include <avr/io.h>
#include <stdlib.h>
#include <string.h>
#include "clksys.h"
#include "sp_driver.h"
#include "eeprom_driver.h"
#include "boot.h"

uint8_t write_buffer[FLASH_PAGE_SIZE];
uint8_t read_buffer[FLASH_PAGE_SIZE];

static unsigned long address = 0;
static uint32_t page_num = 0;
static uint32_t bytecount = 0;
static uint32_t write_buffer_count = 0;

void  
boot_init(void)
{

}

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
boot_init_write_flash(uint8_t section)
{
	page_num = 0;
	bytecount = 0;
	write_buffer_count = 0;

	memset(write_buffer, 0xff, FLASH_PAGE_SIZE);

	if(section == APP_FW){		// Application Firmware
		address = APP_FW_START;
	}else if(section == APP_BK){	// Application Firmware Backup
		address = APP_BK_START;
	}else{

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
		address++;
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
boot_reset_ipmc(void)
{

}
