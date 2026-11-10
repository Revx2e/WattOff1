/*
  WattOff 교실용 ESP32 펌웨어 (앱과 바로 연동되는 토픽 규칙)
  - 토픽 접두어(PREFIX)는 앱 설정의 "토픽 접두어"와 똑같이 맞추세요.
  - 앱 → ESP32 : {PREFIX}/school/2-3/light   "ON"/"OFF"
                 {PREFIX}/school/2-3/aircon  "OFF"
  - ESP32 → 앱 : {PREFIX}/school/2-3/occupancy      "1"(사람 있음) / "0"
                 {PREFIX}/school/2-3/light/state    "ON"/"OFF"
                 {PREFIX}/school/2-3/aircon/state   "OFF_SENT"
  필요 라이브러리: PubSubClient, IRremoteESP8266, BH1750
  ⚠️ 릴레이 실습은 5V LED 등 저전압으로! 220V 조명은 KC 인증 스마트 스위치를 쓰세요.
*/
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <Wire.h>
#include <BH1750.h>

const char* WIFI_SSID = "와이파이이름";
const char* WIFI_PASS = "와이파이비번";
const char* MQTT_HOST = "broker.hivemq.com";  // 실제 운영은 HiveMQ Cloud(아이디/비번+TLS) 권장
const int   MQTT_PORT = 8883;
const char* MQTT_USER = "";
const char* MQTT_PW   = "";
String PREFIX = "wattoff/myschool";
String ROOM   = "school/2-3";

#define PIR_PIN   27
#define RELAY_PIN 26
#define IR_PIN    4

WiFiClientSecure net; PubSubClient client(net);
IRsend irsend(IR_PIN); BH1750 lux;

// IRrecvDumpV2 예제로 우리 교실 에어컨 리모컨의 "끄기" 신호를 학습해서 붙여넣기
uint16_t acOffSignal[] = { 9000, 4500, 560, 560 /* ... 학습한 값 ... */ };
const uint16_t AC_LEN = sizeof(acOffSignal) / sizeof(acOffSignal[0]);

bool occupied = false; unsigned long lastMotion = 0, lastPub = 0;
const unsigned long EMPTY_DELAY = 5UL * 60 * 1000; // 5분간 움직임 없으면 '비어 있음' (자습 오탐 방지)

String T(String s) { return PREFIX + "/" + ROOM + "/" + s; }

void callback(char* topic, byte* payload, unsigned int len) {
  String t = String(topic), msg = "";
  for (unsigned i = 0; i < len; i++) msg += (char)payload[i];
  if (t == T("light")) {
    digitalWrite(RELAY_PIN, msg == "ON" ? HIGH : LOW);
    delay(1500);
    // 조도센서로 실제로 꺼졌는지 재확인 후 보고
    float l = lux.readLightLevel();
    client.publish(T("light/state").c_str(), (msg == "OFF" && l < 50) || msg == "ON" ? msg.c_str() : "ON", true);
  }
  if (t == T("aircon") && msg == "OFF") {
    irsend.sendRaw(acOffSignal, AC_LEN, 38);
    client.publish(T("aircon/state").c_str(), "OFF_SENT", true);
  }
}

void reconnect() {
  while (!client.connected()) {
    String id = "esp32-" + String((uint32_t)ESP.getEfuseMac(), HEX);
    if (client.connect(id.c_str(), MQTT_USER, MQTT_PW)) {
      client.subscribe(T("light").c_str());
      client.subscribe(T("aircon").c_str());
    } else delay(3000);
  }
}

void setup() {
  pinMode(PIR_PIN, INPUT); pinMode(RELAY_PIN, OUTPUT); digitalWrite(RELAY_PIN, HIGH);
  irsend.begin(); Wire.begin(); lux.begin();
  WiFi.begin(WIFI_SSID, WIFI_PASS); while (WiFi.status() != WL_CONNECTED) delay(500);
  net.setInsecure(); // 학습용. 운영 시 CA 인증서 등록
  client.setServer(MQTT_HOST, MQTT_PORT); client.setCallback(callback);
}

void loop() {
  if (!client.connected()) reconnect();
  client.loop();
  if (digitalRead(PIR_PIN) == HIGH) lastMotion = millis();
  bool now = (millis() - lastMotion) < EMPTY_DELAY;
  if (now != occupied || millis() - lastPub > 60000) {
    occupied = now; lastPub = millis();
    client.publish(T("occupancy").c_str(), occupied ? "1" : "0", true);
    client.publish(T("light/state").c_str(), lux.readLightLevel() > 100 ? "ON" : "OFF", true);
  }
}
