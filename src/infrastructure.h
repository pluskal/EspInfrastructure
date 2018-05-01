#include "ESPmDNS.h"
#include <WiFi.h>
#include <ArduinoOTA.h>

#define MAX_WLANS 10
#define WIFI_MAX_RECONNECT_TIME 6000 //v millis

typedef struct
{
    const char *ssid;
    const char *password;
} wlanNet_t;

class Infrastructure
{
  private:
    wlanNet_t *wlans;
    const char *hostname;
    const char *ota_password;

    bool for_each_wifi(bool connect, bool check_reconnect); //TODO Use callback function instead
    void initializeOTA();

  public:
    bool initialize(int boudRate);
    bool initialize(int boudRate, wlanNet_t *wlan);
    bool initialize(int boudRate, wlanNet_t *wlan, const char *hostname);
    bool initialize(int boudRate, wlanNet_t *wlan, const char *hostname, const char * ota_password);
    bool initialize(int boudRate, wlanNet_t **wlans);
    bool initialize(int boudRate, wlanNet_t **wlans, const char *hostname);
    bool initialize(int boudRate, wlanNet_t **wlans, const char *hostname, const char * ota_password);

    bool loop();

    bool register_mdns_hostname(const char *hostname);

    bool connect_wifi();
    bool connect_wifi(wlanNet_t* wlan);
    bool connect_wifi(const char *essid, const char *password);

    bool check_reconnect_Wifi();
    bool check_reconnect_Wifi(wlanNet_t* wlan);
    bool check_reconnect_Wifi(const char *ssid, const char *password);
};
