#ifndef __CLKSYS_H__
#define __CLKSYS_H__

#define CONFIG_SYSCLK_PSADIV	CLK_PSADIV_1_gc
#define CONFIG_SYSCLK_PSBCDIV	CLK_PSBCDIV_1_1_gc

enum sysclk_port_id {
	SYSCLK_PORT_GEN,   
	SYSCLK_PORT_A,     
	SYSCLK_PORT_B,     
	SYSCLK_PORT_C,     
	SYSCLK_PORT_D,     
	SYSCLK_PORT_E,     
	SYSCLK_PORT_F,     
};

void CCPWrite( volatile uint8_t * address, uint8_t value );
void sysclk_init(void);
void sysclk_enable_module(enum sysclk_port_id port, uint8_t id);
uint32_t sysclk_get_main_hz(void);
uint32_t sysclk_get_per4_hz(void);
uint32_t sysclk_get_per2_hz(void);
uint32_t sysclk_get_per_hz(void);
uint32_t sysclk_get_cpu_hz(void);


#endif
