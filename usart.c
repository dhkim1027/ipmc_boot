#include <avr/io.h>
#include "irq.h"
#include "clksys.h"
#include "usart.h"

#if 0
static FILE mystdout = FDEV_SETUP_STREAM(debug_putchar, NULL,
		               _FDEV_SETUP_WRITE);
#endif

usart_data_t USART_data[MAX_BUF];

usart_data_t * payload_data = 0;
usart_data_t * console_data = 0;

static inline void usart_set_mode(USART_t *_usart, USART_CMODE_t _usartmode)
{
	(_usart)->CTRLC = ((_usart)->CTRLC & (~USART_CMODE_gm)) | _usartmode;
}

static inline void usart_format_set(USART_t *_usart, USART_CHSIZE_t _charSize, USART_PMODE_t _parityMode, uint8_t _twoStopBits)
{
	(_usart)->CTRLC = (uint8_t) _charSize | _parityMode | (_twoStopBits ? USART_SBMODE_bm : 0);
}

static inline uint8_t usart_data_register_is_empty(USART_t *_usart)
{
	return (_usart)->STATUS & USART_DREIF_bm;
}

static inline void usart_tx_enable(USART_t *_usart)
{
	(_usart)->CTRLB |= USART_TXEN_bm;
}

static inline void usart_rx_enable(USART_t *_usart)
{
	(_usart)->CTRLB |= USART_RXEN_bm;
}

uint8_t usart_rx_is_complete(USART_t *_usart)
{
	return (_usart)->STATUS & USART_RXCIF_bm;
}

uint8_t usart_flush_data(USART_t *usart)
{
	return ((usart)->DATA);
}

void usart_set_baudrate(USART_t *usart, uint32_t baud, uint32_t cpu_hz)
{
	uint16_t bsel_value = (uint16_t) ((((((cpu_hz)<<1)/(baud*16))+1)>>1)-1);
	// No double transmission speed^M
	(usart)->CTRLB &= ~USART_CLK2X_bm;
	// BSALE not used, BSCALE = 0
	(usart)->BAUDCTRLB = (uint8_t) ((~USART_BSCALE_gm)&(bsel_value>>8));
	(usart)->BAUDCTRLA = (uint8_t) (bsel_value);
}

void usart_init_rs232(USART_t *usart, const usart_rs232_options_t *opt)
{
	usart_set_mode(usart,USART_CMODE_ASYNCHRONOUS_gc);
	usart_format_set(usart, opt->charlength, opt->paritytype, opt->stopbits);
	usart_set_baudrate(usart, opt->baudrate, F_CPU);
	usart_tx_enable(usart);
	usart_rx_enable(usart);
}

void usart_init_interrupt(uint8_t buf_type, USART_t * usart, USART_DREINTLVL_t dreIntLevel)
{
	usart_data_t * usart_data = &USART_data[buf_type];
	usart_data->usart = usart;
	usart_data->dreIntLevel = dreIntLevel;

	usart_data->buffer.RX_Tail = 0;
	usart_data->buffer.RX_Head = 0;
	usart_data->buffer.TX_Tail = 0;
	usart_data->buffer.TX_Head = 0;

	usart_data->filter_type = 0;
	usart_data->callback_fn = 0;
	usart_data->data_ready = RX_DATA_STNBY;
}

void usart_init_callback(uint8_t buf_type, uint8_t filter_type, void (*func)(uint8_t *))
{
	usart_data_t * usart_data = &USART_data[buf_type];

	usart_data->filter_type = filter_type;
	usart_data->callback_fn = func;
	usart_data->data_ready = RX_DATA_STNBY;

}

void usart_rx_int_level_set(USART_t * _usart, uint8_t _rxdIntLevel)
{
	(_usart)->CTRLA = ((_usart)->CTRLA & ~USART_RXCINTLVL_gm) | _rxdIntLevel;
}


