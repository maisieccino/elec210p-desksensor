#include <aJSON.h>

/*
Mic + PIR IOT device
Based on repeating Wifi Web Client

 This sketch connects to a a web server and makes a request

 Circuit:
 * WiFi shield attached to pins SPI pins and pin 7

 created 23 April 2012
 modified 31 May 2012
 by Tom Igoe
 modified 13 Jan 2014
 by Federico Vanzati
 modified 6 July 2014
 by Noah Luskey

 This code is in the public domain.

 Circuit:
 * CC3200 WiFi LaunchPad or CC3100 WiFi BoosterPack
   with TM4C or MSP430 LaunchPad
 */

#ifndef __CC3200R1M1RGC__
// Do not include SPI for CC3200 LaunchPad
#include <SPI.h>
#endif
#include <WiFi.h>

// your network name also called SSID
char ssid[] = "rpinet";
// your network password
char password[] = "rpinet12";

// Initialize the Wifi client library
WiFiClient client;

// server address:
char server[] = "10.42.0.1";
//IPAddress server(10,42,0,1);

unsigned long lastConnectionTime = 0;            // last time you connected to the server, in milliseconds
const unsigned long postingInterval = 10L * 1000L; // delay between updates, in milliseconds


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
const int myID = 1;

long micTotalValue = 0;
int micValue = 0;
int micValueCount = 0;
int pir = 0;


void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(115200);
  
  //setup pins
  pinMode(pirPin, INPUT);
  pinMode(RED_LED, OUTPUT);

  // attempt to connect to Wifi network:
  Serial.print("Attempting to connect to Network named: ");
  // print the network name (SSID);
  Serial.println(ssid); 
  // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
  WiFi.begin(ssid, password);
  while ( WiFi.status() != WL_CONNECTED) {
    // print dots while we wait to connect
    Serial.print(".");
    delay(300);
  }
  
  Serial.println("\nYou're connected to the network");
  Serial.println("Waiting for an ip address");
  
  while (WiFi.localIP() == INADDR_NONE) {
    // print dots while we wait for an ip addresss
    Serial.print(".");
    delay(300);
  }

  Serial.println("\nIP Address obtained");
  // We are connected and have an IP address.
  // Print the WiFi status.
  printWifiStatus();
}

void loop() {
  // if there's incoming data from the net connection.
  // send it out the serial port.  This is for debugging
  // purposes only:
  while (client.available()) {
    char c = client.read();
    Serial.write(c);
  }
  
  //increment mic value
  micValue = getMicValue();
  micTotalValue += micValue;
  micValueCount++;
  
  int pirState = getPIR();
  pir += pirState;
  
  Serial.print("PIR = ");
  Serial.print(pir);
  Serial.print(" mtot = ");
  Serial.print(micTotalValue);
  Serial.print(" mic = " );       
  Serial.println(micValue); 
  

  // if ten seconds have passed since your last connection,
  // then connect again and send data:
  if (millis() - lastConnectionTime > postingInterval) {
    micValue = micTotalValue / micValueCount;
    httpRequest();
    micTotalValue = 0;
    micValueCount = 0;
    pir = 0;
  }

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

// this posts the data to the server
void httpRequest() {
  // close any connection before send a new request.
  // This will free the socket on the WiFi shield
  client.stop();
  
  //create json object with values
  
  //int pirValue = getPIR();
  int occupied = (micValue > 0 && pir > 0);
  
  aJsonObject *root = aJson.createObject();
  aJson.addItemToObject(root, "id", aJson.createItem(myID));
  aJson.addItemToObject(root, "occupied", aJson.createItem(occupied));
  aJson.addItemToObject(root, "micData", aJson.createItem(micValue));
  aJson.addItemToObject(root, "pirData", aJson.createItem(pir));
  char *json_string = aJson.print(root);
  aJson.deleteItem(root);

  // if there's a successful connection:
  if (client.connect(server, 8081)) {
    Serial.println("connecting...");
    // send the HTTP PUT request:
    client.println("POST /data HTTP/1.1");
    client.println("Host: superapp");
    client.println("User-Agent: Energia/1.1");
    client.println("Connection: close");
    client.print("Content-Length: ");
    client.println(strlen(json_string));
    client.println("Content-Type: application/x-www-form-urlencoded");
    client.println();
    client.println(json_string);
    client.println();
    
    free(json_string);

    // note the time that the connection was made:
    lastConnectionTime = millis();
  }
  else {
    // if you couldn't make a connection:
    Serial.println("connection failed");
  }
}


void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}


