#ifndef __USART_H__
#define __USART_H__

//#include <stdio.h>
//#include <stdbool.h>

#define USART_BAUDRATE		38400 
#define CONSOLE_BAUDRATE	USART_BAUDRATE
#define PAYLOAD_BAUDRATE	USART_BAUDRATE

#define CONSOLE_USART	&USARTC1
#define PAYLOAD_USART	&USARTD1

#define CONSOLE_RXC_vect	USARTC1_RXC_vect
#define CONSOLE_DRE_vect	USARTC1_DRE_vect
#define PAYLOAD_RXC_vect	USARTD1_RXC_vect
#define PAYLOAD_DRE_vect	USARTD1_DRE_vect

#define USART_RX_BUFFER_SIZE 4
#define USART_TX_BUFFER_SIZE 4
#define USART_RX_BUFFER_MASK ( USART_RX_BUFFER_SIZE - 1 )
#define USART_TX_BUFFER_MASK ( USART_TX_BUFFER_SIZE - 1 )

#define MAX_BUF			2
#define CONSOLE_DATA		0
#define PAYLOAD_DATA		1
#define RX_DATA_READY	1
#define RX_DATA_STNBY	0

#define FILTER_RAW     0
#define FILTER_TERM    1

#define CR     0x0D
#define LF     0x0A

typedef struct{
	uint32_t baudrate;
	USART_CHSIZE_t charlength;
	USART_PMODE_t paritytype;
	uint8_t stopbits;
}usart_rs232_options_t;

typedef struct usart_buffer{ 
	volatile uint8_t RX[USART_RX_BUFFER_SIZE];
	volatile uint8_t TX[USART_TX_BUFFER_SIZE];
	volatile uint8_t RX_Head;
	volatile uint8_t RX_Tail;
	volatile uint8_t TX_Head;
	volatile uint8_t TX_Tail;
}usart_buffer_t;

typedef struct{
	USART_t * usart;
	USART_DREINTLVL_t dreIntLevel;
	usart_buffer_t buffer;
	uint8_t data_ready;
	uint8_t filter_type;
	void (*callback_fn)(uint8_t *);
	ipmi_ws_t *ws;
}usart_data_t;

void usart_init(void);

void usart_data_reg_empty(uint8_t buf_type);

uint8_t usart_rx_buf_data_available(uint8_t buf_type);
uint8_t usart_rx_buf_get_byte(uint8_t buf_type);
uint8_t usart_rx_complete(uint8_t buf_type);

void usart_set_payload(uint8_t buf_type);
void usart_set_console(uint8_t buf_type);
void sendchar(uint8_t ch);
uint8_t recvchar(void);
void d_sendchar(uint8_t ch);

#endif
