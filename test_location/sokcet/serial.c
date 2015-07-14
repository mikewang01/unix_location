
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
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <assert.h>
#include <sys/select.h>
#include "serial_port.h"
#include <unistd.h>

/*********************************************************************
* MACROS
*/
#ifdef __cplusplus
extern "C" {
#endif
#define MAX_DEV_NAME 256

#define MAX_TIMEOUT      0 //0.1s
#define L_CH_TRIGGER_T   3 //LOW CHARACTOR NUMBER TRIGGER THREHOLD
#define MIN(a,b) ((a) < (b)?(a):(b))

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
static int speed_arr[] = {B115200, B38400, B19200, B9600, B4800, B2400, B1200, B300,
           B38400, B19200, B9600, B4800, B2400, B1200, B300,
};
static int name_arr[] = {115200, 38400,  19200,  9600,  4800,	2400,  1200,  300,
                         38400,  19200,  9600, 4800, 2400, 1200,	300,
                        };


/******************************************************************************
 * FunctionName : int  socket_recieved(void* env)
 * Description  :data recieved function after data sended
 * Parameters   : level : output level of pin
 * Returns      : none
*******************************************************************************/


/**
* @brief  set speed of the serial port
* @param  fd [in] file description of the serial port
* @param  speed [in] speed that want to set value in speed array
* @return error code
*/
int set_serial_port_speed(int fd, int speed)
{
    int	 i;
    int	status;
    struct termios   opt;
    tcgetattr(fd, &opt);
    for ( i= 0;  i < sizeof(speed_arr) / sizeof(int);  i++ ) {
        if(speed == name_arr[i]) {
            tcflush(fd, TCIOFLUSH);
            cfsetispeed(&opt, speed_arr[i]);
            cfsetospeed(&opt, speed_arr[i]);
            status = tcsetattr(fd, TCSANOW, &opt);
            if(status != 0)
                return status;
            else
                return 0;
        }
        tcflush(fd,TCIOFLUSH);
    }

    //invalid baud rate
    return -1;
}

/**
* @brief  set serial port control flag
* @param  fd [in] file description of the serial port
* @param  databits [in] data bits, it's value is 7 or 8
* @param  stopbits [in] stop bits, it's value is 1 or 2
* @param  parity   [in] parity char, it's value is onef of 'N','E','O','S'
* @return error code
*/
int set_serial_port_ctrl_flag(int fd,int databits,int stopbits,int parity)
{
    struct termios options;
    int result = tcgetattr(fd,&options);
    if(result != 0) {
        return result;
    }

    /*8N1*/
    options.c_cflag &= ~CSIZE; /* Mask the character size bits */
    switch (databits) {
    case 7:
        options.c_cflag |= CS7;
        break;
    case 8:
        options.c_cflag |= CS8;
        break;
    default:
        assert(-1);
        break;
    }

    switch (parity) {
    case 'n':
    case 'N':
        options.c_cflag &= ~PARENB;   /* Clear parity enable */
        options.c_iflag &= ~INPCK; 	/* Enable parity checking */
        break;
    case 'o':
    case 'O':
        options.c_cflag |= (PARODD | PARENB);	/* Set odd checking*/
        options.c_iflag |= INPCK;			   /* Disnable parity checking */
        break;
    case 'e':
    case 'E':
        options.c_cflag |= PARENB;	   /* Enable parity */
        options.c_cflag &= ~PARODD;   /* Set event checking*/
        options.c_iflag |= INPCK;		/* Disnable parity checking */
        break;
    case 'S':
    case 's':  /*as no parity*/
        options.c_cflag &= ~PARENB;
        options.c_cflag &= ~CSTOPB;
        break;
    default:
        assert(-1);
        break;
    }
	/*in case of getting rid of contrlling flag*/
	options.c_iflag &= ~(ICRNL | IXON);
    //set stop bits
    switch (stopbits) {
    case 1:
        options.c_cflag &= ~CSTOPB;
        break;
    case 2:
        options.c_cflag |= CSTOPB;
        break;
    default:
        assert(-1);
        break;
    }

    /* Set input parity option */
    if (parity != 'n') {
        options.c_iflag |= INPCK;
    }

    options.c_cc[VTIME] = MAX_TIMEOUT; // 15 seconds
    options.c_cc[VMIN] = L_CH_TRIGGER_T;

    options.c_cflag &= ~CRTSCTS;//disable hardware flow control;
    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);/*raw input*/
    options.c_oflag	&= ~OPOST;	 /*raw output*/

    tcflush(fd,TCIFLUSH); /* Update the options and do it NOW */
    result = tcsetattr(fd,TCSANOW,&options);
    if ( result != 0) {
        return result;
    }

    return 0;
}


/**
* @breif open serial port device
* @param dev [in] name of the device
* @return file description of the device
*/
static int open_dev(const char *dev)
{
    int	 fd = open(dev, O_RDWR | O_NOCTTY | O_NDELAY );
    if (fd != -1 ) {
		//int flag = fcntl(fd, F_GETFL);
		//int ret;
		
//        if ((ret = fcntl(fd, F_SETFL, flag|O_NONBLOCK)) < 0){//set to block
        //if ((ret = fcntl(fd, F_SETFL, 0)) < 0){//set to block
        
		//	return ret;
		//} 
        
    }
    return fd;
}

