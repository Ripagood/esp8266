#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino

//needed for library
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>         //https://github.com/tzapu/WiFiManager



#define CONNECTION_NAME   "AutoConnectAP"
#define TCP_HOST          "192.168.0.21"
#define TCP_PORT          10000


void setup() {
    // put your setup code here, to run once:
    Serial.begin(115200);

    //WiFiManager
    //Local intialization. Once its business is done, there is no need to keep it around
    WiFiManager wifiManager;
    //reset saved settings
    //wifiManager.resetSettings();
    
    //set custom ip for portal
    //wifiManager.setAPConfig(IPAddress(10,0,1,1), IPAddress(10,0,1,1), IPAddress(255,255,255,0));

    //fetches ssid and pass from eeprom and tries to connect
    //if it does not connect it starts an access point with the specified name
    //here  "AutoConnectAP"
    //and goes into a blocking loop awaiting configuration
    wifiManager.autoConnect(CONNECTION_NAME);
    //or use this for auto generated name ESP + ChipID
    //wifiManager.autoConnect();

    
    //if you get here you have connected to the WiFi
    Serial.println("connected...yeey :)");
}

void loop() {
    // put your main code here, to run repeatedly:
    Serial.println("You have connected");
    delay(100);

//The manager should scan the network for the new node
//Send either packets or GET requests to the slave node
//Wait for the manager to register this node
//The manager controls this node completely
//Therefore it should poll its status and then send the data to the server
    WiFiClient client;

    Serial.print("Connecting to :");
    Serial.println(TCP_HOST);

    if (!client.connect(TCP_HOST, TCP_PORT)) {
    Serial.println("connection failed");
    while(1); //halt the program
    }

    while(1){

    client.print("This is the message.  It will be repeated");  
    // if there are incoming bytes available
    // from the server, read them and print them:
    int readAmount;
    char receivedString[32];
    delay(500);
    
    }





    
    
}
