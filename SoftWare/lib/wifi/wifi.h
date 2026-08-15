#ifndef __WIFI_H__
#define __WIFI_H__

#include "Arduino.h"

#ifdef __cplusplus
extern "C" {

#define WIFI_SSID "Flowing Rainbow"



void wifi_init(void);
void wifi_lowpower_on(void);
void wifi_lowpower_off(void);
uint8_t wifi_connection_get(void);

}
#endif /* __cplusplus */

#endif /* __WIFI_H__ */
