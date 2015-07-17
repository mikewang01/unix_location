/******************************************************************************
 * Copyright 2013-2014 Espressif Systems (Wuxi)
 *
 * FileName: user_json.c
 *
 * Description: JSON format set up and parse.
 *              Check your hardware transmation while use this data format.
 *
 * Modification history:
 *     2014/5/09, v1.0 create this file.
*******************************************************************************/
#include "uart_protocol_cmd.h"
#include "uart_protocol_mac.h"
#include "stdio.h"
#include "assert.h"
#include "serial_port.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h> 
#include <errno.h>



#define PACKAGE_HEADER_SYMBOL    0X7D
#define PACKAGE_TRANSFER_SYMBOL  0X7F
#define PACKAGE_END_SYMBOL		 0X7E

#define DATA_0X7D_TRANSFERRED	 0X01
#define DATA_0X7E_TRANSFERRED	 0X02
#define DATA_0X7F_TRANSFERRED	 0X03

/*SEMAPHORE RELATED*/
#define SEMAPHORE 	 int
#define LOCKED		 1
#define UNLOCKED       0
#define IS_SEMAPHORE_LOCKED(__x) (__x == LOCKED)
#define LOCK_SEMAPHORE(__X) 	 (__X =  LOCKED)
#define UNLOCK_SEMAPHORE(__x)	 (__x = UNLOCKED)

#if 0
#define  MAC_SEND_BUFFER(__X, __Y) do{\
	if(reg_serial_fd > 0){\
		if(write(reg_serial_fd, __X,__Y) < 0);\
			printf("serial write error numbeer =%d\n",errno);\
	}\
}while(0)
#endif

#define  MAC_SEND_BUFFER(__X, __Y) do{\
	if(mac_send_data != NULL){\
		mac_send_data(__X, __Y);\
	}\
}while(0)

#define  ENTER_CRITICAL_SECTION() 
#define  LEAVE_CRITICAL_SECTION() 

/*THE UPPER LIMIT OF TIMES THE DATA BEEN SENDED*/

#define  RESEND_TIMES_UPPER_THREADHOLD  5
/*********************************************************************
* TYPEDEFS
*/
enum package_send_state {
    PACKAGE_WAITING_FOR_SENDDING = 0,
    PACKAGE_WAITING_FOR_ACK,
    PACKAGE_SENDDED_SUCCESSFULLY

};

/*used to implement state machine*/
enum package_state {
    PACKAGE_BEGIN = 0,
    PACKAGE_TRANSFER,
    PACKAGE_PAYLOAD,
    PACKAGE_END,
    PACKAGE_NULL
};

enum package_type {
    PACKAGE_ACK = 0,
    PACKAGE_DATA,
    PACKAGE_CMD

};
/*********************************************************************
* GLOBAL VARIABLES
*/
#define MESSAGE_MAX_CHAIN_LENTH 	8
#define CLING_TX_UART_FIFO_POLL_PERIOAD 100

/*********************************************************************
* static VARIABLES
*/

static int reg_serial_fd = 0;
/*send data call back function*/
static int  (*mac_send_data)(char*, unsigned int) = NULL;

static RcvMsgBuff serial_buff;

/*recieve fifo related pointer*/
static enum package_state current_state = PACKAGE_NULL;
static struct mac_layer_payload_rev *rev_payload_list_header = NULL;
static struct mac_layer_payload_rev *rev_payload_list_tail = NULL;

static void (*cmd_recieved_callback)(char cmd) = NULL;

static SEMAPHORE rev_fifo_semphore = UNLOCKED;


/**/
static struct mac_layer_payload_send *send_package_list_header = NULL;
static struct mac_layer_payload_send *send_package_list_tail = NULL;
static SEMAPHORE send_fifo_semphore = UNLOCKED;

//static os_timer_t  uart_send_timer;
static int macsend_ack_recieved();
static int send_package_assemble(struct mac_layer_payload_send *payload_temp, enum package_type type);
static int add_payload2revlist(unsigned char *pbuffer, int size);
void received_data_process(RcvMsgBuff *para);


