/*********
  Rui Santos
  Complete project details at https://randomnerdtutorials.com  
*********/

// Load Wi-Fi library
#include <WiFi.h>
#include <WebServer.h>


#define TRIGGER_PIN  19
#define ECHO_PIN     23
#define MAX_DISTANCE 400 // Maximum distance we want to measure (in centimeters).
#include <NewPing.h> // Used for the HC SR-04 ultrasonic sensor
NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); // NewPing setup of pins and maximum distance.

#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

Adafruit_MPU6050 mpu;

// Replace with your network credentials
const char* ssid = "utexas-iot";
const char* password = "LOL";

// Set web server port number to 80
WebServer server(3000);

// Variable to store the HTTP request
String header;

// Auxiliar variables to store the current output state

int distanceState = 0;
float angleState = 0.0;


// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0; 
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;

void setup() {
  Serial.begin(9600);
  Serial.println(WiFi.macAddress());

  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
   }
  }

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

  server.enableCORS();
  server.on("/read", handleRead);
  server.begin();

}

void handleRead() {
  distanceState = sonar.ping_cm();
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);
  float nonvert = hypot(a.acceleration.z, a.acceleration.y);
  angleState = atan2(nonvert, -1*a.acceleration.x)*180/PI;
  server.send(200, "text/json", "{\"distance\": " + String(distanceState) + ", \"angle\": " + String(angleState) + "}"); 
}

void loop(){
 
  server.handleClient();
  /*WiFiClient client = server.available();   // Listen for incoming clients


  if (client) {                             // If a new client connects,
    currentTime = millis();
    previousTime = currentTime;
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected() && currentTime - previousTime <= timeoutTime) {  // loop while the client's connected
      currentTime = millis();
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/json");
            client.println("Connection: close");
            client.println();

         
            
            // API, basically.
            if (header.indexOf("GET /read") >= 0) {
               distanceState = sonar.ping_cm();
              sensors_event_t a, g, temp;
              mpu.getEvent(&a, &g, &temp);
              float nonvert = hypot(a.acceleration.z, a.acceleration.y);
              angleState = atan2(nonvert, -1*a.acceleration.x)*180/PI;
              //Serial.print("Distance: ");
              //Serial.print(disrta);
             
            }
            
            
            // Display the HTML web page
            //client.println("<!DOCTYPE html><html>");
            //client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            //client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the on/off buttons 
            // Feel free to change the background-color and font-size attributes to fit your preferences
            //client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            //client.println(".button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px;");
            //client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            //client.println(".button2 {background-color: #555555;}</style></head>");
            
            // Web Page Heading
            client.println("{\"distance\": " + String(distanceState) + ", \"angle\": " + String(angleState) + "}");
            //client.println("<p><a href=\"/read\"><button class=\"button\">READ</button></a></p>");
            
           
            //client.println("</body></html>");
            
            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");*/
  //}
}
