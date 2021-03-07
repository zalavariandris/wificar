
#ifndef CAR
#define CAR
#include <Arduino.h>
#include <vector>
#include <deque>
#include <utility> //std::pair

enum CarState{MANUAL, REVERT};

struct Vector2D{
  float x;
  float y;
};

class Car{
private:
  int maxSpeed {1023};

  int LEFT_PWR;
  int LEFT_DIR;
  int RIGHT_PWR;
  int RIGHT_DIR;

  int WALL_SENSOR;
  CarState state;
  std::deque<std::pair<int, int>> history;
  int historySize = 25;
  void setEngines(int left, int right);

  void differentialSteer(float x, float y, float * left,float * right);

public:
  Car();
  
  void setup();
  void update();
  void handleMessage(char* msg);
  void handleSensors();

  CarState getState();
  bool getWallSensor();
};
#endif
