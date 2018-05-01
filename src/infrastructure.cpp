#include "infrastructure.h"

bool Infrastructure::initialize(int boudRate)
{
    Serial.begin(boudRate);
    return true;
}

bool Infrastructure::initialize(int boudRate, wlanNet_t *wlan)
{
    wlanNet_t *wlans = new wlanNet_t[2]{*wlan, {NULL}};
    return initialize(boudRate, &wlans);
}

bool Infrastructure::initialize(int boudRate, wlanNet_t *wlan, const char *hostname)
{
    if (!initialize(boudRate, wlan))
        return false;
    if (!register_mdns_hostname(hostname))
        return false;
    return true;
}

bool Infrastructure::initialize(int boudRate, wlanNet_t *wlan, const char *hostname, const char *ota_password_param)
{
    ota_password = ota_password_param;
    return initialize(boudRate, wlan, hostname);
}

bool Infrastructure::initialize(int boudRate, wlanNet_t **wlans_param)
{
    if (!initialize(boudRate))
        return false;
    wlans = *wlans_param;
    if (!connect_wifi())
        return false;

    initializeOTA();
    return true;
}

bool Infrastructure::initialize(int boudRate, wlanNet_t **wlans, const char *hostname)
{
    if (!initialize(boudRate, wlans))
        return false;
    if (!register_mdns_hostname(hostname))
        return false;
    return true;
}

bool Infrastructure::initialize(int boudRate, wlanNet_t **wlans, const char *hostname, const char *ota_password_param)
{
    ota_password = ota_password_param;
    return initialize(boudRate, wlans, hostname);
}

bool Infrastructure::loop()
{
    check_reconnect_Wifi();
    ArduinoOTA.handle();
}

bool Infrastructure::register_mdns_hostname(const char *hostname)
{
    hostname = hostname;
    if (MDNS.begin(hostname))
    {
        Serial.print("mDNS hostname registered sucessfuly ->");
        Serial.println(hostname);
        return true;
    }
    else
    {
        Serial.print("mDNS hostname registration failed.");
        return false;
    }
}

bool Infrastructure::connect_wifi()
{
    return for_each_wifi(true, false);
}

bool Infrastructure::connect_wifi(wlanNet_t *wlan)
{
    return connect_wifi(wlan->ssid, wlan->password);
}

bool Infrastructure::connect_wifi(const char *ssid, const char *password)
{
    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);
    WiFi.mode(WIFI_STA);

    Serial.println("Establishing connection to WiFi: ");
    Serial.println(ssid);
    Serial.println(password);

    ulong wifi_reconnect_start = millis();

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED && millis() - wifi_reconnect_start < WIFI_MAX_RECONNECT_TIME)
    {
        delay(1000);
        Serial.print(".");
    }

    if (WiFi.status() == WL_CONNECTED)
    {
        Serial.print("Connected to network: ");
        Serial.println(ssid);
        Serial.println(WiFi.macAddress());
        Serial.println(WiFi.localIP());
        return true;
    }
    else
    {
        Serial.print("Connection to network: ");
        Serial.print(ssid);
        Serial.println("failed.");
        return false;
    }
}

bool Infrastructure::check_reconnect_Wifi()
{
    return for_each_wifi(false, true);
}

bool Infrastructure::check_reconnect_Wifi(wlanNet_t *wlan)
{
    return check_reconnect_Wifi(wlan->ssid, wlan->password);
}

bool Infrastructure::check_reconnect_Wifi(const char *ssid, const char *password)
{
    if (WiFi.status() != WL_CONNECTED)
    {
        return connect_wifi(ssid, password);
    }
    return true;
}

//###################################### Private #############################################

//TODO use callback to method instead of flags...
bool Infrastructure::for_each_wifi(bool connect, bool check_reconnect)
{
    if (wlans == NULL)
    {
        Serial.print("Wlans are not initialized. ");
    }
    for (int i = 0; i < MAX_WLANS; i++)
    {
        wlanNet_t wlan = wlans[i];
        if (wlan.ssid == NULL || wlan.password == NULL)
        {
            break;
        }
        if (connect)
        {
            if (connect_wifi(&wlan))
                return true;
        }
        if (check_reconnect)
        {
            if (check_reconnect_Wifi(&wlan))
                return true;
        }
    }
    return false;
}

void Infrastructure::initializeOTA()
{
    ArduinoOTA.setHostname(hostname);
    if (ota_password != NULL)
        ArduinoOTA.setPassword(ota_password);

    ArduinoOTA.onStart([]() {
        Serial.println("* OTA: Start");
    });
    ArduinoOTA.onEnd([]() {
        Serial.println("\n*OTA: End");
    });
    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
        Serial.printf("*OTA: Progress: %u%%\r", (progress / (total / 100)));
    });
    ArduinoOTA.onError([](ota_error_t error) {
        Serial.printf("*OTA: Error[%u]: ", error);
        if (error == OTA_AUTH_ERROR)
            Serial.println("Auth Failed");
        else if (error == OTA_BEGIN_ERROR)
            Serial.println("Begin Failed");
        else if (error == OTA_CONNECT_ERROR)
            Serial.println("Connect Failed");
        else if (error == OTA_RECEIVE_ERROR)
            Serial.println("Receive Failed");
        else if (error == OTA_END_ERROR)
            Serial.println("End Failed");
    });
    ArduinoOTA.begin();
    Serial.println("OTA Ready");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
}