#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "boot.h"
#include "ipmi.h"
#include "ws.h"
#include "usart.h"

uint8_t block_number = 0;

void
picmg_hpm_initiate_upgrade_action( ipmi_pkt_t *pkt )
{
	initiate_upgrade_action_req_t *req = ( initiate_upgrade_action_req_t *)pkt->req;
	initiate_upgrade_action_resp_t *resp = ( initiate_upgrade_action_resp_t *)pkt->resp;

	if(req->upgrade_action == HPM_UPGRADE_ACTION_UPGRADE){
		boot_init_write_flash(APP_FW);
		block_number = 0;
	}

	resp->completion_code = CC_NORMAL;
	resp->picmg_id = PICMG_ID;

	pkt->hdr.resp_data_len = 1;
}

void
picmg_hpm_upload_firmware_block( ipmi_pkt_t *pkt )
{
	upload_firmware_block_req_t *req = ( upload_firmware_block_req_t *)pkt->req;
	upload_firmware_block_resp_t *resp = ( upload_firmware_block_resp_t *)pkt->resp;
	uint8_t data_size = pkt->hdr.req_data_len - 2;


	if(block_number != req->block_number){
		resp->completion_code = 0x82;
		pkt->hdr.resp_data_len = 1;

		sendchar('E');
		sendchar(block_number);
		sendchar('\r');
		sendchar('\n');
		return;
	}

	boot_write_flash(req->data, data_size);
	block_number++;

	resp->completion_code = CC_NORMAL;
	resp->picmg_id = PICMG_ID;
	resp->section_offset[0] = 0;
	resp->section_offset[1] = 0;
	resp->section_offset[2] = 0;
	resp->section_offset[3] = 0;
	resp->section_length[0] = 0;
	resp->section_length[1] = 0;
	resp->section_length[2] = 0;
	resp->section_length[3] = 0;

	pkt->hdr.resp_data_len = 9;
}

void
picmg_hpm_finish_firmware_upload( ipmi_pkt_t *pkt )
{
	ipmi_ws_t *ws = (ipmi_ws_t *)pkt->hdr.ws;
	finish_firmware_upload_req_t *req = ( finish_firmware_upload_req_t *)pkt->req;
	finish_firmware_upload_resp_t *resp = ( finish_firmware_upload_resp_t *)pkt->resp;
	uint32_t image_length = 0;

	image_length |= req->image_length[3];
	image_length <<= 8;
	image_length |= req->image_length[2];
	image_length <<= 8;
	image_length |= req->image_length[1];
	image_length <<= 8;
	image_length |= req->image_length[0];

	boot_finish_write_flash(image_length);

	resp->completion_code = CC_NORMAL;
	resp->picmg_id = PICMG_ID;

	pkt->hdr.resp_data_len = 1;
	ws->ipmi_completion_function = boot_jump_app_section;
	//pkt->xport_completion_function = boot_jump_app_section;
}

void
picmg_hpm_activate_firmware( ipmi_pkt_t *pkt )
{
	ipmi_ws_t *ws = (ipmi_ws_t *)pkt->hdr.ws;
	activate_firmware_req_t *req = ( activate_firmware_req_t *)pkt->req;
	activate_firmware_resp_t *resp = ( activate_firmware_resp_t *)pkt->resp;

	resp->completion_code = CC_NORMAL;
	resp->picmg_id = PICMG_ID;

	pkt->hdr.resp_data_len = 1;
	ws->ipmi_completion_function = boot_reset_ipmc;
	//pkt->xport_completion_function = boot_reset_ipmc;
}

void
picmg_process_command( ipmi_pkt_t *pkt )
{
	picmg_cmd_resp_t *resp = ( picmg_cmd_resp_t* )pkt->resp;

	uint8_t hi_nibble, lo_nibble;
	hi_nibble = pkt->req->command >> 4;
	lo_nibble = pkt->req->command & 0x0f;

	switch( pkt->req->command ) {
		case HPM_INITIATE_UPGRADE_ACTION:
			picmg_hpm_initiate_upgrade_action( pkt );
			break;
		case HPM_UPLOAD_FIRMWARE_BLOCK:
			picmg_hpm_upload_firmware_block( pkt );
			break;
		case HPM_FINISH_FIRMWARE_UPLOAD:
			picmg_hpm_finish_firmware_upload( pkt );
			break;
		case HPM_ACTIVATE_FIRMWARE:
			picmg_hpm_activate_firmware( pkt );
			break;
		default:
			resp->completion_code = CC_INVALID_CMD;
			resp->picmg_id = 0;
			pkt->hdr.resp_data_len = 0;
			break;
	}
}

void
ipmi_process_request( ipmi_pkt_t *pkt )
{
	ipmi_cmd_resp_t *resp = ( ipmi_cmd_resp_t *)(pkt->resp);
	resp->completion_code = CC_NORMAL;

	switch( pkt->hdr.netfn ) {
		case NETFN_GROUP_EXTENSION_REQ:
			picmg_process_command( pkt );
			break;
		default:
			resp->completion_code = CC_INVALID_CMD;
			pkt->hdr.resp_data_len = 0;
			break;
	}
}

void
ipmi_process_pkt( ipmi_ws_t *ws )
{
	uint8_t i;
	ipmi_pkt_t *pkt;

	pkt = &ws->pkt;

#if 0
	sendchar('i');
	sendchar('n');
	sendchar(' ');
	sendchar(':');

	for(i=0;i<ws->len_in;i++){
		sendchar(ws->pkt_in[i]);
		ws->pkt_out[i] = ws->pkt_in[i];
	}
	ws->len_out = ws->len_in;

	sendchar('\r');
	sendchar('\n');
#endif

	ws_set_state( ws, WS_ACTIVE_MASTER_WRITE );

}
