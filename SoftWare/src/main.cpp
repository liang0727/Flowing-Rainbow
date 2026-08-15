#include <Arduino.h>
#include "wifi.h"
#include "http.h"
#include "led.h"
#include "lowpower.h"


void setup() 
{
    Serial.begin(115200);
    Serial.println();
    Serial.println("system starting...");

    wifi_init();
    http_server_init();
    http_lowpower_on();
    led_init();

    // lowpower_init();
}

void loop() 
{
    if(lowpower_loop())
    {
        
    }
    http_server_loop();
    led_loop();
    
}

