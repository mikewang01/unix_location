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

struct s_msg{ /*msgp����Ĳ��ո�ʽ*/
	long type; /* �������0,��Ϣ���� */
	char mtext[256]; /*��Ϣ���ģ������������κ�����*/
};



int  socket_process(void* env);
extern void*  thread_socket(void* env); 

#endif


