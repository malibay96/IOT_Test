#include <ThingSpeak.h>
#include <SPI.h>
#include <WiFi.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include "DHT.h"

// Data wire is plugged into port 2 on the Arduino
#define SECRET_SSID "MySSID"  // replace MySSID with your WiFi network name
#define SECRET_PASS "MyPassword" // replace MyPassword with your WiFi password
#define ONE_WIRE_BUS 32
#define SECRET_CH_ID 0000000                    // replace 0000000 with your channel number
#define SECRET_WRITE_APIKEY "XYZ" // replace XYZ with your channel write API Key

char ssid[] = SECRET_SSID; // your network SSID (name)
char pass[] = SECRET_PASS; // your network password
int keyIndex = 0;          // your network key Index number (needed only for WEP)
WiFiClient client;
unsigned long myChannelNumber = SECRET_CH_ID;
const char *myWriteAPIKey = SECRET_WRITE_APIKEY;

OneWire oneWire(ONE_WIRE_BUS);
// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);

const int switchPin = 33; //switch pin
const int ledPin =  27;  //led pin
int buttonState = 0; //buton konumu

#define DHTPIN 26
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

void setup()
{
 Serial.begin(115200); //Initialize serial
 sensors.begin();
 WiFi.mode(WIFI_STA);
 ThingSpeak.begin(client); // Initialize ThingSpeak
 
 pinMode(ledPin, OUTPUT);
 pinMode(switchPin, INPUT);

 Serial.println(F("DHTxx test!"));
 dht.begin();
}

void loop()
{
 // Connect or reconnect to WiFi
 if (WiFi.status() != WL_CONNECTED)
 {
   Serial.print("Attempting to connect to SSID: ");
   Serial.println(SECRET_SSID);
   while (WiFi.status() != WL_CONNECTED)
   {
     WiFi.begin(ssid, pass); // Connect to WPA/WPA2 network. Change this line if using open or WEP network
     Serial.print(".");
     delay(5000);
   }
   Serial.println("\nConnected.");
 }

 
 buttonState = digitalRead(switchPin);
 if (buttonState == HIGH) {
    // turn LED on:
    digitalWrite(ledPin, HIGH);
  } else {
    // turn LED off:
    digitalWrite(ledPin, LOW);
  }


  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }
  // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(t, h, false);


  
 //Serial.println("Requesting temperatures...");
 sensors.requestTemperatures(); // Send the command to get temperatures
// int x = ThingSpeak.writeField(myChannelNumber, 1, sensors.getTempCByIndex(0), myWriteAPIKey);
ThingSpeak.setField(1, sensors.getTempCByIndex(0));
ThingSpeak.setField(2, buttonState);
ThingSpeak.setField(3, h);
ThingSpeak.setField(4, hic);
ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
//Serial.println(buttonState);

 delay(1000); // Wait 2 seconds to update the channel again
}
