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
pinMode(PI
