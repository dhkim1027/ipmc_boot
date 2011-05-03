#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ipmi.h"
#include "ws.h"
#include "terminal.h"

ipmi_ws_t   ws_array[WS_ARRAY_SIZE];

void
ws_init( void )
{
	uint32_t i;

	for ( i = 0; i < WS_ARRAY_SIZE; i++ )
	{
		ws_array[i].ws_state    = WS_FREE;
		ws_array[i].len_in      = 0;
		ws_array[i].len_out     = 0;
		ws_array[i].len_rx 		= 0;
		ws_array[i].ipmi_completion_function = 0;
		memset(ws_array[i].rx_buf, 0, WS_BUF_LEN);
		memset(ws_array[i].pkt_in, 0, WS_BUF_LEN);
		memset(ws_array[i].pkt_out, 0, WS_BUF_LEN);
	}
}

ipmi_ws_t *
ws_alloc( void )
{
	ipmi_ws_t *ws = 0;
	ipmi_ws_t *ptr = ws_array;
	uint32_t i;

	for ( i = 0; i < WS_ARRAY_SIZE; i++ )
	{
		ptr = &ws_array[i];
		if( ptr->ws_state == WS_FREE ) {
			ptr->ws_state = WS_PENDING;
			ws = ptr;
			break;
		}
	}

	return ws;
}

void
ws_free( ipmi_ws_t *ws )
{
	uint32_t len, i;
	char *ptr = (char *)ws;

	len = sizeof( ipmi_ws_t );
	for( i = 0 ; i < len ; i++ ) {
		*ptr++ = 0;
	}
	ws->ws_state = WS_FREE;
}

ipmi_ws_t *
ws_get_elem( uint32_t state )
{
	ipmi_ws_t *ws = 0;
	ipmi_ws_t *ptr = ws_array;
	uint32_t i;

	for ( i = 0; i < WS_ARRAY_SIZE; i++ )
	{
		ptr = &ws_array[i];
		if( ptr->ws_state == state ) {
			ws = ptr;
		}
	}

	return ws;
}

void
ws_set_state( ipmi_ws_t * ws, uint32_t state )
{
	ws->ws_state = state;
}

void 
ws_process_work_list( void )
{
	ipmi_ws_t *ws;

	ws = ws_get_elem( WS_ACTIVE_IN );
	if( ws ) {
		ws_set_state( ws, WS_ACTIVE_IN_PENDING );
		terminal_process_pkt( ws );
	}

	ws = ws_get_elem( WS_ACTIVE_MASTER_WRITE );
	if( ws ) {
		ws_set_state( ws, WS_ACTIVE_MASTER_WRITE_PENDING );
		terminal_send_pkt( ws );
	}
}
