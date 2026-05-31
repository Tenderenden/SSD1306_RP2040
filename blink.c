#include "pico/stdlib.h"
#include "ssd1306.h"
#include "ssd1306/ssd1306.h"
#include <pico/stdio.h>
#include <stdio.h>

#define LED_PIN 3

#ifndef LED_DELAY_MS
#define LED_DELAY_MS 250
#endif

// Perform initialisation
int pico_led_init(void) {
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    return PICO_OK;
}

// Turn the led on or off
void pico_set_led(bool led_on) {
    // Just set the GPIO on or off
    gpio_put(LED_PIN, led_on);
}

int main() {
    int rc = pico_led_init();
    hard_assert(rc == PICO_OK);
    stdio_init_all();
    printf("Starting_program\n");
    ssd1306_init();
    ssd1306_set_horizontal_mode(0, 127, 0, 7);
    while (true) {
        ssd1306_send_cmd(SSD1306_DISPLAY_ON_FULL_CMD);
        // ssd1306_clear_buff();
        // ssd1306_draw();
        pico_set_led(true);
        sleep_ms(LED_DELAY_MS);
        ssd1306_send_cmd(SSD1306_DISPLAY_ON_RAM_CMD);
        pico_set_led(false);
        // ssd1306_fill_buff();
        // ssd1306_draw();
        sleep_ms(LED_DELAY_MS);
        printf("Loop done\n");
    }
}
