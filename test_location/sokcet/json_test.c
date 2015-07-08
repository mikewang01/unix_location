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
#include <netdb.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/ipc.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <thread_db.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <assert.h>
#include <json.h>
#include <json_tokener.h>
#include <string.h>
#include "internet.h"
#include "json_test.h"
#include "uart_protocol_cmd.h"


/*********************************************************************
* MACROS
*/
#define JOSN_MAC_SIZE		 	20
#define JOSN_VERSION_SIZE	 	32
#define JOSN_TIMESTAMP_SIZE	 	16
#define JOSN_SIGNATURE_SIZE   	32
#define JOSN_REQUEST_BODY_SIZE 	32


#define JOSN_CLING_BTID_SIZE	20
#define JOSN_CLING_BTMAC_SIZE	20
#define JOSN_CLING_BTRSSI_SIZE	1


#define CLING_INF_LENTH_LIMITATION  8

#define  CLING_SCAN_DATA_RANDOMNUM  18459



/*********************************************************************
* TYPEDEFS
*/

/*cling device information related strcutor*/
struct cling_health_inf {
    char btid[JOSN_CLING_BTID_SIZE]; /*device id*/
    char btmac[JOSN_CLING_BTMAC_SIZE];/*device mac*/
    char btrssi[JOSN_CLING_BTRSSI_SIZE]; /*device rssi signal*/
    struct cling_inf_rev *ptr; /*cling inf passed from uart*/
    struct cling_health_inf *pnext; /*next inf to send*/
};
/*upload information struct*/
struct cling_inf_upload_json {

    char udid[JOSN_MAC_SIZE];/**/
    char version[JOSN_VERSION_SIZE];
    uint32_t timestamp;
    char timestamp_str[JOSN_TIMESTAMP_SIZE];
    char signature[JOSN_SIGNATURE_SIZE];
    struct cling_health_inf *pinf;
    uint16_t cling_inf_chain_lenth;
    char request_body[JOSN_REQUEST_BODY_SIZE];
    char enpryption_text[4+JOSN_TIMESTAMP_SIZE + JOSN_MAC_SIZE + JOSN_VERSION_SIZE + JOSN_TIMESTAMP_SIZE + JOSN_REQUEST_BODY_SIZE + sizeof("&blue=") + JOSN_CLING_BTID_SIZE + JOSN_CLING_BTMAC_SIZE];
};


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
 * FunctionName : set_cling_information
 * Description  : internal used to initiate time stamp object object
 * Parameters   : arg -- object pointer

 * Returns      : none
*******************************************************************************/

#if 1
int set_cling_information(CLASS(json_interface) *arg, char *pdev_id, char *pdev_mac, char rssi, uint32_t time_stamp)
{
#define CLING_DEVICE_ID_LENTH 	4
#define CLING_DEVICE_MAC_LENTH	6
    assert((NULL != pdev_id) && (NULL != pdev_mac));
    /*malloc corresponed dparameter buffer*/
    struct cling_inf_upload_json *cling_inf_data = ((struct cling_inf_upload_json*)(arg->user_data));
    assert(NULL != cling_inf_data);

    /*assign specific  to user data aerea*/
    cling_inf_data->timestamp = time_stamp;
    /*if chain list is full*/
    if (cling_inf_data->cling_inf_chain_lenth > CLING_INF_LENTH_LIMITATION) {

        return -1;

    } else {
        /*get cling inf pointer*/
        struct cling_health_inf *p = cling_inf_data->pinf;
        /*this is the first data attatched to the list header*/
        if (p == NULL) {
            cling_inf_data->pinf = (struct cling_health_inf*)malloc(sizeof(struct cling_health_inf));
            p = cling_inf_data->pinf;
        } else {
            /*search to the end of chain list*/
            for(; p->pnext != NULL; p = p->pnext);
            p->pnext =  (struct cling_health_inf*)malloc(sizeof(struct cling_health_inf));
            p = p->pnext;
        }

        assert(NULL != p);
        /*one more new member has been added to list*/
        cling_inf_data->cling_inf_chain_lenth++;
        p->pnext = NULL;
        memcpy((char *)(p->btid), pdev_id, CLING_DEVICE_ID_LENTH);
        p->btid[CLING_DEVICE_ID_LENTH] = 0;
        memcpy((char *)(p->btmac), pdev_mac, CLING_DEVICE_MAC_LENTH);
        p->btmac[CLING_DEVICE_MAC_LENTH] = 0;
        p->btrssi[0] = rssi;

    }
    /*clear enpryption data buffer*/
    memset(cling_inf_data->enpryption_text, 0, sizeof(cling_inf_data->enpryption_text));

}
#endif


