#include <stdlib.h>
#include <string.h>
#include <avr/io.h>
#include "irq.h"
#include "clksys.h"
#include "gpio.h"
#include "timer.h"
#include "usart.h"
#include "sp_driver.h"
#include "eeprom_driver.h"
#include "boot.h"
#include "terminal.h"

extern unsigned long lbolt;

void 
print_boot_mode_msg(uint8_t count)
{
	d_sendchar('\r');
	d_sendchar('H');
	d_sendchar('i');
	d_sendchar('t');
	d_sendchar(' ');
	d_sendchar('a');
	d_sendchar('n');
	d_sendchar('y');
	d_sendchar(' ');
	d_sendchar('k');
	d_sendchar('e');
	d_sendchar('y');
	d_sendchar(' ');
	d_sendchar('t');
	d_sendchar('o');
	d_sendchar(' ');
	d_sendchar('s');
	d_sendchar('t');
	d_sendchar('o');
	d_sendchar('p');
	d_sendchar(' ');
	d_sendchar('a');
	d_sendchar('u');
	d_sendchar('t');
	d_sendchar('o');
	d_sendchar('b');
	d_sendchar('o');
	d_sendchar('o');
	d_sendchar('t');
	d_sendchar(' ');
	d_sendchar(count+'0');
	d_sendchar('\r');
}

void 
print_boot_mode_menu(void)
{
	//Recorvery F/W
	d_sendchar('R');	
	d_sendchar('-');	
	d_sendchar('R');	
	d_sendchar('e');	
	d_sendchar('c');	
	d_sendchar('o');	
	d_sendchar('r');	
	d_sendchar('v');	
	d_sendchar('e');	
	d_sendchar('r');	
	d_sendchar('y');	
	d_sendchar(' ');	
	d_sendchar('F');	
	d_sendchar('/');	
	d_sendchar('W');	
	d_sendchar('\r');	
	d_sendchar('\n');	
}

void
proc_payload_bootmode(void)
{
	uint8_t rx_data;
	usart_set_payload(PAYLOAD_DATA);
	usart_set_console(CONSOLE_DATA);

}

void
proc_console_bootmode(void)
{
	uint8_t rx_data;
	usart_set_payload(CONSOLE_DATA);
	usart_set_console(CONSOLE_DATA);

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
	uint8_t temp;
	uint8_t boot_mode;
	unsigned long time;
	uint8_t count = 3;

	EEPROM_FlushBuffer();
	EEPROM_DisableMapping();
	
	cpu_irq_disable();

	temp = PMIC_CTRL | PMIC_IVSEL_bm;
	CCPWrite( &PMIC.CTRL, temp);

	sysclk_init();
//	gpio_init();
	timer_init();
	usart_init();

	cpu_irq_enable();

	boot_mode = boot_get_mode();

	if(boot_mode == PAYLOAD_BOOT_MODE)
		proc_payload_bootmode();

	time = lbolt;

#if 0
	while(count){
		if((time + HZ) < lbolt){
			print_boot_mode_msg(count);
			if(usart_rx_buf_data_available(CONSOLE_DATA))
				break;
			time = lbolt;
			count--;
		}
	}
#endif

	switch(boot_mode){
		case APP_MODE:
			boot_jump_app_section();
			break;
		case CONSOLE_BOOT_MODE:
		default:
			proc_console_bootmode();
			break;
	}

	return 0;
}


