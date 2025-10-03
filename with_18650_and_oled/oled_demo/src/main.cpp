#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// OLEDディスプレイの設定
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C

// I2Cピンの設定
#define SCL 4
#define SDA 5

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// バッテリー電圧読み取り用
#define BATTERY_PIN 35

float readBatteryVoltage() {
  int rawValue = analogRead(BATTERY_PIN);
  float voltage = (rawValue / 4095.0) * 3.3 * 2.0;
  return voltage;
}

void setup() {
  Serial.begin(115200);
  delay(500);
  
  Serial.println("\n=== ESP32 OLED デモ起動 ===");
  
  // I2C初期化
  Wire.begin(SDA, SCL);
  Wire.setClock(400000); // I2Cクロックを400kHzに設定（高速化）
  
  Serial.println("I2C初期化完了");
  
  // OLEDディスプレイ初期化
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println("エラー: SSD1306の初期化に失敗しました");
    Serial.println("配線を確認してください");
    while(1) {
      delay(1000);
    }
  }
  
  Serial.println("OLED初期化成功！");
  
  // 初期表示テスト
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(15, 10);
  display.println("ESP32");
  display.setTextSize(1);
  display.setCursor(20, 35);
  display.println("Starting...");
  display.display();
  delay(2000);
  
  Serial.println("デモ開始");
}

void loop() {
  static unsigned long lastUpdate = 0;
  static int counter = 0;
  
  // 500msごとに更新（スムーズなアニメーション）
  if (millis() - lastUpdate >= 500) {
    lastUpdate = millis();
    
    // バッテリー電圧読み取り
    float batteryVoltage = readBatteryVoltage();
    
    // ディスプレイクリア
    display.clearDisplay();
    
    // ヘッダー部分（反転表示）
    display.fillRect(0, 0, 128, 16, SSD1306_WHITE);
    display.setTextSize(1);
    display.setTextColor(SSD1306_BLACK);
    display.setCursor(25, 4);
    display.println("ESP32 DEMO");
    
    // カウンター表示
    display.setTextColor(SSD1306_WHITE);
    display.setTextSize(1);
    display.setCursor(0, 20);
    display.print("Count: ");
    display.println(counter);
    
    // バッテリー電圧表示
    display.setCursor(0, 32);
    display.print("Battery: ");
    display.print(batteryVoltage, 2);
    display.println(" V");
    
    // 起動時間表示
    display.setCursor(0, 44);
    display.print("Uptime: ");
    unsigned long seconds = millis() / 1000;
    unsigned long minutes = seconds / 60;
    unsigned long hours = minutes / 60;
    if (hours > 0) {
      display.printf("%luh %lum", hours, minutes % 60);
    } else if (minutes > 0) {
      display.printf("%lum %lus", minutes, seconds % 60);
    } else {
      display.printf("%lus", seconds);
    }
    
    // プログレスバー（アニメーション）
    int barWidth = (counter % 128);
    display.drawRect(0, 56, 128, 8, SSD1306_WHITE);
    display.fillRect(1, 57, barWidth, 6, SSD1306_WHITE);
    
    // ディスプレイに表示を反映
    display.display();
    
    // カウンターをインクリメント
    counter++;
    
    // シリアルモニタにも出力
    if (counter % 2 == 0) {  // 1秒ごとに出力
      Serial.printf("Count: %d | Battery: %.2fV | Uptime: %lus\n", 
                    counter, batteryVoltage, seconds);
    }
  }
}