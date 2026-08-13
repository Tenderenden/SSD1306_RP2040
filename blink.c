// board documentation: https://www.waveshare.com/wiki/RP2040-Zero
#include "pico/stdlib.h"
#include "ssd1306.h"
#include "ssd1306/ssd1306.h"
#include <hardware/gpio.h>
#include <hardware/uart.h>
#include <pico/stdio.h>
#include <pico/types.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <wchar.h>

#define LED_PIN 3

#ifndef LED_DELAY_MS
#define LED_DELAY_MS 250
#endif

#define INPUT_BUFF_LEN 10
char input_buffer[INPUT_BUFF_LEN];

int pico_led_init(void) {
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    return PICO_OK;
}

void pico_set_led(bool led_on) {
    // Just set the GPIO on or off
    gpio_put(LED_PIN, led_on);
}

void get_input(void)
{
    int x=0, y=0, is_on=0;
    static uint8_t index = 0;
    while(uart_is_readable(uart0)){
        input_buffer[index++] = uart_getc(uart0);
    }
    
    if (index != 0){
        // printf("input_buffer:__%10s__\n", input_buffer);
        switch (input_buffer[0]) {
            case 'p':
                sscanf(input_buffer+2, "%d %d %d", &x, &y, &is_on);
                printf("PXL (%d,%d) state: %d\n", x, y, is_on);
                ssd1306_set_pixel(x, y, is_on);
                break;
            default:
                printf("UNN CMD: %c\n", input_buffer[0]);
                break;
        }
    }
    index = 0;
    for(size_t i=0; i<INPUT_BUFF_LEN; i++){
        input_buffer[i] = '\0';
    }
}

int main() {
    int rc = pico_led_init();
    int loop_counter = 0;
    hard_assert(rc == PICO_OK);
    stdio_init_all();
    printf("Starting_program\n");

    ssd1306_init();
    while (true) {
        get_input();
        ssd1306_draw();
        pico_set_led(true);
        sleep_ms(LED_DELAY_MS);
        pico_set_led(false);
        sleep_ms(LED_DELAY_MS);
        // printf("Loop %d done\n", loop_counter++);
    }
}
