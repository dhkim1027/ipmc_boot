#ifndef __GPIO_H__
#define __GPIO_H__

// PORTA
#define GPIO_TEMP_vect	PORTA_INT0_vect

#define CN58_THER_INT_bp	0
#define CN58_AMB_INT_bp		1
#define NP4_AMB_INT_bp		2
#define IPS_AMB_INT_bp		3
#define DPI_AMB_INT_bp		4

#define CN58_THER_INT_DIR	(0 << CN58_THER_INT_bp)
#define CN58_AMB_INT_DIR	(0 << CN58_AMB_INT_bp)
#define NP4_AMB_INT_DIR		(0 << NP4_AMB_INT_bp)
#define IPS_AMB_INT_DIR		(0 << IPS_AMB_INT_bp)
#define DPI_AMB_INT_DIR		(0 << DPI_AMB_INT_bp)

#define CN58_THER_INT	(1 << CN58_THER_INT_bp)
#define CN58_AMB_INT	(1 << CN58_AMB_INT_bp)
#define NP4_AMB_INT		(1 << NP4_AMB_INT_bp)
#define IPS_AMB_INT		(1 << IPS_AMB_INT_bp)
#define DPI_AMB_INT		(1 << DPI_AMB_INT_bp)

// PORTB 
#define IPMC_IN0_bp		0
#define IPMC_IN1_bp		1
#define IPMC_OUT0_bp	2
#define IPMC_OUT1_bp	3
#define BD_POWER_OFF_bp	3


#define IPMC_IN0_DIR	(0 << IPMC_IN0_bp)
#define IPMC_IN1_DIR	(0 << IPMC_IN1_bp)
#define IPMC_OUT0_DIR	(1 << IPMC_OUT0_bp)
#define IPMC_OUT1_DIR	(1 << IPMC_OUT1_bp)
#define BD_POWER_OFF_DIR	(0 << BD_POWER_OFF_bp)
#define BD_POWER_OFF		(1 << BD_POWER_OFF_bp)

#define IPMC_OUT0	(1 << IPMC_OUT0_bp)

// PORTE
#define IPMC_READY_A_bp	2
#define IPMC_FAULT_A_bp	3

#define IPMC_READY_A_DIR	(0 << IPMC_READY_A_bp)
#define IPMC_FAULT_A_DIR	(0 << IPMC_FAULT_A_bp)

#define IPMC_READY_A		(1 << IPMC_READY_A_bp)
#define IPMC_FAULT_A		(1 << IPMC_FAULT_A_bp)

// PORTF
#define IPMC_READY_B_bp	2
#define IPMC_FAULT_B_bp	3

#define IPMC_READY_B_DIR	(0 << IPMC_READY_B_bp)
#define IPMC_FAULT_B_DIR	(0 << IPMC_FAULT_B_bp)

#define IPMC_READY_B		(1 << IPMC_READY_B_bp)
#define IPMC_FAULT_B		(1 << IPMC_FAULT_B_bp)

// PORTH
#define HA_bp			0xFF
#define HA_DIR			0 

// PORTJ
#define N48V_ALARM_bp	0
#define ATC250_CNTL1_bp	1
#define ATC250_CNTL2_bp	2
#define ATC250_CNTL3_bp	3
#define GPIO_PW_12V_D_bp	4
#define GPIO_PW_12V_CP_bp	5
#define GPIO_PW_2_bp	6
#define GPIO_PW_3_bp	7
#define GPIO_LED_2_FAIL_bp	6
#define GPIO_LED_3_STBY_bp	7

#define N48V_ALARM_DIR		(0 << N48V_ALARM_bp)
#define ATC250_CNTL1_DIR	(1 << ATC250_CNTL1_bp)
#define ATC250_CNTL2_DIR	(1 << ATC250_CNTL2_bp)
#define ATC250_CNTL3_DIR	(1 << ATC250_CNTL3_bp)
#define GPIO_PW_12V_D_DIR	(1 << GPIO_PW_12V_D_bp)
#define GPIO_PW_12V_CP_DIR	(1 << GPIO_PW_12V_CP_bp)
#define GPIO_PW_2_DIR		(1 << GPIO_PW_2_bp)
#define GPIO_PW_3_DIR		(1 << GPIO_PW_3_bp)
#define GPIO_LED_2_FAIL_DIR	(1 << GPIO_LED_2_FAIL_bp)
#define GPIO_LED_3_STBY_DIR (1 << GPIO_LED_3_STBY_bp)

#define GPIO_PW_12V_D		(1 << GPIO_PW_12V_D_bp)
#define GPIO_PW_12V_CP		(1 << GPIO_PW_12V_CP_bp)
#define GPIO_PW_2			(1 << GPIO_PW_2_bp)
#define GPIO_PW_3			(1 << GPIO_PW_3_bp)
#define GPIO_LED_2_FAIL		(1 << GPIO_LED_2_FAIL_bp)
#define GPIO_LED_3_STBY		(1 << GPIO_LED_3_STBY_bp)

//PORTK
#define	GPIO_LED_1_bp		0
#define GPIO_LED_2_bp		1
#define GPIO_LED_3_bp		2
#define GPIO_LED_BLUE_bp	3
#define HANDLE_SW_bp		4
#define IPMC_RESTART_bp		5
#define IPMC_BD_RESET_bp	6

#define GPIO_LED_1_DIR	(1 << GPIO_LED_1_bp)
#define GPIO_LED_2_DIR	(1 << GPIO_LED_2_bp)
#define GPIO_LED_3_DIR	(1 << GPIO_LED_3_bp)
#define GPIO_LED_BLUE_DIR	(1 << GPIO_LED_BLUE_bp)
#define HANDLE_SW_DIR	(0 << HANDLE_SW_bp)
#define IPMC_RESTART_DIR	(1 << IPMC_RESTART_bp)
#define IPMC_BD_RESET_DIR	(1 << IPMC_BD_RESET_bp)

#define GPIO_FRU_LED_1		(1 << GPIO_LED_1_bp)
#define GPIO_FRU_LED_2		(1 << GPIO_LED_2_bp)
#define GPIO_FRU_LED_3		(1 << GPIO_LED_3_bp)
#define GPIO_FRU_LED_BLUE	(1 << GPIO_LED_BLUE_bp)
#define HANDLE_SW		(1 << HANDLE_SW_bp)
#define IPMC_RESTART	(1 << IPMC_RESTART_bp)
#define IPMC_BD_RESET	(1 << IPMC_BD_RESET_bp)

#define GPIO_HEALTH_LED_GREEN	1
#define GPIO_HEALTH_LED_AMBER	0
#define GPIO_ACT_LED	1
#define GPIO_STBY_LED	0

#define GPIO_LED_ALL	(GPIO_FRU_LED_1 | GPIO_FRU_LED_2 | GPIO_FRU_LED_3 | GPIO_FRU_LED_BLUE)
#define GPIO_PW_ALL		GPIO_PW_12V_D

#define GPIO_ACTIVITY_LED   GPIO_FRU_LED_1
#define GPIO_IDENTIFY_LED   GPIO_FRU_LED_1

#define GPIO_IPMB_READY		1
#define GPIO_IPMB_IDLE		0

#define GPIO_IPMB_FAULT		0
#define GPIO_IPMB_NORMAL	1

#define GPIO_STATE_POWER_ON		1
#define GPIO_STATE_POWER_OFF	0

void gpio_init(void);

#endif
