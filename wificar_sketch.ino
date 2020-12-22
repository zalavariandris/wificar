#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h> 
#include <ESP8266WebServer.h>
#include <FS.h>

#include "car.h"

int MAX_SPEED = 1023;

// network credentials
const char* ssid = "ANDRISSURFACE";//"wifiitthon-2.4GHz";
const char* password = "alisca22";//"Grimp465M";
// port default is 80
const int port=80;

// Create webserver object on port 80
ESP8266WebServer server(port);
// Set your Static IP address
IPAddress local_IP(192, 168, 1, 184);
// Set your Gateway IP address
IPAddress gateway(192, 168, 1, 1);

IPAddress subnet(255, 255, 0, 0);
IPAddress primaryDNS(8, 8, 8, 8); // optional
IPAddress secondaryDNS(8, 8, 4, 4); // optional





Car car;

void setup() {
    /*Logging*/
    Serial.begin(9600);
    delay(10);

    /* CAR */
    // setup pins for output
    car.setup();

    /* WIFI */
    /* mDNS */
    // Start the mDNS responder for wificar.local
    if (!MDNS.begin("wificar")) {             
        Serial.println("Error setting up MDNS responder!");
    }
    Serial.println("mDNS responder started");
    Serial.println("mDNS name: wificar.local");
    
    /* Station Mode */
    // fix static ip
//  if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) {
//    Serial.println("STA Failed to configure");
//  }

    Serial.println("");
    Serial.print("Connecting to WiFi: ");
    Serial.print(ssid);

    // Connect to WiFi network
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.print("WiFi connected, IP address: ");
    Serial.println(WiFi.localIP());
    Serial.print("ESP Board MAC Address:  ");
    Serial.println(WiFi.macAddress());

    /* FILE SYSTEM*/
    // Initialize SPIFFS
    if(!SPIFFS.begin()){
        Serial.println("An Error has occurred while mounting SPIFFS");
        return;
    }

    /* SERVER */
    server.on("/command", [](){
        String cmd = server.arg("cmd");
        Serial.printf("received command: %s\n", cmd.c_str());
        if( car.handleCommand(cmd) ){
            server.send(200, "OK", "text/plain");
        }else{
            server.send(400, "No such command", "text/plain");
        }
    });
    
    server.on("/engines", [](){
        Serial.print("received engines");
        int left = server.arg("left").toInt();
        int right = server.arg("right").toInt();
        Serial.printf("left: %d, right: %d\n", left, right);
        car.setEngines(left, right);
        server.send(200, "OK", "text/plain");
    });

    server.serveStatic("/", SPIFFS, "/", "max-age=86400");
    
    server.onNotFound([](){
        server.send(404, "text/plain", "404: Not Found");
    });

    server.begin();
    Serial.println("HTTP Server started!");
}


void loop() {
    server.handleClient();
}
