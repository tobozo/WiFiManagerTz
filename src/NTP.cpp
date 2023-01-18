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

#include "prefs.hpp"
#include "NTP.hpp"
#include "sntp.h"


namespace WiFiManagerNS
{

  namespace NTP
  {

    const char* NVS_DST_KEY      = "DST";
    const char* NVS_NTPZONE_KEY  = "NTPZONE";
    const char* NVS_NTP_DELAYMIN = "NTPDELAY";

    const char* defaultServer = "pool.ntp.org";
    uint8_t currentServer = 0;
    unsigned int sync_delay = 60; // minutes

    unsigned int getSyncDelay()
    {
      return sync_delay;
    }

    void setSyncDelay( unsigned int minutes )
    {
      if( sync_delay != minutes ) {
        log_d("Setting NTP sync delay to #%d minutes", minutes );
        prefs::setUInt(  NVS_NTP_DELAYMIN, minutes );
      }
      sync_delay = minutes;
      sntp_set_sync_interval(sync_delay*60*1000);
    }

    // Callback function (get's called when time adjusts via NTP)
    void timeavailable_default(struct timeval *t)
    {
      Serial.println("Got time adjustment from NTP!");
      struct tm timeInfo;
      getLocalTime(&timeInfo, 1000);
      Serial.println(&timeInfo, "%A, %B %d %Y %H:%M:%S zone %Z %z ");
    }


    onTimeAvailable_fn timeavailable = &timeavailable_default;

    void onTimeAvailable( onTimeAvailable_fn fn )
    {
      log_d("Settting custom time notifier");
      timeavailable = fn;
    }


    bool setServer( uint8_t id )
    {
      size_t servers_count = sizeof( Servers ) / sizeof( Server );

      if( id < servers_count ) {
        if( id != currentServer ) {
          currentServer = id;
          log_d("Setting NTP server to #%d ( %s / %s )", currentServer, Servers[currentServer].name, Servers[currentServer].addr );
          prefs::setUChar( NVS_NTPZONE_KEY, currentServer );
        }
        return true;
      }
      log_d("Invalid NTP requested: #%d", id );
      return false;
    }


    uint8_t getServerId()
    {
      return currentServer;
    }


    const char* server()
    {
      uint8_t server_id = getServerId();
      uint8_t servers_count = sizeof(Servers)/sizeof(Server);
      if( servers_count <= server_id ) {
        // uh-oh, server list is messed up, return default
        return "pool.ntp.org";
      }
      return Servers[getServerId()].addr;
    }


    void loadPrefs()
    {
      prefs::getUChar( NVS_NTPZONE_KEY,  &currentServer, currentServer );
      prefs::getUInt(  NVS_NTP_DELAYMIN, &sync_delay,    sync_delay );
      if( timeavailable )
        sntp_set_time_sync_notification_cb( timeavailable );
      sntp_set_sync_interval(sync_delay*60*1000);
    }

  };


};
