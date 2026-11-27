#include <Wire.h>
#include <Zumo32U4.h>

// ---------- Hardware ----------
Zumo32U4Motors motors;
Zumo32U4LineSensors lineSensors;
Zumo32U4ButtonA buttonA;

// ---------- Settings ----------
#define NUM_SENSORS 5
uint16_t sensorValues[NUM_SENSORS];

int speed     = 180;
int turnSpeed = 120;
int threshold = 750;

// ---------- What the sensors see ----------
bool leftBlocked;
bool frontBlocked;
bool rightBlocked;

// ---------- State ----------
// case 0 = drive straight until the robot reaches the black tape
// case 1 = solve the maze
int state = 0;


void setup()
{
  Serial.begin(9600);
  lineSensors.initFiveSensors();

  buttonA.waitForButton();
  delay(500);
}


void loop()
{
  readSensors();

  switch (state)
  {
    case 0:  // Drive straight until the robot reaches the black tape
      motors.setSpeeds(150, 150);

      if (leftBlocked || frontBlocked || rightBlocked)
      {
        motors.setSpeeds(0, 0);
        delay(200);
        state = 1;
      }
      break;

    case 1:  // Solve the maze: pick an action, then do it
      switch (chooseAction())
      {
        case 0:  // Wall in front: back up and turn 90 degrees
          backAndTurn90();
          break;

        case 1:  // Left is open: try left first
          turnLeft();
          break;

        case 2:  // Left is blocked, right is open: turn right
          turnRight();
          break;

        case 3:  // Dead end: turn around
          turnAround();
          break;
      }
      break;
  }
}


// Reads the five sensors and works out which sides are blocked
void readSensors()
{
  lineSensors.read(sensorValues, QTR_EMITTERS_ON);

  leftBlocked  = sensorValues[0] > threshold;

  frontBlocked = sensorValues[1] > threshold ||
                 sensorValues[2] > threshold ||
                 sensorValues[3] > threshold;

  rightBlocked = sensorValues[4] > threshold;
}


// Picks what to do, in the same order as before:
//   0 = wall in front, 1 = turn left, 2 = turn right, 3 = turn around
int chooseAction()
{
  if (frontBlocked)  return 0;
  if (!leftBlocked)  return 1;
  if (!rightBlocked) return 2;
  return 3;
}


void turnLeft()
{
  motors.setSpeeds(-turnSpeed, turnSpeed);
  delay(250);

  motors.setSpeeds(speed, speed);
  delay(100);
}


void turnRight()
{
  motors.setSpeeds(turnSpeed, -turnSpeed);
  delay(500);

  motors.setSpeeds(speed, speed);
  delay(100);
}


void turnAround()
{
  motors.setSpeeds(turnSpeed, -turnSpeed);
  delay(250);

  motors.setSpeeds(speed, speed);
  delay(100);
}


void backAndTurn90()
{
  unsigned long t = millis();

  motors.setSpeeds(-250, -250);      // go back
  while (millis() - t < 180) {}      // for 250 ms

  motors.setSpeeds(150, -150);       // turn right
  t = millis();
  while (millis() - t < 850) {}     // for 1500 ms (TUNE THIS for 90 degrees)

  motors.setSpeeds(0, 0);            // stop
}
