/*\
 *
 * WiFiManagerTz
 *
 * Copyright (c) 2022 tobozo
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
\*/

#include <WiFiManager.h>
#include <WiFiManagerTz.h>


WiFiManager wifiManager;

bool configSaved = false;


#if defined ESP32
  // Optional callback function, fired when NTP gets updated.
  // Used to print the updated time or adjust an external RTC module.
  void on_time_available(struct timeval *t)
  {
    Serial.println("Received time adjustment from NTP");
    struct tm timeInfo;
    getLocalTime(&timeInfo, 1000);
    Serial.println(&timeInfo, "%A, %B %d %Y %H:%M:%S zone %Z %z ");
    // RTC.adjust( &timeInfo );
  }
#endif



void setup()
{
  Serial.begin(115200);
  delay(1000);
  Serial.println("Welcome to the tzupdate example");

  Serial.print("Mac Address: ");
  Serial.println(WiFi.macAddress());

  // wifiManager.resetSettings();

  #if defined ESP32
    // optionally attach external RTC update callback
    WiFiManagerNS::NTP::onTimeAvailable( &on_time_available );
  #endif

  // attach NTP/TZ/Clock-setup page to the WiFi Manager
  WiFiManagerNS::init( &wifiManager, nullptr );

  // /!\ make sure "custom" is listed there as it's required to pull the "Setup Clock" button
  std::vector<const char *> menu = {"wifi", "info", "custom", "param", "sep", "restart", "exit"};
  wifiManager.setMenu(menu);

  wifiManager.setConnectRetries(10);          // necessary with sluggish routers and/or hidden AP
  wifiManager.setCleanConnect(true);          // ESP32 wants this
  wifiManager.setConfigPortalBlocking(false); // /!\ false=use "wifiManager.process();" in the loop()
  wifiManager.setConfigPortalTimeout(120);    // will wait 2mn before closing portal
  wifiManager.setSaveConfigCallback([](){ configSaved = true;}); // restart on credentials save, ESP32 doesn't like to switch between AP/STA

  if(wifiManager.autoConnect("AutoConnectAP", "12345678")){
    Serial.print("Connected to Access Point, visit http://");
    Serial.print(WiFi.localIP());
    Serial.println(" to setup the clock or change WiFi settings");
    wifiManager.startConfigPortal();
  } else {

    Serial.println("Configportal is running, no WiFi has been set yet");
  }

  if(WiFi.status() == WL_CONNECTED){

    WiFiManagerNS::configTime();
  }

}


void loop()
{
  wifiManager.process();

  // TODO: detect reset query from button
  // if( long_button_press ) {
  //    wifiManager.resetSettings();
  //    ESP.restart();
  // }

}
