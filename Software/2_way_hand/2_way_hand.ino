/*
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp-now-two-way-communication-esp32/

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
//#define DEBUG // Activate this to generate lots of lovely debug output for this library.
#include "ServoEasing.hpp"

#include <Wire.h>

/*
  #define rxPin 4
  #define txPin 2
  #define SERVOMIN  100 // This is the 'minimum' pulse length count (out of 4096)
  #define SERVOMAX  500 // This is the 'maximum' pulse length count (out of 4096)
*/

//define pressure sensors
#define pinkyPres 36
#define ringPres 39
#define middlePres 34
#define indexPres 35
#define thumbPres 32

//servo pins
#define indexServo 0 //the pin number on your PCA board
#define middleServo 1
#define ringServo 2
#define pinkyServo 3
#define thumbServo 4
#define thumbRotServo 5

//int indexInitPos =0;

#define servo_speed 250

//define multicore movement
//TaskHandle_t fMovement;

//servo rotation values
short indexRotVal;
short middleRotVal;
short ringRotVal;
short pinkyRotVal;
short thumbRotVal;
short thumbDisRotVal;

// REPLACE WITH THE MAC Address of your receiver
//78:21:84:BB:39:8C


uint8_t broadcastAddress[] = {0x78, 0x21, 0x84, 0xBB, 0x39, 0x8C};

// Define variables to store BME280 readings to be sent

short pinkyPValue;
short ringPValue;
short middlePValue;
short indexPValue;
short thumbPValue;

//float temperature;
//float humidity;
//float pressure;

// Define variables to store incoming readings

short incomingPotPinky;
short incomingPotRing;
short incomingPotMiddle;
short incomingPotIndex;
short incomingPotThumb;

// Variable to store if sending data was successful
String success;

//Structure example to send data
//Must match the receiver structure

typedef struct struct_message_values {
  short pinky;
  short ring;
  short middle;
  short index;
  short thumb;
} struct_message_pres;

// Create a struct_message called BME280Readings to hold sensor readings
struct_message_values pressureReadings;

// Create a struct_message to hold incoming sensor readings
struct_message_values incomingPotReadings;

esp_now_peer_info_t peerInfo;

// Callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
  if (status == 0) {
    success = "Delivery Success :)";
  }
  else {
    success = "Delivery Fail :(";
  }
}

// Callback when data is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&incomingPotReadings, incomingData, sizeof(incomingPotReadings));
  Serial.print("Bytes received: ");
  Serial.println(len);
  incomingPotPinky = incomingPotReadings.pinky;
  incomingPotRing = incomingPotReadings.ring;
  incomingPotMiddle = incomingPotReadings.middle;
  incomingPotIndex = incomingPotReadings.index;
  incomingPotThumb = incomingPotReadings.thumb;
  updateDisplay();

  Serial.print("data handle core: ");
  Serial.println(xPortGetCoreID());
  //moveFingers();
}

void setup() {
  // Init Serial Monitor
  Serial.begin(9600);
  Wire.begin();
  ServoEasing *ServoMaster;

  if (checkI2CConnection(PCA9685_DEFAULT_ADDRESS, &Serial)) {
    Serial.println(F("PCA9685 expander not connected"));
  } else {
    ServoMaster = new ServoEasing(PCA9685_DEFAULT_ADDRESS, &Wire);
    ServoMaster->attach(indexServo);
    ServoMaster = new ServoEasing(PCA9685_DEFAULT_ADDRESS, &Wire);
    ServoMaster->attach(middleServo);
    ServoMaster = new ServoEasing(PCA9685_DEFAULT_ADDRESS, &Wire);
    ServoMaster->attach(ringServo);
    ServoMaster = new ServoEasing(PCA9685_DEFAULT_ADDRESS, &Wire);
    ServoMaster->attach(pinkyServo);
    ServoMaster = new ServoEasing(PCA9685_DEFAULT_ADDRESS, &Wire);
    ServoMaster->attach(thumbServo);
    ServoMaster = new ServoEasing(PCA9685_DEFAULT_ADDRESS, &Wire);
    ServoMaster->attach(thumbRotServo);

 
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

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(OnDataSent);

  // Register peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  // Add peer
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }
  // Register for a callback function that will be called when data is received
  esp_now_register_recv_cb(OnDataRecv);

  ServoEasing::ServoEasingArray[indexServo]->setEasingType(EASE_QUADRATIC_IN);
  ServoEasing::ServoEasingArray[middleServo]->setEasingType(EASE_QUADRATIC_IN);
  ServoEasing::ServoEasingArray[ringServo]->setEasingType(EASE_QUADRATIC_IN);
  ServoEasing::ServoEasingArray[pinkyServo]->setEasingType(EASE_QUADRATIC_IN);
  ServoEasing::ServoEasingArray[thumbServo]->setEasingType(EASE_QUADRATIC_IN);
  ServoEasing::ServoEasingArray[thumbRotServo]->setEasingType(EASE_QUADRATIC_IN);


  ServoEasing::ServoEasingArray[indexServo]->setSpeed(servo_speed);
  ServoEasing::ServoEasingArray[middleServo]->setSpeed(servo_speed);
  ServoEasing::ServoEasingArray[ringServo]->setSpeed(servo_speed);
  ServoEasing::ServoEasingArray[pinkyServo]->setSpeed(servo_speed);
  ServoEasing::ServoEasingArray[thumbServo]->setSpeed(servo_speed);
  ServoEasing::ServoEasingArray[thumbRotServo]->setSpeed(servo_speed);

  /*xTaskCreatePinnedToCore(
   codeMFingers,
   "f_Movement",
   1000,
   NULL,
   1,
   &fMovement,
  1);*/
}


