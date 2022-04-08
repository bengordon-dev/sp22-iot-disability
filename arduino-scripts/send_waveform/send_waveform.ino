int waveForm[] = {255, 0, 255, 0};
int waveForm2[] = {255, 200, 0, 0};
int waveForm3[] = {250, 225, 200, 175, 0, 0, 0, 0};
int waveForm4[] = {255, 255, 255, 255, 0, 0, 0, 0};

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(23, OUTPUT);
  
}

void sendWaveform(int waveform[], int len, int delayMillis, int repeats) {
 for (int i = 0; i < repeats; i++) {
   for (int j = 0; j < len; j++) {
    analogWrite(23, waveform[j]);
    delay(delayMillis);
   }
 }
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println("Waveform 1");
  sendWaveform(waveForm, 4, 25, 50); 
  Serial.println("Waveform 2");
  sendWaveform(waveForm2, 4, 25, 50); 
  Serial.println("Waveform 3");
  sendWaveform(waveForm3, 8, 25, 25);
  Serial.println("Waveform 4");
  sendWaveform(waveForm4, 8, 25, 25);
  
}