/**
* @breif open serial port
* @param port_num [in] number of the serial port, from 0~255
* @return file description of the serial port
*/
int open_serial_port(int port_num)
{
    assert(port_num > -1 && port_num < 256);

    char dev[MAX_DEV_NAME]= {0};
    // dev/ttyS0 = serial port 0
    sprintf(dev,"/dev/ttyUSB%d",port_num);
    return open_dev(dev);
}

/**
 * @brief write serial port data
 * @param fd [in] file description of serial port
 * @param buffer [in] write data buffer
 * @param size [in] write data buffer size
 * @return write data count,return -1, occur error
 */
int write_serial_port(int fd,unsigned char *buffer, size_t size)
{
    int writecount = write(fd,buffer,size);

    return writecount;
}

/**
* @brief read N data from serial port in time out
* @param	fd [in] file description of the serial port
* @param	buffer [in] read data buffer
* @param	size [in] read data buffer size
* @param	 readcount [in,out] at last read (readcount) size data.
*					set it equal to 0, read random count data.
*					it will return received data count.
*					return -1, the serial port closed
*					return N, count of the serial port received
* @param	 timeout [in] read N count data's time out
* @return	  return 0 : time out\n
*					return -1 : error\n
*					return N: received data count\n
*/
int read_serial_port(int fd, unsigned char *buffer,size_t size, size_t *readcount,
                     struct timeval *timeout)
{


    assert(size >= *readcount);
    fd_set inputs;
    int result;	   //select result
    int nread;			 //totla read count
    int iread;			   //ont time read count
    int maxfd;

    maxfd = fd + 1; //NOTE
    nread = 0 ;


    FD_ZERO(&inputs);
    FD_SET(fd, &inputs);

    do {
        result = select(maxfd, &inputs,(fd_set *)NULL, (fd_set *)NULL, timeout);
        switch(result) {
        case 0: //time out
        case -1:		 //error
            return result;
        default:
            if(FD_ISSET(fd,&inputs)) {
                ioctl(fd,FIONREAD,&iread);
                if(iread == 0) {
                    *readcount = -1; //port closed
                    return 0;
                }
                int count = 0;
                if(*readcount != 0) {
                    //only care buffer size
                    count = MIN(iread, size - nread);
                }

                nread += read(fd, buffer+nread, count);

            } else {
                assert(-1);
            }
        }//end of switch
    } while((*readcount != 0) && (nread < *readcount));

    //receive random data, return data count
    if( 0 == *readcount) {
        *readcount = nread;
    }
    return 0;
}

/*
 * @brief close serial port
 * @param fd [in] file description of the serial port
 */
void close_serial_port(int fd)
{
    close(fd);
}

/**
* @brief  serial init
* @param	none
* @return	  return 0 : time out\n
*					return -1 : error\n
*					return N: file description
*/
int serial_init(int serial_num)
{
    int fd;
    int ret;
    char buffer[100];
    fd = open_serial_port(0);
	
    if(fd < 0) {
        perror("com opened error\n");
        goto FAILED;
    }
    if((ret = set_serial_port_speed(fd, 115200)) < 0) {
        perror("set speed failed\r\n");
        goto FAILED;
    }
    if((ret = set_serial_port_ctrl_flag(fd,8 ,1 ,'N')) < 0) {
        perror("set priority failed\r\n");
        goto FAILED;
    }
	
	return fd;
FAILED:
	close(fd);
    return -1;
}



#define TEST_STRING "HELLO CLING"
void serial_test()
{

	
    int a =20, b, ret, fd;
	unsigned char buffer[100];
	
	if((fd = serial_init(0)) < 0) {
        perror("serial init failed\r\n");
    }

	printf("serial fd = %d \r\n", fd);
    while(1) {
        printf("serial\r\n");
        if ((ret = write_serial_port(fd,TEST_STRING, sizeof(TEST_STRING))) < 0) {
            printf("com error\r\n");
        } else {
            sleep(1);
            printf("number data = %d\r\n", ret);
        }

    }


}
#if 0
void *serial_thread(void *arg)
{
    int fd, nread, count, result ,iread;
	fd_set inputs,outputs;
	struct timeval serial_timeout;
	unsigned char buffer[100];
	
	serial_timeout.tv_sec = 1;
	serial_timeout.tv_usec = 0;
	
	if((fd = serial_init(0)) < 0) {
        perror("serial init failed\r\n");
    }

	FD_ZERO(&inputs);
    FD_SET(fd, &inputs);
	FD_ZERO(&outputs);
    FD_SET(fd, &outputs);
	
    for(;;) {

        result = select(fd + 1, &inputs, NULL , (fd_set *)NULL, &serial_timeout);
        switch(result) {
        case 0: //time out
        	FD_ZERO(&inputs);
    		FD_SET(fd, &inputs);
        	serial_timeout.tv_sec = 10;
			serial_timeout.tv_usec = 0;
        	perror("com select timeout\r\n");
			break;
        case -1:		 //error
            return result;
        default:
            if(FD_ISSET(fd,&inputs)) {
                ioctl(fd, FIONREAD, &iread);
                if(iread == 0) {
					continue;
                }
                nread = read(fd, buffer, sizeof(buffer));
				buffer[nread] = 0;
				printf("%s", buffer);
            }else if(FD_ISSET(fd,&outputs)){/*this meeans serial buffer is writravke*/
				printf("serial writable\r\n");
				if ((write_serial_port(fd,TEST_STRING, sizeof(TEST_STRING))) < 0) {
            		printf("com write error\r\n");
        		}
			}else {
                assert(-1);
            }
        }//end of switch

    }
}
#endif
#ifdef __cplusplus
} /* extern "C" */
#endif