/******************************************************************************
 * FunctionName : receive_one_char_callback
 * Description  : call back function when received one char
 * Parameters   : unsigned char rev_char
 * Returns      : none
*******************************************************************************/
void receive_one_char_callback(unsigned char rev_char)
{

	RcvMsgBuff *para = &serial_buff;
	//printf("\r\ndata=");
	
	  //printf(":%02x",rev_char);
#if 0
    MAC_SEND_BUFFER(&rev_char, 1);
#endif
    /*this is a state machien from now on*/
    switch (current_state) {
    case PACKAGE_END:
    case PACKAGE_NULL: {
        if (rev_char == PACKAGE_HEADER_SYMBOL) {
            /*change current state machine*/
            current_state = PACKAGE_BEGIN;
            /*reset message buffer write pointer*/
            para->pWritePos = para->pRcvMsgBuff;
			para->pReadPos = para->pRcvMsgBuff;
            para->BuffState = EMPTY;
        }
    }
    break;

    case PACKAGE_BEGIN: {
        if (rev_char == PACKAGE_HEADER_SYMBOL) {
            /*change current state machine*/
            current_state = PACKAGE_BEGIN;
            /*reset message buffer write pointer*/
            para->pWritePos = para->pRcvMsgBuff;
	 		para->pReadPos = para->pRcvMsgBuff;
            para->BuffState = EMPTY;

        } else if (rev_char == PACKAGE_END_SYMBOL) {
            /*change current state machine*/
            current_state = PACKAGE_END;

        } else if (rev_char == PACKAGE_TRANSFER_SYMBOL) { /*data need transferred*/

            /*change current state machine*/
            current_state = PACKAGE_TRANSFER;
        } else { /*normal data*/

            para->BuffState = UNDER_WRITE;
            /*change current state machine*/
            current_state = PACKAGE_PAYLOAD;
            *(para->pWritePos) = rev_char;
            para->pWritePos++;
        }

    }
    break;

    case PACKAGE_TRANSFER: {
        if (rev_char == PACKAGE_HEADER_SYMBOL) {
            /*change current state machine*/
            current_state = PACKAGE_BEGIN;
            /*reset message buffer write pointer*/
            para->pWritePos = para->pRcvMsgBuff;			
			para->pReadPos = para->pRcvMsgBuff;
            para->BuffState = EMPTY;

        } else if (rev_char == PACKAGE_END_SYMBOL) {
            /*change current state machine*/
            current_state = PACKAGE_END;

        } else if (rev_char == PACKAGE_TRANSFER_SYMBOL) { /*data need transferred*/

            /*change current state machine abandon cuurent pakage*/
            current_state = PACKAGE_END;
        } else { /*normal data*/
            /*this means transferred data recieved*/
            if(rev_char == DATA_0X7D_TRANSFERRED || rev_char == DATA_0X7E_TRANSFERRED || rev_char == DATA_0X7F_TRANSFERRED) {
                para->BuffState = UNDER_WRITE;
                /*change current state machine*/
                current_state = PACKAGE_PAYLOAD;
                *(para->pWritePos) = 0x7d + DATA_0X7D_TRANSFERRED -1;
                para->pWritePos++;

            } else {
                /*change current state machine abandon current pakage*/
                current_state = PACKAGE_END;

            }
        }


    }
    break;

    case PACKAGE_PAYLOAD: {
        if (rev_char == PACKAGE_HEADER_SYMBOL) {
            /*change current state machine*/
            current_state = PACKAGE_BEGIN;
            /*reset message buffer write pointer*/
            para->pWritePos = para->pRcvMsgBuff;			
			para->pReadPos = para->pRcvMsgBuff;
            para->BuffState = EMPTY;

        } else if (rev_char == PACKAGE_END_SYMBOL) {


            para-> BuffState = WRITE_OVER;
            received_data_process(para);
            /*change current state machine*/
            current_state = PACKAGE_END;

        } else if (rev_char == PACKAGE_TRANSFER_SYMBOL) { /*data need transferred*/

            /*change current state machine abandon cuurent pakage*/
            current_state = PACKAGE_TRANSFER;
        } else { /*normal data*/
            /*this means transferred data recieved*/

            *(para->pWritePos) = rev_char;
            para->pWritePos++;

        }



    }
    break;

    default :
        break;
    }
#if 0
    MAC_SEND_BUFFER((u8*)&current_state, 1);
#endif
}
/******************************************************************************
 * FunctionName : receive_one_char_callback
 * Description  : call back function when received one char
 * Parameters   : unsigned char rev_char
 * Returns      : none
*******************************************************************************/
void set_recieved_cmd_call_back(void (*callback)(char))
{

   cmd_recieved_callback = callback;

}

/******************************************************************************
 * FunctionName : receive_one_char_callback
 * Description  : call back function when received one char
 * Parameters   : unsigned char rev_char
 * Returns      : none
*******************************************************************************/

