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
#include <strings.h>
#include "config.h"
#include "internet.h"
#include "json_test.h"
#include "uart_protocol_cmd.h"
#include "version.h"

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


#define CLING_INF_LENTH_LIMITATION  JSON_MAX_BUFFER_LENTH

#define  CLING_SCAN_DATA_RANDOMNUM  18459




#ifndef MAC2STR
#define MAC2STR(a) (a)[0], (a)[1], (a)[2], (a)[3], (a)[4], (a)[5]
#define MACSTR "%02x:%02x:%02x:%02x:%02x:%02x"
#endif



/*********************************************************************
* TYPEDEFS
*/

/*cling device information related strcutor*/
struct cling_location_inf {
    char btid[JOSN_CLING_BTID_SIZE]; /*device id*/
    char btmac[JOSN_CLING_BTMAC_SIZE];/*device mac*/
    int btrssi; /*device rssi signal*/
    struct cling_health_inf_rev *ptr; /*cling inf passed from uart*/
    struct cling_location_inf *pnext; /*next inf to send*/
};
/*cling device information related strcutor*/
struct cling_health_inf {
    char btid[JOSN_CLING_BTID_SIZE]; /*device id*/
    char btmac[JOSN_CLING_BTMAC_SIZE];/*device mac*/
    char btrssi[JOSN_CLING_BTRSSI_SIZE]; /*device rssi signal*/
    struct health_data_inf health;
    char timestamp_ble[JOSN_TIMESTAMP_SIZE];
    struct cling_health_inf_rev *ptr; /*cling inf passed from uart*/
    struct cling_health_inf *pnext;	/*next inf to send*/
};
/*upload information struct*/
struct cling_inf_upload_json_chain {

    char udid[JOSN_MAC_SIZE];/**/
    char version[JOSN_VERSION_SIZE];
    uint32_t timestamp;
    char timestamp_str[JOSN_TIMESTAMP_SIZE];
    char signature[JOSN_SIGNATURE_SIZE];
    struct cling_location_inf *pinf_l;
    struct cling_health_inf *pinf_h;
    uint16_t cling_location_inf_chain_lenth; /**/
    uint16_t cling_health_inf_chain_lenth;
    char request_body[JOSN_REQUEST_BODY_SIZE];
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
 * FunctionName : set_cling_health_information
 * Description  : internal used to initiate time stamp object object
 * Parameters   : arg -- object pointer
 * Returns      : -1 message queue habeen full and current message gonna be abonded
 				   0  messgae add to fifo sucessfully
 				   1  message add to fifo sucessfully and queue has been full
*******************************************************************************/

#if 1
static int set_health_information(CLASS(json_interface) *arg,struct health_data_inf health, char *pdev_id, char *pdev_mac, uint32_t time_stamp)
{
#define CLING_DEVICE_ID_LENTH 	4
    assert((NULL != pdev_id) && (NULL != pdev_mac));
    /*malloc corresponed dparameter buffer*/
    struct cling_inf_upload_json_chain *cling_health_inf_data = ((struct cling_inf_upload_json_chain*)(arg->user_data));
    assert(NULL != cling_health_inf_data);

    /*assign specific  to user data aerea*/
    cling_health_inf_data->timestamp = time_stamp;
    /*if chain list is full*/
    if (cling_health_inf_data->cling_health_inf_chain_lenth > CLING_INF_LENTH_LIMITATION) {

        return -1;

    } else {
        /*get cling inf pointer*/
        struct cling_health_inf *p = cling_health_inf_data->pinf_h;
        /*this is the first data attatched to the list header*/
        if (p == NULL) {
            cling_health_inf_data->pinf_h = (struct cling_health_inf*)malloc(sizeof(struct cling_health_inf));
            p = cling_health_inf_data->pinf_h;
        } else {
            /*search to the end of chain list*/
            for(; p->pnext != NULL; p = p->pnext);
            p->pnext =  (struct cling_health_inf*)malloc(sizeof(struct cling_health_inf));
            p = p->pnext;
        }

        assert(NULL != p);
        /*one more new member has been added to list*/
        cling_health_inf_data->cling_health_inf_chain_lenth++;
        p->pnext = NULL;
        p->health = health;
        memcpy((char *)(p->btid), pdev_id, CLING_DEVICE_ID_LENTH);
        p->btid[CLING_DEVICE_ID_LENTH] = 0;

        char mac_string[20];
        /*convert mac adress to string*/
        sprintf(mac_string, MACSTR, MAC2STR(((unsigned char*)(pdev_mac))));
        memcpy((char *)(p->btmac), mac_string, strlen(mac_string) + 1);

    }

    /*clear enpryption data buffer*/

    /*if chain is full after the last data pushed in  then reutn -1*/
    if (cling_health_inf_data->cling_health_inf_chain_lenth > CLING_INF_LENTH_LIMITATION) {
        return 1;
    }
    return 0;
}
#endif


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
    strcpy(pbuffer, temp);
	return 0;
}


