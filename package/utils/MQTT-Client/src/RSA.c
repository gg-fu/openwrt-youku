#include "RSA.h"

/* 对二进制数据进行base64编码 */
/* input:数组，length：数组长度,result:存储编码后的数据，size：数组大小 */
unsigned char *base64(const void *input, size_t length,char *result, size_t size)
{
 	BIO *bmem, *b64;
  	BUF_MEM *bptr;

  	b64 = BIO_new(BIO_f_base64());
  	bmem = BIO_new(BIO_s_mem());
  	b64 = BIO_push(b64, bmem);
  	BIO_write(b64, input, length);
  	BIO_flush(b64);
  	BIO_get_mem_ptr(b64, &bptr);

	if(size < bptr->length-1)
	{
		printf("the size is so small\n");
		return NULL;
	}
  	memcpy(result, bptr->data, bptr->length-1);
  	result[bptr->length-1] = 0;
  	BIO_free_all(b64);

  	return result;
}
/* 对base64数据进行解码 */
/* input：待解码数组，length：数组长度，result：解码后数据暂存区，暂存区大小 */
unsigned char *debase64(char *input, size_t length, char *result, size_t size)
{
	BIO * b64 = NULL;
	BIO * bmem = NULL;
	assert(NULL != input);
	if (length > size)
		return NULL;
	memset(result, 0, size);

	b64 = BIO_new(BIO_f_base64());
	bmem = BIO_new_mem_buf(input, length);
	BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
    if (NULL == b64 || NULL == bmem) 
	{
       	perror("BIO_new");
        return NULL;
   	}
	bmem = BIO_push(b64, bmem);
	BIO_read(bmem, result, length);
	BIO_free_all(b64);
	return result;
}
/* 生成签名函数 */
/* 1:私钥名，2：待签名的数据，3：签名后的数据存储区，4：传入指针 */
unsigned char * GetSign(char* keyFile,char* plainText,unsigned char* cipherText,unsigned int *cipherTextLen)  
{     
	FILE* fp = fopen (keyFile, "r");  
	if (fp == NULL)
		return NULL;  

	/* Read private key */  
	EVP_PKEY* pkey = PEM_read_PrivateKey(fp, NULL, NULL, NULL);  
	fclose (fp);  

	if (pkey == NULL) 
	{   
		ERR_print_errors_fp (stderr);  
		EVP_PKEY_free(pkey);  
		return NULL;  
	}  

	/* Do the signature */  
	EVP_MD_CTX     md_ctx;  
	EVP_SignInit   (&md_ctx, EVP_md5());  
	EVP_SignUpdate (&md_ctx, plainText, strlen(plainText));  
	int err = EVP_SignFinal (&md_ctx, cipherText, cipherTextLen, pkey);  

	if (err != 1) {  
		ERR_print_errors_fp(stderr); 
		EVP_PKEY_free(pkey);
		EVP_MD_CTX_cleanup(&md_ctx);   		
		return NULL;  
	}  

	EVP_PKEY_free(pkey);
	EVP_MD_CTX_cleanup(&md_ctx);    
	return cipherText;  
}  
/* 验证签名函数 */
/* 1:公钥名，2：签名的数据，3：签名的数据大小，4：原数据 */
bool VerifySign(char* certFile,unsigned char* cipherText,unsigned int cipherTextLen,char* plainText)  
{  
	FILE* fp = fopen(certFile, "r");  
	if (fp == NULL)   
		return false;  
	EVP_PKEY *pkey=PEM_read_PUBKEY(fp,NULL,NULL,NULL);  
	if (pkey == NULL) 
	{  
		perror("pkey fail\n");
		ERR_print_errors_fp (stderr); 
		EVP_PKEY_free(pkey);
		return false;  
	}  
	EVP_MD_CTX md_ctx;  
	EVP_VerifyInit   (&md_ctx, EVP_md5());  
	EVP_VerifyUpdate (&md_ctx, plainText, strlen((char*)plainText));  
	int err = EVP_VerifyFinal (&md_ctx, cipherText, cipherTextLen, pkey);  
	EVP_PKEY_free (pkey);  

	if (err != 1) {  
		ERR_print_errors_fp (stderr);
		EVP_PKEY_free(pkey);
		EVP_MD_CTX_cleanup(&md_ctx);   		
		return false;  
	}
  	fclose(fp);
	EVP_PKEY_free(pkey);
	EVP_MD_CTX_cleanup(&md_ctx);   
    return true;  
}