void received_data_process(RcvMsgBuff *para)
{

    /*this means this is a ack package*/
    if (para->pRcvMsgBuff[0] == PACKAGE_ACK) {
        macsend_ack_recieved();
		printf("ack recieved\n");
    } else if (para->pRcvMsgBuff[0] == PACKAGE_DATA) {
        /*add messge to reveived messge list waiting for processing*/
        add_payload2revlist(para->pRcvMsgBuff + 1, (para->pWritePos - para->pRcvMsgBuff - 1));
        /*after receieving ,ssend back ack package*/
        send_package_assemble(NULL, PACKAGE_ACK);
    }else if (para->pRcvMsgBuff[0] == PACKAGE_CMD){

		
    	if (cmd_recieved_callback != NULL) {
			cmd_recieved_callback(para->pRcvMsgBuff[1]);
		}
		/*for the real time purpose, call back function is prefered*/
		/*after receieving ,ssend back ack package*/
        send_package_assemble(NULL, PACKAGE_ACK);
	}

}

/******************************************************************************
 * FunctionName : add_payload2list
 * Description  : internally usedly add a data struct to the chain list
 * Parameters   : unsigned char rev_char
 * Returns      : none
*******************************************************************************/
int	cling_data_mac_pocess(void * buffer, int lenth);
static int add_payload2revlist(unsigned char *pbuffer, int size)
{

	//printf("add_payload2revlist\n");
    /*if this lock is occupied by other process then return*/
    if (IS_SEMAPHORE_LOCKED(rev_fifo_semphore)) {
        return -1;
    }
#if 0
	printf("\r\ndata=");
	for(int i =0;i< size ;i++){
	  printf(":%02x",pbuffer[i]);
	}
#endif

    /*enter critical section so that this is a atom acess*/
    LOCK_SEMAPHORE(rev_fifo_semphore);
	cling_data_mac_pocess(pbuffer, size);
	
SUCESS:
    /*leave criticla section ,release semphore here*/
    UNLOCK_SEMAPHORE(rev_fifo_semphore);
    return 0;
FAILED:
    /*leave criticla section ,release semphore here*/
    UNLOCK_SEMAPHORE(rev_fifo_semphore);
    return -1;
}

/******************************************************************************
 * FunctionName : obbtain_payload_from_revlist
 * Description  : internally used ,obbtain a data struct to the chain list
 * Parameters   : struct mac_layer_payload_rev *ppayload: mac layer layload pointer
 * Returns      : none
*******************************************************************************/
int obtain_payload_from_revlist(struct mac_layer_payload_rev **ppayload)
{

    /*if this lock is occupied by other process then return*/
    if (IS_SEMAPHORE_LOCKED(rev_fifo_semphore)) {
        return -1;
    }

    /*enter critical section to lock semphore so to make sure this is a atom acess*/
    LOCK_SEMAPHORE(rev_fifo_semphore);

    struct mac_layer_payload_rev *ptemp = rev_payload_list_header;
    /*initialte parameter passed to this function*/
    *ppayload = NULL;
    /*if no data existed*/
    if (ptemp == NULL) {

        goto FAILED;

    } else {
        /*there is only one data struct exsited in fifo*/
        if (ptemp == rev_payload_list_tail) {
            *ppayload = ptemp;
            rev_payload_list_header = NULL;
            rev_payload_list_tail = NULL;
            goto SUCESS;
        }
        /*more than two data block existed, SO first step is to find the the last data block*/
        for (; ptemp->next != rev_payload_list_tail; ptemp = ptemp->next);
        /*assign the last */
        *ppayload = rev_payload_list_tail;
        rev_payload_list_tail = ptemp;
        ptemp->next = NULL;
    }
SUCESS:
    /*leave criticla section ,release semphore here*/
    UNLOCK_SEMAPHORE(rev_fifo_semphore);
    return 0;
FAILED:
    /*leave criticla section ,release semphore here*/
    UNLOCK_SEMAPHORE(rev_fifo_semphore);
    return -1;
}

