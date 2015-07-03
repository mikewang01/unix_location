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
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <assert.h>
#include <json.h>
#include <json_tokener.h>

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
	
	json_tokener *tok;
	json_object *my_string, *my_int, *my_object, *my_array;
	json_object *new_obj;
	int i;
	 MC_SET_DEBUG(1);
	my_string = json_object_new_string("mike");
	printf("my_string=%s\n", json_object_get_string(my_string));
	printf("my_string.to_string()=%s\n", json_object_to_json_string(my_string));
	//json_object_put(my_string);

	my_object =  json_object_new_object();
  	json_object_object_add(my_object, "name", my_string);
	json_object_object_add(my_object, "age", json_object_new_int(32));
	printf("my_object.to_string()=%s\n", json_object_to_json_string(my_object));
		 
	//MC_SET_DEBUG(1);

}




