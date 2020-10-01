#include <ESP8266WiFi.h>

const char *ssid = "TintinA";                  // TODO
const char *password = "Er1EKjEnFXqmlFjtwyMv"; // TODO

void setup()
{
  Serial.begin(115200);
  Serial.print("\nConnecting to network ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA); // or WIFI_AP, WIFI_AP_STA, WIFI_OFF
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(100); // keeps watchdog happy
  }
  Serial.print("Connected, IP = ");
  Serial.println(WiFi.localIP());
}

void loop() {}
