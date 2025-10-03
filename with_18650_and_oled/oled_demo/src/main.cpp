#include <Arduino.h>
#include "SSD1306.h"

#define SDA 5
#define SCL 4
#define BATTERY_PIN 35

SSD1306 display(0x3c, SDA, SCL);

float readBatteryVoltage() {
  int rawValue = analogRead(BATTERY_PIN);
  return (rawValue / 4095.0) * 3.3 * 2.0;
}

void setup() {
  Serial.begin(115200);
  Serial.println("\n=== ESP32 OLED デモ起動 ===");
  
  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_10);
  
  display.clear();
  display.setFont(ArialMT_Plain_16);
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.drawString(64, 10, "ESP32");
  display.setFont(ArialMT_Plain_10);
  display.drawString(64, 35, "Starting...");
  display.display();
  delay(2000);
  
  Serial.println("デモ開始");
}

void loop() {
  static unsigned long lastUpdate = 0;
  static int counter = 0;
  
  if (millis() - lastUpdate >= 500) {
    lastUpdate = millis();
    
    float batteryVoltage = readBatteryVoltage();
    unsigned long seconds = millis() / 1000;
    unsigned long minutes = seconds / 60;
    unsigned long hours = minutes / 60;
    
    display.clear();
    
    display.setColor(WHITE);
    display.fillRect(0, 0, 128, 14);
    display.setColor(BLACK);
    display.setTextAlignment(TEXT_ALIGN_CENTER);
    display.drawString(64, 2, "ESP32 DEMO");
    
    display.setColor(WHITE);
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.drawString(0, 18, "Count: " + String(counter));
    display.drawString(0, 30, "Battery: " + String(batteryVoltage, 2) + " V");
    
    String uptimeStr = "Uptime: ";
    if (hours > 0) {
      uptimeStr += String(hours) + "h " + String(minutes % 60) + "m";
    } else if (minutes > 0) {
      uptimeStr += String(minutes) + "m " + String(seconds % 60) + "s";
    } else {
      uptimeStr += String(seconds) + "s";
    }
    display.drawString(0, 42, uptimeStr);
    
    int progress = (counter % 100);
    display.drawProgressBar(0, 54, 120, 8, progress);
    
    display.display();
    
    counter++;
    
    if (counter % 2 == 0) {
      Serial.printf("Count: %d | Battery: %.2fV | Uptime: %lus\n", 
                    counter, batteryVoltage, seconds);
    }
  }
}