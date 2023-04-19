// Вставьте информацию со своего девайса
// #define BLYNK_TEMPLATE_ID ""
// #define BLYNK_TEMPLATE_NAME ""
// #define BLYNK_AUTH_TOKEN ""
#define DHTPIN 14 // укажите пин к которому подключен датчик
#define DHTTYPE    DHT21
// Comment this out to disable prints and save space
#define BLYNK_PRINT Serial

#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

char auth[] = BLYNK_AUTH_TOKEN;

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "";
char pass[] = "";

BlynkTimer timer;
DHT dht(DHTPIN, DHTTYPE);


// This function sends Arduino's uptime every second to Virtual Pin 2.
void myTimerEvent()
{
  // You can send any value at any time.
  // Please don't send more that 10 values per second.
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  Blynk.virtualWrite(V0, t);
  Blynk.virtualWrite(V1, h);
}

void setup()
{
  Serial.begin(115200);
  dht.begin();
  Blynk.begin(auth, ssid, pass, "fra.blynk-qa.com", 80);
 
  timer.setInterval(1000L, myTimerEvent);
}

void loop()
{
  Blynk.run();
  timer.run();
}
