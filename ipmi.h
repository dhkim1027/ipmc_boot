#ifndef __IPMI_H__
#define __IPMI_H__

#define TERM_MODE_REQ_MAX_DATA_LEN  25
#define TERM_MODE_RESP_MAX_DATA_LEN 25

#define CR     0x0D
#define LF     0x0A

/* generic terminal mode header */
typedef struct ipmi_terminal_mode_hdr {
	uint8_t lun:2,
			netfn:6;
	uint8_t bridge:2,
			seq:6;
	uint8_t command;
} ipmi_terminal_mode_hdr_t;

typedef struct ipmi_terminal_mode_request {
	uint8_t responder_lun:2,
			netfn:6;
	uint8_t bridge:2,
			req_seq:6;
	uint8_t command;
	uint8_t data[TERM_MODE_REQ_MAX_DATA_LEN];
} ipmi_terminal_mode_request_t;

typedef struct ipmi_terminal_mode_response {
	uint8_t   responder_lun:2,
			  netfn:6;
	uint8_t   bridge:2,
			  req_seq:6;
	uint8_t   command;
	uint8_t   completion_code;
	uint8_t   data[TERM_MODE_RESP_MAX_DATA_LEN];
} ipmi_terminal_mode_response_t;

typedef struct pkt_hdr {
	uint8_t   lun;
	uint32_t    req_data_len;
	uint32_t    resp_data_len;
	uint32_t    cmd_len;
	uint8_t       netfn;
	uint8_t       responder_lun;
	char        *ws;
} pkt_hdr_t;

typedef struct ipmi_cmd_req {
	uint8_t command;
	uint8_t data;
} ipmi_cmd_req_t;

typedef struct ipmi_cmd_resp {
	uint8_t completion_code;
	uint8_t data;
} ipmi_cmd_resp_t;

typedef struct ipmi_pkt {
	pkt_hdr_t       hdr;
	ipmi_cmd_req_t  *req;
	ipmi_cmd_resp_t *resp;
	void (*xport_completion_function)(void);
} ipmi_pkt_t;

#define WS_ARRAY_SIZE   8
#define WS_BUF_LEN 		40

typedef struct ipmi_ws {
	uint32_t ws_state;
	ipmi_pkt_t pkt;
	uint8_t len_in;        /* lenght of incoming pkt */
	uint8_t len_out;       /* length of outgoing pkt */
	uint8_t len_rx;
	uint8_t rx_buf[WS_BUF_LEN];
	uint8_t pkt_in[WS_BUF_LEN];
	uint8_t pkt_out[WS_BUF_LEN];
	void (*ipmi_completion_function)( void );
} ipmi_ws_t;

#define CC_NORMAL       0x00    /* Command Completed Normally. */
#define CC_INVALID_CMD      0xC1    /* Invalid Command. Used to */

#define NETFN_GROUP_EXTENSION_REQ   0x2C
#define NETFN_GROUP_EXTENSION_RESP  0x2D

#define PICMG_ID        0
/*----------------------------------------------------------------------*/
/*          PICMG Generic Response              */
/*----------------------------------------------------------------------*/
typedef struct picmg_cmd_resp {
	uint8_t completion_code;    /* Completion Code. */
	uint8_t picmg_id;       /* PICMG Identifier. Indicates that
							   this is a PICMG®-defined group
							   extension command. A value of
							   00h shall be used. */
} picmg_cmd_resp_t;

/*----------------------------------------------------------------------*/
/*			Init Upgrade Action											*/
/*----------------------------------------------------------------------*/
#define HPM_INITIATE_UPGRADE_ACTION	0x31

typedef enum eHpmfwupgUpgradeAction
{
	HPM_UPGRADE_ACTION_BACKUP = 0,
	HPM_UPGRADE_ACTION_PREPARE,
	HPM_UPGRADE_ACTION_UPGRADE,
	HPM_UPGRADE_ACTION_COMPARE,
	HPM_UPGRADE_ACTION_INVALID = 0xff
}  tHpmfwupgUpgradeAction;

typedef struct initiate_upgrade_action_req{
	uint8_t command;
	uint8_t picmg_id;
	uint8_t components_mask;
	uint8_t upgrade_action;
} initiate_upgrade_action_req_t;

typedef struct initiate_upgrade_action_resp{
	uint8_t completion_code;    /* Completion Code */
	uint8_t picmg_id;
} initiate_upgrade_action_resp_t;

/*----------------------------------------------------------------------*/
/*          Upload Firmware Block                                       */
/*----------------------------------------------------------------------*/
#define HPM_UPLOAD_FIRMWARE_BLOCK   0x32
#define MAX_FW_DATA_LEN 20
typedef struct upload_firmware_block_req{
	uint8_t command;
	uint8_t picmg_id;
	uint8_t block_number;
	uint8_t data[MAX_FW_DATA_LEN];
} upload_firmware_block_req_t;

typedef struct upload_firmware_block_resp{
	uint8_t completion_code;
	uint8_t picmg_id;
	uint8_t section_offset[4];
	uint8_t section_length[4];
} upload_firmware_block_resp_t;

/*----------------------------------------------------------------------*/
/*          Finish Firmware Upload                                      */
/*----------------------------------------------------------------------*/
#define HPM_FINISH_FIRMWARE_UPLOAD      0x33

typedef struct finish_firmware_upload_req{
	uint8_t command;
	uint8_t picmg_id;
	uint8_t component_id;
	uint8_t image_length[4];
} finish_firmware_upload_req_t;

typedef struct finish_firmware_upload_resp{
	uint8_t completion_code;
	uint8_t picmg_id;
} finish_firmware_upload_resp_t;

/*----------------------------------------------------------------------*/
/*          Activate Firmware 											*/
/*----------------------------------------------------------------------*/
#define HPM_ACTIVATE_FIRMWARE           0x35

typedef struct activate_firmware_req {
	uint8_t command;
	uint8_t picmg_id;
	uint8_t rollback_override;
} activate_firmware_req_t ;

typedef struct activate_firmware_resp{
	uint8_t completion_code;
	uint8_t picmg_id;
} activate_firmware_resp_t ;

/*----------------------------------------------------------------------*/
/*          Activate Firmware 											*/
/*----------------------------------------------------------------------*/
#define HPM_QUERY_SELFTEST_RESULT       0x36



void ipmi_process_request( ipmi_pkt_t *pkt );
void ipmi_process_pkt( ipmi_ws_t *ws );

#endif
