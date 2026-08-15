#include "led.h"
#include <Arduino.h>
#include "Ticker.h"

static led_status_t led_status = 0x00000000;

Ticker gradual_ticker;
Ticker flicker_ticker;
static bool lastGradual = false;
static bool lastFlicker = false;

static bool flicker_flag = false;

void led_gradual_handle(void)
{
    static uint8_t r_tmp = 255;
    static uint8_t g_tmp = 0;
    static uint8_t b_tmp = 0;

    if(r_tmp == 255 && g_tmp < 255 && b_tmp == 0) 
    {
        g_tmp += 5;
    }
    else if(r_tmp > 0 && g_tmp == 255 && b_tmp == 0) 
    {
        r_tmp -= 5;
    }
    else if(r_tmp == 0 && g_tmp == 255 && b_tmp < 255) 
    {
        b_tmp += 5;
    }
    else if(r_tmp == 0 && g_tmp > 0 && b_tmp == 255) 
    {
        g_tmp -= 5;
    }
    else if(r_tmp < 255 && g_tmp == 0 && b_tmp == 255) 
    {
        r_tmp += 5;
    }
    else if(r_tmp == 255 && g_tmp == 0 && b_tmp > 0) 
    {
        b_tmp -= 5;
    }

    led_status = (led_status & 0xFF000000) | (r_tmp << 16) | (g_tmp << 8) | b_tmp;
}

void led_flicker_handle(void)
{
    flicker_flag = !flicker_flag;
    if (flicker_flag)
    {
        analogWrite(LED_R_PIN, 0);
        analogWrite(LED_G_PIN, 0);
        analogWrite(LED_B_PIN, 0);
    }
    else
    {
        analogWrite(LED_R_PIN, (led_status >> 16) & 0xFF);
        analogWrite(LED_G_PIN, (led_status >> 8) & 0xFF);
        analogWrite(LED_B_PIN, led_status & 0xFF);
    }
}

void led_init(void)
{
    pinMode(LED_R_PIN, OUTPUT);
    pinMode(LED_G_PIN, OUTPUT);
    pinMode(LED_B_PIN, OUTPUT); 
    
    analogWriteFreq(1000);
    analogWriteRange(255);

    analogWrite(LED_R_PIN, (led_status >> 16) & 0xFF);
    analogWrite(LED_G_PIN, ((led_status >> 8) & 0xFF));
    analogWrite(LED_B_PIN, led_status & 0xFF);
}

void led_set_color(uint32_t color)
{
    uint8_t r = (color >> 16) & 0xFF;
    uint8_t g = (color >> 8) & 0xFF;
    uint8_t b = color & 0xFF;

    led_status = (led_status & 0xFF000000) | (r << 16) | (g << 8) | b;

    return;
}

void led_gradual_Mode(void)
{
    led_status = led_status ^ GRADUAL_MODE;
    Serial.print("GRADUAL_MODE:");
    Serial.println(led_status ^ GRADUAL_MODE);
}

void led_flicker_Mode(void)
{
    led_status = led_status ^ FLICKER_MODE;
    Serial.print("FLICKER_MODE:");
    Serial.println(led_status ^ FLICKER_MODE);
}

void led_off(void)
{
    led_status = led_status & 0x00000000;
    Serial.print("LED_OFF");
}

void led_loop(void)
{
    bool currGradual = led_status & GRADUAL_MODE;
    bool currFlicker = led_status & FLICKER_MODE;
    
    if (currGradual != lastGradual) 
    {  
        if (currGradual) 
        {
            gradual_ticker.attach_ms(GRADUAL_RUNTIME / 306, led_gradual_handle);
        } 
        else 
        {
            gradual_ticker.detach();
        }
        lastGradual = currGradual;
    }
    
    if (currFlicker != lastFlicker) 
    {
        if (currFlicker) 
        {
            flicker_ticker.attach_ms(FLICKER_INTERVAL_MS, led_flicker_handle);
        } 
        else
        {
            flicker_ticker.detach();
        }
        lastFlicker = currFlicker;
    }

    if(!currFlicker)
    {
        analogWrite(LED_R_PIN, (led_status >> 16) & 0xFF);
        analogWrite(LED_G_PIN, (led_status >> 8) & 0xFF);
        analogWrite(LED_B_PIN, led_status & 0xFF);
    }
    
}
