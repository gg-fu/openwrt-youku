#ifndef __RSA_H
#define __RSA_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#include <openssl/rsa.h>
#include <openssl/evp.h>
#include <openssl/objects.h>
#include <openssl/x509.h>
#include <openssl/err.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/crypto.h>

unsigned char *base64(const void *input, size_t length,char *result, size_t size);
unsigned char *debase64(char *input, size_t length, char *result, size_t size);
unsigned char * GetSign(char* keyFile,char* plainText,unsigned char* cipherText,unsigned int *cipherTextLen);
bool VerifySign(char* certFile,unsigned char* cipherText,unsigned int cipherTextLen,char* plainText);



#endif
