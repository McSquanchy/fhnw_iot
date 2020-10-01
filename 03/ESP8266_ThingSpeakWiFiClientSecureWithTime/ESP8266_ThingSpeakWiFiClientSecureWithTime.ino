#include <ESP8266WiFi.h>
#include <time.h>
#include <sys/time.h>
#include "DHTesp.h"

const int dhtPin = 5; // Grove adapter I2C_1 or _2 used as D6
const DHTesp::DHT_MODEL_t dhtModel = DHTesp::DHT11;
BearSSL::WiFiClientSecure client; // use TLS

const char *ssid = "TintinA";                  // TODO
const char *password = "Er1EKjEnFXqmlFjtwyMv"; // TODO

const char *verb = "POST";
const char *path = "/update?api_key=DZS6IGPQE8VOL7WT"; // TODO
const char *host = "api.thingspeak.com";
const int port = 443;

long t0 = 0;
DHTesp dht;

time_t getTime() {
  // returns current Unix time, i.e. seconds since Epoch
  struct timeval *tod = (timeval*) malloc(sizeof(struct timeval));
  gettimeofday(tod, NULL);
  return tod->tv_sec;
}

void setTime(time_t t) {
  // expects Unix time, i.e. seconds since Epoch
  struct timeval now = { .tv_sec = t, .tv_usec = 0 };
  // struct timezone utc = { .tz_minuteswest = 0, .tz_dsttime = 0 };
  settimeofday(&now, NULL);
}

void printTime(BearSSL::WiFiClientSecure client, time_t t)
{
  // expects Unix time, i.e. seconds since Epoch,
  // prints ISO format, e.g. 2019-03-02T13:37:00Z
  struct tm *tm = gmtime(&t);
  char s[21];
  size_t r = strftime(s, 21, "%Y-%m-%d %H:%M:%S", tm);
  s[r] = '\0'; // make sure string is terminated
  client.print(s);
}

void printTime(time_t t) {
  // expects Unix time, i.e. seconds since Epoch,
  // prints ISO format, e.g. 2019-03-02T13:37:00Z
  struct tm *tm = gmtime(&t);
  char s[21];
  size_t r = strftime(s, 21, "%Y-%m-%dT%H:%M:%SZ", tm);
  s[r] = '\0'; // make sure string is terminated
  Serial.print(s);
}

void get_initial_time() {
   if (client.connect("google.com", 443)) {
    // write HTTP request:
    client.print("HEAD / HTTP/1.1\r\n" \
      "Host: google.com\r\n" \
      "Connection: close\r\n\r\n");
    // read HTTP response:
    client.setTimeout(10000); // ms
    while (client.connected() || client.available()) {
      String  header = client.readStringUntil('\n');
      if (header.indexOf("Date: ") == 0) {
        time_t t = parseTimeFromDateHeader(header);
        setTime(t); // sync system time to server
      }
    }
  }
}

void setup_wifi() {
  Serial.print("\nConnecting to network ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(100); // keeps watchdog happy
  }
  Serial.print("Connected to network, local IP = ");
  Serial.println(WiFi.localIP());
}

void wait_for_wifi(long t) {
  if(WiFi.status() != WL_CONNECTED) {
      Serial.print("Lost connection to ");
      Serial.print(ssid);
      Serial.println(". Retrying...");
  }
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.println("Retrying...");
    delay(500); // keeps watchdog happy
  }
  if(WiFi.status() == WL_CONNECTED) {
     Serial.print("Connection to ");
     Serial.print(ssid);
     Serial.println(" re-established. Resuming...");
  }
}

bool connect_client() {
  return client.connect(host,port);
}

time_t parseTimeFromDateHeader(String header) {
  // expects HTTP date header in UTC, e.g.
  // "Date: Sat, 02 Mar 2019 13:37:00",
  // returns Unix time, i.e. seconds since Epoch
  time_t t = 0;
  if (header.indexOf("Date: ") == 0) {
    // e.g. "Date: Sat, 02 Mar 2019 13:37:00\r"
    String date = header.substring(6, 6 + 25);
    struct tm tm;
    memset(&tm, 0, sizeof(struct tm)); // initialise all fields to 0
    char *r = strptime(date.c_str(), "%a, %d %b %Y %H:%M:%S", &tm);
    if (r != NULL) { // parsing succeeded
      t = mktime(&tm); // seconds since Epoch
    }
  }
  return t;
}

void client_reconnect() {
  Serial.print("Couldn't connect to ");
  Serial.print(host);
  Serial.print(":");
  Serial.print(port);
  Serial.println(" - Retrying...");
  while(!connect_client()) {}
}

void send_request(float humi,float temp) {
   // send HTTPS request
  client.print(verb);
  client.print(" ");
  client.print(path);
  client.print("&field1=");
  client.print(temp);
  client.print("&field2=");
  client.print(humi);
  
  // add UTC timestamp
  client.print("&created_at=");
  time_t t = getTime();
  printTime(client, t);
  
  client.print(" HTTP/1.1\r\n");
  client.print("Host: ");
  client.print(host);
  client.print("\r\n");
  client.print("Connection: close\r\n\r\n");
}

void setup()
{
  Serial.begin(115200);
  dht.setup(dhtPin, dhtModel);
  //Serial.setDebugOutput(true);
  setup_wifi();
  client.setInsecure();  // no cert validation
  get_initial_time();
}


void loop()
{
 long t = millis();
  if(WiFi.status() != WL_CONNECTED) {
    wait_for_wifi(t); 
  }
  if (t - t0 > 30 * 1000)
  { // every 30s
    t0 = t;

    // read sensor values
    float humi = dht.getHumidity();    // %
    float temp = dht.getTemperature(); // *C

    if (isnan(humi) || isnan(temp))
    {
      Serial.print("Failed to read from DHT11 sensor\n");
    }
    else
    {
      // connect to remote host
      
      if (!connect_client())
      {
        client_reconnect();
      } else {
        send_request(humi, temp);
      }
    }
  }
  delay(1);
}
