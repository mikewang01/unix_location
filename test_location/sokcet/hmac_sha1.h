#ifndef __HMAC_SHA1_H__
#define __HMAC_SHA1_H__


#include "oop_hal.h"
#include <stdint.h>
#include "user_interface.h"



#if 0

CLASS(hmac_sha1) 
{
	int a;
	int (*init)   (CLASS(__NAME) *arg);
 	int (*set_key)  (CLASS(__NAME) *arg, char *key, int size);
 	int (*set_text) (CLASS(__NAME) *arg, char *text, int size);
	int (*process)  (CLASS(__NAME) *arg, char *output_buffer);
	int (*to_base64)(char *input, char *output);
};

#else

DEF_CLASS(hmac_sha1)
	int (*init)    (CLASS(hmac_sha1)*);
	int (*de_init) (CLASS(hmac_sha1)*);
 	int (*set_key) (CLASS(hmac_sha1)*, char*, int );
 	int (*set_text)(CLASS(hmac_sha1)*, char*, int );
	int (*process) (CLASS(hmac_sha1)*, char*, int*);
	int (*process_full)(CLASS(hmac_sha1)*, const char* , unsigned int ,const char* , unsigned int , unsigned char* , unsigned int*);
	int (*base64_encode) (CLASS(hmac_sha1)*, const char*, int, char*, int);
	int (*base64_decode) (CLASS(hmac_sha1)*, char*, char*);
	int (*check_sum)(CLASS(hmac_sha1)*, unsigned char *, int , uint16_t *);
	void *user_data;
END_DEF_CLASS(hmac_sha1)

#endif
/* Hash a single 512-bit block. This is the core of the algorithm.*/




#endif

