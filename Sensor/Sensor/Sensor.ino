#include <nRF24L01.h>  //Downlaod it here: https://www.electronoobs.com/eng_arduino_NRF24_lib.php
#include <RF24.h>

#define nrts A0
#define ldrphotocell A1
#define led 2
#define buzzer 4

unsigned long startMillis;
unsigned long currentMillis;
const unsigned long period = 10000;

const uint64_t pipeOut = 0xE84D84FFLL;  //IMPORTANT: The same as in the receiver!!!

RF24 radio(9, 10);

struct Data {
  int thermometere;
  int brightness;
};

Data data;

void resetData() {
  //This are the start values of each channal
  // Throttle is 0 in order to stop the motors
  //127 is the middle value of the 10ADC.

  data.thermometere = 0;
  data.brightness = 0;
}

void setup() {
  pinMode(led, OUTPUT);
  pinMode(buzzer, OUTPUT);
  digitalWrite(led, 1);
  // put your setup code here, to run once:
  radio.begin();
  // radio.setAutoAck(false);
  // radio.setDataRate(RF24_250KBPS);
  radio.openWritingPipe(pipeOut);
  resetData();

  Serial.begin(9600);

  radio.printDetails();
  Serial.print("Is connected : ");
  Serial.println(radio.isChipConnected());

  startMillis = millis();

  if (radio.isChipConnected() == 1) {
    digitalWrite(2, 0);
  }
}

double Termistor(int analogOkuma) {
  double sicaklik;
  sicaklik = log(((10240000 / analogOkuma) - 10000));
  sicaklik = 1 / (0.001129148 + (0.000234125 + (0.0000000876741 * sicaklik * sicaklik)) * sicaklik);
  sicaklik = sicaklik - 273.15;
  return sicaklik;
}

void loop() {
  if (radio.isChipConnected() == 1) {
    digitalWrite(led, 0);
  }

  if (Termistor(analogRead(nrts)) >= 35) {
    tone(buzzer, 5000);
  }

  currentMillis = millis();
  if (currentMillis - startMillis >= period) {
    data.thermometere = Termistor(analogRead(nrts));
    data.brightness = analogRead(ldrphotocell);

    Serial.println(Termistor(analogRead(nrts)));

    radio.write(&data, sizeof(Data));


    if (Termistor(analogRead(nrts)) <= 35) {
      noTone(buzzer);
    }

    startMillis = currentMillis;
  }
}
