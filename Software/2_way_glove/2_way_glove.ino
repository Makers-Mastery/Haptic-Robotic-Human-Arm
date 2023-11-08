/*
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp-now-two-way-communication-esp32/

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.

  The above copyright notice and this permission notice shall be included in all
  copies or subst antial portions of the Software.
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

#include <Wire.h>

//define pressure sensors
//change the numbers pls)
#define pinkyPot 36
#define ringPot 39
#define middlePot 34
#define indexPot 35
#define thumbPot 32


#define indexServoGl 0 //the pin number on your PCA board
#define middleServoGl 1
#define ringServoGl 2
#define pinkyServoGl 3
#define thumbServoGl 4

#define calibPin 4 //needs to be set

#define servo_speed 60


int indexRotValGl;
int middleRotValGl;
int ringRotValGl;
int pinkyRotValGl;
int thumbRotValGl;

//updated on startup in calibPots
short indexCalibValOpen;
short middleCalibValOpen;
short ringCalibValOpen;
short pinkyCalibValOpen;
short thumbCalibValOpen;

//updated on startup in calibPots
short indexCalibValClosed;
short middleCalibValClosed;
short ringCalibValClosed;
short pinkyCalibValClosed;
short thumbCalibValClosed;


// REPLACE WITH THE MAC Address of your receiver
uint8_t broadcastAddress[] = {0x78, 0x21, 0x84, 0xC4, 0x90, 0xFC};

// Define variables to store BME280 readings to be sent

short pinkyPotValue;
short ringPotValue;
short middlePotValue;
short indexPotValue;
short thumbPotValue;

//float temperature;
//float humidity;
//float pressure;

// Define variables to store incoming readings

short incomingPresPinky;
short incomingPresRing;
short incomingPresMiddle;
short incomingPresIndex;
short incomingPresThumb;


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
  //float temp;
  //float hum;
  //float pres;
} struct_message_values;

// Create a struct_message called BME280Readings to hold sensor readings
struct_message_values potReadings;

//create a struct_message to hold open calibrations to be sent on startup
struct_message_values calibValuesOpen;

//create a struct_message to hold closed calibrations to be sent on startup
struct_message_values calibValuesClosed;

// Create a struct_message to hold incoming sensor readings
struct_message_values incomingPresReadings;

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
  memcpy(&incomingPresReadings, incomingData, sizeof(incomingPresReadings));
  Serial.print("Bytes received: ");
  Serial.println(len);
  incomingPresPinky = incomingPresReadings.pinky;
  incomingPresRing = incomingPresReadings.ring;
  incomingPresMiddle = incomingPresReadings.middle;
  incomingPresIndex = incomingPresReadings.index;
  incomingPresThumb = incomingPresReadings.thumb;
  //moveGloveHaptics();
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
    ServoMaster->attach(indexServoGl);
    ServoMaster = new ServoEasing(PCA9685_DEFAULT_ADDRESS, &Wire);
    ServoMaster->attach(middleServoGl);
    ServoMaster = new ServoEasing(PCA9685_DEFAULT_ADDRESS, &Wire);
    ServoMaster->attach(ringServoGl);
    ServoMaster = new ServoEasing(PCA9685_DEFAULT_ADDRESS, &Wire);
    ServoMaster->attach(pinkyServoGl);
    ServoMaster = new ServoEasing(PCA9685_DEFAULT_ADDRESS, &Wire);
    ServoMaster->attach(thumbServoGl);
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

  ServoEasing::ServoEasingArray[indexServoGl]->setEasingType(EASE_QUADRATIC_IN);
  ServoEasing::ServoEasingArray[middleServoGl]->setEasingType(EASE_QUADRATIC_IN);
  ServoEasing::ServoEasingArray[ringServoGl]->setEasingType(EASE_QUADRATIC_IN);
  ServoEasing::ServoEasingArray[pinkyServoGl]->setEasingType(EASE_QUADRATIC_IN);
  ServoEasing::ServoEasingArray[thumbServoGl]->setEasingType(EASE_QUADRATIC_IN);

  ServoEasing::ServoEasingArray[indexServoGl]->setSpeed(servo_speed);
  ServoEasing::ServoEasingArray[middleServoGl]->setSpeed(servo_speed);
  ServoEasing::ServoEasingArray[ringServoGl]->setSpeed(servo_speed);
  ServoEasing::ServoEasingArray[pinkyServoGl]->setSpeed(servo_speed);
  ServoEasing::ServoEasingArray[thumbServoGl]->setSpeed(servo_speed);

  //traps code in calib, click button once to calib open and again to calib close
  calibPots();

  //transmits calibrations to hand
  // Send message via ESP-NOW
  esp_err_t openValsSend = esp_now_send(broadcastAddress, (uint8_t *) &calibValuesOpen, sizeof(calibValuesOpen));

  if (openValsSend == ESP_OK) {
    Serial.println("Sent with success");
  }
  else {
    Serial.println("Error sending the data");
  }
  updateDisplay();
  delay(3000);

  // Send message via ESP-NOW
  esp_err_t closedValsSend = esp_now_send(broadcastAddress, (uint8_t *) &calibValuesClosed, sizeof(calibValuesClosed));

  if (closedValsSend == ESP_OK) {
    Serial.println("Sent with success");
  }
  else {
    Serial.println("Error sending the data");
  }
  updateDisplay();
  delay(3000);
}

void loop() {
  getReadings();

  // Set values to send
  potReadings.pinky = pinkyPotValue;
  potReadings.ring = ringPotValue;
  potReadings.middle = middlePotValue;
  potReadings.index = indexPotValue;
  potReadings.thumb = thumbPotValue;

  // Send message via ESP-NOW
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &potReadings, sizeof(potReadings));

  if (result == ESP_OK) {
    Serial.println("Sent with success");
  }
  else {
    Serial.println("Error sending the data");
  }
  updateDisplay();
  //delay(1000);

  moveGloveHaptics();
}
void getReadings() {
  pinkyPotValue = analogRead(pinkyPot);
  ringPotValue = analogRead(ringPot);
  middlePotValue = analogRead(middlePot);
  indexPotValue = analogRead(indexPot);
  thumbPotValue = analogRead(thumbPot);
}

void moveGloveHaptics() {
  //85 to 4095
   
  if (incomingPresPinky > 1900) {
    pinkyRotValGl = map(analogRead(ringPot), 4095, pinkyCalibValClosed, 0, 170);
    ServoEasing::ServoEasingArray[pinkyServoGl]->startEaseTo(pinkyRotValGl); //async method (means that you can rotate more then one servo the same time (the method bellow is rotating one servo at a time))
  } else {
    ServoEasing::ServoEasingArray[pinkyServoGl]->startEaseTo(180); //async method (means that you can rotate more then one servo the same time (the method bellow is rotating one servo at a time))
  }  

  if (incomingPresRing > 150) {
    ringRotValGl = map(analogRead(ringPot), 4095, ringCalibValClosed, 0, 170);
    ServoEasing::ServoEasingArray[ringServoGl]->startEaseTo(ringRotValGl); //async method (means that you can rotate more then one servo the same time (the method bellow is rotating one servo at a time))
  } else {
    ServoEasing::ServoEasingArray[ringServoGl]->startEaseTo(180); //async method (means that you can rotate more then one servo the same time (the method bellow is rotating one servo at a time))
  }

  if (incomingPresMiddle > 150) {
    middleRotValGl = map(analogRead(middlePot), 4095, middleCalibValClosed, 0, 170);
    ServoEasing::ServoEasingArray[middleServoGl]->startEaseTo(middleRotValGl); //async method (means that you can rotate more then one servo the same time (the method bellow is rotating one servo at a time))
  } else {
    ServoEasing::ServoEasingArray[middleServoGl]->startEaseTo(180); //async method (means that you can rotate more then one servo the same time (the method bellow is rotating one servo at a time))
  }

  if (incomingPresIndex > 150) {
    indexRotValGl = map(analogRead(indexPot), 4095, indexCalibValClosed, 0, 170);
    ServoEasing::ServoEasingArray[indexServoGl]->startEaseTo(indexRotValGl);
  } else {
    ServoEasing::ServoEasingArray[indexServoGl]->startEaseTo(180); //async method (means that you can rotate more then one servo the same time (the method bellow is rotating one servo at a time))
  }

  if (incomingPresThumb > 150) {
    thumbRotValGl = map(analogRead(thumbPot), 4095 , thumbCalibValClosed, 0, 170);
    ServoEasing::ServoEasingArray[thumbServoGl]->startEaseTo(thumbRotValGl); //async method (means that you can rotate more then one servo the same time (the method bellow is rotating one servo at a time))
  } else {
    ServoEasing::ServoEasingArray[thumbServoGl]->startEaseTo(180); //async method (means that you can rotate more then one servo the same time (the method bellow is rotating one servo at a time))
  }
  /*
    if () {
    } else {
    } */
}

