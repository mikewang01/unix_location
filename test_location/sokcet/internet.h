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
#ifndef __INTERNET_H__
#define __INTERNET_H__

enum msg_type{
	MSG_IN = 1,
};

struct s_msg{ /*msgp定义的参照格式*/
	long type; /* 必须大于0,消息类型 */
	char mtext[256]; /*消息正文，可以是其他任何类型*/
};



int  socket_process(void* env);
extern void*  thread_socket(void* env); 

#endif


