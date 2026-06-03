#include "pico/stdlib.h"
#include "ssd1306.h"
#include "ssd1306/ssd1306.h"
#include <pico/stdio.h>
#include <stdio.h>
#include <string.h>

#define LED_PIN 3

#ifndef LED_DELAY_MS
#define LED_DELAY_MS 250
#endif

ssd1306_buffer_t test_buffer;


int pico_led_init(void) {
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    return PICO_OK;
}

void pico_set_led(bool led_on) {
    // Just set the GPIO on or off
    gpio_put(LED_PIN, led_on);
}

int main() {
    int rc = pico_led_init();
    hard_assert(rc == PICO_OK);
    stdio_init_all();
    printf("Starting_program\n");

    test_buffer.length = 10;
    ssd1306_init();
    
    ssd1306_set_horizontal_mode(0, 4, 0, 2);

    while (true) {
        pico_set_led(true);
        memset(test_buffer.buffer, 0xFF, test_buffer.length);
        // ssd1306_set_page_mode(0xB0, 0x00, 0x1A);
        ssd1306_send_data(&test_buffer);
        sleep_ms(LED_DELAY_MS);
        pico_set_led(false);
        memset(test_buffer.buffer, 0x00, test_buffer.length);
        // ssd1306_set_page_mode(0xB0, 0x00, 0x1A);
        ssd1306_send_data(&test_buffer);
        sleep_ms(LED_DELAY_MS);
        printf("Loop done\n");
    }
}
