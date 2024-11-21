#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <printf.h>
#include "NewPing.h"

unsigned long startMillis;
unsigned long currentMillis;
const unsigned long period = 10000;

#define CE_PIN 7
#define CSN_PIN 10
#define TRIGGER_PIN 9
#define ECHO_PIN 8
#define MAX_DISTANCE 400
// const uint64_t pipe = 0xe7e7e7e7e7LL;
const uint64_t pipe = 0xE84D84FFLL;

RF24 radio(CE_PIN, CSN_PIN);

NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);
float duration, distance;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(5, OUTPUT);

  radio.begin();
  //radio.setAutoAck(false);
  radio.openReadingPipe(1, pipe);
  //radio.setDataRate(RF24_250KBPS);
  //radio.setPALevel(RF24_PA_LOW);
  radio.startListening();

  printf_begin();
  radio.printDetails();
  Serial.print("Is connected : ");
  Serial.println(radio.isChipConnected());

  digitalWrite(2, 1);
  delay(1000);
  if (radio.isChipConnected()) {
    digitalWrite(2, 0);
  }


  // if(!radio.available())
  //   digitalWrite(2,1);
}

byte value[2];

struct data {
  int thermometer;
  int brightness;
  String d2;
  int d3;
} a;

void loop() {
  currentMillis = millis();

  if (sonar.ping_cm() <= 50) {
    //Serial.println(sonar.ping_cm());
    digitalWrite(2, 1);
    Serial.println("alarm");
  } else {
    digitalWrite(2, 0);
  }

  if (currentMillis - startMillis >= period) {
    if (radio.available()) {
      radio.read(&a, sizeof(a));
      Serial.println((String)a.thermometer + " || " + a.brightness + " || " + sonar.ping_cm());
    } else {
    }
    startMillis = currentMillis;
  }
}