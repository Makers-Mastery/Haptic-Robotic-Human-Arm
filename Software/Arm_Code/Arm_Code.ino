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

//servo pins
//check pins
#define shoulderOutServo 0
#define shoulderForServo 1
#define armRotServo 2
#define elbowServo 3
#define wristServo 4

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

  moveArm();
}

void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);
  Wire.begin();
  ServoEasing *ServoMaster;

  if (checkI2CConnection(PCA9685_DEFAULT_ADDRESS, &Serial)) {
    Serial.println(F("PCA9685 expander not connected"));
  } else {
    ServoMaster = new ServoEasing(PCA9685_DEFAULT_ADDRESS, &Wire);
    ServoMaster->attach(shoulderOutServo);
    ServoMaster = new ServoEasing(PCA9685_DEFAULT_ADDRESS, &Wire)
    ServoMaster->attach(shoulderForServo);
    ServoMaster = new ServoEasing(PCA9685_DEFAULT_ADDRESS, &Wire);
    ServoMaster->attach(armRotServo);
    ServoMaster = new ServoEasing(PCA9685_DEFAULT_ADDRESS, &Wire);
    ServoMaster->attach(elbowServo);
    ServoMaster = new ServoEasing(PCA9685_DEFAULT_ADDRESS, &Wire);
    ServoMaster->attach(wristServo);
    /*
      if ((indexServo || middleServo || ringServo || pinkyServo || thumbServo || thumbRotServo) == INVALID_SERVO) {
      Serial.println(F("Error attaching servo index_finger - maybe MAX_EASING_SERVOS=" STR(MAX_EASING_SERVOS) " is to small to hold all servos"));
      } */
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

}

void moveArm() {
  shoulderOutRotVal = map(potValues.shoulderOutPotV, 50, 650, 30, 170);
  shoulderForRotVal = map(potValues.shoulderForPotV, 50, 650, 30, 170);
  armRotServVal = map(potValues.armRotPotV, 50, 650, 30, 170);
  elbowRotVal = map(potValues.elbowPotV, 50, 650, 30, 170);
  wristRotVal = map(potValues.wristPotV, 50, 650, 30, 170);

  ServoEasing::ServoEasingArray[shoulderOutServo]->startEaseTo(shoulderOutRotVal); //async method (means that you can rotate more then one servo the same time (the method bellow is rotating one servo at a time))
  ServoEasing::ServoEasingArray[shoulderForServo]->startEaseTo(shoulderForRotVal);
  ServoEasing::ServoEasingArray[armRotServo]->startEaseTo(armRotServVal);
  ServoEasing::ServoEasingArray[elbowServo]->startEaseTo(elbowRotVal);
  ServoEasing::ServoEasingArray[wristServo]->startEaseTo(wristRotVal);
}
