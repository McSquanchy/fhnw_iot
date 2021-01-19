#include <ESP8266WiFi.h> // v2.4.2
#include <ESP8266MQTTClient.h> // v1.0.4

const char *ssid = "TintinA"; // TODO
const char *password = "Er1EKjEnFXqmlFjtwyMv"; // TODO

MQTTClient client;

void handleConnected() {
  Serial.println("Connected to broker");
  client.publish("hello", "Hello, World!");
}

void setup() {
  Serial.begin(115200);
  Serial.print("\nConnecting to network ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(100); // keeps watchdog happy
  }
  Serial.print("Connected to network, local IP = "); 
  Serial.println(WiFi.localIP());

  client.onConnect(handleConnected);
  client.begin("mqtt://10.0.0.180/");
}

void loop() {
  client.handle();
}
