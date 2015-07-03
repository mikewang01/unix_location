/******************************************************************************
 * Copyright 2015-2016 Hicling Systems (ShangHai)
 *
 * FileName: Hmac_sha1.c
 *
 * Description:  Hmac_sha1 encryption  implemetation.
 *              
 * Modification history:
 *     2015/4/3, v1.0 create this file.   MikeWang
*******************************************************************************/

/*********************************************************************
 * INCLUDES
*/
#include "Hmac_sha1.h"	
#include "assert.h"
#include <openssl/hmac.h>
#include <string.h>
#include "curses.h"
/*********************************************************************
* MACROS
*/




/*********************************************************************
* TYPEDEFS
*/



 enum {
	    SHA1_DIGEST_LENGTH	= 20, /*encryption output data lenth*/
		SHA1_BLOCK_SIZE		= 64, /*ShA1 calculation block size*/
		HMAC_BUF_LEN		= 4096/*maxim size of buffer*/
	 } ;


struct enpryption_information{
	unsigned char  key[64];   /*store the key parameter used to proceed hmac_sha1 enprytion*/
	int key_lenth; /*store the lenth of the key*/
	unsigned char  text[128]; /*store the data needing enprytion*/
	int text_lenth; /*store the lenth of the text*/
};	
	

	
/*********************************************************************
* GLOBAL VARIABLES
*/
	
	
/*********************************************************************
* static VARIABLES
*/

