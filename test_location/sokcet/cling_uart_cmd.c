/******************************************************************************
 * Copyright 2013-2014 Espressif Systems (Wuxi)
 *
 * FileName: user_main.c
 *
 * Description: entry file of user application
 *
 * Modification history:
 *     2014/1/1, v1.0 create this file.
*******************************************************************************/
#include "uart_protocol_cmd.h"
#include "uart_protocol_mac.h"
#include "stdio.h"
#include "assert.h"
/*********************************************************************
* MACROS
*/
#define  CLING_CLING_UART_FIFO_POLL_PERIOAD 50
/*********************************************************************
* TYPEDEFS
*/

struct cling_uart_private_data {
    unsigned char task_id_reg;
};

/*********************************************************************
* GLOBAL VARIABLES
*/
#define  IPC_SEND(__X,__Y) write(ipc_fd, __X, __Y)


/*********************************************************************
* static VARIABLES
*/
CLASS(cling_uart) *this = NULL;


/*********************************************************************
* EXTERNAL VARIABLES
*/


/*********************************************************************
* FUNCTIONS
*/
static int ipc_fd = 0;
static int delete_cling_uart(CLASS(cling_uart) *arg);
static int cling_data_recieved_poll(CLASS(cling_uart) *arg);
static int cling_uart_taskid_register(CLASS(cling_uart) *arg, unsigned int short task_id);
static int cling_data_send(CLASS(cling_uart) *arg, char *pinf, int lenth);
static int enable_recieving(CLASS(cling_uart) *arg);
static int disable_recieving(CLASS(cling_uart) *arg);
static void cling_cmd_rev_callback(char cmd);

/******************************************************************************
 * FunctionName : init_cling_uart
 * Description	: uart cling message receive object init
 * Parameters	: none
 * Returns		: none
*******************************************************************************/

int  init_cling_uart(CLASS(cling_uart) *arg)
{


    assert(NULL != arg);
    if (this == NULL) {
#if 0
        struct cling_uart_private_data *private_data = (struct cling_uart_private_data*)os_malloc(sizeof(struct cling_uart_private_data));
        assert(NULL != private_data);
        /*ini tprivate data*/
        private_data->task_id_reg = USER_TASK_PRIO_MAX + 1;
        arg->user_data = private_data;

        /*initiate cilng command receive object*/
        arg->init    = init_cling_uart;
        arg->de_init = delete_cling_uart;
        arg->task_register = cling_uart_taskid_register;

        arg->send_data = cling_data_send;

        arg->enable_recieving = enable_recieving;
        arg->disable_recieving = disable_recieving;

        uart_init(BIT_RATE_115200, BIT_RATE_115200);

        set_recieved_cmd_call_back(cling_cmd_rev_callback);
#endif
        this = arg;
    } else {

    }
    return 0;
}



/******************************************************************************
 * FunctionName : enable_recieving
 * Description  : enable receving
 * Parameters   : arg -- object pointer

 * Returns      : none
*******************************************************************************/

#if 1
static int  enable_recieving(CLASS(cling_uart) *arg)
{
    /*check object parameter*/
    assert(NULL != arg);
    /*enable receive interrupt enable*/
    //ETS_UART_INTR_ENABLE();

    return 0;

}
#endif
/******************************************************************************
 * FunctionName : disable_recieving
 * Description  : dsible uart receving
 * Parameters   : arg -- object pointer

 * Returns      : none
*******************************************************************************/

#if 1
static int
disable_recieving(CLASS(cling_uart) *arg)
{
    /*check object parameter*/
    assert(NULL != arg);
    /*enable receive interrupt enable*/

    return 0;

}
#endif


/******************************************************************************
 * FunctionName : delete_uart
 * Description  : internal used to delete uart device
 * Parameters   : arg -- object pointer

 * Returns      : none
*******************************************************************************/

#if 1
static int
delete_cling_uart(CLASS(cling_uart) *arg)
{
    /*check object parameter*/
    assert(NULL != arg);


    /*malloc corresponed dparameter buffer*/
    struct cling_uart_private_data *private_data = (struct cling_uart_private_data*)(arg->user_data);
    this = NULL;
    return 0;

}
#endif


