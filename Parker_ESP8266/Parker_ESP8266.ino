#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include <ThingSpeak.h>

#define SSID "parker"
#define PASS "12345678"
WiFiClient  client;

struct DistanceData {
  float distance1;
  float distance2;
  float distance3;
};

unsigned long THINGSPEAK_CHANNEL_ID = 2485028;
const char * THINGSPEAK_API_KEY = "OQZRONOA1SYGWKVH";
unsigned long lastTriggerTime = 0;

SoftwareSerial arduinoSerial(D6, D5); // RX, TX

void updateThingSpeakChannel(float distance1, float distance2, float distance3) {
  ThingSpeak.setField(1, distance1);
  ThingSpeak.setField(2, distance2);
  ThingSpeak.setField(3, distance3);
  int response = ThingSpeak.writeFields(THINGSPEAK_CHANNEL_ID, THINGSPEAK_API_KEY);
  if (response == 200) {
    Serial.println("ThingSpeak update successful");
  } else {
    Serial.println("Error updating ThingSpeak channel");
  }
}

void setup() {
  Serial.begin(9600);
  arduinoSerial.begin(9600);

  WiFi.begin(SSID, PASS);
  WiFi.mode(WIFI_STA);
  
  Serial.print("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  ThingSpeak.begin(client);
}

void loop() {
  DistanceData data;

  if (arduinoSerial.available() >= sizeof(data)) {
    arduinoSerial.readBytes((char*)&data, sizeof(data));
    Serial.print("Distance 1: ");
    Serial.print(data.distance1);
    Serial.println(" cm");
    Serial.print("Distance 2: ");
    Serial.print(data.distance2);
    Serial.println(" cm");
    Serial.print("Distance 3: ");
    Serial.print(data.distance3);
    Serial.println(" cm");

    updateThingSpeakChannel(data.distance1, data.distance2, data.distance3);
  }
}
