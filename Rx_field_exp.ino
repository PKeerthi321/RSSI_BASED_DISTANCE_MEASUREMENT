#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <ESP8266WiFi.h>
#include <SPI.h>
#include <SD.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

const int chipSelect = D8;  // SD card CS pin (change as needed)
const char* tx1SSID = "Tx";
const char* tx2SSID = "Tx2";

int rssiTx1 = -100;
int rssiTx2 = -100;

const int maxSamplesPerSession = 20;

File dataFile;

void setup() {
  Serial.begin(115200);
  lcd.init();
  lcd.backlight();

  // Initialize SD card
  if (!SD.begin(chipSelect)) {
    Serial.println("SD Card initialization failed!");
    lcd.clear();
    lcd.print("SD init failed!");
    while (1); // Halt if SD init fails
  }
  Serial.println("SD Card initialized.");
  lcd.clear();
  lcd.print("SD init success");

  // WiFi init for scanning
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(1000);

  // Open/create CSV file and write header if empty
  dataFile = SD.open("RSSI_LOG.CSV", FILE_WRITE);
  if (dataFile) {
    if (dataFile.size() == 0) {
      dataFile.println("Timestamp,RSSI_Tx1,RSSI_Tx2,Distance");
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

    for (int i = 0; i < n; i++) {
      String ssid = WiFi.SSID(i);
      int rssi = WiFi.RSSI(i);
      if (ssid == tx1SSID) rssiTx1 = rssi;
      if (ssid == tx2SSID) rssiTx2 = rssi;
    }

    // Display RSSI on LCD
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.printf("Tx1 RSSI: %d", rssiTx1);
    lcd.setCursor(0, 1);
    lcd.printf("Tx2 RSSI: %d", rssiTx2);

    // Log data
    logData(rssiTx1, rssiTx2, inputDistance);
    sampleCount++;

    Serial.printf("Sample %d/%d logged.\n", sampleCount, maxSamplesPerSession);

    delay(2000); // Delay between samples
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

void logData(int rssi1, int rssi2, float dist) {
  dataFile = SD.open("RSSI_LOG.CSV", FILE_WRITE);
  if (dataFile) {
    String record = "";
    record += String(millis());     // Timestamp ms since boot
    record += ",";
    record += String(rssi1);
    record += ",";
    record += String(rssi2);
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
