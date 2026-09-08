#include <ESP8266WiFi.h>
#include <SPI.h>
#include <SD.h>

const char* ssid = "Tx1";
const char* password = "Trasmitter_123";

#define SD_CS D8  // Chip Select pin for SD card (GPIO15)

File logFile;

void setup() {
  Serial.begin(115200);
  delay(2000);

  // Initialize SD Card
  Serial.print("Initializing SD card...");
  if (!SD.begin(SD_CS)) {
    Serial.println("Card failed or not present!");
    return;
  }
  Serial.println("SD Card Initialized.");

  // Connect to transmitter AP
  WiFi.begin(ssid, password);
  Serial.print("Connecting to AP");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nConnected to AP!");
  Serial.print("Device IP: ");
  Serial.println(WiFi.localIP());

  // Log session header
  logFile = SD.open("/rssi_log.txt", FILE_WRITE);
  if (logFile) {
    logFile.println("=== New Session ===");
    logFile.println("SSID,RSSI (dBm)");
    logFile.close();
  }
}

void loop() {
  long rssi = WiFi.RSSI();
  String connectedSSID = WiFi.SSID();

  String data = connectedSSID + "," + String(rssi);
  Serial.println("Logging: " + data);

  logFile = SD.open("/rssi_log.txt", FILE_WRITE);
  if (logFile) {
    logFile.println(data);
    logFile.close();
  } else {
    Serial.println("Error opening file!");
  }

  delay(5000);  // Log every 5 seconds
}
