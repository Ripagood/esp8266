#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>
#include <WiFiManager.h> 

#define CONNECTION_NAME "KhanAP"

MDNSResponder mdns;
ESP8266WebServer server(80);

const char* ssid = "KHAN";
String st;
String content,Comodin;
int OutputPin = 13;
int ManualUpdate,KHANDeviceState = 0;
const char* host = "khansystems.com";
String key   = "testKey";
char authorized;

String ipToString(IPAddress ip){
  String s="";
  for (int i=0; i<4; i++)
    s += i  ? "." + String(ip[i]) : String(ip[i]);
  return s;
}

void setup() {


  Serial.begin(115200);
  EEPROM.begin(512);
  delay(10);
  Serial.println();
  Serial.println();
  Serial.println("Startup");
  pinMode(OutputPin, OUTPUT);
  digitalWrite(OutputPin, LOW);
/*
  // read eeprom for ssid and pass
  Serial.println("Reading EEPROM ssid");
  //EEPROM.write(70, 0);
  String esid;
  for (int i = 0; i < 32; ++i)
    {
      esid += char(EEPROM.read(i));
    }
  Serial.print("SSID: ");
  Serial.println(esid);
  Serial.println("Reading EEPROM pass");
  String epass = "";
  for (int i = 32; i < 64; ++i)
    {
      epass += char(EEPROM.read(i));
    }
  Serial.print("PASS: ");
  Serial.println(epass);  
  if ( esid.length() > 1 ) {
      // test esid 
      WiFi.begin(esid.c_str(), epass.c_str());
      if (testWifi()) { 
          launchWeb(0);
          return;
      }
  }
  setupAP(); 
*/


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
    Serial.println("Launching Web Server);

    launchWeb(0);




}


void launchWeb(int webtype) {
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("Local IP: ");
  Serial.println(WiFi.localIP());
  Serial.print("SoftAP IP: ");
  //Serial.println(WiFi.softAPIP());
  if (!mdns.begin("esp8266", WiFi.localIP())) {
    Serial.println("Error setting up MDNS responder!");
    while(1) { 
      delay(1000);
      Serial.println("Error setting up MDNS responder! Program Halted");
	
    }
  }
  Serial.println("mDNS responder started"); 
  createWebServer(webtype);
  // Start the server
  server.begin();
  Serial.println("Server started"); 
}

void setupAP(void) {
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  int n = WiFi.scanNetworks();
  Serial.println("scan done");
  if (n == 0)
    Serial.println("no networks found");
  else
  {
    Serial.print(n);
    Serial.println(" networks found");
    for (int i = 0; i < n; ++i)
     {
      // Print SSID and RSSI for each network found
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(WiFi.SSID(i));
      Serial.print(" (");
      Serial.print(WiFi.RSSI(i));
      Serial.print(")");
      Serial.println((WiFi.encryptionType(i) == ENC_TYPE_NONE)?" ":"*");
      delay(10);
     }
  }
  Serial.println(""); 
  st = "<ol>";
  for (int i = 0; i < n; ++i)
    {
      // Print SSID and RSSI for each network found
      st += "<li>";
      st += WiFi.SSID(i);
      st += " (";
      st += WiFi.RSSI(i);
      st += ")";
      st += (WiFi.encryptionType(i) == ENC_TYPE_NONE)?" ":"*";
      st += "</li>";
    }
  st += "</ol>";
  delay(100);
  WiFi.softAP(ssid);
  Serial.println("softap");
  launchWeb(1);
  Serial.println("over");
}

void createWebServer(int webtype)
{
  // Check for any mDNS queries and send responses
  mdns.update();

  if ( webtype == 1 ) {
    server.on("/", []() {
        IPAddress ip = WiFi.softAPIP();
        String ipStr = String(ip[0]) + '.' + String(ip[1]) + '.' + String(ip[2]) + '.' + String(ip[3]);
        content = "<!DOCTYPE HTML>\r\n<html>Hello from ESP8266 at ";
        content += ipStr;
        content += "<p>";
        content += st;
        content += "</p><form method='get' action='setting'><label>SSID: </label><input name='ssid' length=32><input name='pass' length=64><input type='submit'></form>";
        content += "</html>";
        server.send(200, "text/html", content);  
    });
    server.on("/setting", []() {
        String qsid = server.arg("ssid");
        String qpass = server.arg("pass");
        if (qsid.length() > 0 && qpass.length() > 0) {
          Serial.println("clearing eeprom");
          for (int i = 0; i < 64; ++i) { EEPROM.write(i, 0); }
          Serial.println(qsid);
          Serial.println("");
          Serial.println(qpass);
          Serial.println("");

          Serial.println("writing eeprom ssid:");
          for (int i = 0; i < qsid.length(); ++i)
            {
              EEPROM.write(i, qsid[i]);
              Serial.print("Wrote: ");
              Serial.println(qsid[i]); 
            }
          Serial.println("writing eeprom pass:"); 
          for (int i = 0; i < qpass.length(); ++i)
            {
              EEPROM.write(32+i, qpass[i]);
              Serial.print("Wrote: ");
              Serial.println(qpass[i]); 
            }    
          EEPROM.commit();
          content = "<!DOCTYPE HTML>\r\n<html>";
          content += "<p>saved to eeprom... reset to boot into new wifi</p></html>";
        } else {
          content = "Error";
          Serial.println("Sending 404");
        }
        server.send(200, "text/html", content);
    });
  } else {
    server.on("/", []() {
	 String temp="";
	 if(server.arg("State")){
			String Value = server.arg("State");
			if (Value == "ON"){
			 digitalWrite(OutputPin, HIGH);
			 KHANDeviceState = 1;
              Comodin = key + "ON";
			 if (authorized == 1){
				String Joker = CheckRemoteState("Update",Comodin);
				if (Joker!=String(KHANDeviceState)){
					ManualUpdate = 1;
				}
			 }              
			 temp = "Changed to ON";    
			}if (Value == "OFF"){
			 digitalWrite(OutputPin, LOW);
			 KHANDeviceState = 0;
              Comodin = key + "OF";
			 if (authorized == 1){
				String Joker = CheckRemoteState("Update",Comodin);
				if (Joker!=String(KHANDeviceState)){
					ManualUpdate = 1;
				}
			 } 
			 temp = "Changed to OFF";
			}	
		}
	 Serial.println(temp);
      server.send(200, "text/plain", temp);
    });
	server.on("/KHAN", []() {
      server.send(200, "text/plain", "ACCEPTED");
    });
	server.on("/KHANSTATE", []() {
      server.send(200, "text/plain", String(KHANDeviceState));
    });
	server.on("/REMOTE", []() {
	 EEPROM.write(70, 1);
	 EEPROM.commit();
      server.send(200, "text/plain", "Remote Access ACCEPTED");
    });
	server.on("/NOREMOTE", []() {
	 EEPROM.write(70, 0);
	 EEPROM.commit();
	 ManualUpdate=0;
      server.send(200, "text/plain", "Remote Access DELETED");
    });
	server.on("/GETIP", []() {
      server.send(200, "text/plain", ipToString(WiFi.localIP()));
    });
    server.on("/setting", []() {
      server.send(200, "text/plain", "setting.");
    });
    server.on("/cleareeprom", []() {
      content = "<!DOCTYPE HTML>\r\n<html>";
      content += "<p>Clearing the EEPROM</p></html>";
      server.send(200, "text/html", content);
      Serial.println("clearing eeprom");
      for (int i = 0; i < 96; ++i) { EEPROM.write(i, 0); }
      EEPROM.commit();
    });
  }
}

String CheckRemoteState(String command, String Var){
	WiFiClient client;
	if (!client.connect(host, 80)) {
		Serial.println("connection failed");
		while(1);//halt the program
	}
	String line; 
	delay(100);
	String url = "/clienteQuery/index.php?";
	url += command + "=";
	url += Var;
	
	Serial.print("Requesting URL: ");
	Serial.println(host+url);
	String Ans;
	// This will send the request to the server
	client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" + 
               "Connection: close\r\n\r\n");
	 // Read all the lines of the reply from server and print them to Serial
	 int count = 0;
	 boolean flag = false;
	 while(client.available()){        
		line = client.readStringUntil('\r');
		Serial.println(line);
		count = count + 1;
		if (count == 7){	//Leer respuesta del servidor.
		 Serial.println(line);
		 Ans = line;
		 count = 0; 
		}
	 }
	Serial.println(Ans);
	return Ans;
}

