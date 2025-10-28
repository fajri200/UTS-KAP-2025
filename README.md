# UTS-KAP-2025
UTS KAP 2025
# Smart Home IoT — Ringkasan


Sistem Smart Home ini menampilkan contoh end-to-end:
- Dua sensor: DHT22 (temperature & humidity) dan PIR motion sensor.
- Dua aktuator: Relay untuk lampu (on/off) dan buzzer/LED sebagai alarm (on/off).
- Microcontroller: ESP32 (menghubungkan sensor & aktuator, menampilkan data pada OLED, dan terhubung ke MQTT broker).
- Display: SSD1306 128x64 OLED untuk menampilkan nilai sensor dan status aktuator.
- MQTT Broker: Mosquitto dengan WebSocket enabled agar dashboard HTML dapat terhubung.
- Dashboard: `dash.html` (HTML + JavaScript) yang menggunakan MQTT over WebSockets (Paho.js) untuk menampilkan data real-time dan mengirim perintah.


## Cara Kerja Singkat
ESP32 membaca DHT22 secara periodik dan membaca status PIR. Data dikemas dalam JSON lalu dipublish ke topic MQTT. Dashboard berlangganan (subscribe) ke topic tersebut dan menampilkan data realtime. Dashboard juga dapat mengirim pesan (publish) ke topic kontrol untuk mengubah status relay / alarm; ESP32 mendengarkan topic kontrol untuk mengeksekusi perintah.


---


## Hardware yang digunakan
- ESP32 Dev Kit (mis. ESP32-WROOM)
- Sensor DHT22 (AM2302) — temperature & humidity
- PIR motion sensor (HC-SR501)
- Relay module 1-channel (5V) — mengendalikan lampu/AC
- Buzzer atau LED (sebagai alarm)
- SSD1306 OLED 128x64 (I2C)
- Kabel jumper, breadboard, power supply (5V/USB)


## Software / Tools
- Arduino IDE (board: ESP32)
- Libraries:
- `PubSubClient` (MQTT)
- `Adafruit_SSD1306` & `Adafruit_GFX` (OLED)
- `DHT sensor library` (for DHT22)
- `Wire.h` (I2C)
- Mosquitto MQTT broker (with WebSocket enabled)
- Browser modern (Chrome/Firefox) to open `dash.html`


Tabel koneksi (ESP32 pins contoh):


| Komponen | ESP32 Pin | Keterangan |
|----------------|-------------------|----------------------|
| DHT22 DATA | GPIO 15 | Digital input (pull-up 10k)
| DHT22 VCC | 3.3V | Power
| DHT22 GND | GND | Ground
| PIR OUT | GPIO 14 | Digital input
| PIR VCC | 5V (atau 3.3V) | Power (cek modul)
| PIR GND | GND | Ground
| Relay IN | GPIO 13 | Digital output (drive relay)
| Relay VCC | 5V | Power for relay module
| Relay GND | GND | Ground
| Buzzer/LED | GPIO 12 | Digital output (with resistor if LED)
| OLED SDA | GPIO 21 (SDA) | I2C SDA
| OLED SCL | GPIO 22 (SCL) | I2C SCL
| ESP32 VIN/USB | 5V USB | Power


> Catatan: Pilih pin yang kompatibel dengan papan ESP32 Anda. Jika relay module memerlukan 5V TTL, pastikan ground bersama (common ground). Gunakan transistor/driver jika dibutuhkan untuk mengendalikan beban lebih besar.


Skematik sederhana (ASCII):

[DHT22] DATA --- GPIO15 (ESP32) VCC --- 3.3V GND --- GND

[PIR] OUT --- GPIO14 (ESP32) [PIR] VCC --- 5V [PIR] GND --- GND

[Relay] IN --- GPIO13 (ESP32) [Relay] VCC --- 5V [Relay] GND --- GND [Lamp] connected through relay normally-open contact

[Buzzer/LED] --- GPIO12 (with resistor if LED)

[OLED SSD1306] SDA --- GPIO21 SCL --- GPIO22 VCC --- 3.3V GND --- GND

[ESP32 USB] --- Power


 ESP32 (MQTT client) 
 sensors -> publish -> broker 
 subscribe -> actuators control 

 Network assumptions:
- Broker reachable at LAN IP (e.g., `192.168.1.50`) and WebSocket port `9001` open.


---


## Topik & Payload MQTT


**Sensor topics (publish dari ESP32):**
- `home/sensors/dht` — JSON: `{"temperature": 27.4, "humidity": 65.2}`
- `home/sensors/pir` — JSON: `{"motion": true, "timestamp": "2025-10-28T10:00:00"}`


**Status topics (publish/retain):**
- `home/status/relay` — payload: `ON` or `OFF` (retain)
- `home/status/alarm` — payload: `ON` or `OFF` (retain)


**Control topics (subscribe oleh ESP32):**
- `home/control/relay` — payload: `ON` or `OFF`
- `home/control/alarm` — payload: `ON` or `OFF`


All sensor payloads are JSON for extensibility; control topics accept simple strings.


---


## Cara Instalasi


1. **Persiapkan Mosquitto broker**
- Instal Mosquitto pada server Linux/PC (mis. Debian/Ubuntu): `sudo apt install mosquitto mosquitto-clients`
- Letakkan `mosquitto.conf` yang disediakan atau tambahkan listener WebSocket. Restart service: `sudo systemctl restart mosquitto`.


2. **Setup Arduino IDE**
- Tambahkan board ESP32 pada Arduino IDE (Board Manager), pasang library berikut:
- `PubSubClient`
- `DHT sensor library`
- `Adafruit SSD1306` & `Adafruit GFX`
- Buka `main.ino`, isi SSID/WiFi dan MQTT broker IP.
- Upload ke ESP32.


3. **Dashboard**
- Buka `dash.html` di browser (file local boleh, tapi beberapa browser memblokir websockets pada file:// — sebaiknya jalankan lewat small webserver seperti `python -m http.server 8000` dan buka `http://localhost:8000/dash.html`).


---


## Cara Pengoperasian & Pengujian


1. Nyalakan Mosquitto broker.
2. Upload firmware `main.ino` ke ESP32.
3. Jalankan dashboard di browser.
4. ESP32 akan connect ke WiFi kemudian broker MQTT, mulai mem-publish sensor data.
5. Dashboard akan menampilkan nilai sensor. Gunakan tombol pada dashboard untuk mengirim `ON`/`OFF` ke topic `home/control/relay` dan `home/control/alarm`.
6. Periksa relay / buzzer merespon pesan kontrol.


- Contoh JSON pada topic `home/sensors/dht`:

  



