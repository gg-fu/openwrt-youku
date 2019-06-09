#include "AES.h"
/* 将十六进制数(两位)转化为待解密的数据 */
unsigned char *Hex2Int(const char in[],char out[],int num)
{
	int i,j,k=0,n=0,count=0;
	
	if(in == NULL)
		return NULL;
	for (i=0;i<num;count++,i+=2){
		if (in[i] >= 'a') 
			j = in[i] -'a' +10;
		else
			j = in[i] -'0';
		
		if (in[i+1] >= 'a')
			k = in[i+1] -'a' +10;
		else
			k = in[i+1] -'0';
		
		n = j*16+k;
		if (n >= 128)
			out[count] = n-256;
		else
			out[count] = n;
	}
	out[count]='\0';
	return out;
}

/* 将十进制数(小于256)转化为十六进制数 */
unsigned char *Int2Hex(char in[],char out[],int num)
{
    int j,aa=0,num1=0,num2=0,i=0;
	
	if(in == NULL)
		return NULL;
	
    for (j=0;j<num;j++,i++){
		if ( (int)in[j] < 0 )
			aa = 256+(int)in[j];
		else
			aa = (int)in[j];
		
		if (aa < 16){
			out[i] = '0';
			if (aa < 10)       
				out[++i] = aa +'0';
			else
				out[++i] = aa -10 +'a';
		}
		else{
			num1 = aa/16;
			num2 = aa%16;
			if (num1 < 10)
				out[i] = num1 +'0';
			else
				out[i] = num1 - 10 +'a';
			
			if (num2 < 10)
				out[++i] = num2+'0';
			else
				out[++i] = num2-10+'a';
		}
	}
	out[2*j]='\0';
    return out;
}

int aes_encrypt_ecb(const char *aeskey,const char *data,unsigned char *encrypt_result,int size)
{
	char buffer[size+1];
	int length = strlen(data);
	int num,outl=0,outltmp=0;
	
	EVP_CIPHER_CTX ctx;
    EVP_CIPHER_CTX_init(&ctx);
    int rv= EVP_EncryptInit_ex(&ctx,EVP_aes_128_ecb(),NULL,aeskey,NULL);
	if(size>=2*length)
	{
		if(length%AES_BLOCK_SIZE==0&&length/AES_BLOCK_SIZE!=0)
			num=length/AES_BLOCK_SIZE;
		else	
			num=length/AES_BLOCK_SIZE+1;
		rv=EVP_EncryptUpdate(&ctx,buffer,&outl,data,length);
		rv=EVP_EncryptFinal_ex(&ctx,buffer+outl,&outltmp);
		char *q = Int2Hex(buffer,encrypt_result,num*AES_BLOCK_SIZE);
		if(q==NULL){
			EVP_CIPHER_CTX_cleanup(&ctx);
			return -1;
		}
	}
	else{
		EVP_CIPHER_CTX_cleanup(&ctx);
		printf("the output array space is too small\n");
		return -1;
	}
	EVP_CIPHER_CTX_cleanup(&ctx);
	return 0;
}
int aes_decrypt_ebc(const char *aeskey,const char *data,int length,unsigned char *decrypt_result,int size)
{
  	int outl=0,i;
	int outltmp=0;
	char buf[2*size+1];
	
	EVP_CIPHER_CTX ctx;
	EVP_CIPHER_CTX_init(&ctx);
	int rv= EVP_DecryptInit_ex(&ctx,EVP_aes_128_ecb(),NULL,aeskey,NULL);
	if(size*2>=length)
	{
		char *p = Hex2Int(data,buf,length);
		if(p==NULL){
			EVP_CIPHER_CTX_cleanup(&ctx);
			return -1;
		}
		rv=EVP_DecryptUpdate(&ctx,decrypt_result,&outl,buf,length/2);
		rv=EVP_DecryptFinal_ex(&ctx,decrypt_result+outl,&outltmp);
		int decrypt_len = strlen(decrypt_result);
		if((outltmp+outl)&&decrypt_len%1024!=0)
			decrypt_result[outltmp+outl]='\0';
	}
	else{
		EVP_CIPHER_CTX_cleanup(&ctx);
		fprintf(stderr,"Array size is too samll!\n");
		return -1;
	}
	EVP_CIPHER_CTX_cleanup(&ctx);
	return 0;
	
}

