#ifndef __UART_PROTOCAL_CMD_H__
#define __UART_PROTOCAL_CMD_H__


#include "oop_hal.h"


/*INIDICATE DIFFRENT IN CMD LAYER*/
enum uart_cmd_package_type{
	CMD_LOCATION_TYPE,
	CMD_HEALTH_TYPE	
};

/*********************************************************************
* MACROS
*/
/*to be compatible with old uart protocal*/
#define  UART_OLD_PROTOCAL
#define  CLING_ID_LENTH 4
#define  CLING_MAC_LENTH 6
#define  CLING_RSSI_LENTH 1
#define  CLING_CMD_TYPE_LENTH 1



/*information uploaded*/
struct location_data{
	char cling_id[CLING_ID_LENTH]; /*cling device id*/
	char cling_mac[CLING_MAC_LENTH];/*cling mac adress*/
	char cling_rssi[CLING_RSSI_LENTH];/*cling rssi number*/
};
struct cling_location_rev{
	char package_type;/*indicate current pakage type*/
	struct location_data load;
};


#pragma pack(push) 
#pragma pack(1)	
struct health_data{
	char cling_mac[CLING_MAC_LENTH];/*cling mac adress*/
	unsigned char null_align; /*for align used*/
	unsigned char  date;   /*the time when cling sendout health data */  
	unsigned short int steps;
	unsigned short int distance;
	unsigned short int calories;
	unsigned short int sleep_time;
	unsigned char  heart_rate;
	unsigned char  skin_temp;
};

/*user health information related*/
struct cling_health_rev{
	char package_type;/*indicate current pakage type*/
	struct health_data load;
	//char health_data[sizeof(struct health_data_inf)];
};
#pragma pack(pop) 

struct cling_health_send{
	
};



/*uart protocal cmd layer package*/
struct cling_cmd_rev{
	char package_type;
	char ppackage[1];
};


struct cling_inf_send{
#ifndef UART_OLD_PROTOCAL
	char package_type;/*indicate current pakage type*/
#endif
	char *ptr;
};


/*object prototype declaration*/
DEF_CLASS(cling_uart)
	int (*init)    	(CLASS(cling_uart) *arg); 			/*initiate uart object*/
	int (*de_init) 	(CLASS(cling_uart) *arg);				/*delete uart object*/
	int (*send_data) 		(CLASS(cling_uart) *arg, char *pinf, int lenth);/*data send   function*/
	int (*send_cmd) 		(CLASS(cling_uart) *arg, char *pinf, int lenth);/*data send   function*/
	int (*enable_recieving) (CLASS(cling_uart) *arg);
	int (*disable_recieving) (CLASS(cling_uart) *arg);
	int (*task_register) 	(CLASS(cling_uart) *arg, unsigned short int task_id);			/*register  object*/
	int (*recv_callback_register) 	(CLASS(cling_uart) *arg, void (*callback)(void *pbuffer));/*register  object*/
	void *user_data;/*point to user private data*/
END_DEF_CLASS(cling_uart)

int  cling_u_data_send(char *pinf, int lenth);

#endif

