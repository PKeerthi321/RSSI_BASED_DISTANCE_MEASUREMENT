#include <ESP8266WiFi.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SD.h>

// LCD address 0x27, 16 columns x 2 rows
LiquidCrystal_I2C lcd(0x27, 16, 2);

// SD card CS pin
const int chipSelect = D8;

// WiFi target SSID
const char* targetSSID = "Tx1";

// RSSI-to-Distance model parameters
const float rssiAtOneMeter = -64.0;  // Reference RSSI at 1 meter
const float pathLossExponent = 2.0;  // Typical indoor value (2.0 ~ 4.0)

int rssiBuffer[5];
int bufferIndex = 0;

void setup() {
  Serial.begin(115200);
  delay(1000);

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  // Initialize LCD
  lcd.begin(16, 2);
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("RSSI Logger");

  // Initialize SD card
  if (!SD.begin(chipSelect)) {
    Serial.println("❌ SD Card Init Failed!");
    lcd.setCursor(0, 1);
    lcd.print("SD Init Failed");
  } else {
    Serial.println("✅ SD Card Ready");
    lcd.setCursor(0, 1);
    lcd.print("SD Ready");
  }

  delay(2000);
  lcd.clear();
}

void loop() {
  WiFi.scanNetworks();  // Start scan
  int n = WiFi.scanComplete();
  if (n == -2) return;  // Scan not yet complete
  if (n == 0) {
    Serial.println("No networks found");
    return;
  }

  bool found = false;
  int rssi = 0;

  for (int i = 0; i < n; i++) {
    String ssid = WiFi.SSID(i);
    if (ssid == targetSSID) {
      rssi = WiFi.RSSI(i);
      found = true;
      break;
    }
  }

  WiFi.scanDelete();  // Free memory

  if (found) {
    // Update circular buffer
    rssiBuffer[bufferIndex] = rssi;
    bufferIndex = (bufferIndex + 1) % 5;

    // Compute average RSSI
    int avgRSSI = 0;
    for (int i = 0; i < 5; i++) {
      avgRSSI += rssiBuffer[i];
    }
    avgRSSI /= 5;

    // Estimate distance using log-distance path loss model
    float distance = pow(10, (rssiAtOneMeter - avgRSSI) / (10 * pathLossExponent));

    // Serial output
    Serial.print("SSID: "); Serial.print(targetSSID);
    Serial.print(" | RSSI: "); Serial.print(avgRSSI);
    Serial.print(" dBm | Distance: ");
    Serial.print(distance, 2); Serial.println(" m");

    // Plot graph-friendly format
    Serial.print("RSSI: ");
    Serial.print(avgRSSI);
    Serial.print("  Distance: ");
    Serial.println(distance);

    // LCD output
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("RSSI:");
    lcd.print(avgRSSI);
    lcd.print("dBm");

    lcd.setCursor(0, 1);
    lcd.print("Dist:");
    lcd.print(distance, 1);
    lcd.print(" m");

    // SD card log
    File logFile = SD.open("/rssi_log.txt", FILE_WRITE);
    if (logFile) {
      logFile.print("RSSI: ");
      logFile.print(avgRSSI);
      logFile.print(" dBm, Distance: ");
      logFile.print(distance, 2);
      logFile.println(" m");
      logFile.close();
    } else {
      Serial.println("❌ Failed to write to SD");
    }

  } else {
    Serial.println("❌ Target SSID not found");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("SSID not found");
  }

  delay(5000);  // Scan every 5 sec
}
