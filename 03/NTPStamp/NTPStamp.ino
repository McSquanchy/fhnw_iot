#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

#define NTP_OFFSET   60 * 60      // In seconds
#define NTP_INTERVAL 60 * 1000    // In miliseconds
#define NTP_ADDRESS  "europe.pool.ntp.org"

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, NTP_ADDRESS, NTP_OFFSET, NTP_INTERVAL);

void setup(){
  timeClient.begin();
  Serial.begin(115200);
}

void loop() {
  timeClient.update();
  String formattedTime = timeClient.getFormattedTime();
  unsigned long epochTime =  timeClient.getEpochTime();
  Serial.println(formattedTime);
  Serial.println(epochTime);
  delay(2000);
}
