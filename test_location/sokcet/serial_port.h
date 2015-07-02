#ifndef _SERIAL_H_
#define _SERIAL_H_


typedef enum {
    EMPTY,
    UNDER_WRITE,
    WRITE_OVER
} RcvMsgBuffState;

typedef struct {
    unsigned int   RcvBuffSize;
    unsigned char     pRcvMsgBuff[1024];
    unsigned char     *pWritePos;
    unsigned char     *pReadPos;
    unsigned char      TrigLvl; //JLU: may need to pad
    RcvMsgBuffState  BuffState;
} RcvMsgBuff;


int  set_serial_port_speed(int fd, int speed);
int  set_serial_port_ctrl_flag(int fd,int databits,int stopbits,int parity);
int  open_serial_port(int port_num);
int  write_serial_port(int fd,unsigned char *buffer, size_t size);
int  read_serial_port(int fd, unsigned char *buffer,size_t size, size_t *readcount, struct timeval *timeout);
void close_serial_port(int fd);
#endif


