/*
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp-now-esp32-arduino-ide/
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*/

#include <esp_now.h>
#include <WiFi.h>

#include <ESP32Servo.h>
#include <Arduino.h>

// Must specify this before the include of "ServoEasing.hpp"
#define USE_PCA9685_SERVO_EXPANDER // Activate this to enables the use of the PCA9685 I2C expander chip/board.
#define MAX_EASING_SERVOS 22
#define NUMBER_OF_SERVOS
//#define PROVIDE_ONLY_LINEAR_MOVEMENT // Activate this to disable all but LINEAR movement. Saves up to 1540 bytes program memory.
//#define DISABLE_COMPLEX_FUNCTIONS // Activate this to disable the SINE, CIRCULAR, BACK, ELASTIC and BOUNCE easings. Saves up to 1850 bytes program memory.
#define DEBUG // Activate this to generate lots of lovely debug output for this library.
#include "ServoEasing.hpp"

//#define SERVOMIN  100 // This is the 'minimum' pulse length count (out of 4096)
//#define SERVOMAX  500 // This is the 'maximum' pulse length count (out of 4096)

//LETS DECLARE PCA BOARDS FOR LEFT ARM AND RIGHT ARM
char wristElbowArm = 0x40;
char shoulderAssem = 0x41;

//servo pins
//check pins
#define shoulderOutServo 0
#define shoulderForServo 1
#define armRotServo 2
#define elbowServo 1
#define wristServo 0

#define servo_speed 180


//LETS DECLARE ALL SERVO INDEXES WHICH WE WILL USE IN THE StartEaseTo METHO
int armAssemWrist;
int armAssemElbow;
int armAssemArm;
int shouldAssemOut;
int shouldAssemFor;

#define servo_speed 60

//servo rotation values
int shoulderOutRotVal;
int shoulderForRotVal;
int armRotServVal;
int elbowRotVal;
int wristRotVal;

// Structure example to send data
// Must match the receiver structure
typedef struct struct_message {
  short shoulderOutPotV;
  short shoulderForPotV;
  short armRotPotV;
  short elbowPotV;
  short wristPotV;
  char a[32];
  int b;
  float c;
  bool d;
} struct_message;

// Create a struct_message called myData
struct_message potValues;


// callback function that will be executed when data is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&potValues, incomingData, sizeof(potValues));
  Serial.print("Bytes received: ");
  Serial.println(len);
  Serial.println();

  Serial.print("Shoulder in/out: ");
  Serial.println(potValues.shoulderOutPotV);
  Serial.print("Shoulder front/back: ");
  Serial.println(potValues.shoulderForPotV);
  Serial.print("Arm Rotation: ");
  Serial.println(potValues.armRotPotV);
  Serial.print("Elbow: ");
  Serial.println(potValues.elbowPotV);
  Serial.print("Wrist: ");
  Serial.println(potValues.wristPotV);
  Serial.println();

  Serial.print("Char: ");
  Serial.println(potValues.a);
  Serial.print("Int: ");
  Serial.println(potValues.b);
  Serial.print("Float: ");
  Serial.println(potValues.c);
  Serial.print("Bool: ");
  Serial.println(potValues.d);
  Serial.println();
}

void setup() {
  // Initialize Serial Monitor
  Serial.begin(9600);
  Wire.begin();
  ServoEasing *ServoMaster;

  if (checkI2CConnection(PCA9685_DEFAULT_ADDRESS, &Serial)) {
    Serial.println(F("PCA9685 expander not connected"));
  } else {
    ServoMaster = new ServoEasing(wristElbowArm, &Wire);
    ServoMaster->attach(wristServo);
    armAssemWrist = 0; //the value is 0 because its the first attached

    ServoMaster = new ServoEasing(wristElbowArm, &Wire);
    ServoMaster->attach(elbowServo);
    armAssemElbow = 1; //the value is 1 because its the second attached

    ServoMaster = new ServoEasing(wristElbowArm, &Wire);
    ServoMaster->attach(armRotServo);
    armAssemArm = 2; //the value is 2 because its the third attached

    ServoMaster = new ServoEasing(shoulderAssem, &Wire);
    ServoMaster->attach(shoulderForServo);
    shouldAssemFor = 3; //the value is 3 because its the fourth attached

    ServoMaster = new ServoEasing(shoulderAssem, &Wire);
    ServoMaster->attach(shoulderOutServo);
    shouldAssemOut = 4; //the value is 4 because its the fifth attached

    ServoEasing::ServoEasingArray[armAssemWrist]->setEasingType(EASE_QUADRATIC_IN);
    ServoEasing::ServoEasingArray[armAssemElbow]->setEasingType(EASE_QUADRATIC_IN);
    ServoEasing::ServoEasingArray[armAssemArm]->setEasingType(EASE_QUADRATIC_IN);
    ServoEasing::ServoEasingArray[shouldAssemFor]->setEasingType(EASE_QUADRATIC_IN);
    ServoEasing::ServoEasingArray[shouldAssemOut]->setEasingType(EASE_QUADRATIC_IN);

    ServoEasing::ServoEasingArray[armAssemWrist]->setSpeed(servo_speed);
    ServoEasing::ServoEasingArray[armAssemElbow]->setSpeed(servo_speed);
    ServoEasing::ServoEasingArray[armAssemArm]->setSpeed(servo_speed);
    ServoEasing::ServoEasingArray[shouldAssemFor]->setSpeed(servo_speed);
    ServoEasing::ServoEasingArray[shouldAssemOut]->setSpeed(servo_speed);
  }


  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info
  esp_now_register_recv_cb(OnDataRecv);
}

void loop() {
  moveArm();
}

void moveArm() {
  //servo pos
  //wrist 28 to 206
  //elbow 100 to 200
  // arm rot 85 to 185
  //shoulder out 128 to 200 (keep in mind the linkage makes translation weird)
  //shoulder forward 85 to 177

  //arm pots
  //wrist 1300 left to 3700 right
  //elbow 300 down to 1300 bent
  //armRot 700 in to 1500 out
  //shoulderFor 2000 back 2500 forward
  //shoulderOut 1500 down to 3800 out

  //                        map, pot, servo
  //  indexRotVal = map(incomingPotReadings.index, 1100, 4095, 55, 180);
  shoulderOutRotVal = map(potValues.shoulderOutPotV, 3800, 1500, 128, 200);
  shoulderForRotVal = map(potValues.shoulderForPotV, 2500, 2000, 85, 177);
  armRotServVal = map(potValues.armRotPotV, 1500, 700, 85, 185);
  elbowRotVal = map(potValues.elbowPotV, 1300, 300, 100, 200);
  wristRotVal = map(potValues.wristPotV, 1300, 3700, 28, 206);

  //NOW HERE WHERE YOU WANT TO ROTATE THE SERVOS YOU USE THE DECLARED INDEXES FROM THE SETUP
  ServoEasing::ServoEasingArray[shouldAssemOut]->startEaseTo(shoulderOutRotVal); //async method (means that you can rotate more then one servo the same time (the method bellow is rotating one servo at a time))
  ServoEasing::ServoEasingArray[shouldAssemFor]->startEaseTo(shoulderForRotVal);
  ServoEasing::ServoEasingArray[armAssemArm]->startEaseTo(armRotServVal);
  ServoEasing::ServoEasingArray[armAssemElbow]->startEaseTo(elbowRotVal);
  ServoEasing::ServoEasingArray[armAssemWrist]->startEaseTo(wristRotVal);
}
