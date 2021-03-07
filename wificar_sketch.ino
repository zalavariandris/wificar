#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h> 
#include <WebSocketsServer.h>  
#include <FS.h>
                       
#include "car.h"

// network credentials
const char* ssid = "ANDRISSURFACE";//"wifiitthon-2.4GHz";
const char* password = "masa2masa";//"Grimp465M";
//const char* ssid = "wifiitthon-2.4GHz";
//const char* password = "Grimp465M";
//const char* ssid = "bpgal2";
//const char* password = "lajos158";
//const char* ssid = "Bpg3";
//const char* password = "lajos158";

ESP8266WebServer server(80); // create webserver object on port 80
WebSocketsServer ws(81); // create  a websocket server pn port 81
WiFiManager wifiManager;

// Car
Car car;

void connectToWiFi(){
    /* WIFI */

    /* Station Mode */
    Serial.println("");
    Serial.print("Connecting to WiFi: ");
    Serial.println(ssid);

    WiFi.mode(WIFI_STA);

    // Configure static IP address
//    WiFi.persistent(false);
//    WiFi.setAutoConnect(false);
//    IPAddress ip(192, 168, 137, 200);   
//    IPAddress gateway(192,168,137,1);   
//    IPAddress subnet(255,255,255,0);   
//    if (!WiFi.config(ip, gateway, subnet)) {
//      Serial.println("STA Failed to configure");
//    }

    // Connect to Wi-Fi network with SSID and password
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.print("WiFi connected, IP address: ");
    Serial.println(WiFi.localIP());
    Serial.print("  Subnet Mask: ");
    Serial.println(WiFi.subnetMask());
    Serial.print("  Gateway IP: ");
    Serial.println(WiFi.gatewayIP());
    Serial.print("  MAC Address:  ");
    Serial.println(WiFi.macAddress());
}

void startAccessPoint(){
  WiFi.softAP("WiFiCar Access Point");
  Serial.print("AccessPoint: ");
  Serial.print("WiFiCar");
  Serial.println("started");
  Serial.print("IP address: ");
  Serial.println(WiFi.softAPIP());
}

void startFileServer(){
  /* FILE SYSTEM*/
    // Initialize SPIFFS
    if(!SPIFFS.begin()){
        Serial.println("An Error has occurred while mounting SPIFFS");
        return;
    }

    server.on("/config", [](){
      String html = "<!DOCTYPE html> <html>\n";
      html +="<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
      html +="<title>LED Control</title>\n";
      html +="</head>\n";
      html +="<body>\n";

      html += "SSID:"+WiFi.SSID()+"\n";
      html += "ip:"+WiFi.localIP().toString()+"\n";

      html +="</body>\n";
      html +="</html>\n";
      server.send(200, "text/html", html);
    });
    // serve static files from root folder
    server.serveStatic("/", SPIFFS, "/", "max-age=86400");
    
    server.onNotFound([](){
        server.send(404, "text/plain", "404: Not Found");
    });

    server.begin();
    Serial.println("HTTP Server started!");
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t* payload, size_t length){
    switch(type){
      case WStype_DISCONNECTED:
        Serial.printf("[%u] Disconnected!\n", num);
        break;
      case WStype_CONNECTED: {
          IPAddress ip = ws.remoteIP(num);
          Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);
        }
        break;
      case WStype_TEXT:
        if(strcmp((char*)payload, "__ping__") == 0){
          ws.sendTXT(0, "__pong__");
        }
        car.handleMessage((char*)payload);
        break;
    }
}

void setup() {
    /*Logging*/
    Serial.begin(9600);
    delay(500);

    // startAccessPoint();
    connectToWiFi();
    startFileServer();
    ws.begin();
    ws.onEvent(webSocketEvent);
    Serial.println("WebSocket server started.");
    car.setup();
}

void loop() {
  // Handle websocket
  ws.loop();
  
  // Handle webserver
  server.handleClient();

  // check car sensors
  car.update();
  
  // send value changes to clients
  static CarState last_state{car.getState()};
  CarState current_state = car.getState();
  if(last_state!=current_state){
    Serial.print("State changes");
    
    switch(current_state){
      case MANUAL:
        ws.sendTXT(0, "{\"state\": \"MANUAL\"}");
        Serial.println("MANUAL");
      break;
      case REVERT:
        ws.sendTXT(0, "{\"state\": \"REVERT\"}");
        Serial.println("REVERT");
      break;
    }
    last_state = current_state;
  }
  
  static bool last_wall_sensor = car.getWallSensor();
  bool current_wall_sensor = car.getWallSensor();
  if(last_wall_sensor!=current_wall_sensor){
    Serial.print("wall: ");
    Serial.println(current_wall_sensor);
    ws.sendTXT(0, current_wall_sensor ? "{\"wall\": true}" : "{\"wall\": false}");
    last_wall_sensor = current_wall_sensor;
  }
}
