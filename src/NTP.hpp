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

#pragma once

#include <Arduino.h>
#include <TimeLib.h> // https://github.com/PaulStoffregen/Time

namespace WiFiManagerNS
{

  namespace NTP
  {

    struct Server
    {
      const char* name;
      const char* addr;
    };




    void loadPrefServer();
    void loadPrefs();

    bool setServer( uint8_t id );
    uint8_t getServerId();
    const char* server();

    #if defined ESP32
      void setSyncDelay( unsigned int minutes );
      unsigned int getSyncDelay(); // minutes

      typedef void(*onTimeAvailable_fn)(struct timeval *t);

      void onTimeAvailable( onTimeAvailable_fn fn );
    #endif



    // TODO: extend this list
    const Server Servers[] =
    {
      { "Global",        "pool.ntp.org" },
      { "Africa",        "africa.pool.ntp.org" },
      { "Asia",          "asia.pool.ntp.org" },
      { "Europe",        "europe.pool.ntp.org" },
      { "North America", "north-america.pool.ntp.org" },
      { "Oceania",       "oceania.pool.ntp.org" },
      { "South America", "south-america.pool.ntp.org" },
      // { "DHCP",          "0.0.0.0" }, // TODO: enable private NTP servers
    };

  };

}
