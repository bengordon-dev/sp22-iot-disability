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

#include <Arduino_JSON.h>
#include "ESPAsyncWebServer.h"

const char* ssid = "utexas-iot";
const char* password = "no lol";

JSONVar board;
AsyncWebServer server(80);
AsyncEventSource events("/events");

// Struct for data packets sent wirelessly 
typedef struct struct_message {
    char str;
    float dist;
    float angle;
    bool cal;
} struct_message;

struct_message myData;
esp_now_peer_info_t peerInfo;


int motorPin = 23;
int buttonPin = 34;

bool calibrating = false;

// MAC Address of the other ESP32
uint8_t broadcastAddress[] = {0x0C, 0xB8, 0x15, 0xC1, 0x99, 0xD0}; 

// Motor related code

int waveForm[] = {255, 0, 255, 0};
int pulse[] = {250, 225, 200, 175, 0, 0, 0, 0};

void sendWaveform(int waveform[], int len, int delayMillis, int repeats) {
 for (int i = 0; i < repeats; i++) {
   for (int j = 0; j < len; j++) {
    analogWrite(motorPin, waveform[j]);
    delay(delayMillis);
   }
 }
}

// Waveform simulator: Repeatedly flips motor on for a given time, then off for half that time.
// Intensity <= 255; minimum (transistor cutoff) is unclear 
void sendDelayIntensity(int delayMillis, byte intensity, int repeats, bool printToConsole) {
  if (printToConsole) {
    Serial.println("Delay: " + String(delayMillis) + ", Intensity: " + String(intensity) + ", Repeats: " + String(repeats)); 
  }
  for (int i = 0; i < repeats; i++) {
    analogWrite(motorPin, intensity);
    delay(delayMillis);
    flushMotor(delayMillis/2);
  }
}

// Turn motor off for delayMillis
void flushMotor(int delayMillis) {
  analogWrite(motorPin, 0);
  delay(delayMillis);
}

void sendFreqIntensity(int cyclesPerSecond, int durationMillis, bool printToConsole) {
  sendDelayIntensity(1000/cyclesPerSecond/2, 255, durationMillis*cyclesPerSecond/1000, printToConsole);
}

// Arbitrary but useful. Significant changes in distance --> detectable, consistent changes in output
int delayFunc(float dist) {
  double freq = 10.0 + 1.40625*pow(((double) dist)/30.0, 1.8);
  return (int) 2000.0/freq;
}

int intensityFunc(float dist) {
  if (dist <= 1) {
    return 0;
  }
  double out = 200.0 + 55.0/20.0*sqrt((double) dist);
  return (int) out;
}

// ESP-NOW callbacks, called asynchonously during data transmission.

// called upon receiving a packet from the other ESP
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&myData, incomingData, sizeof(myData));

  if (myData.str == 'D') {
    sendWaveform(pulse, 8, 250, 2);
    Serial.println("Done calibrating");
  }
  if (myData.str == 'A') {
    sendWaveform(pulse, 8, 63, 4);
    Serial.println("Done calibrating accelerometer");
  }
  calibrating = myData.cal; // resume loop operation after calibration
  
  //Serial.println("Distance: " + String(myData.dist) + " cm");
  //Serial.println("Angle to vertical: " + String(myData.angle) + " degrees");
  //if (calibrating) {
    // Serial.println("Calibrating");
  //}
}

// called upon sending a packet to the other ESP
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}
 
void setup() {
  //calibrating = true;
  Serial.begin(9600);
  Serial.println(WiFi.macAddress());
    
  pinMode(motorPin, OUTPUT);
  pinMode(buttonPin, INPUT);

  // ESP-NOW communication setup
  WiFi.mode(WIFI_AP_STA); // Set device as a Wi-Fi Station
  if (esp_now_init() != ESP_OK) { // Initialize ESP-NOW, handle errors
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

  sendWaveform(pulse, 8, 100, 3);

  // Connect to Wi-Fi network with SSID and password
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.print("Wi-Fi Channel: ");
  Serial.println(WiFi.channel());

  //server.enableCORS();
  //server.on("/read", handleRead);
  server.on("/read", HTTP_GET, [](AsyncWebServerRequest *request){
    String json = "{\"distance\": " + String(myData.dist) + ", \"angle\": " + String(myData.angle) + "}";
    
    request->send_P(200, "text/json", json.c_str());
  });

  events.onConnect([](AsyncEventSourceClient *client){
    if(client->lastId()){
      Serial.printf("Client reconnected! Last message ID that it got is: %u\n", client->lastId());
    }
    // send event with message "hello!", id current millis
    // and set reconnect delay to 1 second
    client->send("hello!", NULL, millis(), 10000);
  });
  server.addHandler(&events);
   
  
  //calibrating = false;
  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
  server.begin();


  
}

//void handleRead() {
  //server.send(200, "text/json", "{\"distance\": " + String(myData.dist) + ", \"angle\": " + String(myData.angle) + "}"); 
//}


void loop() {
  if (!calibrating) { 
    int buttonPressed = digitalRead(buttonPin);
    if (buttonPressed == HIGH) { // start calibration sequence, exit normal execution loop
      calibrating = true; 
      myData.cal = true;
      esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
      if (result == ESP_OK) {
        Serial.println("Sent with success");
      }
      else {
        Serial.println("Error sending the data");
      }
    } else if ((myData.angle <= 60)  && myData.dist > 1) { // write to motor output
       // delay: 100 to 10
      sendDelayIntensity(delayFunc(myData.dist), intensityFunc(myData.dist), 1, false);
    }
    //server.handleClient();
  }
}
