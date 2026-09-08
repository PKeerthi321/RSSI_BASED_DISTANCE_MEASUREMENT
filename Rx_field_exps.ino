#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <ESP8266WiFi.h>
#include <SPI.h>
#include <SD.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

const int chipSelect = D8;  // SD card CS pin (modify if needed)

// Tx SSIDs to scan
const char* tx1SSID = "Tx";
const char* tx2SSID = "Tx2";
const char* tx3SSID = "Tx3";

int rssiTx1 = -100;
int rssiTx2 = -100;
int rssiTx3 = -100;

const int maxSamplesPerSession = 20;

File dataFile;

void setup() {
  Serial.begin(115200);
  lcd.init();
  lcd.backlight();

  if (!SD.begin(chipSelect)) {
    Serial.println("SD Card initialization failed!");
    lcd.clear();
    lcd.print("SD init failed!");
    while (1);
  }
  Serial.println("SD Card initialized.");
  lcd.clear();
  lcd.print("SD init success");

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(1000);

  dataFile = SD.open("RSSI_LOG.CSV", FILE_WRITE);
  if (dataFile) {
    if (dataFile.size() == 0) {
      dataFile.println("Timestamp,RSSI_Tx1,RSSI_Tx2,RSSI_Tx3,Distance");
      dataFile.flush();
    }
    dataFile.close();
  }
}

void loop() {
  Serial.println("Enter distance (m) for this session and press Enter:");
  float inputDistance = waitForDistanceInput();

  int sampleCount = 0;
  while (sampleCount < maxSamplesPerSession) {
    int n = WiFi.scanNetworks();
    rssiTx1 = -100;
    rssiTx2 = -100;
    rssiTx3 = -100;

    for (int i = 0; i < n; i++) {
      String ssid = WiFi.SSID(i);
      int rssi = WiFi.RSSI(i);
      if (ssid == tx1SSID) rssiTx1 = rssi;
      if (ssid == tx2SSID) rssiTx2 = rssi;
      if (ssid == tx3SSID) rssiTx3 = rssi;
    }

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.printf("Tx1:%d Tx2:%d", rssiTx1, rssiTx2);
    lcd.setCursor(0, 1);
    lcd.printf("Tx3:%d Dist:%.1f", rssiTx3, inputDistance);

    logData(rssiTx1, rssiTx2, rssiTx3, inputDistance);
    sampleCount++;

    Serial.printf("Sample %d/%d logged.\n", sampleCount, maxSamplesPerSession);
    delay(2000);
  }

  Serial.println("Session complete. Enter next distance.");
}

float waitForDistanceInput() {
  String inputString = "";
  while (true) {
    if (Serial.available()) {
      char c = Serial.read();
      if (c == '\n' || c == '\r') {
        if (inputString.length() > 0) break;
      } else {
        inputString += c;
      }
    }
  }
  float val = inputString.toFloat();
  Serial.print("Distance set to: ");
  Serial.println(val);
  return val;
}

void logData(int rssi1, int rssi2, int rssi3, float dist) {
  dataFile = SD.open("RSSI_LOG.CSV", FILE_WRITE);
  if (dataFile) {
    String record = "";
    record += String(millis());
    record += ",";
    record += String(rssi1);
    record += ",";
    record += String(rssi2);
    record += ",";
    record += String(rssi3);
    record += ",";
    record += String(dist, 2);
    dataFile.println(record);
    dataFile.flush();
    dataFile.close();

    Serial.println("Logged: " + record);
  } else {
    Serial.println("Error opening RSSI_LOG.CSV");
  }
}
