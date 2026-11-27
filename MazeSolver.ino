// ---------- Imports ----------
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

// ---------- Are the sensors detecting tape? ----------
bool leftBlocked;
bool frontBlocked;
bool rightBlocked;

// ---------- State ----------
// case false = drive straight until the robot reaches the black tape
// case true = solve the maze
bool isInsideMaze = false;

void setup()
{
  Serial.begin(9600);
  lineSensors.initFiveSensors();

  buttonA.waitForButton();
  delay(500);
}

void loopFindMaze() {
  motors.setSpeeds(200, 200);
  if (leftBlocked || frontBlocked || rightBlocked)
  {
    motors.setSpeeds(0, 0);
    delay(200);
    isInsideMaze = true;
  }
}

void loopSolveMaze() {
  // Solve the maze: pick an action, then do it
  if (frontBlocked)  // Wall in front: back up and turn 90 degrees
      backAndTurn90();
  else if (!leftBlocked) // Left is open: try left first
      turnLeft();
  else if (!rightBlocked) // Left is blocked, right is open: turn right
      turnRight();
    else  // Dead end: turn around
      turnAround();
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
  motors.setSpeeds(-250, -250);      // go back
  delay(250);                        // for 250 ms

  motors.setSpeeds(150, -150);       // turn right
  delay(1500);                       // for 1500 ms

  motors.setSpeeds(0, 0);            // stop
}

void loop()
{
  readSensors();
  
  if (!isInsideMaze) 
    loopFindMaze();
  else 
    loopSolveMaze();
}