/******************************************************************************
 * FunctionName : get_health_upload_json
 * Description	: CLASS(json_interface) *arg: obj pointer
 *				  char *pbuffer:  json string buffer
 * Parameters	: level : output level of pin
 * Returns		: 0: init successfully
 *				 -1: get json request failed
 *
*******************************************************************************/
static int get_health_upload_json(CLASS(json_interface) *arg, char *pbuffer) /*initiate http object*/
{

    const char encryption_sequnece[][30] = {
        "version",
        "udid",
        "request_body",
        "check_sum",
        "timestamp",
    };
    char mac[60];
    char enpryption_text[1024];
    /*clear memory buffer*/
    bzero(enpryption_text, sizeof(enpryption_text));
    //char pbuffer[1024];
    int lenth;

    arg->internet_interface->get_mac(arg->internet_interface, mac, &lenth);
    /*prapred for enpricption*/
#if 1

    json_object  *loacation_json_detail = NULL;
    loacation_json_detail =  json_object_new_object();
    /*the inner josn specified for location json*/
    json_object_object_add(loacation_json_detail, "btid", json_object_new_string(mac));
    json_object_object_add(loacation_json_detail, "btmac", json_object_new_string(mac));
    json_object_object_add(loacation_json_detail, "btmac", json_object_new_int(-50));
    /*enpulse detailed data into data arryy */
    json_object  *location_data_array = json_object_new_array();
    json_object_array_add(location_data_array, loacation_json_detail);

    /*the outter layer json*/
    json_object  *loacation_json_body = NULL;
    loacation_json_body =  json_object_new_object();
    json_object_object_add(loacation_json_body, "udid", json_object_new_string(mac));
    json_object_object_add(loacation_json_body, "version", json_object_new_string("1.0.0.0"));
    json_object_object_add(loacation_json_body, "timestamp", json_object_new_int(time(NULL)));
    json_object_object_add(loacation_json_body, "signature",  json_object_new_string("NULL"));
    json_object_object_add(loacation_json_body, "blue", location_data_array);
    json_object_object_add(loacation_json_body, "request_body", json_object_new_string("{\"action\":\"time\"}"));

    for(int i = 0 ; i < sizeof(encryption_sequnece)/sizeof(encryption_sequnece[0]); i++ ) {
        struct json_object *value;
        /*if get find the key successfully*/
        if(json_object_object_get_ex(loacation_json_body, encryption_sequnece[i], &value) ==  TRUE) {
            strcat(enpryption_text, json_object_to_json_string(value));
        } else { /*come to checksum part*/
            if(strcmp(encryption_sequnece[i], "check_sum") == 0) {
                if(json_object_object_get_ex(loacation_json_body, "request_body", &value) ==  TRUE) {
                    uint16_t check_sum = 0;
                    printf("json_object_to_json_string(value)= %s\n",json_object_to_json_string(value));
                    arg->sha1_interface->check_sum(arg->sha1_interface, json_object_to_json_string(value), strlen(json_object_to_json_string(value)), &check_sum);
                    /*printf check sum value and random number to the end of string*/
                    sprintf(enpryption_text + strlen(enpryption_text), "%d%d", check_sum, CLING_SCAN_DATA_RANDOMNUM);
                }
            }
        }
    }
    /*add blue mark to the end of th string*/
    strcat(enpryption_text + strlen(enpryption_text), "&blue=");
    get_rid_of_quotation(enpryption_text, strlen(enpryption_text));

    /*start process of sha1 enpryption*/
    char sha1_output[100], sha1_encode64[100];
    int  sha1_output_lenth, sha1_encode64_lenth;
    /*peocess sha1 encode process*/
    arg->sha1_interface->process_full(arg, HMAC_SHA1_KEY, strlen(HMAC_SHA1_KEY), enpryption_text, strlen(enpryption_text),sha1_output, &sha1_output_lenth);
    arg->sha1_interface->base64_encode(arg, sha1_output, sha1_output_lenth, sha1_encode64, 100);
    sha1_output[sha1_output_lenth + 1] = 0;

    printf("text = %s lenth =%d\n", sha1_output, sha1_output_lenth);
    printf("code64 = %s\n", sha1_encode64);
    /*accomplishment the signature key value*/
    json_object_object_add(loacation_json_body, "signature",  json_object_new_string(sha1_encode64));

    //sprintf(pbuffer, "%s", json_object_to_json_string(time_req_object));
    //printf("my_object.to_string()=%s\n", pbuffer);
    printf("location json =%s\n enpryption text= %s\n", json_object_to_json_string(loacation_json_body), enpryption_text);

#endif

    return 0;
}



