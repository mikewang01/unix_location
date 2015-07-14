
/******************************************************************************
* Copyright 2013-2014 hicling Systems (MikeWang)
*
* FileName: serialport.c
*
* Description: The web server mode configration.
*              Check your hardware connection with the host while use this mode.
* Modification history:
*     2015/6/30, v1.0 create this file.
*******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <assert.h>
#include <sys/select.h>
#include "serial_port.h"
#include "config.h"
#include "user_interface.h"

/*********************************************************************
* MACROS
*/
#ifdef __cplusplus
extern "C" {
#endif
#define BUFFER_SIZE 1024
#define 	TEST_STRING "aaa"
/*********************************************************************
* TYPEDEFS
*/
#define SERIAL_SELECT_TIMEOUT 100	//unit as ms

/*********************************************************************
* GLOBAL VARIABLES
*/

/*********************************************************************
* LOCAL VARIABLES
*/
static struct private_thread_para *serial_para;

/******************************************************************************
* FunctionName :register_write_function
* Description  : regiter serial write callback
* Parameters   : write: write call back function
* Returns	   : 0: sucess
*				 -1: error
*******************************************************************************/


int cling_uart_ipc_fd_register(unsigned int  fd);

void cmd_process_callback(char cmd){
	if(cmd == 0x02){
		printf("time sync request recevied\n");
		uint32_t time_stamp = time(NULL);
		cling_u_data_send((char*)(&time_stamp), sizeof(time_stamp));
	}
}
/******************************************************************************
* FunctionName : serial_write_callback
* Description  : serial write call back function for maclayer
* Parameters   : write: write call back function
* Returns	   : 0: sucess
*				 -1: error
*******************************************************************************/

int serial_write_callback(char *pbuffer, unsigned int lenth){
	if(write(serial_para->fd, pbuffer, lenth) < 0);{
		printf("serial write error numbeer =%d\n",errno);
	}

}
/******************************************************************************
* FunctionName : serial_thread
* Description  : regiter serial write callback
* Parameters   : write: write call back function
* Returns      : 0: sucess
*				 -1: error
*******************************************************************************/
void *serial_thread(void *arg)
{
    int fd, nread, result ,iread;
	fd_set inputs,outputs;
	struct timeval serial_timeout;
	serial_para = (struct private_thread_para*)(arg);
	set_recieved_cmd_call_back(cmd_process_callback);
	unsigned char buffer[100];
	
	printf("serial_thread running\r\n");
	serial_timeout.tv_sec = SERIAL_SELECT_TIMEOUT/1000;
	serial_timeout.tv_usec = (SERIAL_SELECT_TIMEOUT%1000)*1000;

	if((fd = serial_init(0)) < 0) {
        perror("serial init failed\r\n");
    }
	/*aobtian thread id*/
	serial_para->fd = fd;
	/*find the max descriptor*/
	serial_para->fd_max = MAX(serial_para->fd, serial_para->fd_max);
	printf("serial_thread id = %d\r\n", serial_para->fd);
	printf("serial_parent write id = %d\r\n", serial_para->pipe_father_thread[0].pipe_write_fd);

	FD_ZERO(&inputs);
    FD_SET(serial_para->fd, &inputs);
	FD_SET(serial_para->pipe_child_thread->pipe_read_fd, &inputs);
	FD_ZERO(&outputs);
    FD_SET(serial_para->fd, &outputs);
	FD_SET(serial_para->pipe_father_thread->pipe_write_fd, &outputs);

	/*mac layer serial device fd*/
	register_write_function(serial_write_callback);
	/*cmd layer piepe ipc device fd*/
	cling_uart_ipc_fd_register(serial_para->pipe_father_thread->pipe_write_fd);
	
    for(;;) {

        result = select(serial_para->fd_max + 1, &inputs, NULL , (fd_set *)NULL, &serial_timeout);
        switch(result) {
        case 0: //time out
        	FD_ZERO(&inputs);
    		FD_SET(fd, &inputs);
			FD_SET(serial_para->pipe_child_thread->pipe_read_fd, &inputs);
        	serial_timeout.tv_sec = SERIAL_SELECT_TIMEOUT/1000;
			serial_timeout.tv_usec = (SERIAL_SELECT_TIMEOUT%1000)*1000;
			//printf("mac_sendlist_mantain_demon\r\n");
	 		mac_sendlist_mantain_demon();
        	//perror("serial select timeout\r\n");
			break;
        case -1:		 //error
        	printf("serial thread exit -1\n");
            exit(result);
			break;
        default:
            if(FD_ISSET(serial_para->fd,&inputs)) {
                ioctl(fd, FIONREAD, &iread);
                if(iread == 0) {
					continue;
                }
                nread = read(serial_para->fd, buffer, sizeof(buffer));
				for(int i = 0; i < nread; i++){
					/*process data package passed from serial device*/
					receive_one_char_callback(buffer[i]);
				}
				//buffer[nread] = 0;
				//printf("%s\r\n", buffer);
            }else if(FD_ISSET(serial_para->pipe_child_thread->pipe_read_fd, &inputs)){
            	/*this meeans serial buffer is writravke*/
				nread = read(serial_para->pipe_child_thread->pipe_read_fd, buffer, sizeof(buffer));
				buffer[nread] = 0;
				printf("%s\r\n", buffer);
				printf("serial data recieved\r\n");
	 			cling_u_data_send(buffer, nread);
			}else {
				printf("serial thread exit -2\n");
                exit(-1);
            }
			break;
        }//end of switch

    }
}
#ifdef __cplusplus
} /* extern "C" */
#endif