static const char Base64[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

/*********************************************************************
* EXTERNAL VARIABLES
*/


/*********************************************************************
* FUNCTIONS
*/
static int hmac_sha1_set_key (CLASS(hmac_sha1) *arg, char *key, int size);
static int hmac_sha1_set_text (CLASS(hmac_sha1) *arg, char *text, int size);

static int hmac_sha1_process_internal(unsigned char *text, int text_len, unsigned char *key, int key_len, unsigned char *digest);
static int hmac_sha1_process  (CLASS(hmac_sha1) *arg, char *output_buffer, int *size);

static int hmac_sha1_base64_decode(CLASS(hmac_sha1) *arg, char *src, char *dst);
static int hmac_sha1_base64_encode(CLASS(hmac_sha1) *arg, const char *src, int src_size, char *dst);


static int delete_hmac_sha1(CLASS(hmac_sha1) *arg);

/******************************************************************************
 * FunctionName : init_hmac_sha1
 * Description	: internal used to initiate object 
 * Parameters	: CLASS(hmac_sha1) *arg point to the struct need initailizing 
 *					
 * Returns		: TRUE   : initialzie successfully
 *				  FALSE  : initilize failedly 	
*******************************************************************************/
int HmacEncode(const char * algo,
                const char * key, unsigned int key_length,
                const char * input, unsigned int input_length,
                unsigned char * output, unsigned int *output_length) {
        const EVP_MD * engine = NULL;
		/*ifoutput buffer is less than lowest threahold*/
		if(sizeof(output) < EVP_MAX_MD_SIZE){
			return -1;
		}
        if(strcasecmp("sha512", algo) == 0) {
                engine = EVP_sha512();
        }
        else if(strcasecmp("sha256", algo) == 0) {
                engine = EVP_sha256();
        }
        else if(strcasecmp("sha1", algo) == 0) {
                engine = EVP_sha1();
        }
        else if(strcasecmp("md5", algo) == 0) {
                engine = EVP_md5();
        }
        else if(strcasecmp("sha224", algo) == 0) {
                engine = EVP_sha224();
        }
        else if(strcasecmp("sha384", algo) == 0) {
                engine = EVP_sha384();
        }
        else if(strcasecmp("sha", algo) == 0) {
                engine = EVP_sha();
        }
        else if(strcasecmp("md2", algo) == 0) {
                engine = EVP_md2();
        }
        else {
				printf("Algorithm %s is not supported by this program!", algo);
    
                return -1;
        }

        //output = (unsigned char*)malloc(EVP_MAX_MD_SIZE);
		
        HMAC_CTX ctx;
        HMAC_CTX_init(&ctx);
        HMAC_Init_ex(&ctx, key, key_length, engine, NULL);
        HMAC_Update(&ctx, (unsigned char*)input, input_length);        // input is OK; &input is WRONG !!!

        HMAC_Final(&ctx, output, output_length);
        HMAC_CTX_cleanup(&ctx);

        return 0;
}


/******************************************************************************
 * FunctionName : init_hmac_sha1
 * Description	: internal used to initiate object 
 * Parameters	: CLASS(hmac_sha1) *arg point to the struct need initailizing 
 *					
 * Returns		: TRUE   : initialzie successfully
 *				  FALSE  : initilize failedly 	
*******************************************************************************/
#if 1/*for debugging purpose*/
int 
init_hmac_sha1(CLASS(hmac_sha1) *arg)
{

	assert(arg != NULL);
	
	return TRUE;
}
#endif
/******************************************************************************
 * FunctionName : init_hmac_sha1
 * Description	: internal used to initiate object 
 * Parameters	: CLASS(hmac_sha1) *arg point to the struct need initailizing 
 *					
 * Returns		: TRUE   : initialzie successfully
 *				  FALSE  : initilize failedly 	
*******************************************************************************/
#if 1/*for debugging purpose*/
static int delete_hmac_sha1(CLASS(hmac_sha1) *arg)
{

	assert(NULL != arg);
	
	
	return TRUE;
}
#endif

/******************************************************************************
 * FunctionName : hmac_sha1_set_key
 * Description	: internal used to initiate object 
 * Parameters	: CLASS(hmac_sha1) *arg point to the struct need initailizing 
 *					
 * Returns		: TRUE   : initialzie successfully
 *				  FALSE  : initilize failedly 	
*******************************************************************************/
static int hmac_sha1_set_key (CLASS(hmac_sha1) *arg, char *key, int size)
{

	return TRUE;
}

/******************************************************************************
 * FunctionName : hmac_sha1_set_text
 * Description	: internal used to move text content from user buffer to internal buffer 
 * Parameters	: CLASS(hmac_sha1) *arg point to the struct need initailizing 
 *					
 * Returns		: TRUE   : initialzie successfully
 *				  FALSE  : initilize failedly 	
*******************************************************************************/
#if 1
static int hmac_sha1_set_text (CLASS(hmac_sha1) *arg, char *text, int size)
{
	/*check parameter object first*/
	

	return TRUE;
}
#endif


/******************************************************************************
 * FunctionName : hmac_sha1_process
 * Description	: internal used to enpryt text
 * Parameters	: CLASS(hmac_sha1) *arg point to the struct need initailizing 
 *					
 * Returns		: TRUE   : initialzie successfully
 *				  FALSE  : initilize failedly 	
*******************************************************************************/
#if 1
static int hmac_sha1_process  (CLASS(hmac_sha1) *arg, char *output_buffer, int *size)
{

	
}
#endif
/******************************************************************************
 * FunctionName : hmac_sha1_process
 * Description	: internal used to enpryt text
 * Parameters	: CLASS(hmac_sha1) *arg point to the struct need initailizing 
 *					
 * Returns		: TRUE   : initialzie successfully
 *				  FALSE  : initilize failedly 	
*******************************************************************************/
#if 1
static int hmac_sha1_translate  (CLASS(hmac_sha1) *arg, const char * key, unsigned int key_length,
                				 const char * input, unsigned int input_length,
                				 unsigned char * output, unsigned int *output_length)
{

	HmacEncode("sha1", key, key_length, input, input_length, output, output_length);
}
#endif



/******************************************************************************
 * FunctionName : hmac_sha1_process
 * Description  : user interface to enpryt data 
 * Parameters   : unsigned char *text  : buffer pointer pointed to the data needing for encryption
 *				  int text_len,		   : lenth of buferr passed to this interface
 * 				  unsigned char *key,  : encryption key poniter
 *				  int key_len, 		   : encryption key lenth
 *				  unsigned char *digest: buffer where encryption result stored
 * Returns      : NONE
*******************************************************************************/

	
static int 
hmac_sha1_process_internal(unsigned char *text, int text_len, unsigned char *key, int key_len, unsigned char *digest)
{


	
	return TRUE;
}

/******************************************************************************
 * FunctionName : base64_decode
 * Description  : user interface to enpryt data 
 * Parameters   : unsigned char *text  : buffer pointer pointed to the data needing for encryption
 *				  int text_len,		   : lenth of buferr passed to this interface
 * 				  unsigned char *key,  : encryption key poniter
 *				  int key_len, 		   : encryption key lenth
 *				  unsigned char *digest: buffer where encryption result stored
 * Returns      : NONE
*******************************************************************************/

static int 
hmac_sha1_base64_decode(CLASS(hmac_sha1) *arg, char *src, char *dst)
{
    char *q = (char*)malloc(strlen(src)+1);
    char *p = dst;
    char *temp = q;
    char *s = src;
    int len = strlen(src),i;
    memset(q, 0, strlen(src)+1);
    while(*s)
    {
        if(*s>='A'&&*s<='Z') *temp=*s-'A';
        else if(*s>='a'&&*s<='z') *temp=*s-'a'+26;
        else if(*s>='0'&&*s<='9') *temp=*s-'0'+52;
        else if(*s=='+') *temp=62;
        else if(*s=='/') *temp=63;
        else if(*s=='=') *temp=-1;
        else
        {
            printf("\n%c:Not a valid base64 string\n",*s);
            return FALSE;
        }
        ++s;
        ++temp;
    }
    for(i=0;i<len-4;i+=4)
    {
        *p++=(*(q+i)<<2)+(*(q+i+1)>>4);
        *p++=(*(q+i+1)<<4)+(*(q+i+2)>>2);
        *p++=(*(q+i+2)<<6)+(*(q+i+3));
    }
    if(*(q+i+3)!=-1)
    {
        *p++=(*(q+i)<<2)+(*(q+i+1)>>4);
        *p++=(*(q+i+1)<<4)+(*(q+i+2)>>2);
        *p++=(*(q+i+2)<<6)+*(q+i+3);
    }
    else if(*(q+i+2)!=-1)
    {
        *p++=(*(q+i)<<2)+(*(q+i+1)>>4);
        *p++=(*(q+i+1)<<4)+(*(q+i+2)>>2);
        *p++=(*(q+i+2)<<6);
    }
    else if(*(q+i+1)!=-1)
    {
        *p++=(*(q+i)<<2)+(*(q+i+1)>>4);
        *p++=(*(q+i+1)<<4);
    }
    else 
    {
        printf("Not a valid base64 string\n");
        return FALSE;
    }
    *p=0;
    free(q);
}

/******************************************************************************
 * FunctionName : base64_encode
 * Description  : user interface to encode data
 * Parameters   : unsigned char *text  : buffer pointer pointed to the data needing for encryption
 *				  int text_len,		   : lenth of buferr passed to this interface
 * 				  unsigned char *key,  : encryption key poniter
 *				  int key_len, 		   : encryption key lenth
 *				  unsigned char *digest: buffer where encryption result stored
 * Returns      : NONE
*******************************************************************************/
static int 
hmac_sha1_base64_encode(CLASS(hmac_sha1) *arg, const char *src, int src_size, char *dst)
{
    int i = 0;
    char *p = dst;
    int d = src_size-3;
    //for(i=0;i<strlen(src)-3;i+=3) ;if (strlen(src)-3)<0 there is a buf
	
    for(i=0;i<=d;i+=3)
    {
        *p++=Base64[((*(src+i))>>2)&0x3f];
        *p++=Base64[(((*(src+i))&0x3)<<4)+((*(src+i+1))>>4)];
        *p++=Base64[((*(src+i+1)&0xf)<<2)+((*(src+i+2))>>6)];
        *p++=Base64[(*(src+i+2))&0x3f];
    }
    if((src_size - i) == 1)
    {
        *p++=Base64[((*(src+i))>>2)&0x3f];
        *p++=Base64[((*(src+i))&0x3)<<4];
        *p++='=';
        *p++='=';
    }
    if((src_size - i) == 2)
    {
        *p++=Base64[((*(src+i))>>2)&0x3f];
        *p++=Base64[(((*(src+i))&0x3)<<4)+((*(src+i+1))>>4)];
        *p++=Base64[((*(src+i+1)&0xf)<<2)];
        *p++='=';
    }
    *p='\0';
}




