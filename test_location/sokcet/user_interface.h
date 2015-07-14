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
#ifndef __USER_INTERFACE__H__
#define __USER_INTERFACE__H__
#include "oop_hal.h"

/*thread related intrafece*/
//fd[0]:read,fd[1]:write
struct private_thread_para {
    int tid;	/*store the pthread id of child thread*/
	int fd;
	int fd_max; /*record max fd for selecting function*/
    struct pipe {
        int pipe_read_fd; /*data read pipe fd*/
        int pipe_write_fd;/*data write pipe fd*/
    } pipe_father_thread[1],pipe_child_thread[1];
    void *usr_data;
};

#define MAX(x,y) ((x)>(y)?(x):(y)) 

static inline void set_timeval(struct timeval *pt, uint32_t ms){
	pt->tv_sec = ms/1000;
	pt->tv_usec = (ms%1000)*1000;
}

#endif


