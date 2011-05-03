#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ipmi.h"
#include "ws.h"
#include "usart.h"
#include "terminal.h"

void 
terminal_process_pkt( ipmi_ws_t *ws )
{
	ipmi_pkt_t *pkt = &ws->pkt;
	uint8_t *ptr = ws->rx_buf;
	uint8_t *buf = ws->rx_buf;
	int nibble[2];
	int nibble_count;
	ipmi_terminal_mode_request_t tm_hdr = {0,};
	ipmi_terminal_mode_request_t  *tm_req;
	ipmi_terminal_mode_response_t *tm_resp;
	unsigned char *req_ptr = (unsigned char *)&tm_hdr;
	int count = 0;
	int buf_len;
	uint8_t val;
#ifdef __DEBUG__
	char hex_chars[16] ;
	int i;

	for(i=0;i<10;i++)
		hex_chars[i] = '0' + i;

	for(i=10;i<16;i++)
		hex_chars[i] = 'A' + (i-10);
#endif


	/* first character must be '[' */
	if( strncmp( ( const char * )ptr++, "[", 1 ) ) {
		d_sendchar('E');
		d_sendchar('1');
		d_sendchar('\r');
		d_sendchar('\n');
		return;
	}

	nibble_count = 0;
	buf_len = ws->len_rx;

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

			memcpy( ws->pkt_in, &tm_hdr, count );
			ws->len_in = count;
			tm_req = ( ipmi_terminal_mode_request_t * )ws->pkt_in;
			tm_resp = (ipmi_terminal_mode_response_t *)ws->pkt_out;

			pkt->req = ( ipmi_cmd_req_t * )&(tm_req->command);
			pkt->resp = ( ipmi_cmd_resp_t * )&(tm_resp->completion_code);

			pkt->hdr.netfn = tm_req->netfn;
			pkt->hdr.responder_lun = tm_req->responder_lun;
			pkt->hdr.req_data_len = count - 3;
			pkt->xport_completion_function = 0;
			pkt->hdr.ws = (char *)ws;

			if( pkt->hdr.netfn % 2 ) {
				d_sendchar('E');
				d_sendchar('3');
				d_sendchar('\r');
				d_sendchar('\n');
				ws_free( ws );
				return;
			}

#ifdef __DEBUG__
			d_sendchar('i');
			d_sendchar(':');
			for(i=0;i<count;i++){
				nibble[0] = ws->pkt_in[i] >> 4;
				nibble[1] = ws->pkt_in[i] & 0x0f;

				d_sendchar(hex_chars[nibble[0]]);
				d_sendchar(hex_chars[nibble[1]]);
			}
			d_sendchar('\r');
			d_sendchar('\n');
#endif

			ipmi_process_request( pkt );

			tm_resp->netfn = tm_req->netfn + 1;
			tm_resp->responder_lun = tm_req->responder_lun;
			tm_resp->req_seq = tm_req->req_seq;
			tm_resp->bridge = tm_req->bridge; /* TODO check */
			tm_resp->command = tm_req->command;
			ws->len_out = sizeof( ipmi_terminal_mode_response_t )
				- TERM_MODE_RESP_MAX_DATA_LEN + pkt->hdr.resp_data_len;

			ws_set_state( ws, WS_ACTIVE_MASTER_WRITE );

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

void 
terminal_send_pkt( ipmi_ws_t *ws )
{
	uint8_t i;
	uint8_t nibble[2];
	char hex_chars[16] ;

	for(i=0;i<10;i++)
		hex_chars[i] = '0' + i;

	for(i=10;i<16;i++)
		hex_chars[i] = 'A' + (i-10);

	sendchar( '[' );

	for(i=0;i<3;i++){
		nibble[0] = ws->pkt_out[i] >> 4;
		nibble[1] = ws->pkt_out[i] & 0x0f;

		sendchar(hex_chars[nibble[0]]);
		sendchar(hex_chars[nibble[1]]);
	}

	for(i=3;i<ws->len_out;i++){
		sendchar( ' ' );
		nibble[0] = ws->pkt_out[i] >> 4;
		nibble[1] = ws->pkt_out[i] & 0x0f;

		sendchar(hex_chars[nibble[0]]);
		sendchar(hex_chars[nibble[1]]);
	}

	sendchar( ']' );
	sendchar( LF );

	if( ws->ipmi_completion_function )
		( ws->ipmi_completion_function )();
	else 
		ws_free( ws );

}
