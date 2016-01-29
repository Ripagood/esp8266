/*
 *  This sketch sends data via HTTP GET requests to data.sparkfun.com service.
 *
 *  You need to get streamId and privateKey at data.sparkfun.com and paste them
 *  below. Or just customize this script to talk to other HTTP servers.
 *
 */

#include <ESP8266WiFi.h>
#include <stdio.h>
#include <string.h>


const char* ssid     = "AXTEL XTREMO";
const char* password = "2092353485";

char* host = "";
char* key   = "Server";

boolean server,IpSent = false;
String IP1,IP2,IP3,myString;
String Command = "Command";
int ultimoOct = 1;

void setup() {
  pinMode(2, OUTPUT);
  Serial.begin(115200);
  delay(10);

  // We start by connecting to a WiFi network

  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");    
  }

  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
    
  myString =  ipToString(WiFi.localIP());
  int uno = myString.indexOf('.');
  int dos = myString.indexOf('.', uno+1);
  int tres = myString.indexOf('.', dos+1);
  int cuatro = myString.indexOf('.', tres+1);

  IP1 = myString.substring(0, uno);
  IP2 = myString.substring(uno+1, dos);
  IP3 = myString.substring(dos+1, tres);
  String IPtemp = myString.substring(tres+1, cuatro);
  int IPtemp1 = IPtemp.toInt() - 5;
  ultimoOct = IPtemp1 + 1;
 
  String address_ip = IP1 + "." + IP2 + "." + IP3 + "." + String(IPtemp1);
  int str_len = address_ip.length() + 1;
  address_ip.toCharArray(host, str_len);
  

  
}

void loop() {
  Serial.print("connecting to ");
  Serial.println(host);

  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    checkServer();
    return;
  }
  
  // We now create a URI for the request
  String url = "/?";
  url += Command + "=";
  url += key;
  
  Serial.print("Requesting URL: ");
  Serial.println(url);
  
  // This will send the request to the server
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" + 
               "Connection: close\r\n\r\n");
  
  // Read all the lines of the reply from server and print them to Serial
  int count = 0;
  boolean flag = false;
  while(client.available()){    
    String line;      
    line = client.readStringUntil('\r'); 
    count = count + 1;
    if (count == 7){
      Serial.println(line);
      count = 0;
      for (int i=0;i<(line.length()-1);i++){
        if(server == false){
          if (line.substring(i) == "Right") {
          Serial.println("Servidor encontrado");  
          server = true;
          }
        }
        if(server == true){
			if(IpSent==false){
				Command = "New_IP";
				int str_len1 = myString.length() + 1;
				myString.toCharArray(key, str_len1);
				delay(30);
			}          
        }        
      }
      
    }
    
    
  }
  
  Serial.println();
  Serial.println("closing connection");
  checkServer();
}

void checkServer(){
  if(server == false){
     String address_ip = IP1 + "." + IP2 + "." + IP3 + "." + ultimoOct;
     int str_len = address_ip.length() + 1;
     address_ip.toCharArray(host, str_len);
     ultimoOct = ultimoOct + 1;
     if(ultimoOct == 256){
        ultimoOct = 1;  
     }
  }
}

String ipToString(IPAddress ip){
  String s="";
  for (int i=0; i<4; i++)
    s += i  ? "." + String(ip[i]) : String(ip[i]);
  return s;
}
