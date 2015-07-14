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
#ifndef __INT_COMPONENT_H__
#define __INT_COMPONENT_H__
#include "oop_hal.h"

DEF_CLASS(int_comp)
	int (*init)    		(CLASS(int_comp)*); /*initiate http object*/
	int (*de_init)    	(CLASS(int_comp)*); /*initiate http object*/
	int (*get_mac)		(CLASS(int_comp)*,  char*, unsigned int*);/*delete  http object*/
	int (*get_ip)		(CLASS(int_comp)*,  char*, unsigned int*);/*disconnect from server*/
	void *user_data;/*point to user private data*/
END_DEF_CLASS(int_comp)

#endif


