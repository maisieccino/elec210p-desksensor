/*
Room Availibility Sensor
Alexis Enston, Matt Bell

Uses microphone and PIR to detect people in a room

 */

// These constants won't change.  They're used to give names
// to the pins used:
const int micPin = 6;  // Analog input pin that the mic is attached to
const int pirPin = 5;
const int micSampleTime = 50; //ms




void setup() {
  // initialize serial communications at 9600 bps:
  Serial.begin(9600); 
  pinMode(pirPin, INPUT);
  pinMode(RED_LED, OUTPUT);
 
}

int getMicValue() {
    //microphone
  //find min and max peaks
  unsigned int sensorValue = 0;
  unsigned int signalMin = 4096;
  unsigned int signalMax = 0;
  unsigned long startM = millis();
  while( millis() < startM + micSampleTime) {
    // read the analog in value:
    sensorValue = analogRead(micPin); 
    if (sensorValue < signalMin) {
     signalMin = sensorValue; 
    }
    if (sensorValue > signalMax) {
      signalMax = sensorValue;
    }
  }
  return signalMax - signalMin;
}

int getPIR() {
  return digitalRead(pirPin);
}

void loop() {
   
  //mic
  int micValue = getMicValue();
  
  //PIR
  int pirState = getPIR();
  if(pirState)
    digitalWrite(RED_LED, HIGH);
  else
    digitalWrite(RED_LED, LOW);
  
          
  // print the results to the serial monitor:
  Serial.print("PIR = ");
  Serial.print(pirState);
  Serial.print(" mic = " );       
  Serial.println(micValue); 
  
  //delay(10);                     
}
