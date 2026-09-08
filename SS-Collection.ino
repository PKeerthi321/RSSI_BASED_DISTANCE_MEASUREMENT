#include <ESP8266WiFi.h>

void setup() {
  Serial.begin(115200);         // Start serial communication
  delay(2000);                  // Wait for Serial Monitor to open

  WiFi.mode(WIFI_STA);          // Set Wi-Fi to Station (client) mode
  WiFi.disconnect();            // Disconnect from any previously connected network
  delay(100);                   // Short delay before scanning
}

void loop() {
  Serial.println("\n🔍 Scanning for Wi-Fi networks...");

  int numNetworks = WiFi.scanNetworks();  // Scan for nearby networks

  if (numNetworks == 0) {
    Serial.println("❌ No networks found.");
  } else {
    Serial.print("✅ ");
    Serial.print(numNetworks);
    Serial.println(" networks found:\n");

    for (int i = 0; i < numNetworks; i++) {
      // Print SSID and signal strength
      Serial.print("📶 SSID: ");
      Serial.print(WiFi.SSID(i));

      Serial.print(" | 🔋 RSSI: ");
      Serial.print(WiFi.RSSI(i));  // Signal strength in dBm

      Serial.print(" dBm | 🔒 Encryption: ");
      String encryptionType;
      switch (WiFi.encryptionType(i)) {
        case ENC_TYPE_WEP: encryptionType = "WEP"; break;
        case ENC_TYPE_TKIP: encryptionType = "WPA"; break;
        case ENC_TYPE_CCMP: encryptionType = "WPA2"; break;
        case ENC_TYPE_NONE: encryptionType = "Open"; break;
        case ENC_TYPE_AUTO: encryptionType = "Auto"; break;
        default: encryptionType = "Unknown"; break;
      }
      Serial.println(encryptionType);
    }
  }

  delay(5000);  // Wait 5 seconds before rescanning
}