/******************************************************************************
 * FunctionName : uart_taskid_register
 * Description  : internal used to register specific taskdi passed over here
 * Parameters   : arg -- object pointer
 *				  task_id -- task-id who gonna recieve message
 * Returns      : none
*******************************************************************************/

#if 1
static int cling_uart_taskid_register(CLASS(cling_uart) *arg, unsigned int short task_id)
{
    /*check object parameter*/
    assert(NULL != arg);

    /*malloc corresponed dparameter buffer*/
    struct cling_uart_private_data *callback_data = (struct cling_uart_private_data*)(arg->user_data);

    assert(NULL != callback_data);

    return 0;


}
#endif

/******************************************************************************
 * FunctionName : uart_taskid_register
 * Description  : internal used to register specific taskdi passed over here
 * Parameters   : arg -- object pointer
 *				  task_id -- task-id who gonna recieve message
 * Returns      : none
*******************************************************************************/

#if 1
int cling_uart_ipc_fd_register(unsigned int  fd)
{
    /*check object parameter*/
    if(fd > 0) {
        ipc_fd =fd;
    }

    return 0;


}
#endif


/******************************************************************************
 * FunctionName : cling_cmd_rev_callback
 * Description  : call funcion is prefered for real time purposre
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
static void  cling_cmd_rev_callback(char cmd)
{
    /*malloc corresponed dparameter buffer*/
    struct cling_uart_private_data *private_data = (struct cling_uart_private_data*)(this->user_data);

}

/******************************************************************************
 * FunctionName : cling_data_recieved_poll
 * Description  : uart received poll implementation to get cling imformation
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
#if 1

static int  cling_data_recieved_poll(CLASS(cling_uart) *arg)
{

}

#endif

/******************************************************************************
 * FunctionName : cling_data_recieved_poll
 * Description  : uart received poll implementation to get cling imformation
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
#if 1
int  cling_data_mac_pocess(void * buffer, int lenth)
{

    struct cling_location_rev 	 inf_l;
	struct cling_health_rev  inf_h;

	
    if (lenth > 0) {
        printf("uart cmd layer recieved%d\n", lenth);
        if (lenth == sizeof(inf_l.load)) {
            printf("location recieved%d\n", lenth);
			inf_l.package_type = CMD_LOCATION_TYPE;
			(inf_l.load) = *((struct location_data*) buffer);
			printf("location data lenth = %d\n", sizeof(inf_l.load));				
            /*post cling device information from ble device to task registered*/
            if (ipc_fd > 0) {
				IPC_SEND(&inf_l, sizeof(inf_l));
                /*buffer gonna be freed in user task*/

            } else {
                /*release cmd data ,because there is no task  gonna accept and release this message*/
  
            }

        } else if (lenth == sizeof(inf_h.load)) {
			
			printf("health data lenth = %d\n", sizeof(inf_h.load));
			inf_h.package_type = CMD_HEALTH_TYPE;
            //CLING_DEBUG("cling_health_rev recieved\n");
            (inf_h.load) =  *((struct health_data*)buffer);
            /*post cling device information from ble device to task registered*/
            if (ipc_fd > 0) {
				printf("IPC SENDED!!!!!!\N");
                /*buffer gonna be freed in user task*/
				IPC_SEND(&inf_h, sizeof(inf_h));

            } else {
                /*release cmd data ,because there is no task  gonna accept and release this message*/
            }
        }


    }
    /*release data object transmitted from mac layer*/
}

#endif


/******************************************************************************
 * FunctionName : cling_data_send
 * Description  : uart received poll implementation to get cling imformation
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
#if 0

static int  cling_data_send(CLASS(cling_uart) *arg, char *pinf, int lenth)
{

    struct mac_layer_payload_rev *i = NULL;
    /*malloc corresponed dparameter buffer*/
    struct cling_uart_private_data *private_data = (struct cling_uart_private_data*)(arg->user_data);
    assert(private_data);

    mac_send_payload((char*)pinf, lenth);

}
#endif
/******************************************************************************
 * FunctionName : cling_data_send
 * Description  : uart received poll implementation to get cling imformation
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
#if 1

int  cling_u_data_send(char *pinf, int lenth)
{

   
    mac_send_payload((char*)pinf, lenth);

}
#endif


/******************************************************************************
 * FunctionName : cling_data_send
 * Description  : uart received poll implementation to get cling imformation
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
#if 1

void  cling_data_init(void)
{

}
#endif


