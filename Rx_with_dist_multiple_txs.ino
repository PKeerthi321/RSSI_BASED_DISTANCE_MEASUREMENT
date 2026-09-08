#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <ESP8266WiFi.h>

// Initialize 16x2 LCD, I2C address 0x27 (check yours)
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Tx SSIDs
const char* tx1SSID = "Tx";
const char* tx2SSID = "Tx2";

// RSSI values
int rssiTx1 = -100;
int rssiTx2 = -100;

// Function to estimate distance from RSSI (path loss model)
float rssiToDistance(int rssi) {
  int txPower = -50; // Adjust based on your Tx power/environment
  if (rssi == 0) return -1.0;
  float ratio = rssi * 1.0 / txPower;
  if (ratio < 1.0) {
    return pow(ratio, 10);
  } else {
    float dist = 0.89976 * pow(ratio, 7.7095) + 0.111;
    return dist;
  }
}

void setup() {
  Serial.begin(115200);
  lcd.init();
  lcd.backlight();

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
}

void loop() {
  int n = WiFi.scanNetworks();

  rssiTx1 = -100; // Reset RSSI
  rssiTx2 = -100;

  for (int i = 0; i < n; ++i) {
    String ssid = WiFi.SSID(i);
    int rssi = WiFi.RSSI(i);

    if (ssid == tx1SSID) {
      rssiTx1 = rssi;
    }
    if (ssid == tx2SSID) {
      rssiTx2 = rssi;
    }
  }

  float distTx1 = rssiToDistance(rssiTx1);
  float distTx2 = rssiToDistance(rssiTx2);

  // Determine best signal Tx
  String bestTx = (rssiTx1 > rssiTx2) ? "Tx1" : "Tx2";

  // Display with arrow mark on best signal
  lcd.clear();
  lcd.setCursor(0, 0);
  if (bestTx == "Tx1") lcd.print(">");
  else lcd.print(" ");
  lcd.print("Tx1: ");
  lcd.print(rssiTx1);
  lcd.print("d:");
  lcd.print(distTx1, 1);
  lcd.print("m");

  lcd.setCursor(0, 1);
  if (bestTx == "Tx2") lcd.print(">");
  else lcd.print(" ");
  lcd.print("Tx2: ");
  lcd.print(rssiTx2);
  lcd.print("d:");
  lcd.print(distTx2, 1);
  lcd.print("m");

  Serial.print("Best signal: ");
  Serial.println(bestTx);
    Serial.println("    ");
  Serial.println("rssiTx1 : ");
  Serial.println(rssiTx1);
    Serial.println("    ");
   Serial.println("rssiTx2 : ");
  Serial.println(rssiTx2);
    Serial.println("    ");
   Serial.println("distanceTx1 : ");
  Serial.println(distTx1);
    Serial.println("    ");
   Serial.println("distTx2 : ");
  Serial.println(distTx2);
  Serial.println("    ");
  delay(3000);
}