/******************************************************************************
 * FunctionName : set_cling_health_information
 * Description  : internal used to initiate time stamp object object
 * Parameters   : arg -- object pointer

 * Returns      : -1 location has been full. without the data current pushed in
 *				   0 location data has been stored in chain successgully
 *				   1 location chain has been full with current data pusded in
*******************************************************************************/

#if 1
int set_location_information(CLASS(json_interface) *arg, char *pdev_id, char *pdev_mac, char rssi, uint32_t time_stamp)
{
    assert((NULL != pdev_id) && (NULL != pdev_mac));
    /*malloc corresponed dparameter buffer*/
    struct cling_inf_upload_json_chain *cling_location_inf_data = ((struct cling_inf_upload_json_chain*)(arg->user_data));
    assert(NULL != cling_location_inf_data);

    /*assign specific  to user data aerea*/
    cling_location_inf_data->timestamp = time_stamp;
    /*if chain list is full*/
    if (cling_location_inf_data->cling_location_inf_chain_lenth > CLING_INF_LENTH_LIMITATION) {

        return -1;

    } else {
        /*get cling inf pointer*/
        struct cling_location_inf *p = cling_location_inf_data->pinf_l;
        /*this is the first data attatched to the list header*/
        if (p == NULL) {
            cling_location_inf_data->pinf_l = (struct cling_location_inf*)malloc(sizeof(struct cling_location_inf));
            p = cling_location_inf_data->pinf_l;
        } else {
            /*search to the end of chain list*/
            for(; p->pnext != NULL; p = p->pnext);
            p->pnext =  (struct cling_location_inf*)malloc(sizeof(struct cling_location_inf));
            p = p->pnext;
        }

        assert(NULL != p);
        /*one more new member has been added to list*/
        cling_location_inf_data->cling_location_inf_chain_lenth++;

		printf("cling_location_inf_data->cling_location_inf_chain_lenth = %d\n", cling_location_inf_data->cling_location_inf_chain_lenth);

		p->pnext = NULL;
        memcpy((char *)(p->btid), pdev_id, CLING_DEVICE_ID_LENTH);
        p->btid[CLING_DEVICE_ID_LENTH] = 0;

        char mac_string[20];
        /*convert mac adress to string*/
        sprintf(mac_string, MACSTR, MAC2STR(((unsigned char*)(pdev_mac))));
        memcpy((char *)(p->btmac), mac_string, strlen(mac_string) + 1);



        p->btrssi = rssi;

    }
    /*clear enpryption data buffer*/

    /*if chain is full after the last data pushed in  then reutn -1*/
    if (cling_location_inf_data->cling_location_inf_chain_lenth > CLING_INF_LENTH_LIMITATION) {
        return 1;
    }
	return 0;

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
char *TEST_STRING  = "[\n{\n\"MAC\":\"31:32:33:34:35:36\",\n\"U\":17,\n\"T\":16,\n\"D\":4,\n\"WS\":2,\n\"RS\":3,\n\"ST\":1,\n\"SP\":5,\n\"HR\":14,\n\"CF\":15,\n\"PT\":11,\n\"WT\":13,\n\"CT\":6,\n\"CS\":7,\n\"CM\":8\n},\n{\n\"MAC\":\"31:32:33:34:35:37\",\n\"U\":17,\n\"T\":16,\n\"D\":4,\n\"WS\":2,\n\"RS\":3,\n\"ST\":1,\n\"SP\":5,\n\"HR\":14,\n\"CF\":15,\n\"PT\":11,\n\"WT\":13,\n\"CT\":6,\n\"CS\":7,\n\"CM\":8\n},\n{\n\"MAC\":\"31:32:33:34:35:38\",\n\"U\":17,\n\"T\":16,\n\"D\":4,\n\"WS\":2,\n\"RS\":3,\n\"ST\":1,\n\"SP\":5,\n\"HR\":14,\n\"CF\":15,\n\"PT\":11,\n\"WT\":13,\n\"CT\":6,\n\"CS\":7,\n\"CM\":8\n},\n{\n\"MAC\":\"31:32:35:34:35:37\",\n\"U\":17,\n\"T\":16,\n\"D\":4,\n\"WS\":2,\n\"RS\":3,\n\"ST\":1,\n\"SP\":5,\n\"HR\":14,\n\"CF\":15,\n\"PT\":11,\n\"WT\":13,\n\"CT\":6,\n\"CS\":7,\n\"CM\":8\n}\n]";

//"[{\"MAC\":\"31:32:33:34:35:36\",\"U\":17,\"T\":16,\"D\":4,\"WS\":2,\"RS\":3,\"ST\":1,\"SP\":5,\"HR\":14,\"CF\":15,\"PT\":11,\"WT\":13,\"CT\":6,\"CS\":7,\"CM\":8},{\"MAC\":\"31:32:33:34:35:37\",\"U\":17,\"T\":16,\"D\":4,\"WS\":2,\"RS\":3,\"ST\":1,\"SP\":5,\"HR\":14,\"CF\":15,\"PT\":11,\"WT\":13,\"CT\":6,\"CS\":7,\"CM\":8},{\"MAC\":\"31:32:33:34:35:38\",\"U\":17,\"T\":16,\"D\":4,\"WS\":2,\"RS\":3,\"ST\":1,\"SP\":5,\"HR\":14,\"CF\":15,\"PT\":11,\"WT\":13,\"CT\":6,\"CS\":7,\"CM\":8},{\"MAC\":\"31:32:35:34:35:37\",\"U\":17,\"T\":16,\"D\":4,\"WS\":2,\"RS\":3,\"ST\":1,\"SP\":5,\"HR\":14,\"CF\":15,\"PT\":11,\"WT\":13,\"CT\":6,\"CS\":7,\"CM\":8}]"

static int get_health_upload_json(CLASS(json_interface) *arg, char *pbuffer) /*initiate http object*/
{

    const char encryption_sequnece[][30] = {
        "version",
        "udid",
        "check_sum",
        "timestamp",
    };

    struct cling_inf_upload_json_chain *cling_health_inf_data = ((struct cling_inf_upload_json_chain*)(arg->user_data));
    /*get cling inf pointer*/
    struct cling_health_inf *p = cling_health_inf_data->pinf_h;
    /*this means there is no data in location chain ,just return -1*/
    if(p == NULL) {
        return -1;
    } else {
        cling_health_inf_data->pinf_h= NULL;
    }

    /*validate pbuffer pointer*/
    assert(pbuffer != NULL);

    char enpryption_text[4096];
    /*clear memory buffer*/
    bzero(enpryption_text, sizeof(enpryption_text));
    //char pbuffer[1024];
    int lenth;
    /*prapred for enpricption*/
#if 1
    /*enpulse detailed data into data arryy */
    json_object  *health_data_array = json_object_new_array();
    /*search to the end of chain list*/
    for(; p != NULL; cling_health_inf_data->cling_health_inf_chain_lenth --) {
        json_object  *health_json_detail =  json_object_new_object();
        /*the inside one specified for location json*/
        json_object_object_add(health_json_detail, "MAC", json_object_new_string(p->btmac));

        /*the utc time when beacon revieved location information*/
        json_object_object_add(health_json_detail, "U",  json_object_new_int(p->health.beacon_timestamp));
        /*tranlate ble date format into */
        struct rtc_time i;
        arg->base_rtc->to_beijing_tm (cling_health_inf_data->timestamp, &i);
        /*merge current year month with  transmitted from ble device*/
        printf("beijign year =%d , month=%d  date = %d\n",i.tm_year, i.tm_mon,p->health.date);
        p->health.ble_timestamp = arg->base_rtc->make_beijing_time (i.tm_year, i.tm_mon, p->health.date, 0, 0, 0);
        json_object_object_add(health_json_detail, "T",  json_object_new_int(p->health.ble_timestamp));
        json_object_object_add(health_json_detail, "D",  json_object_new_int(p->health.total_distance));
        json_object_object_add(health_json_detail, "WS", json_object_new_int(p->health.walk_steps));
        json_object_object_add(health_json_detail, "RS", json_object_new_int(p->health.run_steps));
        json_object_object_add(health_json_detail, "ST", json_object_new_int(p->health.total_steps));
        json_object_object_add(health_json_detail, "SP", json_object_new_int(p->health.sport_time_total));
        json_object_object_add(health_json_detail, "HR", json_object_new_int(p->health.heart_rate));
        json_object_object_add(health_json_detail, "CF", json_object_new_int(p->health.skin_temp));
        json_object_object_add(health_json_detail, "PT", json_object_new_int(p->health.sleep_total));
        json_object_object_add(health_json_detail, "PE", json_object_new_int(p->health.sleep_efficient));
        json_object_object_add(health_json_detail, "WT", json_object_new_int(p->health.wakeup_times));
        json_object_object_add(health_json_detail, "CT", json_object_new_int(p->health.calories_total));
        json_object_object_add(health_json_detail, "CS", json_object_new_int(p->health.calories_sports));
        json_object_object_add(health_json_detail, "CM", json_object_new_int(p->health.calories_mentablisim));
        /*add health data into */
        json_object_array_add(health_data_array, health_json_detail);
        struct cling_health_inf *s =p;
        p = p->pnext;
        /*release memory occupied by location information */
        free(s);

    }

    printf("cling_health_inf_data->cling_health_inf_chain_lenth =%d\n", cling_health_inf_data->cling_health_inf_chain_lenth);
    char mac[60];
    char version[60];
    BEACON_GET_VERSION(version);
    /*the outter layer json*/
    json_object  *loacation_json_body = NULL;
    arg->internet_interface->get_mac(arg->internet_interface, mac, &lenth);
    /*prapred for enpricption*/
    /*the outter layer json*/
    json_object  *health_json_body = NULL;
    health_json_body =  json_object_new_object();
    json_object_object_add(health_json_body, "udid", json_object_new_string(mac));
    json_object_object_add(health_json_body, "version", json_object_new_string(version));

    char s[20];
    sprintf(s,"%d",(cling_health_inf_data->timestamp));
    json_object_object_add(health_json_body, "timestamp", json_object_new_string(s));
    json_object_object_add(health_json_body, "signature",  NULL);
    json_object_object_add(health_json_body, "blue", json_object_new_string(""));
    json_object_object_add(health_json_body, "request_body", json_object_new_string((json_object_to_json_string(health_data_array))));
    //¯*prepare enpryption text*/
    for(int i = 0 ; i < sizeof(encryption_sequnece)/sizeof(encryption_sequnece[0]); i++ ) {
        struct json_object *value;
        /*if get find the key successfully*/
        if(json_object_object_get_ex(health_json_body, encryption_sequnece[i], &value) ==  TRUE) {
            strcat(enpryption_text, json_object_to_json_string(value));
        } else { /*come to checksum part*/
            if(strcmp(encryption_sequnece[i], "check_sum") == 0) {
                if(json_object_object_get_ex(health_json_body, "request_body", &value) ==  TRUE) {
                    uint16_t checksum = 0;
                    value = health_data_array;
                    char *s = (char*)json_object_to_json_string(value);
                    printf("health json_object_to_json_string = %s\n", s);
                    //*(s + strlen(s) - 1) = 0;
                    //printf("json_object_to_json_string(value)= %s\n",s+1);
                    arg->sha1_interface->check_sum(arg->sha1_interface, s, strlen(s), &checksum);
                    //arg->sha1_interface->check_sum(arg->sha1_interface, TEST_STRING, strlen(TEST_STRING), &checksum);
                    printf("check sum value = %u\n", checksum);
                    /*printf check sum value and random number to the end of string*/
                    sprintf(enpryption_text + strlen(enpryption_text), "%d%d", checksum, CLING_SCAN_DATA_RANDOMNUM);
                }
            }
        }
    }
    /*add blue mark to the end of th string*/
    strcat(enpryption_text + strlen(enpryption_text), "&blue=");
    get_rid_of_quotation(enpryption_text, strlen(enpryption_text));
#if 0 /*for debug purpose*/
#define  TEST_MIKE "v1.0.2t45772(a)18:fe:34:a6:e7:4825282184591436782865&blue="
    memcpy(enpryption_text, TEST_MIKE, strlen(TEST_MIKE)+1);
#endif
    printf("health+++  = %s\n", enpryption_text);
    /*start process of sha1 enpryption*/
    char sha1_output[100], sha1_encode64[100];
    int  sha1_output_lenth, sha1_encode64_lenth;
    /*peocess sha1 encode process*/
    arg->sha1_interface->process_full(arg->sha1_interface, HMAC_SHA1_KEY, strlen(HMAC_SHA1_KEY), enpryption_text, strlen(enpryption_text),sha1_output, &sha1_output_lenth);
    arg->sha1_interface->base64_encode(arg->sha1_interface, sha1_output, sha1_output_lenth, sha1_encode64, 100);
    sha1_output[sha1_output_lenth + 1] = 0;

    printf("text = %s lenth =%d\n", sha1_output, sha1_output_lenth);
    printf("code64 = %s\n", sha1_encode64);
    /*accomplishment the signature key value*/
    json_object_object_add(health_json_body, "signature",  json_object_new_string(sha1_encode64));

    //sprintf(pbuffer, "%s", json_object_to_json_string(time_req_object));
    //printf("my_object.to_string()=%s\n", pbuffer);
    sprintf(pbuffer, "%s", json_object_to_json_string(health_json_body));
    /*free memory occupid by json lib*/
    while ((json_object_put(health_json_body) != 1) ||	(json_object_put(health_data_array) != 1)) {
        printf("health_json_body json object freed failed\n");
    }
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

#define LOCATION_REQUEST_BODY "{\"action\":\"time\"}"
    const char encryption_sequnece[][30] = {
        "version",
        "udid",
        "check_sum",
        "timestamp",
        "blue"
    };
    assert(pbuffer != NULL);
    struct cling_inf_upload_json_chain *cling_location_inf_data = ((struct cling_inf_upload_json_chain*)(arg->user_data));
    /*get cling inf pointer*/
    struct cling_location_inf *p = cling_location_inf_data->pinf_l;
    /*this means there is no data in location chain ,just return -1*/
    if(p == NULL) {
        return -1;
    } else {
        cling_location_inf_data->pinf_l = NULL;
    }

    char enpryption_text[1024];
    /*clear memory buffer*/
    bzero(enpryption_text, sizeof(enpryption_text));




#if 1
    json_object  *location_data_array = json_object_new_array();

    /*search to the end of chain list*/
    for(; p != NULL; cling_location_inf_data->cling_location_inf_chain_lenth--) {
        json_object  *loacation_json_detail =  json_object_new_object();
        /*the inner josn specified for location json*/
        json_object_object_add(loacation_json_detail, "btid", json_object_new_string(p->btid));
        json_object_object_add(loacation_json_detail, "btmac", json_object_new_string(p->btmac));
        json_object_object_add(loacation_json_detail, "btrssi", json_object_new_int(p->btrssi));
        /*enpulse detailed data into data arryy */
        json_object_array_add(location_data_array, loacation_json_detail);
        struct cling_location_inf *s =p;
        p = p->pnext;
        /*release location occupied memory*/
        free(s);
    }
    printf("cling_health_inf_data->cling_health_inf_chain_lenth =%d\n", cling_location_inf_data->cling_location_inf_chain_lenth);

    char mac[60],version[60];
    //char pbuffer[1024];
    int lenth;
    BEACON_GET_VERSION(version);
    /*the outter layer json*/
    json_object  *loacation_json_body = NULL;
    arg->internet_interface->get_mac(arg->internet_interface, mac, &lenth);
    /*prapred for enpricption*/
    loacation_json_body =  json_object_new_object();
    json_object_object_add(loacation_json_body, "udid", json_object_new_string(mac));
    json_object_object_add(loacation_json_body, "version", json_object_new_string(version));

	char s[20];
    sprintf(s,"%d",(cling_location_inf_data->timestamp));
    json_object_object_add(loacation_json_body, "timestamp", json_object_new_string(s));
    json_object_object_add(loacation_json_body, "signature",  NULL);
    json_object_object_add(loacation_json_body, "blue", location_data_array);
    json_object_object_add(loacation_json_body, "request_body", json_object_new_string(LOCATION_REQUEST_BODY));

    for(int i = 0 ; i < sizeof(encryption_sequnece)/sizeof(encryption_sequnece[0]); i++ ) {
        struct json_object *value;
        /*if get find the key successfully*/
        if(json_object_object_get_ex(loacation_json_body, encryption_sequnece[i], &value) ==  TRUE) {
            /*when come to blue part, it should be processed alone*/
            if(strcmp(encryption_sequnece[i], "blue") == 0) {
                /*get the first elemwnt*/
                json_object *obj = json_object_array_get_idx(value, 0);
                if(obj != NULL) {
                    json_object *btid;
                    json_object *btmac;
                    json_object_object_get_ex(obj, "btid", &btid);
                    json_object_object_get_ex(obj, "btmac", &btmac);
                    sprintf(enpryption_text + strlen(enpryption_text), "%s%s%s","&blue=", json_object_to_json_string(btid), json_object_to_json_string(btmac));
                }

            } else {
                strcat(enpryption_text, json_object_to_json_string(value));
            }

        } else { /*come to checksum part*/
            if(strcmp(encryption_sequnece[i], "check_sum") == 0) {
                if(json_object_object_get_ex(loacation_json_body, "request_body", &value) ==  TRUE) {
                    char *s =LOCATION_REQUEST_BODY;//json_object_to_json_string(value);
                    uint16_t check_sum = 0;
                    printf("location json_object_to_json_string(value)= %s\n",s);
                    arg->sha1_interface->check_sum(arg->sha1_interface, s, strlen(s), &check_sum);
                    /*printf check sum value and random number to the end of string*/
                    sprintf(enpryption_text + strlen(enpryption_text), "%d%d", check_sum, CLING_SCAN_DATA_RANDOMNUM);
                }
            }
        }
    }
    /*add blue mark to the end of th string*/
    // strcat(enpryption_text + strlen(enpryption_text), "&blue=");
    get_rid_of_quotation(enpryption_text, strlen(enpryption_text));
    printf("location enpryption_text = %s\n\n", enpryption_text);
    /*start process of sha1 enpryption*/
    char sha1_output[100], sha1_encode64[100];
    int  sha1_output_lenth, sha1_encode64_lenth;
    /*peocess sha1 encode process*/
    arg->sha1_interface->process_full(arg->sha1_interface, HMAC_SHA1_KEY, strlen(HMAC_SHA1_KEY), enpryption_text, strlen(enpryption_text),sha1_output, &sha1_output_lenth);
    arg->sha1_interface->base64_encode(arg->sha1_interface, sha1_output, sha1_output_lenth, sha1_encode64, 100);
    sha1_output[sha1_output_lenth + 1] = 0;

    printf("text = %s lenth =%d\n", sha1_output, sha1_output_lenth);
    printf("code64 = %s\n", sha1_encode64);
    /*accomplishment the signature key value*/
    json_object_object_add(loacation_json_body, "signature",  json_object_new_string(sha1_encode64));

//sprintf(pbuffer, "%s", json_object_to_json_string(time_req_object));
//printf("my_object.to_string()=%s\n", pbuffer);
    /*transfer josn to string for better sendding*/
    sprintf(pbuffer, "%s", json_object_to_json_string(loacation_json_body));
    printf("location json =%s\n enpryption text= %s\n", json_object_to_json_string(loacation_json_body), enpryption_text);
    /*free memory occupid by json lib*/
    while (json_object_put(loacation_json_body) != 1) {
        printf("loacation_json_body json object freed failed\n");
    }

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
    json_object_object_add(time_req_object, "signature",  NULL);
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
    arg->sha1_interface->process_full(arg->sha1_interface, HMAC_SHA1_KEY, strlen(HMAC_SHA1_KEY), pbuffer, strlen(pbuffer),sha1_output, &sha1_output_lenth);
    arg->sha1_interface->base64_encode(arg->sha1_interface, sha1_output, sha1_output_lenth, sha1_encode64, 100);
    sha1_output[sha1_output_lenth + 1] = 0;

    printf("text = %s lenth =%d\n", sha1_output, sha1_output_lenth);
    printf("code64 = %s\n", sha1_encode64);
    /*accomplishment the signature key value*/
    json_object_object_add(time_req_object, "signature",  json_object_new_string(sha1_encode64));
    sprintf(pbuffer, "%s", json_object_to_json_string(time_req_object));
    printf("my_object.to_string()=%s\n", pbuffer);

    /*free tim esyc json memory*/
    while (json_object_put(time_req_object) != 1) {
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
static int delete_json_interface(CLASS(json_interface) *arg);

int init_json_interface (CLASS(json_interface) *arg)
{

    char mac[40];
    int lenth;
    assert(NULL != arg);

    arg->get_time_request_json = get_time_request_json;
    arg->get_location_upload_json = get_location_upload_json;
    arg->get_health_upload_json = get_health_upload_json;
    arg->deinit = delete_json_interface;
    arg->set_location_inf = set_location_information;
    arg->set_health_inf = set_health_information;
    /*initiate inheriented interface*/
    NEW(arg->internet_interface, int_comp);
    NEW(arg->sha1_interface, hmac_sha1);
    NEW(arg->base_rtc, rtc_date);
    arg->user_data = malloc(sizeof(struct cling_inf_upload_json_chain));
    /*ckeck if malloc successfully*/
    if(arg->user_data == NULL) {
        goto FAILED;
    }
    bzero(arg->user_data, sizeof(struct cling_inf_upload_json_chain));
    return 0;
FAILED:
    free(arg->user_data);
    return -1;
}

/******************************************************************************
 * FunctionName : delete_cling_health_inf_json
 * Description  : deinitiate time stamp object
 * Parameters   : pbuffer -- The received data from the server

 * Returns      : none
*******************************************************************************/

#if 1
static int delete_json_interface(CLASS(json_interface) *arg)
{
    assert(NULL != arg);
    /*reinitiate global upload json request*/
    //upload_request = NULL;
    DELETE(arg->internet_interface,  int_comp);
    DELETE(arg->sha1_interface , hmac_sha1);

    struct cling_inf_upload_json_chain *cling_health_inf_data = ((struct cling_inf_upload_json_chain*)(arg->user_data));

    /*free private data*/
    free(cling_health_inf_data);

    free(arg);
    return 0;

}
#endif



