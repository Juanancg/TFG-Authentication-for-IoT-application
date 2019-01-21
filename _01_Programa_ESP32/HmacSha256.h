#ifndef HMACSHA256_H
#define HMACSHA256_H

#include "mbedtls/md.h"


class HmacSha256 {
	char hash[65];
	char message[12];
	char raw_message[65];
public:
	char* strComputeHMAC(char* key, String strpayload){

		/*Serial.println(key);
		Serial.println(payload);
		memset(raw_message, 0, strlen(raw_message));*/
		//char *raw_message;
		char * payload;
	    if(strpayload.length()!=0){
        	payload = const_cast<char*>(strpayload.c_str());
        
    	}
		byte hmacResult[32];
		mbedtls_md_context_t ctx;
		mbedtls_md_type_t md_type = MBEDTLS_MD_SHA256;

		size_t payloadLength = strlen(payload);
		size_t keyLength = strlen(key);            
		mbedtls_md_init(&ctx);
		mbedtls_md_setup(&ctx, mbedtls_md_info_from_type(md_type), 1);
		mbedtls_md_hmac_starts(&ctx, (const unsigned char *) key, keyLength);
		mbedtls_md_hmac_update(&ctx, (const unsigned char *) payload, payloadLength);
		mbedtls_md_hmac_finish(&ctx, hmacResult);
		mbedtls_md_free(&ctx);

		//memset(raw_message, 0, strlen(raw_message));
		for(int i = 0; i< sizeof(hmacResult); i++){
			char str[3];
			int x = 0;
			sprintf(str, "%02x", (int)hmacResult[i]);
			if(i==0){
				strcpy(raw_message,str);

			}else{
			  strcat(raw_message, str);
			}
		}
		return (raw_message);

	}

	char* strGetMessageFromRaw(char* msg_ds){ // ds = digital signature
		
		
		memset(message, 0, strlen(message));
		int longitud = strlen(msg_ds);

		for(int i = 64; i < longitud+1 ; i++){
			message[i-64]=msg_ds[i];
		} 
		message[longitud-64] = '\0';
		return(message);
	    
	}


	char* get_digital_sig(char* payload1){
		
		memset(hash, 0, strlen(hash));
		int longitud = strlen(payload1);
		if (longitud<65) {
			return 0;

		} else{
			for(int i = 0; i < 64; i++){
				hash[i]=payload1[i];
			} 

			hash[65]='\0';
			return(hash);
		}
	}

	bool comparacion(char *primero, char *segundo){
		for(int i = 0; i < 64; i++){
			if( primero[i] != segundo[i]) {
				return false;
			}
		}
		return true;  
	}

	/*********************************************FUNCTION******************************************//**
	*	\brief Function that compares HMAC received from MQTT with the HMAC that should be
	*	\return true if is authentic false if not
	***************************************************************************************************/    
	bool bCheckAuth(char *key ,char *mensaje_to_check) {
		// If the message lenght is less than 64, the message doesnt have the HMAC
      	if(strlen(mensaje_to_check) > 64){

			char* strHMACReceived;
			char* strHMACReal;
			/* EXTRAEMOS LA FIRMA DIGITAL */
			strHMACReceived = get_digital_sig(mensaje_to_check);
			/* GENERAMOS LA FIRMA DIGITAL QUE DEBERÍA SER */
			strHMACReal = strComputeHMAC(key, strGetMessageFromRaw(mensaje_to_check));
			/* COMPARAMOS AMBAS FIRMAS */
			return comparacion(strHMACReceived, strHMACReal);

		} else{
			return false;
		}
	}

};
#endif