/******************************************************************************
 * FunctionName : get_location_upload_json
 * Description	: CLASS(json_interface) *arg: obj pointer
 *				  char *pbuffer:  json string buffer
 * Parameters	: level : output level of pin
 * Returns		: 0: init successfully
 *				 -1: get json request failed
 *
*******************************************************************************/
static int get_location_upload_json(CLASS(json_interface) *arg, char *pbuffer) /*initiate http object*/
{

    const char encryption_sequnece[][30] = {
        "version",
        "udid",
        "request_body",
        "check_sum",
        "timestamp",
    };
    char mac[60];
    char enpryption_text[1024];
    /*clear memory buffer*/
    bzero(enpryption_text, sizeof(enpryption_text));
    //char pbuffer[1024];
    int lenth;

    arg->internet_interface->get_mac(arg->internet_interface, mac, &lenth);
    /*prapred for enpricption*/
#if 1

    json_object  *loacation_json_detail = NULL;
    loacation_json_detail =  json_object_new_object();
    /*the inner josn specified for location json*/
    json_object_object_add(loacation_json_detail, "btid", json_object_new_string(mac));
    json_object_object_add(loacation_json_detail, "btmac", json_object_new_string(mac));
    json_object_object_add(loacation_json_detail, "btmac", json_object_new_int(-50));
    /*enpulse detailed data into data arryy */
    json_object  *location_data_array = json_object_new_array();
    json_object_array_add(location_data_array, loacation_json_detail);

    /*the outter layer json*/
    json_object  *loacation_json_body = NULL;
    loacation_json_body =  json_object_new_object();
    json_object_object_add(loacation_json_body, "udid", json_object_new_string(mac));
    json_object_object_add(loacation_json_body, "version", json_object_new_string("1.0.0.0"));
    json_object_object_add(loacation_json_body, "timestamp", json_object_new_int(time(NULL)));
    json_object_object_add(loacation_json_body, "signature",  json_object_new_string("NULL"));
    json_object_object_add(loacation_json_body, "blue", location_data_array);
    json_object_object_add(loacation_json_body, "request_body", json_object_new_string("{\"action\":\"time\"}"));

    for(int i = 0 ; i < sizeof(encryption_sequnece)/sizeof(encryption_sequnece[0]); i++ ) {
        struct json_object *value;
        /*if get find the key successfully*/
        if(json_object_object_get_ex(loacation_json_body, encryption_sequnece[i], &value) ==  TRUE) {
            strcat(enpryption_text, json_object_to_json_string(value));
        } else { /*come to checksum part*/
            if(strcmp(encryption_sequnece[i], "check_sum") == 0) {
                if(json_object_object_get_ex(loacation_json_body, "request_body", &value) ==  TRUE) {
                    uint16_t check_sum = 0;
                    printf("json_object_to_json_string(value)= %s\n",json_object_to_json_string(value));
                    arg->sha1_interface->check_sum(arg->sha1_interface, json_object_to_json_string(value), strlen(json_object_to_json_string(value)), &check_sum);
                    /*printf check sum value and random number to the end of string*/
                    sprintf(enpryption_text + strlen(enpryption_text), "%d%d", check_sum, CLING_SCAN_DATA_RANDOMNUM);
                }
            }
        }
    }
    /*add blue mark to the end of th string*/
    strcat(enpryption_text + strlen(enpryption_text), "&blue=");
    get_rid_of_quotation(enpryption_text, strlen(enpryption_text));

    /*start process of sha1 enpryption*/
    char sha1_output[100], sha1_encode64[100];
    int  sha1_output_lenth, sha1_encode64_lenth;
    /*peocess sha1 encode process*/
    arg->sha1_interface->process_full(arg, HMAC_SHA1_KEY, strlen(HMAC_SHA1_KEY), enpryption_text, strlen(enpryption_text),sha1_output, &sha1_output_lenth);
    arg->sha1_interface->base64_encode(arg, sha1_output, sha1_output_lenth, sha1_encode64, 100);
    sha1_output[sha1_output_lenth + 1] = 0;

    printf("text = %s lenth =%d\n", sha1_output, sha1_output_lenth);
    printf("code64 = %s\n", sha1_encode64);
    /*accomplishment the signature key value*/
    json_object_object_add(loacation_json_body, "signature",  json_object_new_string(sha1_encode64));

    //sprintf(pbuffer, "%s", json_object_to_json_string(time_req_object));
    //printf("my_object.to_string()=%s\n", pbuffer);
    printf("location json =%s\n enpryption text= %s\n", json_object_to_json_string(loacation_json_body), enpryption_text);

#endif

    return 0;
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
int get_time_request_json(CLASS(json_interface) *arg, char *pbuffer)   /*initiate http object*/
{

    const char encryption_sequnece[][30] = {
        "udid",
        "version",
        "request_body",
        "timestamp",
    };
    assert(NULL != arg && NULL != pbuffer);
    bzero(pbuffer, sizeof(pbuffer));
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
    if (json_object_put(time_req_object) == 1) {
        printf("time_req_object freed succesfully\n");
    }
    return 0;
}


/******************************************************************************
 * FunctionName : int  init_json_interface(void* env)
 * Description  : initiate http object
 * Parameters   : level : output level of pin
 * Returns      : 0: init successfully
 *				 -1: init failed
 *
*******************************************************************************/
int init_json_interface (CLASS(json_interface) *arg)
{

    char mac[40];
    int lenth;
    assert(NULL != arg);

    arg->get_time_request_json = get_time_request_json;
    arg->get_location_upload_json = get_location_upload_json;

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

/******************************************************************************
 * FunctionName : delete_cling_inf_json
 * Description  : deinitiate time stamp object
 * Parameters   : pbuffer -- The received data from the server

 * Returns      : none
*******************************************************************************/

#if 0
static int delete_cling_inf_json(CLASS(json_interface) *arg)
{
    assert(NULL != arg);
    /*reinitiate global upload json request*/
    //upload_request = NULL;

    struct cling_inf_upload_json *cling_inf_data = ((struct cling_inf_upload_json*)(arg->user_data));

    /*free private data*/
    free(cling_inf_data);

    free(arg);
    return 0;

}
#endif



