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
//#include <net/if.h>
#include <netdb.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/ipc.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <thread_db.h>
#include <linux/sockios.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/if.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/ethtool.h>
#include "config.h"
#include "internet.h"
#include <string.h>


/*********************************************************************
* MACROS
*/ //caddr_t
#define TIME_OUT  1000 //ms
#define HOST_DOMAIN "www.ibm.com"
#define HOST_PORT    80

#define HICLING_DOMAIN	"101.231.188.66"
#define HICLING_TIME_PATH "/time"
#define HICLING_HEALTH_PATH "/data/day"

#define HICLING_SERVER_PORT 9992//8086


//#define HOST_DOMAIN "101.231.188.66"
#define HTTP_PACKAGE "GET /time HTTP/1.1\r\n\
                Host: 101.231.188.66:9992\r\n\
                User-Agent: wget\r\n\
                Accept: text/xml,text/html;*/*\r\n\r\n"


#define HTTP_HEAD_POST_SEGMENT "POST %s HTTP/1.0 \r\nHost: %s\r\nContent-Length:%d\r\n"

#define HTTP_HEAD_INFO_SEGMENT "Cache-Control: no-cache\r\n\
User-Agent: Mozilla/5.0 (Windows NT 5.1) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/30.0.1599.101 Safari/537.36 \r\n\
Accept: */*\r\n\
Accept-Encoding: gzip,deflate,sdch\r\n\
Accept-Language: zh-CN,zh;q=0.8\r\n\r\n"



#define SET_NETWORK_STATE(__x, __y)  (__x).socket_current_state = (__y)
#define GET_NETWORK_STATE(__x)  	((__x).socket_current_state)
#define RESET_NEWORK_PARA(__x)      do{ \
	__x.socket_current_state = STATE_ETH_DOWN;\
	close(__x.sock_fd);\
} while(0)
/*********************************************************************
* TYPEDEFS
*/
typedef enum { IFSTATUS_UP, IFSTATUS_DOWN, IFSTATUS_ERR } interface_status_t;

typedef signed int u32;


/*USED TO DRIVING SOCJET STAT MACHINE*/
enum int_state_machine {
    STATE_ETH_DOWN = 0,
    STATE_ETH_UP,
    STATE_ETH_DNS,
    STATE_ETH_DISCON,
    STATE_ETH_CONNECTING,
    STATE_ETH_CONNECTED,
    STATE_ETH_SENDING,
    STATE_ETH_SENDED,
    STATE_ETH_RECIEVING,
    STATE_ETH_RECIEVED,
    STATE_ETH_CLOSED,
};


struct net_var {
    int socket_current_state;
    int sock_fd;
    struct addrinfo *aip;
};
/*********************************************************************
* GLOBAL VARIABLES
*/
/*set to 1 to open the key trigger function */

/*********************************************************************
* LOCAL VARIABLES
*/
struct net_var socket_inf = {
    .socket_current_state = STATE_ETH_DOWN,
    .sock_fd = -1,
    .aip = NULL,
};

/*********************************************************************
* LOCAL FUNCTIONS
*/
int check_network_status();

/******************************************************************************
 * FunctionName : int  socket_recieved(void* env)
 * Description  :data recieved function after data sended
 * Parameters   : level : output level of pin
 * Returns      : none
*******************************************************************************/
void *socket_recieved(void* env)
{

    for(;;) {

    }

}
/******************************************************************************
 * FunctionName : int message_exchange_statemachine
 * Description  : statemachine to exchange information with  server
 * Parameters   :
 * Returns      : none
*******************************************************************************/
char *int_state_machine_str[] = {
    "STATE_ETH_DOWN",
    "STATE_ETH_UP",
    "STATE_ETH_DNS",
    "STATE_ETH_DISCON",
    "STATE_ETH_CONNECTING",
    "STATE_ETH_CONNECTED",
    "STATE_ETH_SENDING",
    "STATE_ETH_SENDED",
    "STATE_ETH_RECIEVING",
    "STATE_ETH_RECIEVED",
    "STATE_ETH_CLOSED",
};


