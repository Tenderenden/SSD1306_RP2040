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

void decode_pixel_cmd(char *buffer, uint8_t *x, uint8_t *y)
{
    uint8_t index=0;
    //Get x
    while(buffer[index]!= ' '){
        *x = (*x*10) + (uint8_t)(buffer[index] - '0');
        index++;
    }
    index++;
    //Get y
    while((' ' != buffer[index]) && ('\0' != buffer[index]) && ('\n' != buffer[index])){
        *y = (*y*10) + (uint8_t)(buffer[index] - '0');
        index++;
    }
}

void get_input(void)
{
    uint8_t x=0, y=0;
    static uint8_t index = 0;
    while(uart_is_readable(uart0)){
        input_buffer[index++] = uart_getc(uart0);
    }
    
    if (index != 0){
        // printf("input_buffer:__%10s__\n", input_buffer);
        switch (input_buffer[0]) {
            case 'p':
                decode_pixel_cmd(input_buffer+2, &x, &y);
                printf("pixel x:%d, y:%d\n", x, y);
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
        pico_set_led(true);
        ssd1306_set_horizontal_mode(0, SSD1306_WIDTH-1, 0, (SSD1306_HEIGHT/8)-1);
        // ssd1306_fill_buff();
        ssd1306_set_pixel(127, 0, true);
        ssd1306_draw();
        sleep_ms(LED_DELAY_MS);
        
        pico_set_led(false);
        ssd1306_set_horizontal_mode(0, SSD1306_WIDTH-1, 0, (SSD1306_HEIGHT/8)-1);
        // ssd1306_clear_buff();
        ssd1306_set_pixel(127, 0, false);
        ssd1306_draw();
        sleep_ms(LED_DELAY_MS);
        printf("Loop %d done\n", loop_counter++);
    }
}
