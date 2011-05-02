#include <stdlib.h>
#include <string.h>
#include <avr/io.h>
#include <util/delay.h>
#include "irq.h"
#include "clksys.h"
#include "gpio.h"
#include "timer.h"
#include "ipmi.h"
#include "ws.h"
#include "usart.h"
#include "sp_driver.h"
#include "eeprom_driver.h"
#include "boot.h"
#include "terminal.h"

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

	//run application
	d_sendchar('A');	
	d_sendchar('-');	
	d_sendchar('R');	
	d_sendchar('u');	
	d_sendchar('n');	
	d_sendchar(' ');	
	d_sendchar('A');	
	d_sendchar('p');	
	d_sendchar('p');	
	d_sendchar('l');	
	d_sendchar('i');	
	d_sendchar('c');	
	d_sendchar('a');	
	d_sendchar('t');	
	d_sendchar('i');	
	d_sendchar('o');	
	d_sendchar('n');	
	d_sendchar('\r');	
	d_sendchar('\n');	

	// Exit
	d_sendchar('E');	
	d_sendchar('-');	
	d_sendchar('E');	
	d_sendchar('x');	
	d_sendchar('i');	
	d_sendchar('t');	
	d_sendchar('\r');	
	d_sendchar('\n');	

	//Help
	d_sendchar('H');	
	d_sendchar('-');	
	d_sendchar('H');	
	d_sendchar('e');	
	d_sendchar('l');	
	d_sendchar('p');	
	d_sendchar('\r');	
	d_sendchar('\n');	
}

void 
run_cli_boot_mode(void)
{
	uint8_t rx_data;
	while(1){
		if(!usart_rx_buf_data_available(CONSOLE_DATA))
			continue;

		rx_data = usart_rx_buf_get_byte(CONSOLE_DATA);
		d_sendchar(rx_data);

		switch(rx_data){
			case 'A':
				d_sendchar('g');
				d_sendchar('o');
				d_sendchar(' ');
				d_sendchar('a');
				d_sendchar('p');
				d_sendchar('p');
				d_sendchar('\r');
				d_sendchar('\n');
				boot_jump_app_section();
				break;
			case 'E':
				d_sendchar('e');
				d_sendchar('x');
				d_sendchar('i ');
				d_sendchar('t');
				d_sendchar('\r');
				d_sendchar('\n');
				return;
			case 'R':
				d_sendchar('r');	
				d_sendchar('e');	
				d_sendchar('c');	
				d_sendchar('o');	
				d_sendchar('r');	
				d_sendchar('v');	
				d_sendchar('e');	
				d_sendchar('r');	
				d_sendchar('y');	
				d_sendchar('\r');
				d_sendchar('\n');
				break;
			case 'H':
			default:
				print_boot_mode_menu();
				break;
		}
	}
}

void
proc_console_boot_mode(void)
{
	usart_set_payload(CONSOLE_DATA);

	while(1){
		ws_process_work_list();
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
	ws_init();
//	gpio_init();
	timer_init();
	usart_init();
	usart_set_console(CONSOLE_DATA);

	cpu_irq_enable();

	boot_mode = boot_get_mode();

	if(boot_mode == PAYLOAD_BOOT_MODE){
		usart_set_payload(PAYLOAD_DATA);
		while(1){
			ws_process_work_list();
		}
	}

	while(count){
		print_boot_mode_msg(count);
		if(usart_rx_buf_data_available(CONSOLE_DATA))
			break;
		_delay_ms(1000);
		count--;
	}

	if(count)
		run_cli_boot_mode();

	switch(boot_mode){
		case APP_MODE:
			boot_jump_app_section();
			break;
		case CONSOLE_BOOT_MODE:
		default:
			proc_console_boot_mode();
			break;
	}

	return 0;
}


