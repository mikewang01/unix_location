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
#define HMAC_SHA1_KEY "5EHdd8_334dyUjjddleqH6YHHm"

/*********************************************************************
* GLOBAL VARIABLES
*/
/*set to 1 to open the key trigger function */

/*********************************************************************
* LOCAL VARIABLES
*/

/******************************************************************************
 * FunctionName : get json string
 * Description	: get timer request json
 * Parameters	: level : output level of pin
 * Returns		: 0: init successfully
 *				 -1: init failed
 *
*******************************************************************************/
int get_rid_of_quotation(char *pbuffer, int lenth) /*initiate http object*/
{
    assert(NULL != pbuffer);
    char temp[1024];
    int i = 0;
    char *s = pbuffer;
    for(i = 0; (s - pbuffer) < lenth;) {
        if((*s =='\\')) {
            temp[i++] = *s++;
            if((*(s + 1) == '"')) {
                temp[i++] = *s++;
                continue;
            }
        } else if((*(s + 0) == '"')) {
            s++;
            continue;
        }
        temp[i++] = *s++;
    }
    temp[i] = '\0';
    //printf("i = %d \t lenth = %d, %s\n",i,lenth, temp);
    return strcpy(pbuffer, temp);
}


/******************************************************************************
 * FunctionName : get_time_request_json
 * Description	: CLASS(json_interface) *arg: obj pointer 
 *				  char *pbuffer:  json string buffer
 * Parameters	: level : output level of pin
 * Returns		: 0: init successfully
 *				 -1: get json request failed
 *
*******************************************************************************/
int get_time_request_json(CLASS(json_interface) *arg, char *pbuffer) /*initiate http object*/
{

    char encryption_sequnece[][30] = {
        "udid",
        "version",
        "request_body",
        "timestamp",
    };
    assert(NULL != arg && NULL != pbuffer);
    __bzero(pbuffer, sizeof(pbuffer));


    int i;
    char mac[60];
    int lenth;
    arg->internet_interface->get_mac(arg->internet_interface, mac, &lenth);
    /*prapred for enpricption*/
#if 1
    json_object  *time_req_object = NULL;
    time_req_object =  json_object_new_object();
    json_object_object_add(time_req_object, "udid", json_object_new_string(mac));
    json_object_object_add(time_req_object, "version", json_object_new_string("1.0.0.0"));
    json_object_object_add(time_req_object, "timestamp", json_object_new_int(time(NULL)));
    json_object_object_add(time_req_object, "signature",  json_object_new_string("NULL"));
    json_object_object_add(time_req_object, "request_body", json_object_new_string("{\"action\":\"time\"}"));

    printf("my_object.to_string()=%s \n", json_object_to_json_string(time_req_object));
    /*add coressponed key calue to encryption text */
    for(int i = 0 ; i < sizeof(encryption_sequnece)/sizeof(encryption_sequnece[0]); i++ ) {
        struct json_object *value;
        printf("%s", encryption_sequnece[i]);
        /*if get find the key successfully*/
        if(json_object_object_get_ex(time_req_object, encryption_sequnece[i], &value) ==  TRUE) {
            strcat(pbuffer, json_object_to_json_string(value));
            //json_object_put(time_req_object);

            //json_object_put(time_req_object);
            //json_object_put(time_req_object);
        }

    }
#if 0
    json_object_object_foreach(time_req_object, key, val) {
        if(strcmp(json_object_to_json_string(val), "\"NULL\"") != 0 ) {
            strcat(pbuffer, json_object_to_json_string(val));
        }
    }
#endif
#endif
    printf("encryption text= %s\n", pbuffer);
    get_rid_of_quotation(pbuffer, strlen(pbuffer));
    printf("encryption text= %s\n", pbuffer);

    char sha1_output[100], sha1_encode64[100];
    int  sha1_output_lenth, sha1_encode64_lenth;
    /*peocess sha1 encode process*/
    arg->sha1_interface->process_full(arg, HMAC_SHA1_KEY, strlen(HMAC_SHA1_KEY), pbuffer, strlen(pbuffer),sha1_output, &sha1_output_lenth);
    arg->sha1_interface->base64_encode(arg, sha1_output, sha1_output_lenth, sha1_encode64, 100);
    sha1_output[sha1_output_lenth + 1] = 0;

    printf("text = %s lenth =%d\n", sha1_output, sha1_output_lenth);
    printf("code64 = %s\n", sha1_encode64);
    /*accomplishment the signature key value*/
    json_object_object_add(time_req_object, "signature",  json_object_new_string(sha1_encode64));
    sprintf(pbuffer, "%s", json_object_to_json_string(time_req_object));
    printf("my_object.to_string()=%s\n", pbuffer);

	/*free tim esyc json memory*/
    if (json_object_put(time_req_object) == 1){
		printf("time_req_object freed succesfully\n");
	}
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
int init_json_interface (CLASS(json_interface) *arg) /*initiate http object*/
{


    char mac[40];
    int lenth;
    assert(NULL != arg);

    arg->get_time_request_json = get_time_request_json;

    NEW(arg->internet_interface,  int_comp);
    NEW(arg->sha1_interface , hmac_sha1);
    arg->internet_interface->get_mac(arg->internet_interface, mac, &lenth);

    printf("mac adress = %s /n", mac);
#if 0
    printf("mac from int json = %s \n", mac);
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
#endif
    return 0;
}




