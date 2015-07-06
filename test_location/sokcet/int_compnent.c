/******************************************************************************
 * Copyright 2013-2014 hicling Systems (MikeWang)
 *
 * FileName: json_test.c
 *
 * Description: The web server mode configration.
 *              Check your hardware connection with the host while use this mode.
 * Modification history:
 *     2015/6/30, v1.0 create this file.
*******************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include "int_compnent.h"
 
/*********************************************************************
* MACROS
*/

#define ETH_NAME "eth0"

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
 * Description	:data recieved function after data sended
 * Parameters	: level : output level of pin
 * Returns		: 0: init successfully
 *				 -1: init failed
 *
*******************************************************************************/
int get_mac(CLASS(int_comp) *arg, char *buffer, char *output_lenth) /*initiate http object*/
{
    struct sockaddr_in *addr;
    struct ifreq ifr;
    char *name,*address;
    int sockfd;
    int i;

	name = ETH_NAME;

    sockfd = socket(AF_INET,SOCK_DGRAM,0);

    strncpy(ifr.ifr_name,name,IFNAMSIZ-1);
#if 0
    if(ioctl(sockfd,SIOCGIFADDR,&ifr) == -1)
        perror("ioctl error"),exit(1);
    addr = (struct sockaddr_in *)&(ifr.ifr_addr);
    address = inet_ntoa(addr->sin_addr);
    printf("inet addr: %s ",address);

    if(ioctl(sockfd,SIOCGIFBRDADDR,&ifr) == -1)
        perror("ioctl error"),exit(1);
    addr = (struct sockaddr_in *)&ifr.ifr_broadaddr;
    address = inet_ntoa(addr->sin_addr);
    printf("broad addr: %s ",address);

    if(ioctl(sockfd,SIOCGIFNETMASK,&ifr) == -1)
        perror("ioctl error"),exit(1);
    addr = (struct sockaddr_in *)&ifr.ifr_addr;
    address = inet_ntoa(addr->sin_addr);
    printf("inet mask: %s ",address);
    printf("\n");
#endif
    if(ioctl(sockfd, SIOCGIFHWADDR, &ifr) == -1){
        perror("ioctl error");
			return -1;
    }
    unsigned char * mac = (unsigned char *)ifr.ifr_hwaddr.sa_data;
	sprintf(buffer,"%02x:%02x:%02x:%02x:%02x:%02x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5], mac[6]);
	printf("mac adress = %s\n", buffer);	
	close(sockfd);
    return 0;
}
/******************************************************************************
 * FunctionName : get_ip
 * Description	: get ip of current machine
 * Parameters	: CLASS(int_comp) *arg, char *buffer, char *output_lenth
 * Returns		: 0: init successfully
 *				 -1: init failed
 *
*******************************************************************************/
int get_ip(CLASS(int_comp) *arg, char *buffer, char *output_lenth) /*initiate http object*/
{
    struct sockaddr_in *addr;
    struct ifreq ifr;
    char *name,*address;
    int sockfd;
    int i;

	name = ETH_NAME;

    sockfd = socket(AF_INET,SOCK_DGRAM,0);

    strncpy(ifr.ifr_name,name,IFNAMSIZ-1);
    if(ioctl(sockfd,SIOCGIFADDR,&ifr) == -1)
        perror("ioctl error"),exit(1);
    addr = (struct sockaddr_in *)&(ifr.ifr_addr);
    address = inet_ntoa(addr->sin_addr);
    printf("inet addr: %s ",address);

    unsigned char * mac = (unsigned char *)ifr.ifr_hwaddr.sa_data;
    for(i=0; i<6; i++)
        printf("%02x:",mac[i]);
    printf("\n");
	close(sockfd);
    return 0;

}



/******************************************************************************
 * FunctionName : int  socket_recieved(void* env)
 * Description  :data recieved function after data sended
 * Parameters   : level : output level of pin
 * Returns      : 0: init successfully
 *				 -1: init failed
 *
*******************************************************************************/
int init_int_comp(CLASS(int_comp) *arg) /*initiate http object*/
{
	assert(arg != NULL);
	arg->get_mac = get_mac;
	arg->get_ip =  get_ip;

}




