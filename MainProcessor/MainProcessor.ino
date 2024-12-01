#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <printf.h>
#include "NewPing.h"
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const char* ssid = "...";
const char* password = "...";
const char* mqtt_server = "...";

unsigned long startMillis;
unsigned long startMillis2;
unsigned long currentMillis;
const unsigned long period = 10000;

WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE (50)
char msg[MSG_BUFFER_SIZE];
int value = 0;

#define CE_PIN D4
#define CSN_PIN D8
#define TRIGGER_PIN D1
#define ECHO_PIN D2
#define led D3
#define MAX_DISTANCE 400
// const uint64_t pipe = 0xe7e7e7e7e7LL;
const uint64_t pipe = 0xE84D84FFLL;
const uint64_t pipe2 = 0xE84D85FFLL;

RF24 radio(CE_PIN, CSN_PIN);

NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);
float duration, distance;

struct data {
  byte thermometere;
  byte brightness;
  bool lamp;
} a;

bool lampStatus = true;

void setup_wifi() {
  delay(10);

  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  printf_begin();
  radio.printDetails();
  Serial.print("Is connected : ");
  Serial.println(radio.isChipConnected());

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  digitalWrite(led, 1);
  delay(500);
  digitalWrite(led, 0);
  delay(500);
  digitalWrite(led, 1);
  delay(500);
  digitalWrite(led, 0);
  delay(500);

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  char* response;
  for (int i = 0; i < length; i++) {
    response += (char)payload[i];
  }
  Serial.println();

  // if (topic == "lamp")
  lampStatus = (char)payload[0] == '1';
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(D3, OUTPUT);
  // pinMode(3, OUTPUT);
  // pinMode(5, OUTPUT);

  radio.begin();
  radio.setChannel(84);            // Channel(0 ... 127)
  radio.setDataRate(RF24_1MBPS);   // RF24_250KBPS, RF24_1MBPS, RF24_2MBPS)
  radio.setPALevel(RF24_PA_HIGH);  //RF24_PA_MIN=-18dBm, RF24_PA_LOW=-12dBm, RF24_PA_HIGH=-6dBm, RF24_PA_MAX=0dBm
  radio.openReadingPipe(0, pipe);
  radio.openWritingPipe(pipe2);
  radio.startListening();

  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  digitalWrite(led, 1);
  delay(1000);
  if (radio.isChipConnected()) {
    digitalWrite(led, 0);
  }


  // if(!radio.available())
  //   digitalWrite(2,1);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  if (currentMillis - startMillis2 >= 100) {
    radio.stopListening();
    a.lamp = lampStatus;
    radio.write(&a, sizeof(a));
    radio.startListening();
    startMillis2 = currentMillis;
  }

  currentMillis = millis();

  if (sonar.ping_cm() <= 50) {
    //Serial.println(sonar.ping_cm());
    // digitalWrite(2, 1);
    // Serial.println("alarm");
  } else {
    // digitalWrite(2, 0);
  }

  if (currentMillis - startMillis >= period) {
    if (radio.available()) {
      radio.read(&a, sizeof(a));
      Serial.println((String)a.thermometere + " || " + a.brightness + " || " + sonar.ping_cm());
    }
    startMillis = currentMillis;
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      digitalWrite(led, 1);
      delay(500);
      digitalWrite(led, 0);
      delay(500);
      digitalWrite(led, 1);
      delay(500);
      digitalWrite(led, 0);
      delay(500);
      digitalWrite(led, 1);
      delay(500);
      digitalWrite(led, 0);
      delay(500);

      Serial.println("connected");
      // Once connected, publish an announcement...
      // client.publish("outTopic", "hello world");
      // ... and resubscribe
      client.subscribe("lamp");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}