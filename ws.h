#ifndef __WS_H__
#define __WS_H__

/* Working Set states */
#define WS_FREE             0x1 /* ws free */
#define WS_PENDING          0x2 /* ws not in any queue, ready for use */
#define WS_ACTIVE_IN            0x3 /* ws in incoming queue, ready for ipmi processing */
#define WS_ACTIVE_IN_PENDING        0x4 /* ws in use by the ipmi layer */
#define WS_ACTIVE_MASTER_WRITE      0x5 /* ws in outgoing queue */
#define WS_ACTIVE_MASTER_WRITE_PENDING  0x6 /* outgoing request in progress */
#define WS_ACTIVE_MASTER_WRITE_SUCCESS  0x7


void ws_init( void );
ipmi_ws_t *ws_alloc( void );
void ws_set_state( ipmi_ws_t * ws, uint32_t state );
void ws_process_work_list( void );
void ws_free( ipmi_ws_t *ws );

#endif
