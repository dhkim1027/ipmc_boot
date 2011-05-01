#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "usart.h"
#include "ipmi.h"
#include "terminal.h"

#define TERM_MAX_SIZ        40
#define CR     0x0D
#define LF     0x0A

uint8_t term_input_index = 0;
uint8_t term_input[TERM_MAX_SIZ];

void 
terminal_process_work_list(void)
{
	uint8_t rx_data;

	if(!usart_rx_data_available())
		return;

	rx_data = recvchar();

//	sendchar(rx_data);

	if(term_input_index >= TERM_MAX_SIZ)
		term_input_index = 0;

	term_input[term_input_index++] = rx_data;

	if (rx_data == LF) {
	//	sendchar(LF);

		term_input[term_input_index] = 0; // end of string
		terminal_process(term_input);
		term_input_index = 0;
	}
}

void
terminal_send(unsigned char *pkt_out,  unsigned char len_out )
{
	unsigned int i;                        
	unsigned int hi_nibble, lo_nibble;
//	char hex_chars[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
	char hex_chars[16] ;

	for(i=0;i<10;i++)
		hex_chars[i] = '0' + i;

	for(i=10;i<16;i++)
		hex_chars[i] = 'A' + (i-10);

	sendchar( '[' );

	for( i = 0; i < 3; i++ ) {
		hi_nibble = pkt_out[i] >> 4;
		lo_nibble = pkt_out[i] & 0x0f;
		sendchar( hex_chars[hi_nibble] );
		sendchar( hex_chars[lo_nibble] );
	}

	for( i = 3; i < len_out; i++ ) {
		sendchar( ' ' );
		hi_nibble = pkt_out[i] >> 4;
		lo_nibble = pkt_out[i] & 0x0f;
		sendchar( hex_chars[hi_nibble] );
		sendchar( hex_chars[lo_nibble] );
	}

	sendchar( ']' );
	sendchar( LF );
}

void 
terminal_process(uint8_t *buf)
{
	ipmi_pkt_t pkt;
	unsigned char pkt_in[32];
	unsigned char pkt_out[32];
	ipmi_terminal_mode_request_t  *tm_req;
	ipmi_terminal_mode_response_t *tm_resp;
	ipmi_terminal_mode_request_t tm_hdr = {0,};
	unsigned char *req_ptr = (unsigned char *)&tm_hdr;
	uint8_t *ptr = buf;
	int nibble[2];
	int nibble_count;
	unsigned char val;
	int count = 0;
	int buf_len;
	int len_out;

	if( strncmp( ( const char * )ptr++, "[", 1 ) ) {
		d_sendchar( 'E' );
		d_sendchar( '1' );
		d_sendchar( '\r' );
		d_sendchar( '\n' );
		return;
	}

#if 0
	if( strncmp( ( char * )ptr, sys_str, 3 ) && strncmp( ( char * )ptr, sys_str2, 3 ) )
		goto message_process;
#endif

	nibble_count = 0;
	buf_len = strlen( ( const char * )buf );

	while( ptr < buf + buf_len ) {
		if( ( ( ( *ptr >= 'A' ) && ( *ptr <= 'F' ) ) ||
			  ( ( *ptr >= 'a' ) && ( *ptr <= 'f' ) ) ||
			  ( ( *ptr >= '0' ) && ( *ptr <= '9' ) ) )&&
			  ( nibble_count < 2 ) ) {

			nibble[nibble_count] = *ptr;
			nibble_count++;
			ptr++;

			if( nibble_count == 2 ) {
				switch( nibble[0] ) {
					case 'A': case 'a': val = 10 << 4; break;
					case 'B': case 'b': val = 11 << 4; break;
					case 'C': case 'c': val = 12 << 4; break;
					case 'D': case 'd': val = 13 << 4; break;
					case 'E': case 'e': val = 14 << 4; break;
					case 'F': case 'f': val = 15 << 4; break;
					default: val = ( nibble[0] - 48 ) << 4; break;
				}

				switch( nibble[1] ) {
					case 'A': case 'a': val += 10; break;
					case 'B': case 'b': val += 11; break;
					case 'C': case 'c': val += 12; break;
					case 'D': case 'd': val += 13; break;
					case 'E': case 'e': val += 14; break;
					case 'F': case 'f': val += 15; break;
					default: val += ( nibble[1] - 48 ); break;
				}
				*req_ptr++ = val;
				count++;
				nibble_count = 0;
			}
		} else if ( *ptr == ' ' ) {
			ptr++;
		} else if ( *ptr == ']' ) {
			memcpy( pkt_in, &tm_hdr, count );
			tm_req = ( ipmi_terminal_mode_request_t * )pkt_in;
			tm_resp = (ipmi_terminal_mode_response_t *)pkt_out;
			pkt.req = ( ipmi_cmd_req_t * )&(tm_req->command);
			pkt.resp = ( ipmi_cmd_resp_t * )&(tm_resp->completion_code);

			pkt.hdr.netfn = tm_req->netfn;
			pkt.hdr.responder_lun = tm_req->responder_lun;
			pkt.hdr.req_data_len = count - 3;
			pkt.xport_completion_function = 0;

			if( pkt.hdr.netfn % 2 ) {
				d_sendchar('E');
				d_sendchar('3');
				d_sendchar('\r');
				d_sendchar('\n');
				return;
			}

			ipmi_process_request( &pkt );

			tm_resp->netfn = tm_req->netfn + 1;
			tm_resp->responder_lun = tm_req->responder_lun;
			tm_resp->req_seq = tm_req->req_seq;
			tm_resp->bridge = tm_req->bridge; /* TODO check */
			tm_resp->command = tm_req->command;
			len_out = sizeof( ipmi_terminal_mode_response_t )
				- TERM_MODE_RESP_MAX_DATA_LEN + pkt.hdr.resp_data_len;

			terminal_send(pkt_out, len_out);
			if(pkt.xport_completion_function)
				(pkt.xport_completion_function)();

			return;
		} else {
			d_sendchar( 'E' );
			d_sendchar( '2' );
			d_sendchar( '\r' );
			d_sendchar( '\n' );
			return;
		}

	}
}

