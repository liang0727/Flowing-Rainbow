#include "lowpower.h"
#include "Ticker.h"
#include "wifi.h"
#include "http.h"

Ticker lowpower_ticker;

void lowpower_handler(void)
{
    static bool lowpower_flag = false;

    if (lowpower_flag)
    {
        lowpower_flag = false;

        wifi_lowpower_on();
        http_lowpower_on();
    }
    else
    {
        lowpower_flag = true;

        http_lowpower_off();
        wifi_lowpower_off();
    }
}

void lowpower_init(void)
{
    lowpower_ticker.attach_ms(LOWPOWER_RUNTIME, lowpower_handler);
}

bool lowpower_loop(void)
{
    static uint8_t last_connection_count = 0;
    uint8_t current_connection_count = wifi_connection_get();
    if (last_connection_count != current_connection_count)
    {
        last_connection_count = current_connection_count;
        if (last_connection_count > 0)
        {
            lowpower_ticker.detach();
            return true;
        }
        else
        {
            lowpower_ticker.attach_ms(LOWPOWER_RUNTIME, lowpower_handler);
            return false;
        }
    }
    return last_connection_count > 0;
}
