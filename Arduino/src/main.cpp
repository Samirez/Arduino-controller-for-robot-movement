#include <Arduino.h>
#include <MotorDriver.h>
#include <WiFiNINA.h>
#include <WiFiUdp.h>

#include "secrets.h"

MotorDriver m;

const int motorRight = 3;
const int motorLeft = 4;

int leftMotorCommand = RELEASE;
int rightMotorCommand = RELEASE;

const int lineSensorRight = A0;
const int lineSensorLeft = A1;
const int lineSensorArm = A2;

const int lineStrength = 300;
const int notLineStrength = 500;

const int maxSpeedTurn = 200;
const int maxSpeed = 160;
const int minSpeed = 80;
const int motorStallDelay = 100;

volatile int leftEncoderTime = 0;
volatile int leftEncoderCount = 0;
volatile int rightEncoderTime = 0;
volatile int rightEncoderCount = 0;

char ssid[] = WIFI_SSID;
char pass[] = WIFI_PASS;

unsigned int localPort = 10000;
char packetBuffer[256];

IPAddress remoteIp = IPAddress(WIFI_FIRST_OCTET, WIFI_SECOND_OCTET, WIFI_THIRD_OCTET, WIFI_FOURTH_OCTET);
uint16_t remotePort = 10000;

WiFiUDP Udp;

enum State
{
  // Waiting for command
  WAITING,
  // Done with command
  DONE,
  FOLLOWING_LINE,
  TURNING_LEFT,
  TURNING_RIGHT,
  TURNING_AROUND,
};

State state = DONE;

enum Command
{
  PING,
  FOLLOW_LINE,
  TURN_LEFT,
  TURN_RIGHT,
  TURN_AROUND,
};

void sendMessage(char *message)
{
  Udp.beginPacket(remoteIp, remotePort);
  Udp.write(message);
  Udp.write(",");
  char buffer[16];
  ultoa(millis(), buffer, 10);
  Udp.write(buffer);
  Udp.endPacket();
}

void encoderLeft()
{
  leftEncoderTime = millis();

  if (state > DONE && (leftMotorCommand == FORWARD || leftMotorCommand == BACKWARD))
  {
    leftEncoderCount++;
    char message[20];
    sprintf(message, "left,%s", (leftMotorCommand == FORWARD) ? "forward" : "backward");
    sendMessage(message);
  }
}

void encoderRight()
{
  rightEncoderTime = millis();
  if (state > DONE && (rightMotorCommand == FORWARD || rightMotorCommand == BACKWARD))
  {
    rightEncoderCount++;
    char message[20];
    sprintf(message, "right,%s", (rightMotorCommand == FORWARD) ? "forward" : "backward");
    sendMessage(message);
  }
}

void setupWiFi()
{
  int status = WiFi.status();

  if (status != WL_CONNECTED)
  {
    Serial.print("Attempting to connect to WiFi network");

    Udp.stop();

    status = WiFi.begin(ssid, pass);

    while (status != WL_CONNECTED)
    {
      Serial.print(".");
      delay(1000);
      status = WiFi.begin(ssid, pass);
    }

    Serial.print("\nConnected! IP address: ");
    Serial.println(WiFi.localIP());

    Udp.begin(localPort);
  }
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

  setupWiFi();
}

void motor(bool left, int command, int power)
{
  int now = millis();

  if (left)
  {
    if (now - leftEncoderTime > motorStallDelay)
    {
      m.motor(motorLeft, command, maxSpeedTurn);
    }
    else
    {
      m.motor(motorLeft, command, power);
    }

    leftMotorCommand = command;
  }
  else
  {
    if (now - rightEncoderTime > motorStallDelay)
    {
      m.motor(motorRight, command, maxSpeedTurn);
    }
    else
    {
      m.motor(motorRight, command, power);
    }

    rightMotorCommand = command;
  }
}

struct FollowLineState
{
  // If the robot starts in an intersection it needs to exit it to follow the line
  bool enteredLine = false;
};

FollowLineState followLineState = {};

bool state_follow_line()
{
  int sensorLeft = analogRead(lineSensorLeft);
  int sensorRight = analogRead(lineSensorRight);

  bool rightRunning = sensorRight > lineStrength;
  bool leftRunning = sensorLeft > lineStrength;

  if (!followLineState.enteredLine)
  {
    // We have never entered the line, check if we are on an intersection
    if (rightRunning || leftRunning)
    {
      // We are not on an intersection = we assume we are on the line
      followLineState.enteredLine = true;
    }
    else
    {
      // We are in an intersection, start both motors to exit intersection
      leftRunning = true;
      rightRunning = true;
    }
  }

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

  if (followLineState.enteredLine && sensorLeft < lineStrength && sensorRight < lineStrength)
  {
    // Both left and right sensors are on the line / entered an intersection
    return true;
  }

  return false;
}

struct TurnLeftState
{
  bool armLeftLine = false;
  bool armEnteredLineAgain = false;
};

TurnLeftState turnLeftState = {};

