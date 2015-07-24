
/******************************************************************************
 * Copyright 2013-2014 hicling Systems (mikewang)
 *
 * FileName: main thread.c
 *
 * Description: mian thead to manage data flow.
 *              
 * Modification history:
 *     2015/6/12, v1.0 create this file.
*******************************************************************************/

#include <netdb.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <thread_db.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <assert.h>
#include <sys/select.h>
#include "serial_port.h"
#include "json_test.h"
#include "pthread.h"
#include "int_compnent.h"
#include "json_test.h"
#include "internet.h"
#include "serial_port.h"
#include "config.h"
#include "uart_protocol_cmd.h"
#include "user_interface.h"
/*********************************************************************
* MACROS
*/
#define MAX_COM  2

#define TANSFER_ENDIAN_16(X)                 ((((uint16_t)(X) & 0xff00) >> 8) |(((uint16_t)(X) & 0x00ff) << 8))
#define TANSFER_ENDIAN_32(X)                 ((((uint32_t)(X) & 0xff000000) >> 24) | \
                                                            (((uint32_t)(X) & 0x00ff0000) >> 8) | \
                                                            (((uint32_t)(X) & 0x0000ff00) << 8) | \
                                                            (((uint32_t)(X) & 0x000000ff) << 24))

#define JSON_BUFFER_SIZE	4096*4
/*********************************************************************
* TYPEDEFS
*/
struct process_property {
    pthread_mutex_t property_mutex; /*used to protect  process property*/
    char mac_adress[30];
} ;

/*used to record file descrptor for main thread*/
union thread_fd_record{
	struct {
		struct pipe_private serial_pipe;
		struct pipe_private socket_pipe; /*serial fd used to communicate with serial thread*/ 
	}pipe_fd;
	struct pipe_private t[MAX_COM];

};

/*********************************************************************
* GLOBAL VARIABLES
*/
/*set to 1 to open the key trigger function */

int  json_test1(void);
void serial_test(void);
void *serial_thread(void *arg);
static CLASS(json_interface) *json_obj;


/*********************************************************************
* LOCAL VARIABLES
*/
static int message_queue_id = 1;

/*thread arry*/
static void *(*pthread_arry[])(void *)= {
    serial_thread,
    thread_socket,

};

/******************************************************************************
 * FunctionName : int  socket_process(void* env)
 * Description  :
 * Parameters   : level : output level of pin
 * Returns      : none
*******************************************************************************/

/*initiate http object*/
int main()
{

    pthread_t pid[MAX_COM];
    pthread_t serid[MAX_COM];
    key_t key;
    char pbuffer[2048];
    struct s_msg in_msg_test;
	union thread_fd_record fd_record;
#if 0 /*json official test debug*/
    json_test1();
#endif
    NEW(json_obj,json_interface);

#if 0/*for locatoon and json debug purpose*/
    /*get time syc json sucessfully*/
    json_obj->set_location_inf(json_obj, "ecling 123", "00:22:33:44", -100, time(NULL));
    json_obj->set_location_inf(json_obj, "tcling 123", "00:22:33:44", -100, time(NULL));
    struct health_data_inf health_temp = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18};

    json_obj->set_health_inf(json_obj, health_temp, "mike", "123444567", time(NULL));
    health_temp.date = 2;
    json_obj->set_health_inf(json_obj, health_temp, "mike", "123444567", time(NULL));
#if 1
    if(json_obj->get_location_upload_json(json_obj, pbuffer) == 0) {
        log_printf("get get_location_upload_json = %s\n", pbuffer);
    }
    if(json_obj->get_health_upload_json(json_obj, pbuffer) == 0) {
        log_printf("get get_health_upload_json = %s\n", pbuffer);
    } else {
        log_printf("get health json eror\n");
    }
