#ifndef _SERIAL_H_
#define _SERIAL_H_

int  set_serial_port_speed(int fd, int speed);
int  set_serial_port_ctrl_flag(int fd,int databits,int stopbits,int parity);
int  open_serial_port(int port_num);
int  write_serial_port(int fd,unsigned char *buffer, size_t size);
int  read_serial_port(int fd, unsigned char *buffer,size_t size, size_t *readcount, struct timeval *timeout);
void close_serial_port(int fd);
#endif