int message_exchange_statemachine(char *host_domain, int port, char *p_obuffer, char *p_ibuffer, int lenth)
{

    int ret = -1;

    printf("socket current state of state machine = %s\n",int_state_machine_str[socket_inf.socket_current_state]);
    switch(GET_NETWORK_STATE(socket_inf)) {

    case STATE_ETH_DOWN: {
        /*first check if */
        int status = check_network_status();
        if(status == IFSTATUS_UP) {

            SET_NETWORK_STATE(socket_inf, STATE_ETH_UP);
        } else {
            printf("interface network down\n");
        }
    }
    break;
    case STATE_ETH_UP:
        /*once wifi interface is up ready, dns and sock fd IS READY to go*/
    {
        struct addrinfo *ailist, *aip;
        struct addrinfo  hint;
        int err, n;
        bzero(&hint, sizeof(struct addrinfo));
        hint.ai_family = AF_INET;
        hint.ai_socktype= SOCK_STREAM;
        hint.ai_canonname = NULL;
        hint.ai_addr = NULL;
        hint.ai_next = NULL;
        //int i = 80;
        if(socket_inf.aip == NULL) {
            if((err = getaddrinfo(host_domain, NULL, &hint,&ailist)) != 0) {
                fprintf(stderr, "get adress info error error num = %d, error message = %s\r\n", errno, strerror(errno));
                /*if get dns fialed return immidiately*/
                break;
            } else {
                (((struct sockaddr_in*)(ailist->ai_addr))->sin_port) = htons(port);
                //(((struct sockaddr_in*)(ailist->ai_addr))->sin_addr.saddr) = inet
                printf("ip adress = %s  ports= %d\r\n",inet_ntoa(((struct sockaddr_in*)(ailist->ai_addr))->sin_addr) , ntohs(((struct sockaddr_in*)(ailist->ai_addr))->sin_port));
            }

            for(aip = ailist; aip != NULL; aip = aip->ai_next) {
                if((socket_inf.sock_fd = socket(aip->ai_family, SOCK_STREAM, 0)) < 0) {
                    fprintf(stderr, "socket error error num = %d, error message = %s\r\n", errno, strerror(errno));
                    break;
                } else {
                    socket_inf.aip = aip;
                }
            }
        } else {/*if dns has been peoceeded*/
            if((socket_inf.sock_fd = socket(socket_inf.aip->ai_family, SOCK_STREAM, 0)) < 0) {
                fprintf(stderr, "socket error error num = %d, error message = %s\r\n", errno, strerror(errno));
                break;
            }
        }

        /*set fd to unblcock mode*/
        int flag = fcntl(socket_inf.sock_fd , F_GETFL);
        if(flag < 0) {
            perror("flag get error\r\n");
            close(socket_inf.sock_fd);
        }
        if(fcntl(socket_inf.sock_fd, F_SETFL, flag|O_NONBLOCK) < 0) {
            perror("set socket flag error\r\n");
            close(socket_inf.sock_fd);
        }
        /*set current state to  STATE_ETH_DISCON the connect to server*/
        SET_NETWORK_STATE(socket_inf, STATE_ETH_DISCON);


    }

    break;
	/*ether net disconnect*/
    case STATE_ETH_DISCON: {
        int ret = connect(socket_inf.sock_fd, socket_inf.aip->ai_addr, socket_inf.aip->ai_addrlen);
        if(ret == 0) {
            printf("connected imidiately\r\n");
            SET_NETWORK_STATE(socket_inf, STATE_ETH_CONNECTED);
        } else {
            /*this means socet connecting is under progress*/
            if(((ret) < 0) && (errno == EINPROGRESS)) {
                /*set to  connecting mode  take advatege of select function to observe this state*/
                SET_NETWORK_STATE(socket_inf, STATE_ETH_CONNECTING);
            }

        }
    }
    break;
    case STATE_ETH_CONNECTING: {
        int ret;
        fd_set w_set;
        fd_set r_set;
        FD_ZERO(&w_set);
        FD_SET(socket_inf.sock_fd, &w_set);
        r_set = w_set;
        struct timeval timeout;
        timeout.tv_sec = TIME_OUT/1000;
        timeout.tv_usec = (TIME_OUT%1000)*1000;
        ret = select(socket_inf.sock_fd + 1, &r_set, &w_set, NULL, &timeout);

        if(ret > 0) {
            int len =  sizeof(int);
            int error;
            /*this means sconnected errored*/
            if(FD_ISSET(socket_inf.sock_fd, &r_set) || FD_ISSET(socket_inf.sock_fd, &w_set)) {
                perror("connection errors by fd_isset\r\n");

                if ((ret = getsockopt(socket_inf.sock_fd, SOL_SOCKET, SO_ERROR, &error, (socklen_t *)&len)  < 0)) {
                    perror("get sockopy error \n ");
                }

                if(error == 0) {
                    printf("coneccted successfully\r\n");
                    SET_NETWORK_STATE(socket_inf, STATE_ETH_CONNECTED);
                } else {
					perror("connection failed\n");
					RESET_NEWORK_PARA(socket_inf);
                }

            } else if(FD_ISSET(socket_inf.sock_fd, &w_set)) { /*device coneccted successfully*/
                printf("coneccted successfully\r\n");
                SET_NETWORK_STATE(socket_inf, STATE_ETH_CONNECTED);
            }

        } else if(ret == 0) {
            perror("select timeout\r\n");
			RESET_NEWORK_PARA(socket_inf);
        } else if(ret < 0) {

            perror("select failed\r\n");
			RESET_NEWORK_PARA(socket_inf);
        }
    }
    break;
	
    case STATE_ETH_CONNECTED:
        if(send(socket_inf.sock_fd, p_obuffer, strlen(p_obuffer),0) < 0) {
            perror("send error\n");
            close(socket_inf.sock_fd);
            RESET_NEWORK_PARA(socket_inf);
        } else {
            SET_NETWORK_STATE(socket_inf, STATE_ETH_RECIEVING);
        }

        printf("data ended  = %s\r\n",p_obuffer);
        break;
    case STATE_ETH_SENDING:
        break;
    case STATE_ETH_SENDED:
        break;
    case STATE_ETH_RECIEVING: {
        int n;
        fd_set r_set;
        FD_ZERO(&r_set);
        FD_SET(socket_inf.sock_fd, &r_set);
        struct timeval timeout;
        timeout.tv_sec = TIME_OUT/100;
        timeout.tv_usec = (TIME_OUT%1000)*1000;
        ret = select(socket_inf.sock_fd + 1, &r_set, NULL, NULL, &timeout);
        if(ret > 0) {
            /*this means data habeen recieved */
            if(FD_ISSET(socket_inf.sock_fd, &r_set)) {
                if ((n = read(socket_inf.sock_fd, (void*)p_ibuffer, lenth)) < 0) {
                    perror("received error\n");
                } else {
                    p_ibuffer[n] = 0;
                    ret = n;
                    SET_NETWORK_STATE(socket_inf, STATE_ETH_RECIEVED);
                }

            }
        }
    }
    break;
    case STATE_ETH_RECIEVED:
        printf("socket_inf.sock_fd= %d\n",socket_inf.sock_fd);
        close(socket_inf.sock_fd);
        /*one cycle finished, reset network state*/
        SET_NETWORK_STATE(socket_inf, STATE_ETH_DOWN);

        break;
    case STATE_ETH_CLOSED:
        break;

    }
    return ret;
}

