
#ifndef CAR
#define CAR
#include <Arduino.h>
class Car{
public:
  int maxSpeed {1023};
  Car();
  
  void setup();
  void setEngines(int left, int right);
  void moveForwardLeft();
  void moveForward();
  void moveForwardRight();
  void rotateLeft();
  void moveStop();
  void rotateRight();
  void moveBackwardLeft();
  void moveBackward();
  void moveBackwardRight();
  bool handleCommand(String command);
};
#endif