#endif
#endif

    log_printf("main thread\r\n");
    for (int i = 1; i < MAX_COM+1 ; i++) {
        int pipe_fd[4];
        struct private_thread_para *t;
        /*created data parameter passed to child thread*/
        if ((pipe(pipe_fd) >= 0) && ((pipe(&pipe_fd[2])) >= 0)) {
            if((t = malloc(sizeof(struct private_thread_para))) == NULL) {
                return -1;
            }
			
			fd_record.t[i-1].pipe_read_fd  = pipe_fd[0];
			fd_record.t[i-1].pipe_write_fd = pipe_fd[3];
 			log_printf("pipe created %d times %d %d \n ", i, fd_record.t[i-1].pipe_read_fd, fd_record.t[i-1].pipe_write_fd);
			/*set fd record structor for child thread*/
            t->pipe_father_thread->pipe_read_fd = pipe_fd[0];
            t->pipe_father_thread->pipe_write_fd = pipe_fd[1]; /*for other thread to write data to its pipe*/
            t->pipe_child_thread->pipe_read_fd = pipe_fd[2];
            t->pipe_child_thread->pipe_write_fd = pipe_fd[3];
            t->fd_max = (t->pipe_father_thread->pipe_write_fd, t->pipe_child_thread->pipe_read_fd);
        }
        if(pthread_create(&pid[i-1], NULL, pthread_arry[i-1],  t) < 0) {
            log_printf("can not create thread_socket\n\r");
        }
        pthread_detach(pid[i-1]);
        t = NULL;
        //pthread_create(&serid[i], NULL, serial_proc, &server_env);
    } 

#if 1
#if 0
    log_printf("*********fd_record.pipe_fd.serial_pipe.pipe_read_fd id =%d**********\n",fd_record.pipe_fd.serial_pipe.pipe_read_fd );
    log_printf("*********fd_record.pipe_fd.serial_pipe.pipe_write_fd id =%d**********\n",fd_record.pipe_fd.serial_pipe.pipe_write_fd );
    log_printf("*********fd_record.pipe_fd.socket_pipe.pipe_read_fd id =%d**********\n",fd_record.pipe_fd.socket_pipe.pipe_read_fd );
    log_printf("*********fd_record.pipe_fd.socket_pipe.pipe_write_fd id =%d**********\n",fd_record.pipe_fd.socket_pipe.pipe_write_fd );
