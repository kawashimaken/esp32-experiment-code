#include <Arduino.h>
#include <WiFi.h>
#include "SSD1306.h"

#define SDA 5
#define SCL 4

SSD1306 display(0x3c, SDA, SCL);

int scrollOffset = 0;
int scrollDirection = 1;
int totalNetworks = 0;
unsigned long lastScan = 0;
unsigned long lastScroll = 0;
const unsigned long SCAN_INTERVAL = 10000;
const unsigned long SCROLL_INTERVAL = 50;
const int ITEM_HEIGHT = 20;

void drawSignalBars(int x, int y, int rssi) {
  int bars = 0;
  if (rssi > -55) bars = 5;
  else if (rssi > -65) bars = 4;
  else if (rssi > -75) bars = 3;
  else if (rssi > -85) bars = 2;
  else bars = 1;
  
  for (int i = 0; i < 5; i++) {
    int h = (i + 1) * 2;
    if (i < bars) {
      display.fillRect(x + i * 3, y + 10 - h, 2, h);
    } else {
      display.drawRect(x + i * 3, y + 10 - h, 2, h);
    }
  }
}

String getEncryptionType(wifi_auth_mode_t encryption) {
  switch (encryption) {
    case WIFI_AUTH_OPEN: return "Open";
    case WIFI_AUTH_WEP: return "WEP";
    case WIFI_AUTH_WPA_PSK: return "WPA";
    case WIFI_AUTH_WPA2_PSK: return "WPA2";
    case WIFI_AUTH_WPA_WPA2_PSK: return "WPA/2";
    case WIFI_AUTH_WPA2_ENTERPRISE: return "WPA2-E";
    case WIFI_AUTH_WPA3_PSK: return "WPA3";
    default: return "Unknown";
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println("\n=== WiFiスキャナー起動 ===");
  
  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_10);
  
  display.clear();
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.drawString(64, 20, "WiFi Scanner");
  display.drawString(64, 35, "Scanning...");
  display.display();
  
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  
  Serial.println("初回スキャン開始");
  totalNetworks = WiFi.scanNetworks();
  Serial.printf("%d ネットワーク検出\n", totalNetworks);
}

void loop() {
  if (millis() - lastScan > SCAN_INTERVAL) {
    lastScan = millis();
    Serial.println("再スキャン中...");
    totalNetworks = WiFi.scanNetworks();
    Serial.printf("%d ネットワーク検出\n", totalNetworks);
    scrollOffset = 0;
    scrollDirection = 1;
  }
  
  if (totalNetworks == 0) {
    display.clear();
    display.setTextAlignment(TEXT_ALIGN_CENTER);
    display.drawString(64, 25, "No Networks");
    display.drawString(64, 40, "Found");
    display.display();
    delay(1000);
    return;
  }
  
  if (millis() - lastScroll > SCROLL_INTERVAL) {
    lastScroll = millis();
    
    scrollOffset += scrollDirection;
    
    int maxScroll = (totalNetworks * ITEM_HEIGHT) - 52;
    if (maxScroll < 0) maxScroll = 0;
    
    if (scrollOffset >= maxScroll && scrollDirection > 0) {
      scrollDirection = -1;
      delay(1000);
    } else if (scrollOffset <= 0 && scrollDirection < 0) {
      scrollDirection = 1;
      delay(1000);
    }
  }
  
  display.clear();
  
  display.fillRect(0, 0, 128, 12);
  display.setColor(BLACK);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.drawString(2, 1, "WiFi: " + String(totalNetworks));
  display.setTextAlignment(TEXT_ALIGN_RIGHT);
  display.drawString(126, 1, String(scrollOffset) + "px");
  
  display.setColor(WHITE);
  
  for (int i = 0; i < totalNetworks; i++) {
    int yPos = 14 + (i * ITEM_HEIGHT) - scrollOffset;
    
    if (yPos < 10 || yPos > 64) continue;
    
    String ssid = WiFi.SSID(i);
    if (ssid.length() > 14) {
      ssid = ssid.substring(0, 14) + "..";
    }
    
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.setFont(ArialMT_Plain_10);
    display.drawString(0, yPos, ssid);
    
    int rssi = WiFi.RSSI(i);
    display.setTextAlignment(TEXT_ALIGN_RIGHT);
    display.drawString(128, yPos, String(rssi));
    
    drawSignalBars(95, yPos, rssi);
    
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.setFont(ArialMT_Plain_10);
    String info = "Ch" + String(WiFi.channel(i)) + " " + getEncryptionType(WiFi.encryptionType(i));
    display.drawString(0, yPos + 10, info);
  }
  
  display.display();
}