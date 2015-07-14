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
#ifndef __CONFIG__H__
#define __CONFIG__H__
#include "oop_hal.h"

/*internet related config*/
#define HICLING_DOMAIN	"101.231.188.66"
#define HICLING_TIME_PATH "/time"
#define HICLING_HEALTH_PATH "/data/day"
#define HICLING_SERVER_PORT 9992//8086
#define INTERNET_TIMEOUT  1000 //ms

/*serial pipe process related*/
#define SERIAL_PIPE_TIMEOUT   10000 //ms
#define JSON_MAX_BUFFER_LENTH 8




#endif


