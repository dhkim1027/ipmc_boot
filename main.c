#include <stdlib.h>
#include <string.h>
#include <avr/io.h>
#include <util/delay.h>
#include "irq.h"
#include "clksys.h"
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

uint8_t boot_mode;

void 
print_boot_mode_menu(void)
{

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


	//Help
	d_sendchar('H');	
	d_sendchar('-');	
	d_sendchar('H');	
	d_sendchar('e');	
	d_sendchar('l');	
	d_sendchar('p');	
	d_sendchar('\r');	
	d_sendchar('\n');	

	if(boot_mode == PAYLOAD_BOOT_MODE)
		return;

	//Programming Mode
	d_sendchar('P');
	d_sendchar('-');
	d_sendchar('P');
	d_sendchar('r');
	d_sendchar('o');
	d_sendchar('g');
	d_sendchar('r');
	d_sendchar('a');
	d_sendchar('m');
	d_sendchar('m');
	d_sendchar('i');
	d_sendchar('n');
	d_sendchar('g');
	d_sendchar(' ');
	d_sendchar('M');
	d_sendchar('o');
	d_sendchar('d');
	d_sendchar('e');
	d_sendchar('\r');
	d_sendchar('\n');

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
print_cli_prompt(void)
{
	d_sendchar('b');
	d_sendchar('o');
	d_sendchar('o');
	d_sendchar('t');
	if(boot_mode == APP_MODE)
		d_sendchar('A');
	else if(boot_mode == PAYLOAD_BOOT_MODE)
		d_sendchar('P');
	else
		d_sendchar('C');
	d_sendchar('>');
}

uint8_t
run_cli_boot_mode(void)
{
	uint8_t rx_data;

	if(!usart_rx_buf_data_available(CONSOLE_DATA))
		return 1;

	rx_data = usart_rx_buf_get_byte(CONSOLE_DATA);
	d_sendchar(rx_data);
	d_sendchar('\r');
	d_sendchar('\n');

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
			d_sendchar('r');
			d_sendchar('a');
			d_sendchar('s');
			d_sendchar('e');
			d_sendchar(' ');
			d_sendchar('a');
			d_sendchar('p');
			d_sendchar('p');
			d_sendchar('\r');
			d_sendchar('\n');
			boot_erase_flash(FW_TYPE_APP);
			break;
		case 'P':
			d_sendchar('P');
			d_sendchar('r');
			d_sendchar('o');
			d_sendchar('g');
			d_sendchar('r');
			d_sendchar('a');
			d_sendchar('m');
			d_sendchar('m');
			d_sendchar('i');
			d_sendchar('n');
			d_sendchar('g');
			d_sendchar(' ');
			d_sendchar('M');
			d_sendchar('o');
			d_sendchar('d');
			d_sendchar('e');
			d_sendchar('\r');
			d_sendchar('\n');
			return 2;
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
			boot_recovery_fw();
			boot_jump_app_section();
			break;
		case 'H':
		case 'h':
			print_boot_mode_menu();
			d_sendchar('\r');
			d_sendchar('\n');
			break;
		default:
			break;;
	}
	print_cli_prompt();
	return 0;
}

int 
main(void)
{
	uint8_t temp;
	uint8_t count = 5;

//	EEPROM_FlushBuffer();
	EEPROM_DisableMapping();
	
	cpu_irq_disable();

	temp = PMIC_CTRL | PMIC_IVSEL_bm;
	CCPWrite( &PMIC.CTRL, temp);

	sysclk_init();
	ws_init();
	usart_init();
	usart_set_console(CONSOLE_DATA);

	cpu_irq_enable();

	d_sendchar('\r');
	d_sendchar('\n');
	d_sendchar('\r');
	d_sendchar('\n');
	d_sendchar('\r');
	d_sendchar('\n');

	boot_mode = boot_get_mode();

	if(boot_mode == PAYLOAD_BOOT_MODE){
	//	boot_set_mode(CONSOLE_BOOT_MODE);
		boot_init_write_flash();
		usart_set_payload(PAYLOAD_DATA);
		print_cli_prompt();
		while(1){
			run_cli_boot_mode();
			ws_process_work_list();
		}
	}

	if(boot_mode == APP_MODE){
		while(count){
			print_boot_mode_msg(count);
			if(usart_rx_buf_data_available(CONSOLE_DATA)){
				d_sendchar('\r');
				d_sendchar('\n');
				break;
			}
			_delay_ms(1000);
			count--;
		}
	}else{
		count = 1;
	}

	if(count){
		print_cli_prompt();
		while(run_cli_boot_mode() != 2);
	}else
		boot_jump_app_section();

	usart_set_payload(CONSOLE_DATA);
	while(1){
		ws_process_work_list();
	}

	return 0;
}


