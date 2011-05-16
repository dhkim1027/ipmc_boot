#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "eeprom_driver.h"
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
		boot_init_write_flash();
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
		return;
	}

	boot_write_flash(req->data, data_size);
	block_number++;

	resp->completion_code = CC_NORMAL;
	resp->picmg_id = PICMG_ID;
#if 0
	resp->section_offset[0] = 0;
	resp->section_offset[1] = 0;
	resp->section_offset[2] = 0;
	resp->section_offset[3] = 0;
	resp->section_length[0] = 0;
	resp->section_length[1] = 0;
	resp->section_length[2] = 0;
	resp->section_length[3] = 0;
#endif

	pkt->hdr.resp_data_len = 9;
}

void
picmg_hpm_finish_firmware_upload( ipmi_pkt_t *pkt )
{
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
	boot_set_mode(APP_MODE);

	resp->completion_code = CC_NORMAL;
	resp->picmg_id = PICMG_ID;

	pkt->hdr.resp_data_len = 1;
}

void  
activate_fw_function(void)
{
//	boot_backup_fw();
	boot_jump_app_section();
}

void
picmg_hpm_activate_firmware( ipmi_pkt_t *pkt )
{
	ipmi_ws_t *ws = (ipmi_ws_t *)pkt->hdr.ws;
	activate_firmware_resp_t *resp = ( activate_firmware_resp_t *)pkt->resp;

	resp->completion_code = CC_NORMAL;
	resp->picmg_id = PICMG_ID;

	pkt->hdr.resp_data_len = 1;
	ws->ipmi_completion_function = boot_jump_app_section;
}

void
picmg_process_command( ipmi_pkt_t *pkt )
{
	picmg_cmd_resp_t *resp = ( picmg_cmd_resp_t* )pkt->resp;

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
ipmi_get_device_id_cmd( ipmi_pkt_t *pkt )
{
	get_device_id_cmd_resp_t *gdi_resp = (get_device_id_cmd_resp_t *)(pkt->resp);

	gdi_resp->completion_code = CC_NORMAL;
	gdi_resp->device_id = 0x0;
	gdi_resp->device_sdr_provided = 1;  /* 1 = device provides Device SDRs */
	gdi_resp->device_revision = 0;      /* 4 bit field, binary encoded */
	gdi_resp->device_available = 0;
	gdi_resp->major_fw_rev = EEPROM_ReadByte(0, FW_MAJ_VER_ADDR);
	gdi_resp->minor_fw_rev = EEPROM_ReadByte(0, FW_MIN_VER_ADDR);
	gdi_resp->ipmi_version = 0x02;
	gdi_resp->add_dev_support =
		DEV_SUP_IPMB_EVENT_GEN |
		DEV_SUP_FRU_INVENTORY |
		DEV_SUP_SDR_REPOSITORY |
		DEV_SUP_SENSOR;

	gdi_resp->manuf_id[0] = 0xf9;
	gdi_resp->manuf_id[1] = 0x19;
	gdi_resp->manuf_id[2] = 0x0;
	gdi_resp->product_id[0] = 0x01;
	gdi_resp->product_id[1] = 0x00;
	gdi_resp->aux_fw_rev[0] = 0x0;
	gdi_resp->aux_fw_rev[1] = 0x0;
	gdi_resp->aux_fw_rev[2] = 0x0;
	gdi_resp->aux_fw_rev[3] = 0x0;
	pkt->hdr.resp_data_len = 15;
}

void
ipmi_process_app_req( ipmi_pkt_t *pkt )
{
	ipmi_cmd_resp_t *resp = ( ipmi_cmd_resp_t*)(pkt->resp);

	switch( pkt->req->command )
	{
		case IPMI_CMD_GET_DEVICE_ID:
			ipmi_get_device_id_cmd( pkt );
			break;
		default:
			resp->completion_code = CC_INVALID_CMD;
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
		case NETFN_APP_REQ:
			ipmi_process_app_req( pkt );
			break;
		case NETFN_GROUP_EXTENSION_REQ:
			picmg_process_command( pkt );
			break;
		default:
			resp->completion_code = CC_INVALID_CMD;
			pkt->hdr.resp_data_len = 0;
			break;
	}
}
