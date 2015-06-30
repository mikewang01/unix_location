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

DEF_CLASS(json_interface)
	int (*init)    	(CLASS(json_interface)*); /*initiate http object*/
	int (*register_upgrade_startup_callback)(CLASS(json_interface)*, void (*upgrade_startup_callback)());
	int (*register_upgrade_finish_callback)(CLASS(json_interface)*, void (*upgrade_callback)());
	int (*de_init) 	(CLASS(json_interface)*);/*delete http object*/
	int (*config)		(CLASS(json_interface)*, const char*, const char*);/*delete  http object*/
	int (*disconnect)	(CLASS(json_interface)*);/*disconnect from server*/
	int (*config_server)	(CLASS(json_interface)*, void*);/*start update fw from serverr*/
	int (*update_start)	(CLASS(json_interface)*, void*);/*start update fw from serverr*/
	int (*set_user_flow)	(CLASS(json_interface)*, int (*pfunc)(char*, size_t));/*start update fw from serverr*/ 
	void *user_data;/*point to user private data*/
END_DEF_CLASS(json_interface)

#endif


