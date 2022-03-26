
#define TRIGGER_PIN  22
#define ECHO_PIN     23
#define MAX_DISTANCE 400 // Maximum distance we want to measure (in centimeters).
#include <NewPing.h> // Used for the HC SR-04 ultrasonic sensor

NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); // NewPing setup of pins and maximum distance.


void setup() {
   Serial.begin(9600);
}

void loop() {
    delay(100);
    int distance = sonar.ping_cm();   
    Serial.print("Distance: ");
    Serial.print(distance);
    Serial.println("cm");
}
