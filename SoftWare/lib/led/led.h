#ifndef __LED_H__
#define __LED_H__

#include <Arduino.h>
#include <esp8266_peri.h>

#ifdef __cplusplus
extern "C" {

#define led_status_t uint32_t

#define GRADUAL_MODE (0x00000001 << 31)
#define FLICKER_MODE (0x00000001 << 30)

#define FLICKER_INTERVAL_MS 100
#define GRADUAL_RUNTIME 3060

#define LED_R_PIN 14
#define LED_G_PIN 12
#define LED_B_PIN 13 


void led_init(void);
void led_set_color(uint32_t color);
void led_gradual_Mode(void);
void led_flicker_Mode(void);
void led_off(void);
void led_loop(void);


}
#endif /* __cplusplus */

#endif /* __LED_H__ */
