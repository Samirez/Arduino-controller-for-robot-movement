#include <Arduino.h>
#include <MotorDriver.h>

MotorDriver m;

const int motorRight = 3;
const int motorLeft = 4;

const int lineSensorRight = A0;
const int lineSensorLeft = A1;
const int lineSensorArm = A2;

const int lineStrength = 300;
const int notLineStrength = 500;

const int maxSpeedTurn = 200;
const int maxSpeed = 160;
const int minSpeed = 80;

volatile int leftEncoderTime = 0;
volatile int rightEncoderTime = 0;

void encoderLeft()
{
  Serial.println("Left");
  leftEncoderTime = millis();
}

void encoderRight()
{
  Serial.println("Right");
  rightEncoderTime = millis();
}

void setup()
{
  Serial.begin(9600);
  m.motor(1, RELEASE, 0);
  m.motor(2, RELEASE, 0);
  m.motor(3, RELEASE, 0);
  m.motor(4, RELEASE, 0);

  pinMode(2, INPUT);
  pinMode(3, INPUT);
  attachInterrupt(digitalPinToInterrupt(2), encoderLeft, RISING);
  attachInterrupt(digitalPinToInterrupt(3), encoderRight, RISING);
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  pinMode(A2, INPUT);
}

void motor(bool left, int command, int power)
{
  int now = millis();

  if (left)
  {
    if (now - leftEncoderTime > 100)
    {
      m.motor(motorLeft, command, maxSpeedTurn);
    }
    else
    {
      m.motor(motorLeft, command, power);
    }
  }
  else
  {
    if (now - rightEncoderTime > 100)
    {
      m.motor(motorRight, command, maxSpeedTurn);
    }
    else
    {
      m.motor(motorRight, command, power);
    }
  }
}

void followLine(bool stopBeforeIntersection)
{
  bool enteredIntersection = false;

  while (true)
  {
    int sensorLeft = analogRead(lineSensorLeft);
    int sensorRight = analogRead(lineSensorRight);

    bool rightRunning = sensorRight > lineStrength;
    bool leftRunning = sensorLeft > lineStrength;

    if (rightRunning)
    {
      // We are on the line or to the right of it
      motor(false, FORWARD, map(sensorRight, lineStrength, 1024, minSpeed, leftRunning ? maxSpeed : maxSpeedTurn));
    }
    else
    {
      // We are too much to the left of the line (sensor is in the middle of line)
      motor(false, RELEASE, 0);
    }

    if (leftRunning)
    {
      // We are on the line or to the left of it
      motor(true, FORWARD, map(sensorLeft, lineStrength, 1024, minSpeed, rightRunning ? maxSpeed : maxSpeedTurn));
    }
    else
    {
      // We are too much to the right of the line (sensor is in middle of line)
      motor(true, RELEASE, 0);
    }

    if (sensorLeft < lineStrength && sensorRight < lineStrength)
    {
      // Both left and right sensors are on the line / entered an intersection
      enteredIntersection = true;

      if (stopBeforeIntersection)
      {
        break;
      }
    }

    if (!stopBeforeIntersection && enteredIntersection && (sensorLeft > notLineStrength || sensorRight > notLineStrength))
    {
      // We have left the intersection
      break;
    }
  }
}

void turnLeft()
{
  motor(true, BRAKE, 0);

  bool armLeftLine = false;

  // Perform left turn using right motor

  while (true)
  {
    motor(false, FORWARD, minSpeed);

    int sensorArm = analogRead(lineSensorArm);

    if (sensorArm < lineStrength && armLeftLine)
    {
      // Arm entered line again
      break;
    }

    if (sensorArm > lineStrength * 1.5)
    {
      // Arm left line
      armLeftLine = true;
    }
  }

  motor(false, RELEASE, 0);

  // Turn right till left sensor sees the line
  while (true)
  {
    int sensorLeft = analogRead(lineSensorLeft);

    if (sensorLeft < lineStrength)
    {
      break;
    }

    motor(true, FORWARD, maxSpeed);
  }
}

void loop()
{
  followLine(true);
  turnLeft();
}