void calibPots() {
  int gloveCounter = 0;
  while (gloveCounter < 2) {
    Serial.println("entering calibration");
    Serial.print("glove counter: ");
    Serial.println(gloveCounter);
    Serial.print("calibPin: ");
    Serial.println(digitalRead(calibPin));
    if (digitalRead(calibPin) == 1 && gloveCounter == 0) {
      Serial.println("calibrating open...");
      indexCalibValOpen = analogRead(indexPot);
      middleCalibValOpen = analogRead(middlePot);
      ringCalibValOpen = analogRead(ringPot);
      pinkyCalibValOpen = analogRead(pinkyPot);
      thumbCalibValOpen = analogRead(thumbPot);

      calibValuesOpen.pinky = pinkyCalibValOpen;
      calibValuesOpen.ring = ringCalibValOpen;
      calibValuesOpen.middle = middleCalibValOpen;
      calibValuesOpen.index = indexCalibValOpen;
      calibValuesOpen.thumb = thumbCalibValOpen;

      gloveCounter++;
      delay(3000);

    } else if (digitalRead(calibPin) == 1 && gloveCounter == 1) {
      Serial.println("calibrating closed...");
      indexCalibValClosed = analogRead(indexPot);
      middleCalibValClosed = analogRead(middlePot);
      ringCalibValClosed = analogRead(ringPot);
      pinkyCalibValClosed = analogRead(pinkyPot);
      thumbCalibValClosed = analogRead(thumbPot);

      calibValuesClosed.pinky = pinkyCalibValClosed;
      calibValuesClosed.ring = ringCalibValClosed;
      calibValuesClosed.middle = middleCalibValClosed;
      calibValuesClosed.index = indexCalibValClosed;
      calibValuesClosed.thumb = thumbCalibValClosed;

      gloveCounter++;
      delay(3000);
    }
  }
}

void updateDisplay() {

  // Display Readings in Serial Monitor
  Serial.println("INCOMING READINGS");
  Serial.print("pinky pres: ");
  Serial.println(incomingPresReadings.pinky);  Serial.print("ring pres: ");
  Serial.println(incomingPresReadings.ring);
  Serial.print("middle pres: ");
  Serial.println(incomingPresReadings.middle);
  Serial.print("index pres: ");
  Serial.println(incomingPresReadings.index);
  Serial.print("thumb pres: ");
  Serial.println(incomingPresReadings.thumb);
  Serial.println();
}