void usart_init(void)
{
	static usart_rs232_options_t usart_opt;
	static usart_rs232_options_t payload_usart_opt;
	uint8_t tmp;

	usart_opt.baudrate 		= USART_BAUDRATE;
	usart_opt.charlength 	= USART_CHSIZE_8BIT_gc;
	usart_opt.paritytype	= USART_PMODE_DISABLED_gc;
	usart_opt.stopbits 		= 0;

	payload_usart_opt.baudrate 		= USART_BAUDRATE;
	payload_usart_opt.charlength 	= USART_CHSIZE_8BIT_gc;
	payload_usart_opt.paritytype	= USART_PMODE_DISABLED_gc;
	payload_usart_opt.stopbits 		= 0;

	PORTC.DIRCLR |= PIN6_bm;
	PORTC.DIRSET |= PIN7_bm;

	PORTD.DIRCLR |= PIN6_bm;
	PORTD.DIRSET |= PIN7_bm;

	/* initialize buffer */
	usart_init_interrupt(CONSOLE_DATA, CONSOLE_USART, USART_DREINTLVL_LO_gc);
	usart_init_interrupt(PAYLOAD_DATA, PAYLOAD_USART, USART_DREINTLVL_LO_gc);

	usart_init_rs232(CONSOLE_USART, &usart_opt);
	usart_init_rs232(PAYLOAD_USART, &payload_usart_opt);

	usart_rx_int_level_set(CONSOLE_USART, USART_RXCINTLVL_MED_gc);
	usart_rx_int_level_set(PAYLOAD_USART, USART_RXCINTLVL_MED_gc);

	// dummy data
	tmp = usart_flush_data(CONSOLE_USART);
	tmp = usart_flush_data(PAYLOAD_USART);

//	stdout = &mystdout;

	PMIC.CTRL |= PMIC_MEDLVLEX_bm;

}

uint8_t usart_getchar(USART_t *usart)
{
	while(usart_rx_is_complete(usart)==0);
	return ((uint16_t)(usart)->DATA);
}

uint8_t usart_putchar(USART_t *usart, uint8_t c)
{
	while(usart_data_register_is_empty(usart)==0);
	(usart)->DATA = c;
	return 1;
}

void
usart_process_terminal(uint8_t data)
{
	
}

bool usart_rx_complete(uint8_t buf_type)
{
	usart_data_t * usart_data = &USART_data[buf_type];
	usart_buffer_t * bufPtr = &usart_data->buffer;
	bool ans;

	/* Advance buffer head. */
	uint8_t tempRX_Head = (bufPtr->RX_Head + 1) & USART_RX_BUFFER_MASK;

	/* Check for overflow. */
	uint8_t tempRX_Tail = bufPtr->RX_Tail;
	uint8_t data = usart_data->usart->DATA;

	if(usart_data == payload_data)
		usart_process_terminal(data);

	if (tempRX_Head == tempRX_Tail) {
		ans = false;
	}else{
		ans = true;
		usart_data->buffer.RX[usart_data->buffer.RX_Head] = data;
		usart_data->buffer.RX_Head = tempRX_Head;
#if 0
		if(usart_data->filter_type == FILTER_RAW){
			usart_data->data_ready = RX_DATA_READY;	
		}else{	// FILTER_TERM
			usart_tx_buf_put_byte(buf_type, data);
			if(data == CR){
				usart_tx_buf_put_byte(buf_type, LF);

				tempRX_Head = (bufPtr->RX_Head + 1) & USART_RX_BUFFER_MASK;
				usart_data->buffer.RX[usart_data->buffer.RX_Head] = 0;
				usart_data->buffer.RX_Head = tempRX_Head;
					
				usart_data->data_ready = RX_DATA_READY;	
			}
		}
#endif
	}
	return ans;
}

uint8_t usart_rx_buf_get_byte(uint8_t buf_type)
{
	usart_data_t * usart_data = &USART_data[buf_type];
	usart_buffer_t * bufPtr = &usart_data->buffer;
	uint8_t ans;

	ans = (bufPtr->RX[bufPtr->RX_Tail]);

	bufPtr->RX_Tail = (bufPtr->RX_Tail + 1) & USART_RX_BUFFER_MASK;
	return ans;
}

bool usart_rx_buf_data_available(uint8_t buf_type)
{
	usart_data_t * usart_data = &USART_data[buf_type];
	uint8_t tempHead = usart_data->buffer.RX_Head;
	uint8_t tempTail = usart_data->buffer.RX_Tail;

	return (tempHead != tempTail);
}

bool usart_tx_buf_free_space(usart_data_t * usart_data)
{
	uint8_t tempHead = (usart_data->buffer.TX_Head + 1) & USART_TX_BUFFER_MASK;
	uint8_t tempTail = usart_data->buffer.TX_Tail;

	return (tempHead != tempTail);
}


