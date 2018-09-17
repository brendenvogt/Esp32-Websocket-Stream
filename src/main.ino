/*
    Name: main.ino
    Author: Brenden Vogt
    Date: September 17, 2018
*/

//declare wifi related variables
#include <WiFi.h>
const char* ssid     = "Esp32";
const char* password = "testpass";
WiFiServer server(80);

//declare socket related variables
#include <WebSocketServer.h>
WebSocketServer webSocketServer;

//other libs
#include <string> 

//set defines for basic config
#define READ_PIN 32
#define LED 2

bool streaming = false;
int dataVal = 0;

// init Serial comms
void initSerial(){
    Serial.begin(115200);
}

// init gpio for led and other functions
void initGpio(){
    pinMode(LED, OUTPUT);
}

// init wifi with access point mode
void initWifi(){

    WiFi.softAP(ssid, password);
    Serial.print("Broadcasting AP SSID: ");
    Serial.println(ssid);

    IPAddress IP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(IP);
    
    server.begin();
}

void setup() {
    initSerial();
    initGpio();
    initWifi();
}

void loop() {
    // listen for incoming clients
    WiFiClient client = server.available();   
    if (client.connected() && webSocketServer.handshake(client)) {

        IPAddress ip = client.remoteIP();
        Serial.print("Connecting to: ");
        Serial.println(ip);

        String data;      
        while (client.connected()) {
            
            if (streaming){
                dataVal++;
                webSocketServer.sendData(String(dataVal));
            }        

            data = webSocketServer.getData();
            if (data.length() > 0) {
                Serial.println(data);
                if (data == "start"){
                    digitalWrite(LED, HIGH);
                    streaming = true;
                }else if (data == "stop"){
                    digitalWrite(LED, LOW);
                    streaming = false;
                }else if (data == "reset"){
                    dataVal = 0;
                }
            }

        }

        Serial.print("Disconnecting from: ");
        Serial.println(ip);
        delay(100);
    }
}
