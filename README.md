**Sending SMS using Particle Electron**
-----------------------------------

Introduction
------------
Particle Electron is a Cellular IoT Development kit. It comes with a SIM card with global data plan. You can use your own SIM card and activate it if you like. It is same as Particle Core/Photon with an exception, it uses Cellular technology instead of WiFi as in the case of Core/Photon. It means that you can go use the potential of the Core/Photon development board and go outside, you are not depending on the WiFi no longer. It uses the U-Blox SARA U-series (3G) or G-series (2G) module instead of WiFi.

It is backward compatible with exiting products. You can use the same APIs to program it. One of the interesting API comes with it is the Cellular API. With Cellular API you can turn on/off the cellular module, read the RSSI, set custom APN in case you are using your own SIM card, etc... There is an interesting API *Cellular.command*. This API allows you to send AT commands directly to the cellular module. This is a powerful API, especially it gives you to the ability to send AT commands supported by the U-Blox cellular module and parse the response.

Here is a sample application to text messages (SMS) using the *Cellular.command* API. You can refer to this [U-Blox Commands Manual](https://www.u-blox.com/en/product-resources/2432?f%5B0%5D=field_file_category:210) and this [Example Application Note](https://www.u-blox.com/sites/default/files/AT-CommandsExamples_AppNote_%28UBX-13001820%29.pdf) for the supported AT Commands. From the document you can see that ***+CMGS*** command can be used to send direct message. Format to send this AT Command is:

    AT+CMGS="<number>"<129/145><CR><message body><ctrl+z>

An Example is:
    
    AT+CMGS="+00123456789",145<CR>Message Body<Ctrl+Z>

The first part `AT+MGS="123456789",145<CR>` specifies the telephone number and its address type. Address type can be 129 or 145. The type 129 specifies the number is formatted with ISDN/ITU but does not specify whether it is international, national or other type. The type 145 specifies this is ISDN/ITU formatted and is an international number starts with "+" symbol. The last character in the first part is Carriage Return (CR), ASCII code 13 (0x0D). 

The second part is the message body terminates with Ctrl+Z ASCII Code 26 (0x1A). 

Sample Application
------------------
The sample application uses a TSL256 Luminosity sensor to measure light value, If it is below a configurable minimum value, it will send text message to a predefined telephone number. A sample text message is 

    It's too dark here (9.25 lux)

Here is the a code snippet from the application that sends text message:

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

Schematics
----------

![Schematics](https://raw.githubusercontent.com/krvarma/electron-sms/master/schematics.png)

Screenshot
----------

![Screenshot](https://github.com/krvarma/electron-sms/blob/master/screenshot.jpg)

Demo Video
----------

https://www.youtube.com/watch?v=njswhDRzbVM
