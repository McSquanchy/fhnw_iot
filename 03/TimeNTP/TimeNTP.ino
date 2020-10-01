#include <sys/time.h>
#include <WiFi.h>

void setup() {
  Serial.begin(115200);
  int epoch_time = 1527369964;
  timeval epoch = {epoch_time, 0};
  const timeval *tv = &epoch;
  timezone utc = {0,0};
  const timezone *tz = &utc;
  settimeofday(tv, tz);
}

void loop() {
  struct tm now;
  getLocalTime(&now,0);
  Serial.println(&now," %B %d %Y %H:%M:%S (%A)");
  delay(1000);
}
