#ifndef __HMAC_SHA1_H__
#define __HMAC_SHA1_H__


#include "oop_hal.h"



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
	int (*init)    (CLASS(hmac_sha1) *arg);
	int (*de_init) (CLASS(hmac_sha1) *arg);
 	int (*set_key) (CLASS(hmac_sha1) *arg, char *key, int size);
 	int (*set_text)(CLASS(hmac_sha1) *arg, char *text, int size);
	int (*process) (CLASS(hmac_sha1) *arg, char *output_buffer, int *size);
	int (*base64_encode) (CLASS(hmac_sha1) *arg, const char *src, int src_size, char *dst);
	int (*base64_decode) (CLASS(hmac_sha1) *arg, char *src, char *dst);
	void *user_data;
END_DEF_CLASS(hmac_sha1)

#endif
/* Hash a single 512-bit block. This is the core of the algorithm.*/




#endif

