
#pragma once

#include "prefs.hpp"
#include "NTP.hpp"
#include "TZ.hpp"

namespace WiFiManagerNS
{

  bool NTPEnabled = false; // overriden by prefs
  bool DSTEnabled = true;
  String TimeConfHTML;
  char systime[64];

  std::function<void()> _webserverPreCallback;
  void bindServerCallback();


  void configTime()
  {
    const char* tz = TZ::getTzByLocation( TZ::tzName );
    Serial.printf("Setting up time: NTPServer=%s, TZ-Name=%s, TZ=%s\n", NTP::server(), TZ::tzName, tz );
    TZ::configTimeWithTz( tz, NTP::server() );
  }


  String getSystimeStr()
  {
    static String systimeStr;
    struct timeval tv;
    time_t nowtime;
    struct tm *nowtm;
    gettimeofday(&tv, NULL);
    nowtime = tv.tv_sec;
    nowtm = localtime(&nowtime);
    strftime(systime, sizeof systime, "%Y-%m-%dT%H:%M", nowtm);
    systimeStr = String( systime );
    return systimeStr;
  }

  #if ! defined WiFiManager_h

    void init()
    {
      TZ::loadPrefs();
      prefs::getBool("NTPEnabled", &NTPEnabled, false );
      prefs::getBool("DSTEnabled", &DSTEnabled, NTPEnabled );
      if( NTPEnabled ) {
        NTP::loadPrefs();
      }
    }

  #else //  defined WiFiManager_h


    constexpr const char* menuhtml = "<form action='/custom' method='get'><button>Setup Clock</button></form><br/>\n";

    enum Element_t
    {
      HTML_HEAD_START,
      HTML_STYLE,
      HTML_SCRIPT,
      HTML_HEAD_END,
      HTML_PORTAL_OPTIONS,
      HTML_ITEM,
      HTML_FORM_START,
      HTML_FORM_PARAM,
      HTML_FORM_END,
      HTML_SCAN_LINK,
      HTML_SAVED,
      HTML_END,
    };

    constexpr const size_t count = sizeof( Element_t );

    struct TemplateSet_t
    {
      Element_t element;
      const char* content;
    };

    TemplateSet_t templates[] =
    {
      { HTML_HEAD_START,     HTTP_HEAD_START     },
      { HTML_STYLE,          HTTP_STYLE          },
      { HTML_SCRIPT,         HTTP_SCRIPT         },
      { HTML_HEAD_END,       HTTP_HEAD_END       },
      { HTML_PORTAL_OPTIONS, HTTP_PORTAL_OPTIONS },
      { HTML_ITEM,           HTTP_ITEM           },
      { HTML_FORM_START,     HTTP_FORM_START     },
      { HTML_FORM_PARAM,     HTTP_FORM_PARAM     },
      { HTML_FORM_END,       HTTP_FORM_END       },
      { HTML_SCAN_LINK,      HTTP_SCAN_LINK      },
      { HTML_SAVED,          HTTP_SAVED          },
      { HTML_END,            HTTP_END            },
    };


    void setTemplate( Element_t element, const char* content )
    {
      templates[element].content = content;
    }


    const char* getTemplate( Element_t element )
    {
      return templates[element].content;
    }

    // teeny-tiny 57 bytes GIF favicon
    const char favicon[] =
    {
      0x47, 0x49, 0x46, 0x38, 0x39, 0x61, 0x09, 0x00, 0x08, 0x00, 0x80, 0x00, 0x00, 0xff, 0xff, 0xff,
      0x00, 0x00, 0x00, 0x21, 0xf9, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2c, 0x00, 0x00, 0x00, 0x00,
      0x09, 0x00, 0x08, 0x00, 0x00, 0x02, 0x10, 0x8c, 0x8f, 0x01, 0xbb, 0xc7, 0x60, 0xa0, 0x8a, 0x54,
      0xbd, 0x16, 0x1f, 0x82, 0x3c, 0xf9, 0x02, 0x00, 0x3b
    };



    WiFiManager *_wifiManager;