bool usart_tx_buf_put_byte(uint8_t buf_type, uint8_t data)
{
	usart_data_t * usart_data = &USART_data[buf_type];
	uint8_t tempCTRLA;
	uint8_t tempTX_Head;
	bool TXBuffer_FreeSpace;
	usart_buffer_t * TXbufPtr;

	TXbufPtr = &usart_data->buffer;
	TXBuffer_FreeSpace = usart_tx_buf_free_space(usart_data);

	if(TXBuffer_FreeSpace)
	{
		tempTX_Head = TXbufPtr->TX_Head;
		TXbufPtr->TX[tempTX_Head]= data;
		/* Advance buffer head. */
		TXbufPtr->TX_Head = (tempTX_Head + 1) & USART_TX_BUFFER_MASK;

		/* Enable DRE interrupt. */
		tempCTRLA = usart_data->usart->CTRLA;
		tempCTRLA = (tempCTRLA & ~USART_DREINTLVL_gm) | usart_data->dreIntLevel;
		usart_data->usart->CTRLA = tempCTRLA;
	}
	return TXBuffer_FreeSpace;
}

void usart_data_reg_empty(uint8_t buf_type)
{
	usart_data_t * usart_data = &USART_data[buf_type];
	usart_buffer_t * bufPtr;
	bufPtr = &usart_data->buffer;

	uint8_t tempTX_Tail = usart_data->buffer.TX_Tail;
	if (bufPtr->TX_Head == tempTX_Tail){
		/* Disable DRE interrupts. */
		uint8_t tempCTRLA = usart_data->usart->CTRLA;
		tempCTRLA = (tempCTRLA & ~USART_DREINTLVL_gm) | USART_DREINTLVL_OFF_gc;
		usart_data->usart->CTRLA = tempCTRLA;
	}else{
		/* Start transmitting. */
		uint8_t data = bufPtr->TX[usart_data->buffer.TX_Tail];
		usart_data->usart->DATA = data;

		/* Advance buffer tail. */
		bufPtr->TX_Tail = (bufPtr->TX_Tail + 1) & USART_TX_BUFFER_MASK;
	}
}

// USARTC1 RXC interrupt handler
ISR(CONSOLE_RXC_vect)
{
	usart_rx_complete(CONSOLE_DATA);
}

// USARTC1 DRE interrupt handler
ISR(CONSOLE_DRE_vect)
{
	usart_data_reg_empty(CONSOLE_DATA);
}

// USARTD1 RXC interrupt handler
ISR(PAYLOAD_RXC_vect)
{
	usart_rx_complete(PAYLOAD_DATA);
}

// USARTD1 DRE interrupt handler
ISR(PAYLOAD_DRE_vect)
{
	usart_data_reg_empty(PAYLOAD_DATA);
}


void 
usart_set_payload(uint8_t buf_type)
{
	payload_data = &USART_data[buf_type];
}

void 
usart_set_console(uint8_t buf_type)
{
	console_data = &USART_data[buf_type];
}

void 
sendchar(uint8_t ch)
{
	if(payload_data == 0)
		return;

	while(usart_data_register_is_empty(payload_data->usart)==0);
	(payload_data->usart)->DATA = ch;
}

bool 
usart_rx_data_available(void)
{
	uint8_t tempHead ;
	uint8_t tempTail ;

	if(payload_data == 0)
		return 0;

	tempHead = payload_data->buffer.RX_Head;
	tempTail = payload_data->buffer.RX_Tail;

	return (tempHead != tempTail);

}

uint8_t 
recvchar(void)
{
	uint8_t ans;
	usart_buffer_t * bufPtr ;

	if(payload_data == 0)
		return -1;

	bufPtr = &payload_data->buffer;

	ans = (bufPtr->RX[bufPtr->RX_Tail]);

	bufPtr->RX_Tail = (bufPtr->RX_Tail + 1) & USART_RX_BUFFER_MASK;
	return ans;
}

void 
d_sendchar(uint8_t ch)
{
	if(console_data == 0)
		return;

	while(usart_data_register_is_empty(console_data->usart)==0);
	(console_data->usart)->DATA = ch;

}