void loop() {
  // put your main code here, to run repeatedly:
  //Serial.println("Reading EEPROM Autorizacion Consulta Web");
  authorized = char(EEPROM.read(70));
  if(ManualUpdate==1){
	Serial.println("Sincronizando...");
	EEPROM.write(70, 0);
	EEPROM.commit();
	String Joker = CheckRemoteState("Update",Comodin);
	if (Joker==String(KHANDeviceState)){
		ManualUpdate = 0;
		EEPROM.write(70, 1);
		EEPROM.commit();
		Serial.println("Sincronizado.");
	}
  }
  if (authorized == 1){
	Serial.println("Autorizado para cosulta de estado webclient");
	String CheckStillPermiso = CheckRemoteState("Version",key);
	if (CheckStillPermiso == "ON"){
		Serial.println("Autorizado para consulta web de estado modulos");
		String Value = CheckRemoteState("State",key);
		if (Value == "ON"){
			digitalWrite(OutputPin, HIGH); 
			KHANDeviceState = 1;
		}if (Value == "OFF"){
			digitalWrite(OutputPin, LOW);
			KHANDeviceState = 0;
		}
	}if (CheckStillPermiso == "OFF"){
		Serial.println("No autorizado para consulta web de estado modulo");
	}
  }
  server.handleClient();
}
