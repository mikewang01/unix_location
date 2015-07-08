/******************************************************************************
 * Copyright 2013-2014 Espressif Systems (Wuxi)
 *
 * FileName: user_webserver.c
 *
 * Description: The web server mode configration.
 *              Check your hardware connection with the host while use this mode.
 * Modification history:
 *     2014/3/12, v1.0 create this file.
*******************************************************************************/
#ifndef __JSON_TEST_H__
#define __JSON_TEST_H__
#include "oop_hal.h"
#include "int_compnent.h"
#include "hmac_sha1.h"
DEF_CLASS(json_interface)
	/*extended interface*/
	CLASS(int_comp)  *internet_interface;
	/*extended sha1 interface*/
	CLASS(hmac_sha1) *sha1_interface;
	int (*init)    	(CLASS(json_interface)*); /*initiate http object*/
	int (*get_location_upload_json)(CLASS(json_interface) *arg, char *pbuffer);/*initiate http object*/
	int (*get_time_request_json) (CLASS(json_interface) *arg, char *pbuffer);
	int (*set_user_flow)	(CLASS(json_interface)*, int (*pfunc)(char*, size_t));/*start update fw from serverr*/ 
	void *user_data;/*point to user private data*/
END_DEF_CLASS(json_interface)

#endif


