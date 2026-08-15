#include "wifi.h"
#include <ESP8266WiFi.h>
#include <Arduino.h>





void wifi_init(void)
{
    WiFi.mode(WIFI_AP);
    bool result = WiFi.softAP(WIFI_SSID, NULL, 1, false, 1);
    if(result == true)
    {
        Serial.println("WiFi AP mode started successfully");
    }
    else
    {
        Serial.println("WiFi AP mode failed to start");
    }
}

void wifi_lowpower_on(void)
{
    wifi_init(); 
}

void wifi_lowpower_off(void)
{
    WiFi.mode(WIFI_OFF);   
}

uint8_t wifi_connection_get(void)
{
    WiFiMode_t currentMode = WiFi.getMode();
    if(currentMode == WIFI_AP)
    {
        return WiFi.softAPgetStationNum();
    }
    return 0;
}

