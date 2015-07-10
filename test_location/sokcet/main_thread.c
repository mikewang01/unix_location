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

/*********************************************************************
* MACROS
*/
#define MAX_COM  2

/*********************************************************************
* TYPEDEFS
*/
struct process_property{
	pthread_mutex_t property_mutex; /*used to protect  process property*/
	char mac_adress[30];
} ;


/*********************************************************************
* GLOBAL VARIABLES
*/
/*set to 1 to open the key trigger function */

int json_test1(void);
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

int get_mac(CLASS(int_comp) *arg, char *buffer, char *output_lenth); /*initiate http object*/

int init_json_interface (CLASS(json_interface) *arg); /*initiate http object*/
int HmacEncode(const char * algo,
                const char * key, unsigned int key_length,
                const char * input, unsigned int input_length,
                unsigned char * output, unsigned int *output_length);
int check_network_status ();

int main()
{

    thread_t pid[MAX_COM];
    thread_t serid[MAX_COM];
    key_t key;
	char pbuffer[2048];
    struct s_msg in_msg_test;
    int serial_pipe_read, serial_pipe_write;
    int socket_pipe_read, socket_pipe_write;
	//get_mac(NULL, NULL, NULL);
	json_test1();
	check_network_status ();
	NEW(json_obj,json_interface);
	/*get time syc json sucessfully*/
	json_obj->set_location_inf(json_obj, "ecling 123", "00:22:33:44", -100, time(NULL));
	json_obj->set_location_inf(json_obj, "tcling 123", "00:22:33:44", -100, time(NULL));
	
	if(json_obj->get_location_upload_json(json_obj, pbuffer) == 0){
		printf("get get_location_upload_json = %s\n", pbuffer);
	}
	if(json_obj->get_health_upload_json(json_obj, pbuffer) == 0){
		printf("get get_location_upload_json = %s\n", pbuffer);
	}else{
		printf("get health json eror\n");
	}
	//static int get_location_upload_json(CLASS(json_interface) *arg, char *pbuffer)
#if 0
	#define KEY  "5EHdd8_334dyUjjddleqH6YHHm"
	#define TEXT "v1.0.2t45772(a)18:fe:34:9b:b4:85{\"action\":\"time\"}1429269133&blue=CLING E35931:32:33:34:35:36"

	int  output_len=0;
	
	HmacEncode("sha1", KEY, strlen(KEY), TEXT, strlen(TEXT),pbuffer, &output_len);
	printf("output lenth = %d\n\r", output_len);
	
	for(int i=0; i< output_len; i++){
		//printf("output data = %d\n\r", i);
		printf("0x%02x ", pbuffer[i]);

	}
	printf("output finished\n");
#endif
	//init_json_interface (NULL); /*initiate http object*/
//	while(1);
    //json_test1(); 
    //serial_test();
    printf("main thread\r\n");
	
    for (int i = 1; i < MAX_COM+1 ; i++) {
        int pipe_fd[4];
        struct private_thread_para *t;
        /*created data parameter passed to child thread*/
        if ((pipe(pipe_fd) >= 0) && ((pipe(&pipe_fd[2])) >= 0)) {
            if((t = malloc(sizeof(struct private_thread_para))) == NULL) {
                return -1;
            }
            if(i == 1) {
                serial_pipe_read  =  pipe_fd[0];
                serial_pipe_write =  pipe_fd[3];
                //printf("serial_pipe_read id =%d",serial_pipe_read );
            } else {
                socket_pipe_read = pipe_fd[0];
                socket_pipe_write = pipe_fd[3];
            }
            t->pipe_father_thread->pipe_read_fd = pipe_fd[0];
            t->pipe_father_thread->pipe_write_fd = pipe_fd[1]; /*for other thread to write data to its pipe*/
            t->pipe_child_thread->pipe_read_fd = pipe_fd[2];
            t->pipe_child_thread->pipe_write_fd = pipe_fd[3];
            t->fd_max = (t->pipe_father_thread->pipe_write_fd, t->pipe_child_thread->pipe_read_fd);
        }
        if(pthread_create(&pid[i-1], NULL, pthread_arry[i-1],  t) < 0) {
            printf("can not create thread_socket\n\r");
        }
        pthread_detach(pid[i-1]);
        t = NULL;
        //pthread_create(&serid[i], NULL, serial_proc, &server_env);
    }

#if 0
    printf("*********serial_pipe_read id =%d\n",serial_pipe_read );
    fd_set inputs;
    FD_ZERO(&inputs);
    FD_SET(serial_pipe_read, &inputs);

    int nread,result;
    char buffer[100];
    struct timeval temp;
    temp.tv_sec = 10;

	
    for(;;) {
        //perror("com select timeout(((((((((\r\n");
        FD_ZERO(&inputs);
        FD_SET(serial_pipe_read, &inputs);
        result = select(serial_pipe_read + 1, &inputs, NULL , (fd_set *)NULL, &temp);
#if 1

        switch(result) {
        case 0: //time out
            temp.tv_sec = 10;
            FD_ZERO(&inputs);
            FD_SET(serial_pipe_read, &inputs);
            perror("main thread wait timeout\r\n");
			//write(serial_pipe_write, "12345", 5);
            break;
        case -1:		 //error
            perror("com select timeout\r\n");
            //return result;
        default:
#if 1
            if(FD_ISSET(serial_pipe_read, &inputs)) {
                ioctl(serial_pipe_read, FIONREAD, &nread);
                if(nread == 0) {
                    continue;
                }
                nread = read(serial_pipe_read, buffer, nread);
                printf("main thread pipe read lenth =%d\n", nread);
				
                for(int i=0; i < nread ; i++) {
                    // printf("main thread pipe read le\n")
                    fprintf(stdout,"=%02x", buffer[i]);
                }
				printf("\n");
                /*write data to serial thread through pipe*/
                write(serial_pipe_write, "12345", 5);

            }

            //buffer[nread] = 0;
            //printf("%s\r\n", buffer);
#endif
            break;
        }//end of switch

#endif
    }
#endif

//while(1);
	printf("main thread serial_pipe_write = %d\n", serial_pipe_write);
    char a[]="01234567891";
    for(;;) {
        write(socket_pipe_write,pbuffer,strlen(pbuffer)+1);
        a[0]++;
        sleep(2);
    }

    //pthread_join(pid[0], NULL);
    //socket_process(NULL);
    pthread_exit(0);
}

