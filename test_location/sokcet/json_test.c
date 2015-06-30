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
#include<netdb.h>
#include<errno.h>
#include<sys/socket.h>
#include <sys/ipc.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include<fcntl.h>
#include<thread_db.h>
#include "internet.h"
#include "json_test.h"
/*********************************************************************
* MACROS
*/



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
 * Description  :data recieved function after data sended
 * Parameters   : level : output level of pin
 * Returns      : 0: init successfully
 *				 -1: init failed
 *				
*******************************************************************************/
int init_json_interface (CLASS(json_interface) *arg) /*initiate http object*/
{
	

}




