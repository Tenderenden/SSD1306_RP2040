// board documentation: https://www.waveshare.com/wiki/RP2040-Zero
#include <hardware/gpio.h>
#include <hardware/uart.h>
#include <pico/stdio.h>
#include <stdio.h>
#include "ssd1306.h"

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
                //set pixel cmd "p x y"
                sscanf(input_buffer+2, "%d %d %d", &x, &y, &is_on);
                printf("PXL (%d,%d) state: %d\n", x, y, is_on);
                ssd1306_set_pixel(x, y, is_on);
                break;
            case 'g':
                //get pixel value cmd "p x y"
                sscanf(input_buffer+2, "%d %d", &x, &y);
                is_on = ssd1306_get_pixel(x, y);
                printf("(%d, %d) has state:%d\n", x, y, is_on);
                break;
            case 'l':
                //draw line
                break;
            case 'c':
                //clear screen cmd
                ssd1306_fill_buff(0x00);
                break;
            default:
                printf("UNN CMD: %c\n", input_buffer[0]);
                break;
        }
    }
    index = 0;
    for(size_t i=0; i<INPUT_BUFF_LEN; i++){
        //After parsing cmd fill buffer with null terminator chars
        input_buffer[i] = '\0';
    }
}

int main() {
    int rc = pico_led_init();
    hard_assert(rc == PICO_OK);
    stdio_init_all();
    printf("Starting_program\n");

    rc = ssd1306_init();
    if (SSD1306_NO_ERR == rc) {
        printf("SSD1306 init successful\n");
    }
    else {
        printf("SSD1306 Init err: %d\n", rc);
    }
    ssd1306_draw();
    printf("Main loop start\n");
    uint8_t iterator = 0;
    bool pixel_val = false;
    while (true) {
        get_input();
        pixel_val = ~(ssd1306_get_pixel(iterator, iterator));
        ssd1306_set_pixel(iterator, iterator, pixel_val);
        ssd1306_draw();
        pico_set_led(true);
        sleep_ms(LED_DELAY_MS);
        pico_set_led(false);
        sleep_ms(LED_DELAY_MS);
        iterator++;
        iterator %= SSD1306_HEIGHT;
    }
}
