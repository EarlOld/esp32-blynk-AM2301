/*************************************************************
  Blynk is a platform with iOS and Android apps to control
  ESP32, Arduino, Raspberry Pi and the likes over the Internet.
  You can easily build mobile and web interfaces for any
  projects by simply dragging and dropping widgets.

    Downloads, docs, tutorials: https://www.blynk.io
    Sketch generator:           https://examples.blynk.cc
    Blynk community:            https://community.blynk.cc
    Follow us:                  https://www.fb.com/blynkapp
                                https://twitter.com/blynk_app

  This example code is in public domain.

 *************************************************************
  App dashboard setup:
    Value Display widget on V2

 *************************************************************/

#ifdef ESP32
#include <WiFi.h>
#else
#include <ESP8266WiFi.h>
#endif

#include <Adafruit_Sensor.h>
#include <DHT.h>

// *** MAIN SETTINGS ***
// Replace this block with correct template settings [HTTPS LINK TO Console/Templates/TemplateID]

#define BLYNK_TEMPLATE_ID "TMPLxxxxxx"      // TemplateID can be found in Console -> Templates -> Template 
#define BLYNK_TEMPLATE_NAME "Template Name" // Template Name can be found in Console -> Templates -> Template
#define BLYNK_AUTH_TOKEN "YourAuthToken"    // Each device needs a unique AuthToken. Create new device to obtain a new Auth Token
#define WIFI_SSID "WiFi network name"       // Your WiFi network name
#define WIFI_PASS "WiFi password"           // Your WiFi password. Double-check that it's correct. For open WiFi with no password, use "".

// *********************

#define DHTPIN 14
#define DHTTYPE DHT21

// Network settings
const char ssid[] = "";
const char pass[] = "";

// Blynk cloud server
const char *host = "fra.blynk-qa.com";
unsigned int port = 80;
DHT dht(DHTPIN, DHTTYPE);

WiFiClient client;

// Start the WiFi connection
void connectNetwork()
{
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi connected");
}

bool httpRequest(const String &method,
                 const String &url,
                 const String &request,
                 String &response)
{
  Serial.print(F("Connecting to "));
  Serial.print(host);
  Serial.print(":");
  Serial.print(port);
  Serial.print("... ");
  if (client.connect(host, port))
  {
    Serial.println("OK");
  }
  else
  {
    Serial.println("failed");
    return false;
  }

  Serial.print(method);
  Serial.print(" ");
  Serial.println(url);

  client.print(method);
  client.print(" ");
  client.print(url);
  client.println(F(" HTTP/1.1"));
  client.print(F("Host: "));
  client.println(host);
  client.println(F("Connection: close"));
  if (request.length())
  {
    client.println(F("Content-Type: application/json"));
    client.print(F("Content-Length: "));
    client.println(request.length());
    client.println();
    client.print(request);
  }
  else
  {
    client.println();
  }

  // Serial.println("Waiting response");
  int timeout = millis() + 5000;
  while (client.available() == 0)
  {
    if (timeout - millis() < 0)
    {
      Serial.println(">>> Client Timeout !");
      client.stop();
      return false;
    }
  }

  // Serial.println("Reading response");
  int contentLength = -1;
  while (client.available())
  {
    String line = client.readStringUntil('\n');
    line.trim();
    line.toLowerCase();
    if (line.startsWith("content-length:"))
    {
      contentLength = line.substring(line.lastIndexOf(':') + 1).toInt();
    }
    else if (line.length() == 0)
    {
      break;
    }
  }

  // Serial.println("Reading response body");
  response = "";
  response.reserve(contentLength + 1);
  while (response.length() < contentLength)
  {
    if (client.available())
    {
      char c = client.read();
      response += c;
    }
    else if (!client.connected())
    {
      break;
    }
  }
  client.stop();
  return true;
}

void setup()
{
  Serial.begin(9600);
  delay(10);
  Serial.println();
  Serial.println();
  dht.begin();
  connectNetwork();
}

void loop()
{
  String response;

  Serial.print("Sending value: ");

  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (httpRequest("GET", String("/external/api/update?token=") + BLYNK_AUTH_TOKEN + "&pin=V0&value=" + t, "", response))
  {
    if (response.length() != 0)
    {
      Serial.print("WARNING: ");
      Serial.println(response);
    }
  }
  if (httpRequest("GET", String("/external/api/update?token=") + BLYNK_AUTH_TOKEN + "&pin=V1&value=" + h, "", response))
  {
    if (response.length() != 0)
    {
      Serial.print("WARNING: ");
      Serial.println(response);
    }
  }

  delay(30000L);
}
