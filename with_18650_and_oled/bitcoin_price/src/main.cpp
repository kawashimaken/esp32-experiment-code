#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include "SSD1306.h"
#include "secrets.h"

#define SDA 5
#define SCL 4

SSD1306 display(0x3c, SDA, SCL);

float btcPrice = 0;
unsigned long lastUpdate = 0;
const unsigned long UPDATE_INTERVAL = 10000;

void connectWiFi() {
  display.clear();
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.setFont(ArialMT_Plain_10);
  display.drawString(64, 20, "Connecting WiFi");
  display.drawString(64, 35, WIFI_SSID);
  display.display();
  
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  WiFi.setHostname("ESP32-BTC");
  WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE, IPAddress(8, 8, 8, 8));
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 30) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi接続成功");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
    
    display.clear();
    display.drawString(64, 20, "WiFi Connected!");
    display.setFont(ArialMT_Plain_10);
    display.drawString(64, 35, WiFi.localIP().toString());
    display.display();
    delay(2000);
  } else {
    Serial.println("\nWiFi接続失敗");
    display.clear();
    display.drawString(64, 25, "WiFi Failed");
    display.display();
  }
}

void fetchBitcoinPrice() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi未接続");
    return;
  }
  
  WiFiClientSecure client;
  client.setInsecure();
  
  HTTPClient http;
  http.setTimeout(15000);
  
  Serial.println("Bitcoin価格を取得中...");
  
  String url = "https://api.coingecko.com/api/v3/simple/price?ids=bitcoin&vs_currencies=usd";
  http.begin(client, url);
  http.addHeader("User-Agent", "ESP32-Bitcoin-Monitor");
  
  int httpCode = http.GET();
  Serial.printf("HTTPコード: %d\n", httpCode);
  
  if (httpCode == 200) {
    String payload = http.getString();
    Serial.println("Response: " + payload);
    
    StaticJsonDocument<512> doc;
    DeserializationError error = deserializeJson(doc, payload);
    
    if (!error) {
      btcPrice = doc["bitcoin"]["usd"];
      Serial.printf("Bitcoin価格: $%.2f\n", btcPrice);
    } else {
      Serial.println("JSON解析エラー: " + String(error.c_str()));
    }
  } else if (httpCode > 0) {
    Serial.printf("HTTP エラー: %d\n", httpCode);
    String response = http.getString();
    Serial.println("Response: " + response);
  } else {
    Serial.printf("接続エラー: %d\n", httpCode);
  }
  
  http.end();
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("\n=== Bitcoin価格モニター起動 ===");
  
  display.init();
  display.flipScreenVertically();
  
  connectWiFi();
  
  if (WiFi.status() == WL_CONNECTED) {
    display.clear();
    display.setTextAlignment(TEXT_ALIGN_CENTER);
    display.drawString(64, 25, "Fetching Price...");
    display.display();
    
    delay(2000);
    fetchBitcoinPrice();
  }
}

void loop() {
  if (millis() - lastUpdate > UPDATE_INTERVAL) {
    lastUpdate = millis();
    fetchBitcoinPrice();
  }
  
  display.clear();
  
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.setFont(ArialMT_Plain_10);
  display.drawString(64, 0, "BITCOIN");
  
  if (btcPrice > 0) {
    String priceStr = "$" + String((int)btcPrice);
    
    display.setFont(ArialMT_Plain_24);
    display.drawString(64, 15, priceStr);
    
    display.setFont(ArialMT_Plain_10);
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.drawString(0, 50, "USD");
    
    int seconds = (millis() - lastUpdate) / 1000;
    
    display.setTextAlignment(TEXT_ALIGN_RIGHT);
    display.drawString(128, 50, String(seconds) + "s ago");
  } else {
    display.setFont(ArialMT_Plain_10);
    display.setTextAlignment(TEXT_ALIGN_CENTER);
    display.drawString(64, 28, "Loading...");
  }
  
  display.display();
  
  delay(1000);
}