    void init(WiFiManager *manager, std::function<void()> webserverPreCallback)
    {
      _webserverPreCallback = webserverPreCallback;

      _wifiManager = manager;
      _wifiManager->setWebServerCallback(bindServerCallback);
      _wifiManager->setCustomMenuHTML( menuhtml );
      _wifiManager->setClass("invert");
      // _wifiManager->wm.setDarkMode(true);
      TZ::loadPrefs();
      #if defined ESP32
        prefs::getBool("NTPEnabled", &NTPEnabled, false );
      #else
        NTPEnabled = prefs::getPref(prefs::NTP_ENABLED);
      #endif
      if( NTPEnabled ) {
        NTP::loadPrefs();
      }
    }


    void handleFavicon()
    {
      _wifiManager->server->send_P(200, "image/gif", favicon, sizeof(favicon) );
    }


    void handleRoute()
    {
      Serial.println("[HTTP] handle route Custom");

      TimeConfHTML = "";
      TimeConfHTML += getTemplate(HTML_HEAD_START);
      TimeConfHTML.replace(FPSTR(T_v), "Timezone setup");
      TimeConfHTML += getTemplate(HTML_SCRIPT);

      TimeConfHTML += "<script>";
      TimeConfHTML += "window.addEventListener('load', function() { var now = new Date(); var offset = now.getTimezoneOffset() * 60000; var adjustedDate = new Date(now.getTime() - offset);";
      TimeConfHTML += "document.getElementById('set-time').value = adjustedDate.toISOString().substring(0,16); });";
      TimeConfHTML += "</script>";

      TimeConfHTML += getTemplate(HTML_STYLE);
      TimeConfHTML += "<style> input[type='checkbox'][name='use-ntp-server']:not(:checked) ~.collapsable { display:none; }</style>";
      TimeConfHTML += "<style> input[type='checkbox'][name='use-ntp-server']:checked       ~.collapsed   { display:none; }</style>";
      TimeConfHTML += getTemplate(HTML_HEAD_END);
      TimeConfHTML.replace(FPSTR(T_c), "invert"); // add class str
      TimeConfHTML += "<h1>Time Settings</h1>";

      String systimeStr = getSystimeStr();

      TimeConfHTML += "<label for='ntp-server'>System Time ";

      TimeConfHTML += "<input readonly style=width:auto name='system-time' type='datetime-local' value='"+systimeStr+"'>";
      TimeConfHTML += " <button onclick=location.reload() style=width:auto type=button> Refresh </button></label><br>";

      TimeConfHTML += "<iframe name='dummyframe' id='dummyframe' style='display: none;'></iframe><form action='/save-tz' target='dummyframe' method='POST'>";

      //const char *currentTimeZone = "Europe/Paris";
      TimeConfHTML += "<label for='timezone'>Time Zone</label>";
      TimeConfHTML += "<select id='timezone' name='timezone'>";
      for(size_t i = 0; i < TZ::zones(); i += 2) {
        bool selected = (strcmp(TZ::tzName, TZ::timezones[i]) == 0);
        TimeConfHTML += "<option value='"+ String(i) +"'" + String(selected ? "selected":"") + ">"+ String( TZ::timezones[i]) +"</option>";
      }
      TimeConfHTML += "</select><br>";


      TimeConfHTML += "<label for='use-ntp-server'>Enable NTP Client</label> ";
      TimeConfHTML += " <input value='1' type=checkbox name='use-ntp-server' id='use-ntp-server'" + String(NTPEnabled?"checked":"") + ">";
      TimeConfHTML += "<br>";

      TimeConfHTML += "<div class='collapsed'>";
      TimeConfHTML += "<label for='set-time'>Set Time ";
      TimeConfHTML += "<input style=width:auto name='set-time' id='set-time' type='datetime-local' value='"+systimeStr+"'>";
      TimeConfHTML += "</div>";


      TimeConfHTML += "<div class='collapsable'>";

      TimeConfHTML += "<h2>NTP Client Setup</h2>";

      TimeConfHTML += "<label for='enable-dst'>Auto-adjust clock for DST</label> ";
      TimeConfHTML += " <input value='1' type=checkbox name='enable-dst' id='enable-dst'" + String(DSTEnabled?"checked":"") + ">";
      TimeConfHTML += "<br>";

      TimeConfHTML += "<label for='ntp-server'>Server:</label>";
      //TimeConfHTML += "<input list='ntp-server-list' id='ntp-server' name='ntp-server' placeholder='pool.ntp.org'";
      //TimeConfHTML += " value='"+ NTP::server() +"'>";
      TimeConfHTML += "<select id='ntp-server-list' name='ntp-server'>";
      size_t servers_count = sizeof( NTP::Servers ) / sizeof( NTP::Server );
      uint8_t server_id = NTP::getServerId();
      for( size_t i=0; i<servers_count; i++ ) {
        TimeConfHTML += "<option value='"+ String(i) +"'" + String(i==server_id ? "selected":"") + ">"+ String(NTP::Servers[i].name) +"("+ String(NTP::Servers[i].addr) +")</option>";
      }
      TimeConfHTML += "</select><br>";

      #if defined ESP32
        TimeConfHTML += "<label for='ntp-server-interval'>Sync interval:</label>";
        TimeConfHTML += "<select id='ntp-server-interval' name='ntp-server-interval'>";
        TimeConfHTML += "<option value=60>Hourly</option>";
        TimeConfHTML += "<option value=14400>Daily</option>";
        TimeConfHTML += "<option value=10080>Weekly</option>";
        TimeConfHTML += "</select><br>";
      #endif

      TimeConfHTML += "</div>";

      TimeConfHTML += "<button type=submit>Submit</button>";
      TimeConfHTML += "</form>";

      TimeConfHTML += getTemplate(HTML_END);

      _wifiManager->server->send_P( 200, "text/html", TimeConfHTML.c_str(), TimeConfHTML.length() );

      TimeConfHTML = String();
    }


