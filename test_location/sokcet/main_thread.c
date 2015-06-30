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

#include<netdb.h>
#include<errno.h>
#include<sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include<fcntl.h>
#include<thread_db.h>
#include <sys/ipc.h>
#include "internet.h"

/*********************************************************************
* MACROS
*/




/*********************************************************************
* TYPEDEFS
*/


/*********************************************************************
* GLOBAL VARIABLES
*/
/*set to 1 to open the key trigger function */

/*********************************************************************
* LOCAL VARIABLES
*/
static int message_queue_id = 1;

/******************************************************************************
 * FunctionName : int  socket_process(void* env)
 * Description  :
 * Parameters   : level : output level of pin
 * Returns      : none
*******************************************************************************/
#define MAX_COM  1
int json_test1(void);
int main()
{

    thread_t pid[MAX_COM];
    thread_t serid[MAX_COM];
    key_t key;

    struct s_msg in_msg_test;
	json_test1();
    key = ftok(".", 't');
    if(key < 0) {
        perror("intit ftock error\r\n");

    }


    if((message_queue_id = msgget(key, IPC_CREAT|0666)) < 0) {
		perror("crate message queue error!\r\n");
    }


    printf("create msg queue successfully...\n");

    int i;
    printf("main thread\r\n");
    for (i = 1; i < MAX_COM+1 ; i++) {

    if(pthread_create(&pid[i-1], NULL, thread_socket,  &message_queue_id) < 0) {
            printf("can not create thread_socket\n\r");
        }

        //pthread_create(&serid[i], NULL, serial_proc, &server_env);
    }
    pthread_detach(pid[0]);
    memcpy(in_msg_test.mtext, "hello mike", sizeof("hello mike"));
    in_msg_test.type = MSG_IN;

    for(;;) {
    if(msgsnd(message_queue_id, &in_msg_test, sizeof(in_msg_test.mtext), IPC_NOWAIT) < 0) {
            perror("message sended errorrd\r\n");

        }
	in_msg_test.mtext[0]++;
	sleep(2);
    }

    //pthread_join(pid[0], NULL);
    //socket_process(NULL);
    pthread_exit(0);
}

