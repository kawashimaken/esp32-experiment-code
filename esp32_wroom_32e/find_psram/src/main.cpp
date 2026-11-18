#include <Arduino.h>

void setup() {
  Serial.begin(115200);
  delay(2000);
  
  Serial.println("\n========== ESP32 Memory Details ==========");
  
  // チップ情報
  Serial.printf("Chip Model: %s\n", ESP.getChipModel());
  Serial.printf("Chip Revision: %d\n", ESP.getChipRevision());
  Serial.printf("CPU Frequency: %d MHz\n", ESP.getCpuFreqMHz());
  
  Serial.println("\n--- Internal RAM ---");
  Serial.printf("Total Heap Size: %d bytes (%.2f KB)\n", 
                ESP.getHeapSize(), ESP.getHeapSize()/1024.0);
  Serial.printf("Free Heap: %d bytes (%.2f KB)\n", 
                ESP.getFreeHeap(), ESP.getFreeHeap()/1024.0);
  Serial.printf("Used Heap: %d bytes (%.2f KB)\n", 
                ESP.getHeapSize() - ESP.getFreeHeap(),
                (ESP.getHeapSize() - ESP.getFreeHeap())/1024.0);
  Serial.printf("Heap Usage: %.1f%%\n", 
                100.0 * (ESP.getHeapSize() - ESP.getFreeHeap()) / ESP.getHeapSize());
  
  Serial.println("\n--- PSRAM (External RAM) ---");
  if(ESP.getPsramSize() > 0) {
    Serial.printf("Total PSRAM: %d bytes (%.2f MB)\n", 
                  ESP.getPsramSize(), ESP.getPsramSize()/1024.0/1024.0);
    Serial.printf("Free PSRAM: %d bytes (%.2f MB)\n", 
                  ESP.getFreePsram(), ESP.getFreePsram()/1024.0/1024.0);
    Serial.printf("Used PSRAM: %d bytes (%.2f MB)\n", 
                  ESP.getPsramSize() - ESP.getFreePsram(),
                  (ESP.getPsramSize() - ESP.getFreePsram())/1024.0/1024.0);
    Serial.printf("PSRAM Usage: %.1f%%\n", 
                  100.0 * (ESP.getPsramSize() - ESP.getFreePsram()) / ESP.getPsramSize());
  } else {
    Serial.println("No PSRAM detected");
  }
  
  Serial.println("\n--- Flash Memory ---");
  Serial.printf("Flash Size: %d bytes (%.2f MB)\n", 
                ESP.getFlashChipSize(), ESP.getFlashChipSize()/1024.0/1024.0);
  Serial.printf("Flash Speed: %d MHz\n", ESP.getFlashChipSpeed()/1000000);
  
  Serial.println("\n==========================================\n");
}

void loop() {
  delay(5000);
  
  // 動的なメモリ使用状況を表示
  Serial.printf("[Runtime] Free Heap: %d KB, Free PSRAM: %d KB\n", 
                ESP.getFreeHeap()/1024, ESP.getFreePsram()/1024);
}