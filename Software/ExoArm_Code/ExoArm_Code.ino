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
#include <Wire.h>

#define shoulderOut 35
#define shoulderFor 34
#define armRot 32
#define elbow 33
#define wrist 25

// REPLACE WITH YOUR RECEIVER MAC Address
uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

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

esp_now_peer_info_t peerInfo;

// callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}
 
void setup() {
  // Init Serial Monitor
  Serial.begin(9600);
 
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
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
}
 
void loop() {
  //wrist 1300 left to 3700 right
  //elbow 300 down to 1300 bent
  //armRot 700 in to 1500 out
  //shoulderFor 2000 back 2500 forward
  //shoulderOut 1500 down to 3800 out
  // put your main code here, to run repeatedly:
  
  // Set values to send
  potValues.shoulderOutPotV = analogRead(shoulderOut);
  potValues.shoulderForPotV = analogRead(shoulderFor);
  potValues.armRotPotV = analogRead(armRot);
  potValues.elbowPotV = analogRead(elbow);
  potValues.wristPotV = analogRead(wrist);
  
  strcpy(potValues.a, "THIS IS A CHAR");
  potValues.b = random(1,20);
  potValues.c = 1.2;
  potValues.d = false;
  
  // Send message via ESP-NOW
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &potValues, sizeof(potValues));
   
  if (result == ESP_OK) {
    Serial.println("Sent with success");
  }
  else {
    Serial.println("Error sending the data");
  }
  delay(2000);
}
