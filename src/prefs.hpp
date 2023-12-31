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
#if defined ESP32
  #include <Preferences.h>
#else
  #include <EEPROM.h>
#endif

#if !defined(log_d)
  #define log_d printf
#endif

#define PREF_NAMESPACE "wm"

namespace WiFiManagerNS
{

  namespace prefs
  {

    enum pref_name_t
    {
      NTP_DELAYMIN,
      NTP_ZONE_KEY,
      NTP_ENABLED,
      TIMEZONE_ID,
    };

    constexpr const char* NVS_DST_KEY      = "DST";
    constexpr const char* NVS_NTPZONE_KEY  = "NTPZONE";
    constexpr const char* NVS_NTP_DELAYMIN = "NTPDELAY";
    constexpr const char* prefName         = "TZNAME";
    constexpr const char* TZ_ID            = "TZID";
    constexpr const char* NTP_SYNC_ENABLED = "NTPEnabled";

    void reset();

    #if defined ESP32
      void set( const char *name, const char *value, size_t len );
      void get( const char *name, char *dest, size_t max_len, const char *default_value );
      void setUChar( const char *name, uint8_t value );
      void getUChar( const char *name, uint8_t *dest, uint8_t default_value );
      void setFloat( const char *name, float value );
      void getFloat( const char *name, float *dest, float default_value );
      void setBool( const char* key, bool value);
      void getBool( const char *name, bool *dest, bool default_value );
      void setUInt( const char* name, unsigned int value);
      void getUInt( const char *name, unsigned int *dest, unsigned int default_value );
    #endif

    void setPref( pref_name_t pref_name, int val );
    int getPref(  pref_name_t pref_name );

    /*
    {
      switch( pref_name )
      {
      #if defined ESP32
        case NTP_DELAYMIN: prefs::setUInt( NVS_NTP_DELAYMIN, val ); break;
        case NTP_ZONE_KEY: prefs::setUChar( NVS_NTPZONE_KEY, val ); break;
        case NTP_ENABLED : prefs::setBool("NTPEnabled", val ); break;
        case TIMEZONE_ID : prefs::setUInt( "TZID", val ); break;
      #elif defined ESP8266
        case NTP_DELAYMIN: EEPROM.write( NTP_DELAYMIN, val ); break;
        case NTP_ZONE_KEY: EEPROM.write( NTP_ZONE_KEY, val ); break;
        case NTP_ENABLED : EEPROM.write( NTP_ENABLED, val ); break;
        case TIMEZONE_ID : EEPROM.write( TIMEZONE_ID, val ); break;
      #endif

      }
    }*/



  };

}
