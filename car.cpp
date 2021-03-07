#include "car.h"
#include <numeric>

Car::Car(){
    LEFT_PWR = D1;
    LEFT_DIR = D3;
    RIGHT_PWR = D2;
    RIGHT_DIR = D4;
    WALL_SENSOR = D5;
}

bool Car::getWallSensor(){
  return digitalRead(WALL_SENSOR)<1;
}

CarState Car::getState(){
  return state;
}

void Car::setup(){
    pinMode(LEFT_PWR, OUTPUT);
    pinMode(LEFT_DIR, OUTPUT);
    pinMode(RIGHT_PWR, OUTPUT);
    pinMode(RIGHT_DIR, OUTPUT);
    setEngines(0,0);
}

void Car::update(){
  // UPDATE STATE MACHINE
  if(state==MANUAL){
    if(false && getWallSensor() && history.size()>0){
      state=REVERT;
    }
  }else if(state==REVERT){
    if(history.size()>0){
      auto last_element = history.back();
      history.pop_back();
      setEngines(-last_element.first, -last_element.second);
      delay(10);
    }else{
      setEngines(0,0);
      state=MANUAL;
    }
  };
}

void Car::setEngines(int left, int right){
    /*handling values from -1023 to +1023*/
    // left engine
    digitalWrite(LEFT_DIR, left < 0 ? HIGH : LOW);
    analogWrite(LEFT_PWR, abs(left));

    // right engine
    digitalWrite(RIGHT_DIR, right < 0 ? HIGH : LOW);
    analogWrite(RIGHT_PWR, abs(right));

//    Serial.print("setEngines: ");
//    Serial.print(left);
//    Serial.print(", ");
//    Serial.println(right);
}

void Car::handleMessage(char* msg){
  if(state==MANUAL){
    char *ptr = strtok(msg, " ");
    if(strcmp(ptr, "engines")==0){
      // Handle driving engines direclty 'engines [left] [right]'
      // -------------------------------
  
      // parse engine values from message
      ptr = strtok(NULL, " ");
      int left = atoi(ptr);
      ptr = strtok(NULL, " ");
      int right = atoi(ptr);
  
      // set the engines
      setEngines(left, right);

      if(left!=0 || right!=0){
        std::pair<int, int> new_value{left, right};
        history.push_back(new_value);
        if(history.size()>historySize){
          history.pop_front();
        }
      }
    }
  }
}