/******************************************************************************
 * FunctionName : obbtain_payload_from_revlist
 * Description  : internally used ,obbtain a data struct to the chain list
 * Parameters   : struct mac_layer_payload_rev *ppayload: mac layer layload pointer
 * Returns      : none
*******************************************************************************/
int mac_send_payload(char *ppayload, int lenth)
{

	//printf("==============ble fota============%d\n", lenth);
    /*if this lock is occupied by other process then return*/
    if (IS_SEMAPHORE_LOCKED(send_fifo_semphore)) {
        return -1;
    }
    /*enter critical section to lock semphore so to make sure this is a atom acess*/
    LOCK_SEMAPHORE(send_fifo_semphore);
	/*check  heap is enough or not*/
	
    struct mac_layer_payload_send *payload_temp = (struct mac_layer_payload_send*)malloc(sizeof(struct mac_layer_payload_send));
    if	(NULL == payload_temp) {
        goto FAILED;
    }
    /*RESET RESEND TIMES*/
    payload_temp->resend_times = 0;

#if 1
    payload_temp->ppayload = (char*)malloc(lenth);
    if	(NULL == payload_temp->ppayload) {
        goto FAILED;
    }
#endif
	/*copy data from buffer*/
	memcpy(payload_temp->ppayload, ppayload, lenth);
    payload_temp->state = PACKAGE_WAITING_FOR_SENDDING;
    payload_temp->lenth = lenth;
    payload_temp->next = NULL;

    /**/
    if (NULL == send_package_list_header) {
		
        send_package_list_header = payload_temp;
		/*if there is no more data listed in chain send this immmidiately*/
		payload_temp->state = PACKAGE_WAITING_FOR_ACK;/*this must be set v=before being sended*/
	 	send_package_assemble(send_package_list_header, PACKAGE_DATA);
		
		/*restart flag updating progress*/
		//os_timer_disarm(&(uart_send_timer));
		//os_timer_setfn(&(uart_send_timer), (os_timer_func_t *)mac_sendlist_mantain_demon, 0);
		//os_timer_arm(&(uart_send_timer), CLING_TX_UART_FIFO_POLL_PERIOAD, 0);
		
    } else {
 
        send_package_list_tail->next = payload_temp;
    }
    /*rewind the pointer to the end of chain*/
    send_package_list_tail = payload_temp;

SUCESS:
    /*leave criticla section ,release semphore here*/
    UNLOCK_SEMAPHORE(send_fifo_semphore);
    return 0;
FAILED:
    /*leave criticla section ,release semphore here*/
    UNLOCK_SEMAPHORE(send_fifo_semphore);
    return -1;


}

/******************************************************************************
 * FunctionName : obbtain_payload_from_revlist
 * Description  : internally used ,obbtain a data struct to the chain list
 * Parameters   : struct mac_layer_payload_rev *ppayload: mac layer layload pointer
 * Returns      : none
*******************************************************************************/
int mac_sendlist_mantain_demon()
{

    struct mac_layer_payload_send *payload_temp = send_package_list_header;
    /*if this lock is occupied by other process then return*/
    if (IS_SEMAPHORE_LOCKED(send_fifo_semphore)) {
        return -1;
    }
    /*enter critical section to lock semphore so to make sure this is a atom acess*/
    LOCK_SEMAPHORE(send_fifo_semphore);
    /**/
    if (NULL == payload_temp) {
		/*if the fifo ha already been empty,then it is not gonna rearm the timer again*/
        goto SUCESS;

    } else {
        /*current data need to be transmitted*/
        if (payload_temp->state == PACKAGE_WAITING_FOR_SENDDING) {

            send_package_assemble(payload_temp, PACKAGE_DATA);
            payload_temp->resend_times = 0;
            /*send data over here*/
            payload_temp->state = PACKAGE_WAITING_FOR_ACK;

        } else if (payload_temp->state == PACKAGE_WAITING_FOR_ACK) {

            if (payload_temp->resend_times < RESEND_TIMES_UPPER_THREADHOLD) {
                send_package_assemble(payload_temp, PACKAGE_DATA);
               // MAC_SEND_BUFFER(payload_temp->, position);
                payload_temp->resend_times ++;
            } else {

				printf("send time up\n");
                /*remove the packege sended successfully*/
                send_package_list_header = send_package_list_header->next;
                /*free payload buffer*/
                free(payload_temp->ppayload);
                free(payload_temp); 
				/*once abandon one package send nex package*/
				payload_temp = send_package_list_header;
				if (payload_temp != NULL) {
					send_package_assemble(payload_temp, PACKAGE_DATA);
				}

            }
            /*timeout process*/

        } else if (payload_temp->state == PACKAGE_SENDDED_SUCCESSFULLY) {
            /*remove the packege sended successfully*/
            send_package_list_header = send_package_list_header->next;
            /*free payload buffer*/
            free(payload_temp->ppayload);
            free(payload_temp);
			/*once recieved ack send nex package*/
			payload_temp = send_package_list_header;
			if (payload_temp != NULL) {
				send_package_assemble(payload_temp, PACKAGE_DATA);
			}
			 
			
        }			
			/*arm the check routine period again*/
			//os_timer_arm(&(uart_send_timer), CLING_TX_UART_FIFO_POLL_PERIOAD, 0);
       }

SUCESS:
    /*leave criticla section ,release semphore here*/
    UNLOCK_SEMAPHORE(send_fifo_semphore);
    return 0;
FAILED:
    /*leave criticla section ,release semphore here*/
    UNLOCK_SEMAPHORE(send_fifo_semphore);
    return -1;
}