#if 0
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
    bzero(&hint, sizeof(struct addrinfo));
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
#endif
/******************************************************************************
 * FunctionName : int  socket_process(void* env)
 * Description  :
 * Parameters   : level : output level of pin
 * Returns      : none
*******************************************************************************/
void* thread_socket(void* env)
{
    int a =0,n;
    int msg_len;// g_msg_queue_id =  *((int*)env);
    struct s_msg msg;
    struct private_thread_para *socket_para = (struct private_thread_para*)(env);
    char buffer[1024];

    /*prepare for pipe selection operation*/
    fd_set pipe_rset,pipe_wset;
    FD_ZERO(&pipe_rset);
    FD_ZERO(&pipe_wset);
    FD_SET(socket_para->pipe_child_thread->pipe_read_fd, &pipe_rset);
    pipe_wset = pipe_rset;
    //timeval socket_pipe_time;
    //socket_pipe_time.tv_sec = 1;
    //socket_pipe_time.tv_usec = 0;
    /*set timeout structor*/

    printf("pthread_socket_test_num = %d", a++);
    for(;;) {
        fprintf(stdout,"pthread_socket_test_num = %d\r\n", a++);
        char buffer[1024];
        char httphead[1024];
        /*WHEN THA LAST  INTERNET SENDING PEOCESS FINISHED, CHANGE THE ROLE TO  LISTEN TO PIPE WAIETING FOR NEW DATA*/
        if(GET_NETWORK_STATE(socket_inf) == STATE_ETH_DOWN) {
            if(select(socket_para->fd_max + 1, &pipe_rset, NULL , (fd_set *)NULL, NULL) < 0) {

            }
            /*this means there is some data has been emmitted here*/
            if(FD_ISSET(socket_para->pipe_child_thread->pipe_read_fd, &pipe_rset)) {
                /*this meeans serial buffer is writravke*/
                int nread = read(socket_para->pipe_child_thread->pipe_read_fd, buffer, sizeof(buffer));
                buffer[nread] = 0;
                sprintf(httphead, HTTP_HEAD_POST_SEGMENT, HICLING_TIME_PATH, HICLING_DOMAIN, buffer ? strlen(buffer) : 0);
                sprintf(httphead + strlen(httphead), HTTP_HEAD_INFO_SEGMENT);
                strcat(httphead +  strlen(httphead), buffer);
                //sprintf(httphead + os_strlen(httphead), HTTP_HEAD_INFO_SEGMENT);
            }
        };
		
        n =  message_exchange_statemachine(HICLING_DOMAIN, HICLING_SERVER_PORT ,httphead, buffer, 1024);
        //pthread_exit(NULL);
        if(GET_NETWORK_STATE(socket_inf) == STATE_ETH_RECIEVED) {
            printf("data recieved from %s = %s data lenth = %d\n", HOST_DOMAIN, buffer ,n);
        }
    }
}





