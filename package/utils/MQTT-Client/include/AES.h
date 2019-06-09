#ifndef __AES_H
#define __AES_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <openssl/aes.h>
#include <openssl/evp.h>
#include <openssl/ssl.h>

unsigned char *Int2Hex(char s[],char buffer[],int num);
unsigned char *Hex2Int(const char in[],char out[],int num);
int aes_encrypt_ecb(const char *aeskey,const char *data,unsigned char *encrypt_result,int size);
int aes_decrypt_ecb(const char *aeskey,const char *data,int length,unsigned char *decrypt_result,int size);
int aes_encrypt_cbc(unsigned char *in_buf, int in_len, unsigned char *key,unsigned char *iv, unsigned char *out_buf,int out_len);
int aes_decrypt_cbc(unsigned char *in_buf, int in_len, unsigned char *key,unsigned char *iv, unsigned char *out_buf,int out_len);

#endif


