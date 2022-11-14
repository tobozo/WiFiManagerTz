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


void setup()
{
  Serial.begin(115200);
  delay(1000);
  Serial.println("Welcome to the tzupdate example");

  // attach NTP/TZ/Clock-setup page to the WiFi Manager
  WiFiManagerNS::init( &wifiManager );

  // /!\ make sure "custom" is listed there as it's required to pull the "Setup Clock" button
  std::vector<const char *> menu = {"wifi", "info", "custom", "param", "sep", "restart", "exit"};
  wifiManager.setMenu(menu);

  wifiManager.setConfigPortalBlocking(false);
  wifiManager.setConfigPortalTimeout(600);

  if(wifiManager.autoConnect("AutoConnectAP", "12345678")){
    Serial.println("connected...yeey :)");
  }else {
    Serial.println("Configportal running");
  }

  Serial.println("local ip");
  Serial.println(WiFi.localIP());
}


void loop()
{
  //wifiManager.process();
}
