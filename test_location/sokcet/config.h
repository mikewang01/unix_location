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


#endif


