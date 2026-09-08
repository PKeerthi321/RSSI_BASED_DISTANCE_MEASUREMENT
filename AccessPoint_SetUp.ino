#include <ESP8266WiFi.h>

const char* ssid = "Tx1";
const char* password = "Transmitter_123";

void setup() {
  delay(500);                  // Give USB some time
  Serial.begin(115200);
  delay(2000);                 // Let Serial settle before printing

  Serial.println("Booting...");

  WiFi.softAP(ssid, password);
  Serial.println("Access Point Started");
  Serial.print("SSID: ");
  Serial.println(ssid);
  Serial.print("IP Address: ");
  Serial.println(WiFi.softAPIP());
}

void loop() {
  //Nothing here for now
}
