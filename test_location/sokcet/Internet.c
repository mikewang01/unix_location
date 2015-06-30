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
#include <sys/ipc.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include<fcntl.h>
#include<thread_db.h>
#include "internet.h"
/*********************************************************************
* MACROS
*/

#define TIME_OUT  1000 //ms
#define HOST_DOMAIN "www.hicling.com"
//#define HOST_DOMAIN "101.231.188.66"
#define HTTP_PACKAGE "GET / HTTP/1.1 \r\n\
	Host: www.baidu.com:80\r\n\
	Proxy-Connection: keep-alive\r\n\
	Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8\r\n\
	User-Agent: Mozilla/5.0 (Windows NT 6.3; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/41.0.2272.118 Safari/537.36\r\n\
	Accept-Encoding: gzip, deflate, sdch\r\n\
	Accept-Language: zh-CN,zh;q=0.8\r\n\r\n"


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

/******************************************************************************
 * FunctionName : int  socket_recieved(void* env)
 * Description  :data recieved function after data sended
 * Parameters   : level : output level of pin
 * Returns      : none
*******************************************************************************/
void *socket_recieved(void* env){

	for(;;){
		
		
	}

}
/******************************************************************************
 * FunctionName : int  socket_process(void* env)
 * Description  :
 * Parameters   : level : output level of pin
 * Returns      : none
*******************************************************************************/
int  socket_process(void* env)
{
    int sock_fd;
    char buffer[1024];
    extern int errno;
    struct addrinfo *ailist, *aip;
    struct addrinfo  hint;
    int err, n;
    __bzero(&hint, sizeof(struct addrinfo));
    hint.ai_family = AF_INET;
    hint.ai_socktype= SOCK_STREAM;
    hint.ai_canonname = NULL;
    hint.ai_addr = NULL;
    hint.ai_next = NULL;
    //int i = 80;
    if((err = getaddrinfo(HOST_DOMAIN, NULL, &hint,&ailist)) != 0) {
        fprintf(stderr, "get adress info error error num = %d, error message = %s\r\n", errno, strerror(errno));
    } else {
        (((struct sockaddr_in*)(ailist->ai_addr))->sin_port) = htons(80);
        //(((struct sockaddr_in*)(ailist->ai_addr))->sin_addr.saddr) = inet
        printf("ip adress = %s  ports= %d\r\n",inet_ntoa(((struct sockaddr_in*)(ailist->ai_addr))->sin_addr) , ntohs(((struct sockaddr_in*)(ailist->ai_addr))->sin_port));
    }
    for(aip = ailist; aip != NULL; aip = aip->ai_next) {
        if((sock_fd = socket(aip->ai_family, SOCK_STREAM, 0)) < 0) {
            fprintf(stderr, "socket error error num = %d, error message = %s\r\n", errno, strerror(errno));
        } else {

            int flag = fcntl(sock_fd, F_GETFL);
            int w_fd;
            if(flag < 0) {
                perror("flag get error\r\n");
            }

            if(fcntl(sock_fd, F_SETFL, flag|O_NONBLOCK) < 0) {
                perror("set socket flag error\r\n");
            }

            int ret = connect(sock_fd, aip->ai_addr,aip->ai_addrlen);
            if(ret == 0) {
                printf("connected successfully\r\n");
            }
            /*when under nonblock mode , a negtive number of EINPROGRESS ganna be retruned by which we can  tell if connec successfully*/
            if((ret) < 0 && errno == EINPROGRESS) {
                fd_set w_set;
                fd_set r_set;
                FD_ZERO(&w_set);
                FD_SET(sock_fd, &w_set);
                r_set = w_set;
                struct timeval timeout;
                timeout.tv_sec = TIME_OUT/100;
                timeout.tv_usec = (TIME_OUT%1000)*1000;
                ret = select(sock_fd + 1, &r_set, &w_set, NULL, &timeout);

                if(ret > 0) {
                    int len =  sizeof(int);
                    int error;
                    /*this means sconnected errored*/
                    if(FD_ISSET(sock_fd, &r_set) && FD_ISSET(sock_fd, &w_set)) {
                        perror("connection errors by fd_isset\r\n");
                    } else if(FD_ISSET(sock_fd, &w_set)) { /*device coneccted successfully*/
                        printf("coneccted successfully\r\n");
                    }

                    if ((ret = getsockopt(sock_fd, SOL_SOCKET, SO_ERROR, &error, (socklen_t *)&len)  < 0)) {
                        perror("get sock option error\r\n");
                        goto F_FAILED;
                    } else { /*get error successfully*/

                        printf("error code = %d", error);
                        goto CONN_SUCESS;
                    }

                    if(error == 0)
                        goto CONN_SUCESS;
                    else goto F_FAILED;

                } else if(ret == 0) {
                    perror("select timeout\r\n");
                    goto F_FAILED;
                } else if(ret < 0) {

                    perror("select failed\r\n");
                }


            }
            if(ret < 0) {
                perror("connect error");
                goto F_FAILED;
            }
CONN_SUCESS:

            /*connect finished  */
            if (fcntl(sock_fd, F_SETFL, flag) < 0) {
                perror("change socket to block mode error\r\n");
            }

            if(send(sock_fd, HTTP_PACKAGE, strlen(HTTP_PACKAGE),0) < 0) {
                perror("send error\n");
            }

            printf("data ended  = %s\r\n",HTTP_PACKAGE);

            if ((n = read(sock_fd, (void*)buffer, 1024)) < 0) {
                perror("received error\n");
            } else {
                buffer[n] = 0;
                printf("data recieved from %s = %s data lenth = %d\r\n", HOST_DOMAIN, buffer ,n);

            }
            close(sock_fd);
        }
    }
F_SUCCESS:
    return 1;

F_FAILED:
    perror(" socket_process error \n");
    close(sock_fd);
    return -1;

}

/******************************************************************************
 * FunctionName : int  socket_process(void* env)
 * Description  :
 * Parameters   : level : output level of pin
 * Returns      : none
*******************************************************************************/
void* thread_socket(void* env)
{
    int a =0;
    int msg_len, g_msg_queue_id =  *((int*)env);
    struct s_msg msg;
	
    printf("pthread_socket = %d", a++);
    for(;;) {
        fprintf(stdout,"pthread_socket = %d\r\n", a++);
        //socket_process(NULL);
        sleep(1);
        msg_len = msgrcv(g_msg_queue_id, &msg, sizeof(msg.mtext), 0, IPC_NOWAIT);
        if(msg_len < 0) {
            perror("no data existed,d error number");
        } else {
            printf("received data = %s, type = %d, lenth = %d\r\n", msg.mtext, msg.type, msg_len);
        }
    }
    pthread_exit(NULL);
}


