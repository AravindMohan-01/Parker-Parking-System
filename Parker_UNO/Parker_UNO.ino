#include <SoftwareSerial.h>
#include <SPI.h>
#include <RFID.h>
#include <Servo.h>

struct DistanceData {
  float distance1;
  float distance2;
  float distance3;
};

SoftwareSerial espSerial(5, 6); // RX, TX

const int trig1 = 7;
const int echo1 = 8;
const int trig2 = 3;
const int echo2 = 4;
const int trig3 = 0;
const int echo3 = 1;
float pingTime1, pingTime2, pingTime3;
float sSpeed = 0.034;

RFID rfid(10, 9);
Servo servoMotor;

void setup() {
  Serial.begin(9600);
  espSerial.begin(9600);
  pinMode(trig1, OUTPUT);
  pinMode(echo1, INPUT);
  pinMode(trig2, OUTPUT);
  pinMode(echo2, INPUT);
  pinMode(trig3, OUTPUT);
  pinMode(echo3, INPUT);

  SPI.begin();
  rfid.init();
  pinMode(8, OUTPUT);
  servoMotor.attach(8);
  Serial.println("");
  Serial.println("Parker Parking System");
  Serial.println("-----------------------");
  Serial.println("193 91 136 36");
  Serial.println("Registered User");
  Serial.println("Allowed...");
}

void loop() {
  DistanceData data;

  digitalWrite(trig1, LOW);
  delayMicroseconds(2000);
  digitalWrite(trig1, HIGH);
  delayMicroseconds(15);
  digitalWrite(trig1, LOW);
  pingTime1 = pulseIn(echo1, HIGH);
  data.distance1 = sSpeed * pingTime1 / 2;

  digitalWrite(trig2, LOW);
  delayMicroseconds(2000);
  digitalWrite(trig2, HIGH);
  delayMicroseconds(15);
  digitalWrite(trig2, LOW);
  pingTime2 = pulseIn(echo2, HIGH);
  data.distance2 = sSpeed * pingTime2 / 2;

  digitalWrite(trig3, LOW);
  delayMicroseconds(2000);
  digitalWrite(trig3, HIGH);
  delayMicroseconds(15);
  digitalWrite(trig3, LOW);
  pingTime3 = pulseIn(echo3, HIGH);
  data.distance3 = sSpeed * pingTime3 / 2;


  // Sending data to ESP8266
  espSerial.write((uint8_t*)&data, sizeof(data));

  // RFID functionality
  if (rfid.isCard()) {
    if (rfid.readCardSerial()) {
      String rfidCard = String(rfid.serNum[0]) + " " + String(rfid.serNum[1]) + " " + String(rfid.serNum[2]) + " " + String(rfid.serNum[3]);
      Serial.println(rfidCard);
      if (rfidCard == "193 91 136 36") {
        Serial.println("Registered User");
        Serial.println("Allowed...");
        servoMotor.write(270);
        delay(1000);
        servoMotor.write(90);
        delay(1000);
      } else {
        Serial.println("Not a Registered User");
        Serial.println("Not Allowed...");
      }
    }
    rfid.halt();
  }

  delay(250);
}
