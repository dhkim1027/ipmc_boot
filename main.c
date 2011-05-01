#include <stdlib.h>
#include <string.h>
#include <avr/io.h>
#include "irq.h"
#include "clksys.h"
#include "gpio.h"
#include "usart.h"
#include "sp_driver.h"
#include "eeprom_driver.h"
#include "boot.h"
#include "terminal.h"

void
proc_payload_bootmode(void)
{

}

void
proc_console_bootmode(void)
{
	uint8_t rx_data;
	usart_set_payload(CONSOLE_DATA);

	while(1){
		terminal_process_work_list();
#if 0
		if(usart_rx_data_available()){
			rx_data = recvchar();
			sendchar(rx_data);
		}
#endif
	}
}

int 
main(void)
{
	uint8_t rx_data;
	uint8_t temp;
	uint8_t boot_mode;

	EEPROM_FlushBuffer();
	EEPROM_DisableMapping();
	
	cpu_irq_disable();

	temp = PMIC_CTRL | PMIC_IVSEL_bm;
	CCPWrite( &PMIC.CTRL, temp);

	sysclk_init();
	gpio_init();
	usart_init();

	cpu_irq_enable();

	boot_mode = boot_get_mode();
	switch(boot_mode){
		case PAYLOAD_BOOT_MODE:
			proc_payload_bootmode();
			break;
		case APP_MODE:
			boot_jump_app_section();
			break;
		case CONSOLE_BOOT_MODE:
		default:
			proc_console_bootmode();
			break;
	}

#if 0
	while(1){

		if(usart_rx_buf_data_available(CONSOLE_DATA)){
			rx_data = usart_rx_buf_get_byte(CONSOLE_DATA);
			usart_putchar(CONSOLE_USART, rx_data);
		}
	}
#endif


	return 0;
}


