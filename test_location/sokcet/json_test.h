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
#include "cling_date_utc.h"
/*health related data*/
struct health_data_inf {
    uint8_t   	date; /*cling watch cuurent time*/
    uint16_t 	total_steps;
    uint16_t 	walk_steps;
    uint16_t 	run_steps;
    uint16_t 	total_distance;
    uint16_t  	sport_time_total;
    uint16_t	calories_total;
    uint16_t	calories_sports;
    uint16_t	calories_mentablisim;
    uint16_t	deep_sleep;
    uint16_t	light_sleep;
    uint16_t  	sleep_total;
    uint16_t  	sleep_efficient;
    uint16_t  	wakeup_times;
    uint16_t  	heart_rate;
    uint16_t	skin_temp;
    uint32_t  	ble_timestamp;
    uint32_t  	beacon_timestamp;/*time when beacon recieved the message*/
};


DEF_CLASS(json_interface)
/*extended interface*/
CLASS(int_comp)  *internet_interface;
/*extended sha1 interface*/
CLASS(hmac_sha1) *sha1_interface;
CLASS(rtc_date) 	*base_rtc;
int (*init)    	(CLASS(json_interface)*); /*initiate http object*/
int (*deinit)    	(CLASS(json_interface)*); /*initiate http object*/
int (*set_location_inf)(CLASS(json_interface) *arg, char *pdev_id, char *pdev_mac, char rssi, uint32_t time_stamp);
int (*get_location_upload_json)(CLASS(json_interface) *arg, char *pbuffer);/*initiate http object*/
int (*get_time_request_json) (CLASS(json_interface) *arg, char *pbuffer);
int (*set_health_inf)(CLASS(json_interface) *arg,struct health_data_inf health, char *pdev_id, char *pdev_mac, uint32_t time_stamp);
int (*get_health_upload_json) (CLASS(json_interface) *arg, char *pbuffer);
int (*set_user_flow)	(CLASS(json_interface)*, int (*pfunc)(char*, size_t));/*start update fw from serverr*/
void *user_data;/*point to user private data*/
END_DEF_CLASS(json_interface)

#endif


