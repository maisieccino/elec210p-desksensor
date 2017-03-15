/*
  Analog input, analog output, serial output
 
 Reads an analog input pin, maps the result to a range from 0 to 255
 and uses the result to set the pulsewidth modulation (PWM) of an output pin.
 Also prints the results to the serial monitor.
 
 The circuit:
 * potentiometer connected to analog pin 0.
   Center pin of the potentiometer goes to the analog pin.
   side pins of the potentiometer go to +3.3V and ground
 * LED connected from digital pin 9 to ground
 
 created 29 Dec. 2008
 modified 30 Aug 2011
 by Tom Igoe
 
 This example code is in the public domain.
 
 */

// These constants won't change.  They're used to give names
// to the pins used:
const int analogInPin = 6;  // Analog input pin that the mic is attached to
const int pirPin = 5;


int sensorValue = 0;        // value read from the pot
int outputValue = 0;        // value output to the PWM (analog out)

void setup() {
  // initialize serial communications at 9600 bps:
  Serial.begin(9600); 
  pinMode(pirPin, INPUT);
  pinMode(RED_LED, OUTPUT);
 
}

void loop() {
  
  
  //microphone
  //find min and max peaks
  unsigned int signalMin = 4096;
  unsigned int signalMax = 0;
  unsigned long startM = millis();
  while( millis() < startM + 50) {
    // read the analog in value:
    sensorValue = analogRead(analogInPin); 
    if (sensorValue < signalMin) {
     signalMin = sensorValue; 
    }
    if (sensorValue > signalMax) {
      signalMax = sensorValue;
    }
  }
  
  //PIR
  int pirState = digitalRead(pirPin);
  if(pirState)
    digitalWrite(RED_LED, HIGH);
  else
    digitalWrite(RED_LED, LOW);
  
          
  // print the results to the serial monitor:
  Serial.print("PIR = ");
  Serial.print(pirState);
  Serial.print(" mic = " );                       
  //Serial.print(sensorValue);     
  //Serial.print(" p2p = ");
  Serial.println(signalMax - signalMin); 
  //Serial.print("\t output = ");      
  //Serial.println(outputValue);   

  // wait 10 milliseconds before the next loop
  // for the analog-to-digital converter to settle
  // after the last reading:
  //delay(10);                     
}
