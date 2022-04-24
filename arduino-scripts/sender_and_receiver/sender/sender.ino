/*
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp-now-esp32-arduino-ide/
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*/


// HC SR-04 ultrasonic distance sensor variables/imports 
#define TRIGGER_PIN  19
#define ECHO_PIN     23
#define MAX_DISTANCE 400 
#include <NewPing.h> 
NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); // NewPing setup of pins and maximum distance.

#define SECOND_TRIGGER 32
#define SECOND_ECHO 33
NewPing secondSensor(SECOND_TRIGGER, SECOND_ECHO, MAX_DISTANCE);

// Accelerometer/gyroscope module 
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
Adafruit_MPU6050 mpu;
sensors_event_t a, g, temp;
#include <EEPROM.h>
#define EEPROM_SIZE 1


// Two-way commnuication
#include <esp_now.h>
#include <WiFi.h>
uint8_t broadcastAddress[] = {0x0C, 0xB8, 0x15, 0xC3, 0x30, 0x00}; 

// Struct for data packets sent wirelessly 
typedef struct struct_message {
  char str[32];
  int dist;
  float angle;
  bool cal;
} struct_message;

struct_message myData;
esp_now_peer_info_t peerInfo;


typedef struct offset_struct {
  float x;
  float y;
  float z;
} offset_t;

offset_t offsets;

bool calibrating;

// callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  //Serial.print("\r\nLast Packet Send Status:\t");
  //Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  calibrating = true;
  memcpy(&myData, incomingData, sizeof(myData));
  calibrating = myData.cal; // should be true
  if (calibrating) {
    delay(4000); // give user some time to stand their cane straight up
    calibrateAccel(50);
  }
  
}

void clearOffsets() {
  offsets.x = 0;
  offsets.y = 0;
  offsets.z = 0;
}

void calibrateAccel(int measurements) { // recommended measurements = 50
  Serial.println("Calibrating now. POINT ULTRASONIC SENSOR STRAIGHT DOWN.");
  delay(500);
  
  float x = 0, y = 0, z = 0; // do not update offsets until value is calculated
  for (int i = 0; i < measurements; i++) { 
    mpu.getEvent(&a, &g, &temp); // take a measurement
    x += a.acceleration.x; y += a.acceleration.y; z += a.acceleration.z; 
    delay(100); 
  }
  
  x /= measurements; y /= measurements; z /= measurements;  // turn sums into averages 
  x += 9.81; // presumed to be the vertical downward direction 
  offsets.x = x; offsets.y = y; offsets.z = z; // update state 
  Serial.println("Done calibrating");
  
  myData.angle = 90.0; // write fake values that will not turn on motor
  myData.dist = 0;
  myData.cal = false; // resume normal loop operation on UI
  esp_err_t result = (broadcastAddress, (uint8_t *) &myData, sizeof(myData));
  calibrating = false; // resume normal loop opoeration on 

  Serial.println("Reaadings: " + String(offsets.x) + " " + String(offsets.y) + " " + String(offsets.z));

  offset_t old_offsets;
  EEPROM.get(0, old_offsets);
  Serial.println("Old offsets: " + String(old_offsets.x) + " " + String(old_offsets.y) + " " + String(old_offsets.z));
  EEPROM.put(0, offsets);
  EEPROM.commit();
  EEPROM.get(0, offsets);
  Serial.println("New offsets: " + String(offsets.x) + " " + String(offsets.y) + " " + String(offsets.z));

}

// Uses trig to calculate the angle between the cane's orientation and straight down 
float angle(float vert, float hor1, float hor2) {
    return atan2(hypot(hor1, hor2), -1*vert)*180.0/PI;
}
// uses measurement and offsets global variables 
float angleWithOffsets() {
  return angle(a.acceleration.x - offsets.x, a.acceleration.y - offsets.y, 
               a.acceleration.z - offsets.z);
}
 
void setup() {
  Serial.begin(9600);
  
   if (!mpu.begin()) { // error handling for accelerometer 
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }
  EEPROM.begin(EEPROM_SIZE);

  // ESP-NOW communication setup
  WiFi.mode(WIFI_STA); // Set device as a Wi-Fi Station
  if (esp_now_init() != ESP_OK) {  // Initialize ESP-NOW, handle errors
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_register_send_cb(OnDataSent); // bind the OnDataSent callback
  
  // Register the other ESP32 as a peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;       
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer"); // handle errors
    return;
  }

  esp_now_register_recv_cb(OnDataRecv); // bind the OnDataRecv callback

  Serial.println(EEPROM.read(0));
   EEPROM.get(0, offsets);
   Serial.println(String(offsets.x) + " " + String(offsets.y) + " " + String(offsets.z));
}
 
void loop() {
  if (!calibrating) { 
    myData.cal = false;

    // take measurements
    mpu.getEvent(&a, &g, &temp);
    myData.angle = angleWithOffsets();

    if (myData.angle <= 15) {
      myData.dist = sonar.ping_cm(); 
    } else if (myData.angle >= 165) {
      myData.dist = secondSensor.ping_cm();
      Serial.println(myData.dist); 
    } else {
      //myData.dist = 0;
    }

    
    // Send message via ESP-NOW, handle errors 
    esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
    if (result == ESP_OK) {
      //Serial.println("Sent with success");
    }
    else {
      //Serial.println("Error sending the data");
    }
  }
  delay(50);
}
