#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h> 
#include <ESP8266WebServer.h>
#include <FS.h>

int MAX_SPEED = 1023;

// network credentials
const char* ssid = "wifiitthon-2.4GHz";
const char* password = "Grimp465M";
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

class Car{
  public:
  Car(){
  }

  void setup(){
      pinMode(D1, OUTPUT);
      pinMode(D2, OUTPUT);
      pinMode(D3, OUTPUT);
      pinMode(D4, OUTPUT);
      MoveStop();
  }

  void SetEngines(int left, int right){
    /*handling values from -1023 to +1023*/
    Serial.printf("SetEngines: [%d - %d]\n", left, right);
    // left engine
    digitalWrite(D3, left < 0 ? LOW : HIGH);
    analogWrite(D1, abs(left));
  
    // right engine
    digitalWrite(D4, right < 0 ? LOW : HIGH);
    analogWrite(D2, abs(right));
    
  }
  
  void MoveForwardLeft(){
    //left
    digitalWrite(D3, HIGH);
    analogWrite(D1, MAX_SPEED/2);
    
    //right
    digitalWrite(D3, HIGH);
    analogWrite(D2, MAX_SPEED);
  }
  
  void MoveForward(){
    Serial.println("Car->MoveForward");
    //left
    digitalWrite(D3, LOW);
    analogWrite(D1, MAX_SPEED);
    
    //right
    digitalWrite(D4, LOW);
    analogWrite(D2, MAX_SPEED);
  }
  
  void MoveForwardRight(){
      //left
    digitalWrite(D3, HIGH);
    analogWrite(D1, MAX_SPEED);
    
    //right
    digitalWrite(D4, HIGH);
    analogWrite(D2, MAX_SPEED/2);
  }
  
  void RotateLeft(){
    //left
    digitalWrite(D3, LOW);
    analogWrite(D1, MAX_SPEED);
  
    // right
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
    digitalWrite(D4, LOW);
    analogWrite(D2, MAX_SPEED);
  }
  
  void MoveBackwardLeft(){
    //left
    digitalWrite(D3, LOW);
    analogWrite(D1, MAX_SPEED/2);
    
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
  
  void MoveBackwardRight(){
    //left
    digitalWrite(D3, LOW);
    analogWrite(D1, MAX_SPEED);
    
    //right
    digitalWrite(D4, LOW);
    analogWrite(D2, MAX_SPEED/2);
  }
  
  bool HandleCommand(String command){
    Serial.print("handleCommand: ");
    Serial.println(command);
    if(command=="MoveStop"){
      MoveStop();
      return true;
    }else if(command=="MoveForward"){
      MoveForward();
      return true;
    }if(command=="MoveForwardLeft"){
      MoveForwardLeft();
      return true;
    }if(command=="MoveForwardRight"){
      MoveForwardRight();
      return true;
    }if(command=="RotateLeft"){
      RotateLeft();
      return true;
    }if(command=="RotateRight"){
      RotateRight();
      return true;
    }if(command=="MoveBackwardLeft"){
      MoveBackwardLeft();
      return true;
    }if(command=="MoveBackward"){
      MoveBackward();
      return true;
    }if(command=="MoveBackwardRight"){
      MoveBackwardRight();
      return true;
    }else{
      return false;
    }
  }
};

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
    if( car.HandleCommand(cmd) ){
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
    car.SetEngines(left, right);
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