int aes_encrypt_cbc(unsigned char *in_buf, int in_len, unsigned char *key,unsigned char *iv, unsigned char *out_buf,int out_len)
{
    EVP_CIPHER_CTX ctx;

    int len=0,isSuccess = 0;
    unsigned char in[AES_BLOCK_SIZE];  
	unsigned char tmp[2*in_len+16];
    int outl = 0;   
    int outl_total = 0; 
    EVP_CIPHER_CTX_init(&ctx);  
    isSuccess = EVP_EncryptInit_ex(&ctx, EVP_aes_128_cbc(), NULL, key, iv);
	if(!isSuccess)  
	{  
		printf("EVP_DecryptInit_ex() failed");  
		EVP_CIPHER_CTX_cleanup(&ctx);  
		return 0; 
	} 
	if(out_len < 2*(in_len+in_len%AES_BLOCK_SIZE))
	{
		printf("the size of output size is too samll\n ");
		return 0;
	}
    while(in_len >=AES_BLOCK_SIZE)
    {
        memcpy(in, in_buf, AES_BLOCK_SIZE);  
        in_len -= AES_BLOCK_SIZE;  
        in_buf += AES_BLOCK_SIZE;  
        isSuccess = EVP_EncryptUpdate(&ctx, out_buf + outl_total, &outl, in, AES_BLOCK_SIZE);  
		if(!isSuccess)  
		{  
			printf("EVP_EncryptUpdate() failed");  
			EVP_CIPHER_CTX_cleanup(&ctx);  
			return 0;  
		}  
		outl_total += outl;  
	}

	if(in_len > 0)  
	{  
		memcpy(in, in_buf, in_len); 
		isSuccess = EVP_EncryptUpdate(&ctx,out_buf + outl_total, &outl, in, in_len);  
		outl_total += outl;  

		isSuccess = EVP_EncryptFinal_ex(&ctx,out_buf + outl_total,&outl);  
		if(!isSuccess)  
		{  
			printf("EVP_EncryptFinal_ex() failed");  
			EVP_CIPHER_CTX_cleanup(&ctx);  
			return 0;  
		}  
		outl_total += outl;  
	}     
	Int2Hex(out_buf,tmp,outl_total);
	memcpy(out_buf,tmp,2*outl_total);   
    EVP_CIPHER_CTX_cleanup(&ctx); 
    return outl_total;
}


int aes_decrypt_cbc(unsigned char *in_buf, int in_len, unsigned char *key,unsigned char *iv, unsigned char *out_buf,int out_len)
{
	unsigned char in[AES_BLOCK_SIZE]; 
	unsigned char tmp[in_len];
	int outl = 0;  
	int outl_total = 0;  
	int isSuccess;  

	EVP_CIPHER_CTX ctx;     
	//初始化ctx，加密算法初始化  
	EVP_CIPHER_CTX_init(&ctx);  
	isSuccess = EVP_DecryptInit_ex(&ctx,EVP_aes_128_cbc(),NULL,key,iv);  
	if(!isSuccess)  
	{  
		printf("EVP_DecryptInit_ex() failed");  
		EVP_CIPHER_CTX_cleanup(&ctx);  
		return 0; 
	}  
	if(out_len < in_len/2)
	{
		printf("the size of output array is too small\n");
		return 0;
	}
	char *p = Hex2Int(in_buf,in_buf,2*in_len);
	if(p==NULL)
		return 0;
	//解密数据  
	while(in_len >AES_BLOCK_SIZE)  
	{  
		memcpy(in, in_buf, AES_BLOCK_SIZE);  
		in_len -= AES_BLOCK_SIZE;  
		in_buf += AES_BLOCK_SIZE;  

		isSuccess = EVP_DecryptUpdate(&ctx, out_buf + outl_total, &outl, in, AES_BLOCK_SIZE);  
		if(!isSuccess)  
		{  
			printf("EVP_DecryptUpdate() failed");  
			EVP_CIPHER_CTX_cleanup(&ctx);  
			return 0;  
		}  
		outl_total += outl;  
	}


	if(in_len > 0)  
	{  
		memcpy(in, in_buf, in_len);  
		isSuccess = EVP_DecryptUpdate(&ctx, out_buf + outl_total, &outl, in, in_len);  
		outl_total += outl;  
	} 

	/*解密数据块不为16整数倍时执行 */
	if(in_len % AES_BLOCK_SIZE != 0)  
	{  
		isSuccess = EVP_DecryptFinal_ex(&ctx, out_buf + outl_total, &outl);  
		if(!isSuccess)  
		{  
			printf("EVP_DecryptFinal_ex() failed\n");  
			EVP_CIPHER_CTX_cleanup(&ctx);  
			return 0;  
		}
		outl_total += outl;  
	}  
	EVP_CIPHER_CTX_cleanup(&ctx);  
	return outl_total;  
}