/******************************************************************************
 * FunctionName : macsend_ack_recieved()
 * Description  : internally used when a ack package is recieved
 * Parameters   : struct mac_layer_payload_rev *ppayload: mac layer layload pointer
 * Returns      : none
*******************************************************************************/
static int macsend_ack_recieved()
{
    struct mac_layer_payload_send *payload_temp = send_package_list_header;
    /**/
    if (NULL == payload_temp) {
        return 0;

    } else {

        if (payload_temp->state == PACKAGE_WAITING_FOR_ACK) {
            /*clent has revieced the package successfully */
            payload_temp->state = PACKAGE_SENDDED_SUCCESSFULLY;
			mac_sendlist_mantain_demon();
        }
    }


    return 0;


}

/******************************************************************************
 * FunctionName : macsend_ack_recieved()
 * Description  : internally used ,obbtain a data struct to the chain list
 * Parameters   : struct mac_layer_payload_rev *ppayload: mac layer layload pointer
 * Returns      : none
*******************************************************************************/
static int send_package_assemble(struct mac_layer_payload_send *payload_temp, enum package_type type)
{

    unsigned short int i = 0;
    if (type == PACKAGE_ACK) {
        char ack_buffer[] = {PACKAGE_HEADER_SYMBOL, PACKAGE_ACK, PACKAGE_END_SYMBOL};
        /*send ddta to client*/
		
        MAC_SEND_BUFFER(ack_buffer, sizeof(ack_buffer));
    } else if (type == PACKAGE_DATA || type == PACKAGE_CMD) {
        char data_buffer[2048];
        unsigned short int position = 0;

        data_buffer[position] = PACKAGE_HEADER_SYMBOL;
        position++;
        data_buffer[position] = type;
        position ++;
        /*process the data transferred by uart*/
        for(i = 0; i < (payload_temp->lenth); i++) {
            /*Sspecail data*/
            if((payload_temp->ppayload[i]) == PACKAGE_HEADER_SYMBOL || (payload_temp->ppayload[i]) == PACKAGE_END_SYMBOL || (payload_temp->ppayload[i]) == PACKAGE_TRANSFER_SYMBOL) {

                data_buffer[position] = PACKAGE_TRANSFER_SYMBOL;
                position++;
                data_buffer[position] = (payload_temp->ppayload[i] - PACKAGE_HEADER_SYMBOL + 1);
                position++;
            } else {

                data_buffer[position] = (payload_temp->ppayload[i]);
                position++;
            }

        }
        /*full fill end chractor*/
        data_buffer[position] = PACKAGE_END_SYMBOL;
        position++;

        MAC_SEND_BUFFER(data_buffer, position);
    }


    return 0;


}
/******************************************************************************
 * FunctionName : init_uart_implement
 * Description	: implement uart mac layer interface
 * Parameters	: none
 * Returns		: none
*******************************************************************************/

int delete_init_uart_implement(CLASS(uart_implement) *arg)
{

    assert(NULL != arg);
	free(arg);
}

/******************************************************************************
 * FunctionName : init_uart_implement
 * Description	: implement uart mac layer interface
 * Parameters	: none
 * Returns		: none
*******************************************************************************/

int init_uart_implement(CLASS(uart_implement) *arg)
{


    assert(NULL != arg);
    /*initiate cilng command receive object*/
    arg->init    = init_uart_implement;
    arg->de_init = delete_init_uart_implement;
    arg->send_data = mac_send_payload;
    arg->get_data = NULL;
	//uart_init(BIT_RATE_115200, BIT_RATE_115200);
	return 0;

}

/******************************************************************************
 * FunctionName :register_serial_fd
 * Description  : regiter opened serial file description
 * Parameters   : struct mac_layer_payload_rev *ppayload: mac layer layload pointer
 * Returns      : none
*******************************************************************************/
int register_serial_fd(int fd)
{
   reg_serial_fd = fd;
	return 0;
}
/******************************************************************************
 * FunctionName :register_write_function
 * Description  : regiter serial write callback
 * Parameters   : write: write call back function
 * Returns      : 0: sucess
 *				 -1: error
*******************************************************************************/
int register_write_function(int (*write)(char* , unsigned int ))
{
	
	mac_send_data = write;
	return 0;
}