/******************************************************************************
 * FunctionName : interface_detect_beat_ethtool
 * Description  : used to detetct uif internet is connected or not
 * Parameters   : int socket file description, char *iface
 * Returns      : none
*******************************************************************************/

interface_status_t interface_detect_beat_ethtool(int fd, char *iface)
{
    struct ifreq ifr;
    struct ethtool_value edata;

    memset(&ifr, 0, sizeof(ifr));
    strncpy(ifr.ifr_name, iface, sizeof(ifr.ifr_name)-1);

    edata.cmd = ETHTOOL_GLINK;
    ifr.ifr_data = (void*) &edata;

    if (ioctl(fd, SIOCETHTOOL, &ifr) == -1) {
        perror("ETHTOOL_GLINK failed ");
        return IFSTATUS_ERR;
    }
    return edata.data ? IFSTATUS_UP : IFSTATUS_DOWN;
}

/******************************************************************************
 * FunctionName : check_network_status
 * Description  : user interface to get current internet device status
 * Parameters   : NULL
 * Returns      : -1 error: >=0: specific ethernet status
*******************************************************************************/
int check_network_status ()
{
    FILE *fp;
    interface_status_t status;
    char buf[512] = {'\0'};
    char hw_name[10] = {'\0'};
    char *token = NULL;
    int ret = -1;
    /* 获取网卡名称 */
    if ((fp = fopen("/proc/net/dev", "r")) != NULL) {
        while (fgets(buf, sizeof(buf), fp) != NULL) {
            if(strstr(buf, "eth") != NULL) {
                token = strtok(buf, ":");
                while (*token == ' ') ++token;
                strncpy(hw_name, token, strlen(token));
            }
        }
    }
    fclose(fp);
//方法一：查看一个文件文件，相对来说比较简单
#if 0
    char carrier_path[512] = {'\0'};

    memset(buf, 0, sizeof(buf));
    snprintf(carrier_path, sizeof(carrier_path), "/sys/class/net/%s/carrier", hw_name);
    if ((fp = fopen(carrier_path, "r")) != NULL) {
        while (fgets(buf, sizeof(buf), fp) != NULL) {
            if (buf[0] == '0') {
                status = IFSTATUS_DOWN;
            } else {
                status = IFSTATUS_UP;
            }
        }
    } else {
        perror("Open carrier ");
    }
    fclose(fp);
#endif

//方法二：用函数吧！有点复杂，但是也是一种有效的办法
#if 1
    int fd;
    if((fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket ");
        goto FAILED;
    }
    status = interface_detect_beat_ethtool(fd, hw_name);
    close(fd);
#endif

    switch (status) {
    case IFSTATUS_UP:
        ret = IFSTATUS_UP;
        printf("%s : link up\n", hw_name);
        break;

    case IFSTATUS_DOWN:
        ret = IFSTATUS_UP;
        printf("%s : link down\n", hw_name);
        break;

    default:

        printf("Detect Error\n");
        goto FAILED;
        break;
    }
    return ret;
FAILED:
    return -1;
}






