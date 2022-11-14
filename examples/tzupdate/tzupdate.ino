

#include <WiFiManager.h>
#include <WiFiManagerTz.h>

WiFiManager wifiManager;

void setupWiFi()
{
  WiFi.mode(WIFI_STA); // explicitly set mode, esp defaults to STA+AP
  Serial.println(WiFi.macAddress());
  WiFi.begin();
  while(WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(100);
  }
}


void setup()
{
  Serial.begin(115200);
  delay(1000);
  Serial.println("Welcome to the tzupdate example");

  setupWiFi();

  // attach NTP/TZ/Clock-setup page to the WiFi Manager
  WiFiManagerNS::init( &wifiManager );

  // /!\ make sure "custom" is listed there as it's required to pull the "Setup Clock" button
  std::vector<const char *> menu = {"wifi", "info", "custom", "param", "sep", "restart", "exit"};
  wifiManager.setMenu(menu);

  wifiManager.setConfigPortalBlocking(false);
  wifiManager.setConfigPortalTimeout(60);

  wifiManager.startWebPortal();

  Serial.println("local ip");
  Serial.println(WiFi.localIP());
}


void loop()
{
  wifiManager.process();
}
