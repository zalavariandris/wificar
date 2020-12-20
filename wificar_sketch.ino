#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <FS.h>

int MAX_SPEED = 1023;

/* Utility function */
static inline int8_t sgn(int val) {
 if (val < 0) return -1;
 if (val==0) return 0;
 return 1;
}

/* networking*/
// network credentials
const char* ssid = "wifiitthon-2.4GHz";
const char* password = "Grimp465M";

// Create webserver object on port 80
AsyncWebServer server(80);

void MoveForward(){
  //left
  digitalWrite(D3, LOW);
  analogWrite(D1, MAX_SPEED);
  
  //right
  digitalWrite(D4, LOW);
  analogWrite(D2, MAX_SPEED);
}

void MoveBackward(){
  //left
  digitalWrite(D3, HIGH);
  analogWrite(D1, MAX_SPEED);
  
  //right
  digitalWrite(D4, HIGH);
  analogWrite(D2, MAX_SPEED);
}

void MoveStop(){
  //left
  digitalWrite(D3, LOW);
  analogWrite(D1, 0);

  // right
  digitalWrite(D4, LOW);
  analogWrite(D2, 0);
}

void RotateRight(){
    //left
  digitalWrite(D3, HIGH);
  analogWrite(D1, MAX_SPEED);

  // right
  digitalWrite(D4, HIGH);
  analogWrite(D2, 0);
}

void RotateLeft(){
    //left
  digitalWrite(D3, HIGH);
  analogWrite(D1, 0);

  // right
  digitalWrite(D4, HIGH);
  analogWrite(D2, MAX_SPEED);
}

void SetEngines(int left, int right){
  // left engine
  analogWrite(D1, abs(left));
  digitalWrite(D3, left < 0 ? LOW : HIGH);

  // right engine
  analogWrite(D2, abs(right));
  digitalWrite(D4, right < 0 ? LOW : HIGH);
}

void setup() {
  pinMode(D1, OUTPUT);
  pinMode(D2, OUTPUT);
  pinMode(D3, OUTPUT);
  pinMode(D4, OUTPUT);

  MoveStop();
  
  Serial.begin(115200);
  delay(10);

  // Initialize SPIFFS
  if(!SPIFFS.begin()){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  // Connect to WiFi network
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");


  // Start the server
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html", "text/html");
  });
  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/style.css", "text/css");
  });
  server.on("/vue.js", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/vue.js", "text/javascript");
  });
  
  server.on("/command", HTTP_POST, [](AsyncWebServerRequest *request){
    Serial.println("handle command:");
    int params = request->params();
    AsyncWebParameter* cmdParam = request->getParam("cmd");
    String command = cmdParam->value();

    if(command == "MoveStop"){MoveStop();}
    else if (command == "MoveForward" ){MoveForward(); }
    else if (command == "MoveBackward"){MoveBackward();}
    request->send(200);
  });
  
  server.on("/engines", HTTP_POST, [](AsyncWebServerRequest *request){
    Serial.println("Handle engines:");
    AsyncWebParameter* leftEngineParam = request->getParam("left");
    AsyncWebParameter* rightEngineParam = request->getParam("right");
    Serial.printf(" left:%s right: %s\n", leftEngineParam->value().c_str(), rightEngineParam->value().c_str());
    
    // Set engines
    SetEngines(leftEngineParam->value().toInt(), rightEngineParam->value().toInt()); 
    request->send(200);
  });
  server.begin();
  Serial.println("Server started");

  // Print the IP address
  Serial.print("Use this URL to connect: ");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");
}

void loop() {
  
}
