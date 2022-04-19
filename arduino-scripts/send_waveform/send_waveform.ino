int waveForm[] = {255, 0, 255, 0};
int waveForm2[] = {255, 200, 0, 0};
int waveForm3[] = {250, 225, 200, 175, 0, 0, 0, 0};
int waveForm4[] = {255, 255, 255, 255, 0, 0, 0, 0};

int waveForm5[] = {0, 155, 0, 255, 0};
int waveForm6[] = {255, 200, 0, 0};
int waveForm7[] = {250, 225, 200, 175, 0, 0, 0, 0};
int waveForm8[] = {255, 255, 255, 255, 0, 0, 0, 0};



int motorPin = 23;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(23, OUTPUT);
  
}

void sendWaveform(int waveform[], int len, int delayMillis, int repeats, bool printToConsole) {
  if (printToConsole) {
    Serial.println("Sending waveform"); 
  }
  for (int i = 0; i < repeats; i++) {
    for (int j = 0; j < len; j++) {
      analogWrite(23, waveform[j]);
      delay(delayMillis);
    }
  }
}

void sendFreqIntensity(int delayMillis, byte intensity, int repeats, bool printToConsole) {
  if (printToConsole) {
    Serial.println("Delay: " + String(delayMillis) + ", Intensity: " + String(intensity) + ", Repeats: " + String(repeats)); 
  }
  for (int i = 0; i < repeats; i++) {
    analogWrite(motorPin, intensity);
    delay(delayMillis);
    analogWrite(motorPin, 0);
    delay(delayMillis);
  }
}

void flushMotor(int delayMillis) {
  analogWrite(23, 0);
  delay(delayMillis);
}

void loop() {
  // put your main code here, to run repeatedly:
  /*Serial.println("Waveform 1");
  sendWaveform(waveForm, 4, 25, 50); 
  Serial.println("Waveform 2");
  sendWaveform(waveForm2, 4, 25, 50); 
  Serial.println("Waveform 3");
  sendWaveform(waveForm3, 8, 25, 25);
  Serial.println("Waveform 4");
  sendWaveform(waveForm4, 8, 25, 25);*/

  Serial.println("Waveform 3, repeated 2 times");
  sendWaveform(waveForm3, 8, 100, 3, true);
  flushMotor(2000);

  for (int i = 1; i <= 10; i++) {
    sendFreqIntensity(i*5, 255, 1000/i/5, true);
    flushMotor(2000);
  }

  
  
}
