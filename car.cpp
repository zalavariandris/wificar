#include "car.h"


Car::Car(){}

void Car::setup(){
    pinMode(D1, OUTPUT);
    pinMode(D2, OUTPUT);
    pinMode(D3, OUTPUT);
    pinMode(D4, OUTPUT);
    moveStop();
}

void Car::setEngines(int left, int right){
    /*handling values from -1023 to +1023*/
    Serial.printf("SetEngines: [%d - %d]\n", left, right);
    // left engine
    digitalWrite(D3, left < 0 ? LOW : HIGH);
    analogWrite(D1, abs(left));

    // right engine
    digitalWrite(D4, right < 0 ? LOW : HIGH);
    analogWrite(D2, abs(right));
}

void Car::moveForwardLeft(){
    //left
    digitalWrite(D3, HIGH);
    analogWrite(D1, maxSpeed/2);
    
    //right
    digitalWrite(D3, HIGH);
    analogWrite(D2, maxSpeed);
}

void Car::moveForward(){
    Serial.println("Car->MoveForward");
    //left
    digitalWrite(D3, LOW);
    analogWrite(D1, maxSpeed);
    
    //right
    digitalWrite(D4, LOW);
    analogWrite(D2, maxSpeed);
}

void Car::moveForwardRight(){
    //left
    digitalWrite(D3, HIGH);
    analogWrite(D1, maxSpeed);
    
    //right
    digitalWrite(D4, HIGH);
    analogWrite(D2, maxSpeed/2);
}

void Car::rotateLeft(){
    //left
    digitalWrite(D3, LOW);
    analogWrite(D1, maxSpeed);

    // right
    digitalWrite(D4, HIGH);
    analogWrite(D2, maxSpeed);
}

void Car::moveStop(){
    //left
    digitalWrite(D3, LOW);
    analogWrite(D1, 0);

    // right
    digitalWrite(D4, LOW);
    analogWrite(D2, 0);
}

void Car::rotateRight(){
    //left
    digitalWrite(D3, HIGH);
    analogWrite(D1, maxSpeed);

    // right
    digitalWrite(D4, LOW);
    analogWrite(D2, maxSpeed);
}

void Car::moveBackwardLeft(){
    //left
    digitalWrite(D3, LOW);
    analogWrite(D1, maxSpeed/2);
    
    //right
    digitalWrite(D4, LOW);
    analogWrite(D2, maxSpeed);
}

void Car::moveBackward(){
    //left
    digitalWrite(D3, HIGH);
    analogWrite(D1, maxSpeed);
    
    //right
    digitalWrite(D4, HIGH);
    analogWrite(D2, maxSpeed);
}

void Car::moveBackwardRight(){
    //left
    digitalWrite(D3, LOW);
    analogWrite(D1, maxSpeed);
    
    //right
    digitalWrite(D4, LOW);
    analogWrite(D2, maxSpeed/2);
}

bool Car::handleCommand(String command){
    Serial.print("handleCommand: ");
    Serial.println(command);
    if(command=="MoveStop"){
        moveStop();
        return true;
    }else if(command=="MoveForward"){
        moveForward();
        return true;
    }if(command=="MoveForwardLeft"){
        moveForwardLeft();
        return true;
    }if(command=="MoveForwardRight"){
        moveForwardRight();
        return true;
    }if(command=="RotateLeft"){
        rotateLeft();
        return true;
    }if(command=="RotateRight"){
        rotateRight();
        return true;
    }if(command=="MoveBackwardLeft"){
        moveBackwardLeft();
        return true;
    }if(command=="MoveBackward"){
        moveBackward();
        return true;
    }if(command=="MoveBackwardRight"){
        moveBackwardRight();
        return true;
    }else{
        return false;
    }
}
