#ifndef HMACSHA256_H
#define HMACSHA256_H


#include "mbedtls/md.h"



char hash[65];
char codigo[5];

char mensaje1[1000];

char* strComputeHMAC(char* key, char* payload){
	
	byte hmacResult[32];

	mbedtls_md_context_t ctx;
	mbedtls_md_type_t md_type = MBEDTLS_MD_SHA256;

	const size_t payloadLength = strlen(payload);
	const size_t keyLength = strlen(key);            
	mbedtls_md_init(&ctx);
	mbedtls_md_setup(&ctx, mbedtls_md_info_from_type(md_type), 1);
	mbedtls_md_hmac_starts(&ctx, (const unsigned char *) key, keyLength);
	mbedtls_md_hmac_update(&ctx, (const unsigned char *) payload, payloadLength);
	mbedtls_md_hmac_finish(&ctx, hmacResult);
	mbedtls_md_free(&ctx);

	for(int i = 0; i< sizeof(hmacResult); i++){
		char str[3];
		int x = 0;
		sprintf(str, "%02x", (int)hmacResult[i]);
		
		if(i==0){strcpy(mensaje1,str);}
		else{
			strcat(mensaje1, str);
		}
	}
	
	return (mensaje1);

}

char* strGetMessageFromRaw(char* msg_ds){ //ds=digital signature
	
	int longitud = strlen(msg_ds);
	
	for(int i = 64; i < longitud+1 ; i++){
		
		codigo[i-64]=msg_ds[i];

	}	
	
	return(codigo);
		
}


char* get_digital_sig(char* payload1){
	
	
	int longitud = strlen(payload1);
	
	if (longitud<65) {return 0;}
	else{
	
		for(int i=0; i<64;i++){
			
			hash[i]=payload1[i];
		}	
		hash[65]='\0';
		return(hash);
	}
}

bool comparacion(char *primero, char *segundo){

	for(int i = 0; i < 64; i++){
		
		if( primero[i] != segundo[i]) return false;
	}
	return true;	
}


#endif
