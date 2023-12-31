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
#include "TZ.hpp"



namespace WiFiManagerNS
{

  #if defined ESP32
    Preferences _prefs;
  #endif

  namespace prefs
  {


    void setPref( pref_name_t pref_name, int val )
    {
      switch( pref_name )
      {
        #if defined ESP32
          default: break;
          // case NTP_DELAYMIN: prefs::setUInt(  NVS_NTP_DELAYMIN, val ); break;
          // case NTP_ZONE_KEY: prefs::setUChar( NVS_NTPZONE_KEY, val ); break;
          // case NTP_ENABLED : prefs::setBool(  NTP_SYNC_ENABLED, val ); break;
          // case TIMEZONE_ID :
          //   prefs::set( prefName, TZ::timezones[val], strlen(TZ::timezones[val]) );
          //   prefs::setUInt( TZ_ID, val );
          // break;
        #elif defined ESP8266
          case NTP_DELAYMIN: EEPROM.write( NTP_DELAYMIN, val ); EEPROM.commit(); break;
          case NTP_ZONE_KEY: EEPROM.write( NTP_ZONE_KEY, val ); EEPROM.commit(); break;
          case NTP_ENABLED : EEPROM.write( NTP_ENABLED, val );  EEPROM.commit(); break;
          case TIMEZONE_ID : EEPROM.write( TIMEZONE_ID, val );  EEPROM.commit(); break;
        #endif
      }
    }


    int getPref(  pref_name_t pref_name )
    {
      switch( pref_name )
      {
        #if defined ESP32
          default: return 0;
          // case NTP_DELAYMIN: return prefs::getUInt(  NVS_NTP_DELAYMIN, 60 );
          // case NTP_ZONE_KEY: prefs::getUChar( NVS_NTPZONE_KEY,  &currentServer, currentServer ); return 0;
          // case NTP_ENABLED : return prefs::getBool(  NTP_SYNC_ENABLED, false );
          // case TIMEZONE_ID : return prefs::getUInt(  TZ_ID, val, 0 );
        #elif defined ESP8266
          case NTP_DELAYMIN: return EEPROM.read( NTP_DELAYMIN );
          case NTP_ZONE_KEY: return EEPROM.read( NTP_ZONE_KEY );
          case NTP_ENABLED : return EEPROM.read( NTP_ENABLED );
          case TIMEZONE_ID : return EEPROM.read( TIMEZONE_ID );
          default: return 0;
        #endif
      }
    }


    #if defined ESP32

      void reset()
      {
        _prefs.begin(PREF_NAMESPACE, false );
        _prefs.clear();
        _prefs.end();
      }


      void set( const char *name, const char *value, size_t len )
      {
        _prefs.begin(PREF_NAMESPACE, false );
        char buf[len+2] = {0};
        snprintf( buf, len+1, "%s", value );
        if( _prefs.putString(name, buf) ) {
          log_d("[Pref] '%s' Saved: char[%d]", name, len-1 );
        } else {
          log_d("[Pref] '%s' Saving failed!  (char[%d])", name, len-1 );
        }
        _prefs.end();
      }


      void get( const char *name, char *dest, size_t max_len, const char *default_value )
      {
        _prefs.begin(PREF_NAMESPACE, true );
        size_t len = _prefs.getString(name, dest, max_len );
        if( len > 0 ) {
          log_d("[Pref] '%s' Thawed: char[%d]", name, len );
        } else {
          len = strlen(default_value)+1;
          snprintf( dest, max_len, "%s", default_value );
          log_d("[Pref] '%s' Defaulted to '%s')", name, default_value );
        }
        _prefs.end();
      }


      void setUChar( const char *name, uint8_t value )
      {
        _prefs.begin(PREF_NAMESPACE, false );
        if( _prefs.putUChar(name, value) ) {
          log_d("[Pref] Saved: '%s' => %d", name, value );
        } else {
          log_d("[Pref] Saving failed! '%s' => %d", name, value );
        }
        _prefs.end();
      }


      void getUChar( const char *name, uint8_t *dest, uint8_t default_value )
      {
        _prefs.begin(PREF_NAMESPACE, true );
        *dest = _prefs.getUChar(name, default_value );
        if( *dest != default_value ) {
          log_d("[Pref] Thawed: '%s' => %d", name, *dest );
        } else {
          log_d("[Pref] Defaulted: '%s' => %d", name, default_value );
        }
        _prefs.end();
      }


      void setFloat( const char *name, float value )
      {
        _prefs.begin(PREF_NAMESPACE, false );
        if( _prefs.putFloat(name, value) ) {
          log_d("[Pref] Saved: '%s' => %.2f", name, value );
        } else {
          log_d("[Pref] Saving failed! '%s' => %.2f", name, value );
        }
        _prefs.end();
      }


      void getFloat( const char *name, float *dest, float default_value )
      {
        _prefs.begin(PREF_NAMESPACE, true );
        *dest = _prefs.getFloat(name, default_value );
        if( *dest != default_value ) {
          log_d("[Pref] Thawed: '%s' => %.2f", name, *dest );
        } else {
          log_d("[Pref] Defaulted: '%s' => %.2f", name, default_value );
        }
        _prefs.end();
      }


      void setBool(const char* name, bool value)
      {
        _prefs.begin(PREF_NAMESPACE, false );
        if( _prefs.putBool(name, value) ) {
          log_d("[Pref] Saved: '%s' => %s", name, value?"true":"false" );
        } else {
          log_d("[Pref] Saving failed! '%s' => %s", name, value?"true":"false" );
        }
        _prefs.end();
      }


      void getBool( const char *name, bool *dest, bool default_value )
      {
        _prefs.begin(PREF_NAMESPACE, true );
        *dest = _prefs.getBool(name, default_value );
        if( *dest != default_value ) {
          log_d("[Pref] Thawed: '%s' => %s", name, *dest?"true":"false" );
        } else {
          log_d("[Pref] Defaulted: '%s' => %s", name, default_value?"true":"false" );
        }
        _prefs.end();
      }


      void setUInt(  const char* name, unsigned int value)
      {
        _prefs.begin(PREF_NAMESPACE, false );
        if( _prefs.putUInt(name, value) ) {
          log_d("[Pref] Saved: '%s' => %d", name, value );
        } else {
          log_d("[Pref] Saving failed! '%s' => %d", name, value );
        }
        _prefs.end();
      }


      void getUInt(  const char *name, unsigned int *dest, unsigned int default_value )
      {
        _prefs.begin(PREF_NAMESPACE, true );
        *dest = _prefs.getUInt(name, default_value );
        if( *dest != default_value ) {
          log_d("[Pref] Thawed: '%s' => %d", name, *dest );
        } else {
          log_d("[Pref] Defaulted: '%s' => %d", name, default_value );
        }
        _prefs.end();
      }
    #endif

  };

};
