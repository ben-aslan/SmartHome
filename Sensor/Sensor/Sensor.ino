#include <nRF24L01.h>
#include <RF24.h>

#define nrts A0
#define ldrphotocell A1
#define led 2
#define buzzer 4
#define lamp_pin 3
#define komed_1 8
#define komed_2 7

unsigned long startMillis;
unsigned long startMillis2;
unsigned long startMillis3;
unsigned long currentMillis;
const unsigned long period = 10000;

const uint64_t pipeOut = 0xE84D84FFLL;   //IMPORTANT: The same as in the receiver!!!
const uint64_t pipeOut2 = 0xE84D85FFLL;  //IMPORTANT: The same as in the receiver!!!

RF24 radio(9, 10);

bool lampStatus = 0;

struct Data {
  float thermometere;
  uint8_t brightness = 1023;
  bool lamp;
  bool commode1;
  bool commode2;
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
  pinMode(komed_1, INPUT);
  pinMode(komed_2, INPUT);
  digitalWrite(led, 0);
  // put your setup code here, to run once:
  radio.begin();
  // radio.setAutoAck(false);
  // radio.setDataRate(RF24_250KBPS);

  pinMode(lamp_pin, OUTPUT);

  digitalWrite(lamp_pin, lampStatus);

  radio.setChannel(84);            // Channel(0 ... 127)
  radio.setDataRate(RF24_2MBPS);   // RF24_250KBPS, RF24_1MBPS, RF24_2MBPS)
  radio.setPALevel(RF24_PA_HIGH);  //RF24_PA_MIN=-18dBm, RF24_PA_LOW=-12dBm, RF24_PA_HIGH=-6dBm, RF24_PA_MAX=0dBm

  radio.openWritingPipe(pipeOut);
  radio.openReadingPipe(0, pipeOut2);
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
  currentMillis = millis();
  if (currentMillis - startMillis2 >= 100) {
    if (radio.available()) {
      resetData();
      Data a;
      radio.read(&a, sizeof(a));

      lampStatus = a.lamp;

      startMillis2 = currentMillis;
    }

    if (currentMillis - startMillis >= 100) {
      radio.stopListening();
      data.thermometere = Termistor(analogRead(nrts));
      data.brightness = map(analogRead(ldrphotocell), 0, 1023, 0, 255);
      data.lamp = digitalRead(3);
      data.commode1 = digitalRead(7);
      data.commode2 = digitalRead(8);

      radio.write(&data, sizeof(Data));


      if (Termistor(analogRead(nrts)) <= 35) {
        noTone(buzzer);
      }

      radio.startListening();
      startMillis = currentMillis;
    }
  }

  if (currentMillis - startMillis3 >= 100) {
    digitalWrite(lamp_pin, lampStatus);
    startMillis3 = currentMillis;
  }

  if (radio.isChipConnected() == 1) {
    digitalWrite(led, 0);
  }

  if (Termistor(analogRead(nrts)) >= 35) {
    tone(buzzer, 5000);
  }
}
