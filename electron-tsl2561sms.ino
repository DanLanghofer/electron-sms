// This #include statement was automatically added by the Spark IDE.
#include "SFE_TSL2561.h"

#define MAX_PHONE_NUMBER    14
#define CTRL_Z 0x1A
#define TIMEOUT 10000

SFE_TSL2561 tsl = SFE_TSL2561();
unsigned char id;
boolean gain;     // Gain setting, 0 = X1, 1 = X16;
unsigned int ms;  // Integration ("shutter") time in milliseconds
unsigned int data1;
unsigned int data2;
char szPhoneNumber[MAX_PHONE_NUMBER] = "<<replace your phone number with country code";

int lowerLimit = 10;
boolean smsSend = false;

int callback(int type, const char* buf, int len, char* param){  
    Serial.print("Return: ");
    Serial.write((const uint8_t*)buf, len);
    Serial.println();
    
    return WAIT;
}

int setLowerLimit(String args){
    lowerLimit = args.toInt();
    
    return lowerLimit;
}

int sendMessage(char* pMessage){
    char szCmd[64];
    
    sprintf(szCmd, "AT+CMGS=\"+%s\",145\r\n", szPhoneNumber);
    
    Serial.print("Sending command ");
    Serial.print(szCmd);
    Serial.println();
    
    char szReturn[32] = "";
    
    Cellular.command(callback, szReturn, TIMEOUT, "AT+CMGF=1\r\n");
    Cellular.command(callback, szReturn, TIMEOUT, szCmd);
    Cellular.command(callback, szReturn, TIMEOUT, pMessage);
    
    sprintf(szCmd, "%c", CTRL_Z);
    
    int retVal = Cellular.command(callback, szReturn, TIMEOUT, szCmd);
    
    if(RESP_OK == retVal){
        Serial.println("+OK, Message Send");
    }
    else{
        Serial.println("+ERROR, error sending message");
    }
    
    return retVal;
}

void setup() {
    Serial.begin(115200);
    tsl.begin();
    
    tsl.getID(id);
    tsl.setTiming(0, 2, ms);
    tsl.setPowerUp();
    
    Spark.function("setmin", setLowerLimit);
}

void loop() {
    tsl.getData(data1, data2);
    
    double lux;
    boolean good = tsl.getLux(gain,ms,data1,data2,lux);
    
    Serial.println();
    Serial.print("Light: ");
    Serial.print(lux);
    Serial.print(" lux");
    Serial.println();
    
    if(lux < lowerLimit && lux != 0){
        if(!smsSend){
			char szMessage[64];
			
			sprintf(szMessage, "It's too dark here (%.2f lux)", lux);
			
			sendMessage(szMessage);
            
            smsSend = true;
        }
    } 
    else{
        smsSend = false;
    }
    
    delay(1000);
}