void loop() {
  getReadings();

  // Set values to send
  pressureReadings.pinky = pinkyPValue;
  pressureReadings.ring = ringPValue;
  pressureReadings.middle = middlePValue;
  pressureReadings.index = indexPValue;
  pressureReadings.thumb = thumbPValue;

  // Send message via ESP-NOW
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &pressureReadings, sizeof(pressureReadings));

  if (result == ESP_OK) {
    Serial.println("Sent with success");
  }
  else {
    Serial.println("Error sending the data");
  }
  moveFingers();
  //delay(1000);
}
void getReadings() {
  pinkyPValue = analogRead(pinkyPres);
  ringPValue = analogRead(ringPres);
  middlePValue = analogRead(middlePres);
  indexPValue = analogRead(indexPres);
  thumbPValue = analogRead(thumbPres);
}

void moveFingers() {
  //change values
  //1 to 255
  indexRotVal = map(incomingPotReadings.index, 1100, 4095, 55, 180);
  middleRotVal = map(incomingPotReadings.middle, 500, 4000, 42, 195);
  ringRotVal = map(incomingPotReadings.ring, 970, 4095, 40, 180);
  pinkyRotVal = map(incomingPotReadings.pinky, 1000, 2960, 40, 170);
  thumbRotVal = map(incomingPotReadings.thumb, 1600, 4095, 0, 180);
  thumbDisRotVal = map(incomingPotReadings.thumb, 1600, 4095, 160, 40);

  ServoEasing::ServoEasingArray[indexServo]->startEaseTo(indexRotVal); //async method (means that you can rotate more then one servo the same time (the method bellow is rotating one servo at a time))
  ServoEasing::ServoEasingArray[middleServo]->startEaseTo(middleRotVal); //async method (means that you can rotate more then one servo the same time (the method bellow is rotating one servo at a time))
  ServoEasing::ServoEasingArray[ringServo]->startEaseTo(ringRotVal); //async method (means that you can rotate more then one servo the same time (the method bellow is rotating one servo at a time))
  ServoEasing::ServoEasingArray[pinkyServo]->startEaseTo(pinkyRotVal); //async method (means that you can rotate more then one servo the same time (the method bellow is rotating one servo at a time))
  ServoEasing::ServoEasingArray[thumbServo]->startEaseTo(thumbRotVal); //async method (means that you can rotate more then one servo the same time (the method bellow is rotating one servo at a time))
  ServoEasing::ServoEasingArray[thumbRotServo]->startEaseTo(thumbDisRotVal); //async method (means that you can rotate more then one servo the same time (the method bellow is rotating one servo at a time))

  Serial.print("fingers core: ");
  Serial.println(xPortGetCoreID());
  //ServoEasing::ServoEasingArray[index_finger_servo]->easeTo(rotation_value); //the is the sync method (non-asyng) if the above method doesn't work, uncomment this and comment the one above)

}

void updateDisplay() {
  // Display Readings in Serial Monitor
  Serial.println("INCOMING READINGS");
  Serial.print("pinky pot: ");
  Serial.println(incomingPotReadings.pinky);
  Serial.print("ring pot: ");
  Serial.println(incomingPotReadings.ring);
  Serial.print("middle pot: ");
  Serial.println(incomingPotReadings.middle);
  Serial.print("index pot");
  Serial.println(incomingPotReadings.index);
  Serial.print("thumb pot: ");
  Serial.println(incomingPotReadings.thumb);
  Serial.println();
}
