#include <WiFi.h>
#include <HTTPClient.h>
#include <ESP32GithubOTA.h>

WiFiClientSecure client;
ESP32GithubOTA updater;

const char *rootCa =
    "-----BEGIN CERTIFICATE-----\n"
    "ROOT CERTIFICATE OF GITHUB API\n"
    "-----END CERTIFICATE-----";

void setup()
{
  Serial.begin(115200);
  delay(500);
  client.setCACert(rootCa);

  updater = ESP32GithubOTA(client);
  updater.setRepository("Owner", "Repository", "Branch");
  updater.setAccessToken("YOUR ACCESS TOKEN");

  //displayInit();
  WiFi.begin("SSID", "PSK");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
  }
  Serial.println("Connected!");
}

void loop()
{
  updater.check();
  delay(60000);
}