#endif
    fd_set inputs;
    FD_ZERO(&inputs);
    FD_SET(fd_record.pipe_fd.serial_pipe.pipe_read_fd, &inputs);

    int nread,result;
    char buffer[100];
	
    struct timeval temp;
	set_timeval(&temp, SERIAL_PIPE_TIMEOUT);
		
    char location_json_buffer[JSON_BUFFER_SIZE];
    char health_json_buffer[JSON_BUFFER_SIZE];

    for(;;) {
        //perror("com select timeout(((((((((\r\n");
        FD_ZERO(&inputs);
        FD_SET(fd_record.pipe_fd.serial_pipe.pipe_read_fd, &inputs);
        result = select(fd_record.pipe_fd.serial_pipe.pipe_read_fd + 1, &inputs, NULL , (fd_set *)NULL, &temp);
#if 1

        switch(result) {
        case 0: //time out
 
            FD_ZERO(&inputs);
            FD_SET(fd_record.pipe_fd.serial_pipe.pipe_read_fd, &inputs);
			FD_SET(fd_record.pipe_fd.serial_pipe.pipe_read_fd, &inputs);
            perror("main thread wait timeout\r\n");
	 		set_timeval(&temp, SERIAL_PIPE_TIMEOUT);
            /*indicate there is location related data existed*/
#if 1
            if(json_obj->get_location_upload_json(json_obj, location_json_buffer) == 0) {
                write(fd_record.pipe_fd.socket_pipe.pipe_write_fd, location_json_buffer, strlen(location_json_buffer)+1);
            }
#endif
            /*indicate there is location related data existed*/
            if(json_obj->get_health_upload_json(json_obj, health_json_buffer) == 0) {
                write(fd_record.pipe_fd.socket_pipe.pipe_write_fd, health_json_buffer, strlen(health_json_buffer)+1);
            }
            //write(fd_record.pipe_fd.serial_pipe.pipe_write_fd, "12345", 5);
            break;
        case -1:		 //error
            perror("com select timeout\r\n");
            //return result;
        default:
#if 1
            if(FD_ISSET(fd_record.pipe_fd.serial_pipe.pipe_read_fd, &inputs)) {
                ioctl(fd_record.pipe_fd.serial_pipe.pipe_read_fd, FIONREAD, &nread);
                if(nread == 0) {
                    continue;
                }
                nread = read(fd_record.pipe_fd.serial_pipe.pipe_read_fd, buffer, nread);
                log_printf("main thread pipe read lenth =%d\n", nread);
#if 0
                for(int i=0; i < nread ; i++) {
                    //log_printf("main thread pipe read le\n")
                    log_printf("=%02x", buffer[i]);
                }
#endif
                struct cling_location_rev *pserial_buffer = (struct cling_location_rev *)buffer;

                /*take use of bute packagetype to tell what kind of package it is*/
                if(pserial_buffer->package_type == (char)CMD_LOCATION_TYPE) {
                    log_printf("set location json!!\n");
                    int ret = json_obj->set_location_inf(json_obj, pserial_buffer->load.cling_id, pserial_buffer->load.cling_mac, pserial_buffer->load.cling_rssi[0], time(NULL));
                    /*if location queue is full then send it*/
                    if (ret < 0 || ret == 1) {
                        log_printf("location queque has been full\n");
                        if(json_obj->get_location_upload_json(json_obj, location_json_buffer) == 0) {
                            write(fd_record.pipe_fd.socket_pipe.pipe_write_fd, location_json_buffer, strlen(location_json_buffer)+1);
                        }
                    } else {
                        log_printf("location inf added suceefully\n");
                    }
                } else if(pserial_buffer->package_type == CMD_HEALTH_TYPE) {

                    log_printf("set health json!!\n");
                    struct cling_health_rev *pbuffer_health = (struct cling_health_rev *)buffer;
                    struct health_data_inf a = {0};
                    a.date = pbuffer_health->load.date;
                    a.total_steps = TANSFER_ENDIAN_16(pbuffer_health->load.steps);
                    a.total_distance = TANSFER_ENDIAN_16(pbuffer_health->load.distance);
                    a.calories_total = TANSFER_ENDIAN_16(pbuffer_health->load.calories);
                    a.sleep_total = TANSFER_ENDIAN_16(pbuffer_health->load.sleep_time);
                    a.heart_rate = pbuffer_health->load.heart_rate;
                    a.skin_temp = pbuffer_health->load.skin_temp;
                    /*for thsre is no buffer in serial thread so get current timestamp as the time recieved this message*/
                    a.beacon_timestamp = time(NULL);

                    int ret = json_obj->set_health_inf(json_obj, a, "ECIG", pbuffer_health->load.cling_mac, time(NULL));
                    if (ret < 0 || ret == 1) {
                        log_printf("health queque has been full\n");
                        /*indicate there is location related data existed*/
                        if(json_obj->get_health_upload_json(json_obj, health_json_buffer) == 0) {
                            write(fd_record.pipe_fd.socket_pipe.pipe_write_fd, health_json_buffer, strlen(health_json_buffer)+1);
                        }
                    } else {
                        log_printf("health inf added sucessfully\n");
                    }

                }


            }

#endif
            break;
        }//end of switch

#endif
    }
#endif
#if 0
//while(1);
    log_printf("main thread fd_record.pipe_fd.serial_pipe.pipe_write_fd = %d\n", fd_record.pipe_fd.serial_pipe.pipe_write_fd);
    char a[]="01234567891";
    for(;;) {
        write(fd_record.pipe_fd.socket_pipe.pipe_write_fd,pbuffer,strlen(pbuffer)+1);
        a[0]++;
        sleep(2);
    }

    //pthread_join(pid[0], NULL);
    //socket_process(NULL);
#endif
    pthread_exit(0);
}
