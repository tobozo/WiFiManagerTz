
# WiFiManagerTz for ESP32

Adds NTP/TimeZone and Clock tools to @tzapu's [WiFiManager](https://github.com/tzapu/WiFiManager)

## Integration


```cpp

  WiFiManager wifiManager;

  WiFiManagerNS::init( &wifiManager );

  // /!\ make sure "custom" is listed there as it's required to pull the "Setup Clock" button
  std::vector<const char *> menu = {"wifi", "info", "custom", "param", "sep", "restart", "exit"};
  wifiManager.setMenu(menu);


```


## Credits

  - https://werner.rothschopf.net/microcontroller/202103_arduino_esp32_ntp_en.htm
  - https://github.com/marciot/esp32-hacks
  - https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv
