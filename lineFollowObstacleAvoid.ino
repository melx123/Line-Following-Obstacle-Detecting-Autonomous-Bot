//trying to change stuff

#include <Servo.h>
#include <AFMotor.h>

AF_DCMotor motor1(1, MOTOR12_1KHZ);
AF_DCMotor motor2(2, MOTOR12_1KHZ);
AF_DCMotor motor3(3, MOTOR34_1KHZ);
AF_DCMotor motor4(4, MOTOR34_1KHZ);

Servo claw;

const int IRSensor1 = A2; //left
const int IRSensor2 = A3; //right
const int clawPin = 10;
const int trigPin = A1, echoPin = A0;

const int obstacleDist = 15;
int speed1 = 85;
int speed2 = 95;
int speedback1 = 85;
int speedback2 = 95;
int speedfast1 = 235;
int speedfast2 = 245;

int dist;
int sensorStatus1, sensorStatus2;
bool onLine = false;
bool skewRight = false;
bool skewLeft = false;
bool noLine = false;
bool obstacle = false;

// Timed turning logic
unsigned long turnStartTime = 0;
int turnDuration = 30;  // ms
bool isTurning = false;
String turnDirection = "";

void setup() {
  Serial.begin(9600);
  claw.attach(clawPin);

  pinMode(trigPin, OUTPUT); 
  pinMode(echoPin, INPUT);
  pinMode(IRSensor1, INPUT);
  pinMode(IRSensor2, INPUT);

  claw.write(90);
  delay(500);
}

void loop() {
  sensorStatus1 = digitalRead(IRSensor1);
  sensorStatus2 = digitalRead(IRSensor2);
  dist = readUltrasonicCM();

  Serial.print("Dist: "); 
  
  Serial.print(dist); 
  
  
  Serial.println(" cm");

  onLine = (!sensorStatus1 && !sensorStatus2);
  skewRight = (sensorStatus1 && !sensorStatus2);
  skewLeft = (!sensorStatus1 && sensorStatus2);
  noLine = (sensorStatus1 && sensorStatus2);
  obstacle = ((dist < obstacleDist));

  if (!obstacle) {
    while (isTurning) {
      if (millis() - turnStartTime >= turnDuration) {
        straight();
        isTurning = false;
        Serial.println("...Straight");
      }
    }

    if (onLine) {
      Serial.print("Line, ");
      straight();
    } else if (skewRight) {
      Serial.print("Right turn, ");
      left();
      turnStartTime = millis();
      isTurning = true;
      turnDirection = "left";
    } else if (skewLeft) {
      Serial.print("Left turn, ");
      right();
      turnStartTime = millis();
      isTurning = true;
      turnDirection = "right";
    } else {
      Serial.print("Stopped, ");
      stop();
    }
  } else {
    stop();
    checkSides();
  }
}

void checkSides() {
  int turnTime = 300;
  int straightTime = 650;
  int stopTime = 10;

  stop();
  delay(1000);
  int distR, distL; 

  claw.write(150);
  delay(500);
  distL = readUltrasonicCM();
  delay(10);
  Serial.print("Right distance = "); Serial.print(distR); Serial.print(" cm, ");
  delay(100);

  claw.write(20);
  delay(500);
  distR = readUltrasonicCM();
  delay(10);
  Serial.print("Left distance = "); Serial.print(distL); Serial.print(" cm, ");
  delay(100);

  claw.write(90);

  if (distR >= distL) {
        Serial.print("Turning Right \n");
    right(); delay(turnTime + 50); stop(); delay(stopTime);
    straight(); delay(straightTime + 100); stop(); delay(stopTime);
    left(); delay(turnTime); stop(); delay(stopTime);
    straight(); delay(straightTime + 200); stop(); delay(stopTime);
    left(); delay(turnTime); stop(); delay(stopTime);
    straight(); delay(straightTime + 100); stop(); delay(stopTime);
    right(); delay(turnTime - 50); stop(); delay(stopTime);
  } else {
        Serial.print("Turning Left \n");
    left(); delay(turnTime + 50); stop(); delay(stopTime);
    straight(); delay(straightTime + 100); stop(); delay(stopTime);
    right(); delay(turnTime); stop(); delay(stopTime);
    straight(); delay(straightTime + 200); stop(); delay(stopTime);
    right(); delay(turnTime); stop(); delay(stopTime);
    straight(); delay(straightTime + 100); stop(); delay(stopTime);
    left(); delay(turnTime - 50); stop(); delay(stopTime);
  } 
}

void stop() {
  motor1.run(RELEASE);
  motor2.run(RELEASE);
  motor3.run(RELEASE);
  motor4.run(RELEASE);
}

void straight() {
  motor1.run(FORWARD);
  motor2.run(FORWARD);
  motor3.run(FORWARD);
  motor4.run(FORWARD);
  motor1.setSpeed(speed1);
  motor2.setSpeed(speed2 + 10);
  motor3.setSpeed(speed1);
  motor4.setSpeed(speed2);
}

void left() {
  motor1.run(BACKWARD);
  motor2.run(BACKWARD);
  motor3.run(FORWARD);
  motor4.run(FORWARD);
  motor1.setSpeed(speedfast2);
  motor2.setSpeed(speedfast2 + 10);
  motor3.setSpeed(speedfast1);
  motor4.setSpeed(speedfast1);
}

void right() {
  motor1.run(FORWARD);
  motor2.run(FORWARD);
  motor3.run(BACKWARD);
  motor4.run(BACKWARD);
  motor1.setSpeed(speedfast1);
  motor2.setSpeed(speedfast1 + 10);
  motor3.setSpeed(speedfast2);
  motor4.setSpeed(speedfast2);
  delay(10);
}

void back() {
  motor1.run(BACKWARD);
  motor2.run(BACKWARD);
  motor3.run(BACKWARD);
  motor4.run(BACKWARD);
  motor1.setSpeed(speedback1);
  motor2.setSpeed(speedback2 + 10);
  motor3.setSpeed(speedback1);
  motor4.setSpeed(speedback2);
}

float readUltrasonicCM() {
  digitalWrite(trigPin, LOW);  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH); delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long dur = pulseIn(echoPin, HIGH, 30000);
  // Serial.print("Raw duration = "); Serial.print(dur);
  if (dur == 0 || dur == 30000) {
    Serial.print("No echo");
    return 255;
  } else {
    return (dur * 0.0343) / 2.0;
  } 
}
