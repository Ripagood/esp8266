#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

const char *ssid = "AXTEL XTREMO";
const char *password = "2092353485";
String clientes = "";
MDNSResponder mdns;

ESP8266WebServer server ( 80 );

void handleRoot() {
    String temp="";        
		if(server.arg("Command")){
			String Value = server.arg("Command");
			if (Value == "Server"){
			  temp = "Right";    
			}else{
			  temp = "Wrong";
			}	
		}
        if(server.arg("New_IP")){		// HAZLO CON STRINGS, UNA SOLA Y LARGA STRING QUE TENGA TODO //// hacer pruebas
			String Value = server.arg("New_IP");
			for (int i=0;i<(clientes.length()-1);i++){
			  if (clientes.substring(i) != Value) {
				  int pos = clientes.indexOf(Value);
				  clientes = clientes + " " + Value + " " + "OF";
				  Serial.println (clientes);
				  temp = "Registred";
				  
			  }else{
				  temp = clientes;
			  }				
			}
		}
        if(server.arg("IP_State")){
			String Value = server.arg("IP_State");			
		}
	Serial.println (temp);
	server.send ( 200, "text/html", temp );
}

void handleNotFound() {
	String message = "File Not Found\n\n";
	message += "URI: ";
	message += server.uri();
	message += "\nMethod: ";
	message += ( server.method() == HTTP_GET ) ? "GET" : "POST";
	message += "\nArguments: ";
	message += server.args();
	message += "\n";

	for ( uint8_t i = 0; i < server.args(); i++ ) {
		message += " " + server.argName ( i ) + ": " + server.arg ( i ) + "\n";
	}

	server.send ( 404, "text/plain", message );
}

void setup ( void ) {
	Serial.begin ( 115200 );
	WiFi.begin ( ssid, password );
	Serial.println ( "" );

	// Wait for connection
	while ( WiFi.status() != WL_CONNECTED ) {
		delay ( 500 );
		Serial.print ( "." );
	}

	Serial.println ( "" );
	Serial.print ( "Connected to " );
	Serial.println ( ssid );
	Serial.print ( "IP address: " );
	Serial.println ( WiFi.localIP() );

	if ( mdns.begin ( "esp8266", WiFi.localIP() ) ) {
		Serial.println ( "MDNS responder started" );
	}

	server.on ( "/", handleRoot );
	//server.on ( "/test.svg", drawGraph );
	server.on ( "/inline", []() {
		server.send ( 200, "text/plain", "this works as well" );
	} );
	server.onNotFound ( handleNotFound );
	server.begin();
	Serial.println ( "HTTP server started" );
}

void loop ( void ) {
	mdns.update();
	server.handleClient();
}