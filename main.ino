/*
#include <WiFi.h>
#include <PubSubClient.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include "DHT.h"


// === USER CONFIG ===
const char* ssid = "Mahasiswa";
const char* password = "mahasiswaupj";
const char* mqtt_server = "192.168.1.50"; // ganti IP broker
const int mqtt_port = 1883; // 1883 for TCP; dash uses ws://:9001


// Pins
#define DHTPIN 15
#define DHTTYPE DHT22
#define PIRPIN 14
#define RELAYPIN 13
#define ALARMPIN 12
// Topics
const char* topic_dht = "home/sensors/dht";
const char* topic_pir = "home/sensors/pir";
const char* topic_control_relay = "home/control/relay";
const char* topic_control_alarm = "home/control/alarm";
const char* topic_status_relay = "home/status/relay";
const char* topic_status_alarm = "home/status/alarm";


WiFiClient espClient;
PubSubClient client(espClient);
DHT dht(DHTPIN, DHTTYPE);


// OLED
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);


unsigned long lastDhtPublish = 0;
const unsigned long DHT_INTERVAL = 5000; // 5s


void callback(char* topic, byte* payload, unsigned int length) {
String msg;
for (unsigned int i = 0; i < length; i++) msg += (char)payload[i];


if (String(topic) == topic_control_relay) {
if (msg == "ON") { digitalWrite(RELAYPIN, HIGH); client.publish(topic_status_relay, "ON", true); }
else { digitalWrite(RELAYPIN, LOW); client.publish(topic_status_relay, "OFF", true); }
} else if (String(topic) == topic_control_alarm) {
if (msg == "ON") { digitalWrite(ALARMPIN, HIGH); client.publish(topic_status_alarm, "ON", true); }
else { digitalWrite(ALARMPIN, LOW); client.publish(topic_status_alarm, "OFF", true); }
}
}


void reconnect() {
while (!client.connected()) {
Serial.print("Attempting MQTT connection...");
String clientId = "ESP32Client-" + String(random(0xffff), HEX);
if (client.connect(clientId.c_str())) {
Serial.println("connected");
client.subscribe(topic_control_relay);
client.subscribe(topic_control_alarm);
} else {
Serial.print("failed, rc="); Serial.print(client.state());
Serial.println(" try again in 2 seconds");
delay(2000);
}
}
}


void setup() {
Serial.begin(115200);
pinMode(PIRPIN, INPUT);
pinMode(RELAYPIN, OUTPUT);
pinMode(ALARMPIN, OUTPUT);
dht.begin();
if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Adjust I2C address if needed
  Serial.println(F("SSD1306 allocation failed"));
  for (;;);
}
display.clearDisplay();
display.setTextSize(1);
display.setTextColor(WHITE);
display.setCursor(0,0);
display.println("Initializing...");
display.display();
WiFi.begin(ssid, password);
while (WiFi.status() != WL_CONNECTED) {
  delay(500);
  Serial.print(".");
}
Serial.println("WiFi connected");
client.setServer(mqtt_server, mqtt_port);
client.setCallback(callback);
reconnect();
}


void loop() {
if (!client.connected()) {
  reconnect();
}
client.loop();

unsigned long now = millis();
if (now - lastDhtPublish > DHT_INTERVAL) {
  lastDhtPublish = now;
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  String dhtMsg = "Temperature: " + String(t) + "C, Humidity: " + String(h) + "%";
  client.publish(topic_dht, dhtMsg.c_str());
  // Update OLED
  display.clearDisplay();
  display.setCursor(0,0);
  display.println("Temp: " + String(t) + "C");
  display.println("Hum: " + String(h) + "%");
  display.println("Relay: " + (digitalRead(RELAYPIN) ? "ON" : "OFF"));
  display.println("Alarm: " + (digitalRead(ALARMPIN) ? "ON" : "OFF"));
  display.display();
}

int pirState = digitalRead(PIRPIN);
if (pirState == HIGH) {
  client.publish(topic_pir, "motion");
  Serial.println("Motion detected!");
  // Optional: Add a delay or flag to prevent spamming
}
}