    void handleValues()
    {
      bool success = true;
      bool _NTPEnabled = NTPEnabled;
      Serial.println("[HTTP] handle route Values");
      if( _wifiManager->server->hasArg("use-ntp-server") ) {
        String UseNtpServer = _wifiManager->server->arg("use-ntp-server");
        log_d("UseNtpServer: %s", UseNtpServer.c_str() );
        uint8_t useNtpServer = atoi( UseNtpServer.c_str() );
        NTPEnabled = useNtpServer==1;
      } else {
        NTPEnabled = false;
      }
      if( _NTPEnabled != NTPEnabled ) {
        #if defined ESP32
          prefs::setBool("NTPEnabled", NTPEnabled );
        #else
          prefs::setPref(prefs::NTP_ENABLED, NTPEnabled );
        #endif
      }

      if( _wifiManager->server->hasArg("timezone") ) {
        String timezoneStr = _wifiManager->server->arg("timezone");
        log_d("timezoneStr: %s", timezoneStr.c_str() );
        size_t tzidx = atoi( timezoneStr.c_str() );
        String timezone = TZ::defaultTzName;
        if( tzidx < TZ::zones() ) {
          timezone = String( TZ::timezones[tzidx] );
          log_d("timezone: %s", timezone.c_str() );
        }

        TZ::setTzName( timezone.c_str() );
        const char* tz = TZ::getTzByLocation( TZ::tzName );
        TZ::configTimeWithTz( tz, NTP::server() );
      }

      if( NTPEnabled ) {
        // also collect tz/server data
        if( _wifiManager->server->hasArg("ntp-server") ) {
          String NtpServer = _wifiManager->server->arg("ntp-server");
          log_d("NtpServer: %s", NtpServer.c_str() );
          uint8_t server_id = atoi( NtpServer.c_str() );
          if( !NTP::setServer( server_id ) ) success = false;
        }

        if( _wifiManager->server->hasArg("ntp-server-interval") ) {
          String NtpServerInterval = _wifiManager->server->arg("ntp-server-interval");
          log_d("NtpServerInterval: %s", NtpServerInterval.c_str() );
          int serverInterval = atoi( NtpServerInterval.c_str() );
          switch( serverInterval ) {
            case 60:
            case 14400:
            case 10080:
            break;
            default:
              serverInterval = 14400;
          }
          #if defined ESP32
            NTP::setSyncDelay( serverInterval );
          #endif
        }
      }

      const char* successResp = "<script>parent.location.href = '/';</script>";
      const char* failureResp = "<script>parent.alert('fail');</script>";

      _wifiManager->server->send(200, "text/html", success?successResp:failureResp);
    }


    void bindServerCallback()
    {
      if (_webserverPreCallback != NULL) {
        _webserverPreCallback(); // callback to add/override server routes
      }

      _wifiManager->server->on("/custom", handleRoute);
      _wifiManager->server->on("/save-tz", handleValues);
      _wifiManager->server->on("/favicon.ico", handleFavicon);
    }

  #endif

};
