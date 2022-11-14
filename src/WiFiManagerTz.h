
#pragma once

#include "prefs.hpp"
#include "NTP.hpp"
#include "TZ.hpp"



namespace WiFiManagerNS
{

  #include "strings_en.h"

  bool NTPEnabled = false; // overriden by prefs
  bool DSTEnabled = true;
  String PixieConfHTML;
  char systime[64];

  constexpr const char* menuhtml = "<form action='/custom' method='get'><button>Setup Clock</button></form><br/>\n";

  WiFiManager *_wifiManager;

  void bindServerCallback();

  void init(WiFiManager *manager)
  {
    _wifiManager = manager;
    _wifiManager->setWebServerCallback(bindServerCallback);
    _wifiManager->setCustomMenuHTML( menuhtml );
    _wifiManager->setClass("invert");
    TZ::loadPrefs();
    prefs::getBool("NTPEnabled", &NTPEnabled, false );
    if( NTPEnabled ) {
      NTP::loadPrefs();
      Serial.printf("Setting up NTP:%s, TZ:%s\n", NTP::server().c_str(), TZ::tzName );
      String tz = TZ::getTzByLocation( String(TZ::tzName) );
      TZ::configTimeWithTz( tz, NTP::server() );
    }
  }


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

  void handleFavicon()
  {
    _wifiManager->server->send_P(200, "image/gif", favicon, sizeof(favicon) );
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


  void handleRoute()
  {
    Serial.println("[HTTP] handle route Custom");

    PixieConfHTML = "";
    PixieConfHTML += getTemplate(HTML_HEAD_START);
    PixieConfHTML.replace(FPSTR(T_v), "Timezone setup");
    PixieConfHTML += getTemplate(HTML_SCRIPT);

    PixieConfHTML += "<script>";
    PixieConfHTML += "window.addEventListener('load', function() { var now = new Date(); var offset = now.getTimezoneOffset() * 60000; var adjustedDate = new Date(now.getTime() - offset);";
    PixieConfHTML += "document.getElementById('set-time').value = adjustedDate.toISOString().substring(0,16); });";
    PixieConfHTML += "</script>";

    PixieConfHTML += getTemplate(HTML_STYLE);
    PixieConfHTML += "<style> input[type='checkbox'][name='use-ntp-server']:not(:checked) ~.collapsable { display:none; }</style>";
    PixieConfHTML += "<style> input[type='checkbox'][name='use-ntp-server']:checked       ~.collapsed   { display:none; }</style>";
    PixieConfHTML += getTemplate(HTML_HEAD_END);
    PixieConfHTML.replace(FPSTR(T_c), "invert"); // add class str
    PixieConfHTML += "<h1>Time Settings</h1>";

    String systimeStr = getSystimeStr();

    PixieConfHTML += "<label for='ntp-server'>System Time ";

    PixieConfHTML += "<input readonly style=width:auto name='system-time' type='datetime-local' value='"+systimeStr+"'>";
    PixieConfHTML += " <button onclick=location.reload() style=width:auto type=button> Refresh </button></label><br>";

    PixieConfHTML += "<iframe name='dummyframe' id='dummyframe' style='display: none;'></iframe><form action='/save-tz' target='dummyframe' method='POST'>";

    //const char *currentTimeZone = "Europe/Paris";
    PixieConfHTML += "<label for='timezone'>Time Zone</label>";
    PixieConfHTML += "<select id='timezone' name='timezone'>";
    for(int i = 0; i < TZ::zones(); i += 2) {
      bool selected = (strcmp(TZ::tzName, TZ::timezones[i]) == 0);
      PixieConfHTML += "<option value='"+ String(i) +"'" + String(selected ? "selected":"") + ">"+ String( TZ::timezones[i]) +"</option>";
    }
    PixieConfHTML += "</select><br>";


    PixieConfHTML += "<label for='use-ntp-server'>Enable NTP Client</label> ";
    PixieConfHTML += " <input value='1' type=checkbox name='use-ntp-server' id='use-ntp-server'" + String(NTPEnabled?"checked":"") + ">";
    PixieConfHTML += "<br>";

    PixieConfHTML += "<div class='collapsed'>";
    PixieConfHTML += "<label for='set-time'>Set Time ";
    PixieConfHTML += "<input style=width:auto name='set-time' id='set-time' type='datetime-local' value='"+systimeStr+"'>";
    PixieConfHTML += "</div>";


    PixieConfHTML += "<div class='collapsable'>";

    PixieConfHTML += "<h2>NTP Client Setup</h2>";

    PixieConfHTML += "<label for='enable-dst'>Auto-adjust clock for DST</label> ";
    PixieConfHTML += " <input value='1' type=checkbox name='enable-dst' id='enable-dst'" + String(DSTEnabled?"checked":"") + ">";
    PixieConfHTML += "<br>";

    PixieConfHTML += "<label for='ntp-server'>Server:</label>";
    //PixieConfHTML += "<input list='ntp-server-list' id='ntp-server' name='ntp-server' placeholder='pool.ntp.org'";
    //PixieConfHTML += " value='"+ NTP::server() +"'>";
    PixieConfHTML += "<select id='ntp-server-list' name='ntp-server'>";
    size_t servers_count = sizeof( NTP::Servers ) / sizeof( NTP::Server );
    uint8_t server_id = NTP::getServerId();
    for( int i=0; i<servers_count; i++ ) {
      PixieConfHTML += "<option value='"+ String(i) +"'" + String(i==server_id ? "selected":"") + ">"+ String(NTP::Servers[i].name) +"("+ String(NTP::Servers[i].addr) +")</option>";
    }
    PixieConfHTML += "</select><br>";

    PixieConfHTML += "<label for='ntp-server-interval'>Sync interval:</label>";
    PixieConfHTML += "<select id='ntp-server-interval' name='ntp-server-interval'>";
    PixieConfHTML += "<option value=60>Hourly</option>";
    PixieConfHTML += "<option value=14400>Daily</option>";
    PixieConfHTML += "<option value=10080>Weekly</option>";
    PixieConfHTML += "</select><br>";

    PixieConfHTML += "</div>";

    PixieConfHTML += "<button type=submit>Submit</button>";
    PixieConfHTML += "</form>";

    PixieConfHTML += getTemplate(HTML_END);

    _wifiManager->server->send(200, "text/html", PixieConfHTML.c_str() );
  }


  void handleValues()
  {
    bool success = true;
    bool _NTPEnabled = NTPEnabled;
    Serial.println("[HTTP] handle route Values");
    if( _wifiManager->server->hasArg("use-ntp-server") ) {
      String UseNtpServer = _wifiManager->server->arg("use-ntp-server");
      log_d("UseNtpServer: %s", UseNtpServer.c_str() );
      _NTPEnabled = UseNtpServer=="1";
      NTPEnabled = _NTPEnabled;
    } else {
      NTPEnabled = false;
    }
    if( _NTPEnabled != NTPEnabled ) {
      prefs::setBool("NTPEnabled", !NTPEnabled );
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
      String tz = TZ::getTzByLocation( String(TZ::tzName) );
      TZ::configTimeWithTz( tz, NTP::server() );
      //printLocalTime(1, 1000);
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
        NTP::setSyncDelay( serverInterval );
      }
    }

    const char* successResp = "<script>parent.location.href = '/';</script>";
    const char* failureResp = "<script>parent.alert('fail');</script>";

    _wifiManager->server->send(200, "text/html", success?successResp:failureResp);
  }


  void bindServerCallback()
  {
    _wifiManager->server->on("/custom", handleRoute);
    _wifiManager->server->on("/save-tz", handleValues);
    _wifiManager->server->on("/favicon.ico", handleFavicon);
  }

};