bool state_turn_left()
{
  if (turnLeftState.armEnteredLineAgain == false)
  {
    // Perform left turn using right motor

    motor(true, BRAKE, 0);
    motor(false, FORWARD, minSpeed);

    int sensorArm = analogRead(lineSensorArm);

    if (sensorArm < lineStrength && turnLeftState.armLeftLine)
    {
      // Arm entered line again
      turnLeftState.armEnteredLineAgain = true;
    }
    else if (sensorArm > lineStrength * 1.5)
    {
      // Arm left line
      turnLeftState.armLeftLine = true;
    }
  }
  else
  {
    motor(false, RELEASE, 0);

    // Turn right till left sensor sees the line

    int sensorLeft = analogRead(lineSensorLeft);

    if (sensorLeft < lineStrength)
    {
      return true;
    }

    motor(true, FORWARD, maxSpeed);
  }

  return false;
}

struct TurnRightState
{
  bool armDone = false;
  bool armLeftLine = false;
  bool leftSensorDone = false;
};

TurnRightState turnRightState = {};

bool state_turn_right()
{
  // Perform right turn using left motor

  motor(false, BRAKE, 0);
  motor(true, FORWARD, minSpeed);

  int sensorArm = analogRead(lineSensorArm);

  if (sensorArm > lineStrength)
  {
    turnRightState.armLeftLine = true;
  }

  if (turnRightState.armLeftLine && sensorArm < lineStrength)
  {
    turnRightState.armDone = true;
  }

  int sensorLeft = analogRead(lineSensorLeft);
  int sensorRight = analogRead(lineSensorRight);

  if (turnRightState.armDone && sensorLeft < lineStrength)
  {
    turnRightState.leftSensorDone = true;
  }

  if (turnRightState.leftSensorDone && sensorRight > notLineStrength)
  {
    return true;
  }

  return false;
}

struct TurnAroundState
{
  bool armLeftLine = false;
  bool leftSensorLeftLine = false;
  bool resetEncoderCount = false;
};

TurnAroundState turnAroundState = {};

bool state_turn_around()
{
  if (!turnAroundState.resetEncoderCount)
  {
    leftEncoderCount = 0;
    rightEncoderCount = 0;
    turnAroundState.resetEncoderCount = true;
  }

  int difference = leftEncoderCount - rightEncoderCount;

  motor(true, difference > 5 ? RELEASE : FORWARD, minSpeed);
  motor(false, difference < 5 ? RELEASE : BACKWARD, minSpeed);

  int sensorLeft = analogRead(lineSensorLeft);
  int sensorRight = analogRead(lineSensorRight);
  int sensorArm = analogRead(lineSensorArm);

  if (turnAroundState.leftSensorLeftLine && sensorLeft < lineStrength)
  {
    // Arm left line, left sensor left line, and left sensor entered line
    return true;
  }
  else if (turnAroundState.armLeftLine && sensorLeft > notLineStrength)
  {
    // Arm left line and left sensor left line
    turnAroundState.leftSensorLeftLine = true;
  }
  else if (sensorArm > notLineStrength)
  {
    // Arm left line
    turnAroundState.armLeftLine = true;
  }

  return false;
}

void stopMotors()
{
  m.motor(motorLeft, BRAKE, 0);
  m.motor(motorRight, BRAKE, 0);
}

unsigned long lastHeartbeat = 0;

void loop()
{
  if (state == DONE)
  {
    Serial.println("Waiting for command...");

    // Stop motors before doing anything else
    stopMotors();

    // Make sure we are connected to WiFi
    setupWiFi();

    // Tell server we are ready for next command
    sendMessage("ready");

    lastHeartbeat = millis();

    state = WAITING;
  }
  else if (state == WAITING)
  {
    // Make sure we are connected to WiFi
    setupWiFi();

    unsigned long now = millis();

    if (now - lastHeartbeat > 10000)
    {
      sendMessage("ready");
      lastHeartbeat = now;
    }

    // if there's data available, read a packet
    int packetSize = Udp.parsePacket();
    if (packetSize)
    {
      remoteIp = Udp.remoteIP();
      int len = Udp.read(packetBuffer, 255);
      if (len > 0)
      {
        packetBuffer[len] = 0;
      }

      int command = atoi(packetBuffer);

      Serial.print("Received command: ");
      Serial.println(command);

      remotePort = Udp.remotePort();

      sendMessage("received");

      switch (command)
      {
      case PING:
        sendMessage("pong");
        break;
      case FOLLOW_LINE:
        state = FOLLOWING_LINE;
        break;
      case TURN_LEFT:
        state = TURNING_LEFT;
        break;
      case TURN_RIGHT:
        state = TURNING_RIGHT;
        break;
      case TURN_AROUND:
        state = TURNING_AROUND;
        break;
      default:
        break;
      }
    }
  }
  else if (state == FOLLOWING_LINE)
  {
    if (state_follow_line())
    {
      followLineState = {};
      state = DONE;
    }
  }
  else if (state == TURNING_LEFT)
  {
    if (state_turn_left())
    {
      turnLeftState = {};
      state = DONE;
    }
  }
  else if (state == TURNING_RIGHT)
  {
    if (state_turn_right())
    {
      turnRightState = {};
      state = DONE;
    }
  }
  else if (state == TURNING_AROUND)
  {
    if (state_turn_around())
    {
      turnAroundState = {};
      state = DONE;
    }
  }
}
