
# WiFiManagerTz for ESP32

Adds NTP/TimeZone and Clock tools to @tzapu's [WiFiManager](https://github.com/tzapu/WiFiManager)


![](assets/screenshot.png)


## Integration


```cpp

#include <WiFiManager.h>
#include <WiFiManagerTz.h>

void setup()
{
  WiFiManager wifiManager;

  WiFiManagerNS::init( &wifiManager );
  // WiFiManagerNS::init( &wifiManager, webserverPreCallback ); // using a custom callback add/override server routes

  // /!\ make sure "custom" is listed there as it's required to pull the "Setup Clock" button
  std::vector<const char *> menu = {"wifi", "info", "custom", "param", "sep", "restart", "exit"};
  wifiManager.setMenu(menu);

  // ....

}

```

## Motivation

I made this library in order to learn how to properly use the WiFiManager, and I'm still learning, so feel free to report any WTF :-)

## Roadmap

  - Add more examples
  - Implement minimal logic for external RTC modules coupling
  - Make the Time Setup page skinnable
  ~~- ESP8266 support~~


## Dependencies

  - https://github.com/tzapu/WiFiManager
  - https://github.com/PaulStoffregen/Time


## Credits / External Sources

  - [@tablatronix](https://github.com/tablatronix)
  - [@tzapu](https://github.com/tzapu)
  - [@marciot](https://github.com/marciot)
  - https://werner.rothschopf.net/microcontroller/202103_arduino_esp32_ntp_en.htm
  - https://github.com/marciot/esp32-hacks
  - https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv
