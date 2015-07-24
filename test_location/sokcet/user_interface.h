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
#include <stdint.h>
#include <time.h>
#include <linux/types.h>
struct pipe_private {
	int pipe_read_fd; /*data read pipe fd*/
	int pipe_write_fd;/*data write pipe fd*/
};

/*thread related intrafece*/
//fd[0]:read,fd[1]:write
struct private_thread_para {
    int tid;	/*store the pthread id of child thread*/
	int fd;
	int fd_max; /*record max fd for selecting function*/
	struct pipe_private pipe_father_thread[1], pipe_child_thread[1];
    void *usr_data;
};

#define MAX(x,y) ((x)>(y)?(x):(y)) 


#define _DEBUG
#ifdef _DEBUG
#define log_printf(format, ...) do{\
		printf(format, ##__VA_ARGS__);\
	}while(0)
#else
	#define log_printf(format, ...) 
#endif

//printf(" at %s<line:%d>\n", __FILE__ , __LINE__);\

#if 0
static inline void set_timeval(struct timeval *pt, long ms){
	pt->tv_sec = (long)(ms/1000);
	pt->tv_usec = (long)((ms%1000)*1000);
}
#endif

#define  set_timeval(__x, ms) do{\
	struct timeval *pt = (__x);\
	((pt))->tv_sec = (long)(ms/1000);\
	((pt))->tv_usec = (long)((ms%1000)*1000);\
}while(0)





#endif


