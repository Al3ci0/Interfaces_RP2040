#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/pio.h"
#include "hardware/timer.h"
#include "ws2812.pio.h"
#include "inc/ssd1306.h"
#include "inc/font.h"
#include "digits.h"  // Biblioteca para os números nos LEDs WS2812

// Configuração do display OLED SSD1306
#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define ENDERECO 0x3C
#define WIDTH 128
#define HEIGHT 64

// Configuração dos LEDs WS2812
#define IS_RGBW false
#define WS2812_PIN 7

// Configuração dos botões e LEDs
#define GREEN_LED_PIN 11
#define BLUE_LED_PIN 12
#define BUTTON_A_PIN 5
#define BUTTON_B_PIN 6



static bool green_led_state = false;
static bool blue_led_state = false;

bool led_buffer[NUM_PIXELS]; // Buffer de LEDs
bool button_pressed = false; // Flag para indicar que um botão foi pressionado


ssd1306_t ssd;  // Objeto do display OLED

void gpio_irq_handler(uint gpio, uint32_t events);
void copy_array(bool *dest, const bool *src);
static inline void put_pixel(uint32_t pixel_grb);
static inline uint32_t urgb_u32(uint8_t r, uint8_t g, uint8_t b);
void set_one_led(uint8_t r, uint8_t g, uint8_t b);

int main() {
    stdio_init_all();  // Inicializa comunicação USB Serial

    // Inicialização do I2C e do display OLED
    i2c_init(I2C_PORT, 400 * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    ssd1306_init(&ssd, WIDTH, HEIGHT, false, ENDERECO, I2C_PORT);
    ssd1306_config(&ssd);
    ssd1306_fill(&ssd, false);
    ssd1306_send_data(&ssd);

    // Inicializa o PIO para os LEDs WS2812
    PIO pio = pio0;
    int sm = 0;
    uint offset = pio_add_program(pio, &ws2812_program);
    ws2812_program_init(pio, sm, offset, WS2812_PIN, 800000, IS_RGBW);

    // Configuração dos botões e LEDs RGB
    gpio_init(GREEN_LED_PIN);
    gpio_set_dir(GREEN_LED_PIN, GPIO_OUT);
    gpio_init(BLUE_LED_PIN);
    gpio_set_dir(BLUE_LED_PIN, GPIO_OUT);

    gpio_init(BUTTON_A_PIN);
    gpio_set_dir(BUTTON_A_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_A_PIN);

    gpio_init(BUTTON_B_PIN);
    gpio_set_dir(BUTTON_B_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_B_PIN);

    // Ativa interrupções nos botões
    gpio_set_irq_enabled_with_callback(BUTTON_A_PIN, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
    gpio_set_irq_enabled_with_callback(BUTTON_B_PIN, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);

    printf("Digite caracteres no Serial Monitor para exibir no OLED:\n");

    while (true) {
        // **Verifica entrada pelo Serial Monitor**
        if (stdio_usb_connected()) {
            char c;
            if (scanf("%c", &c) == 1) {
                printf("Recebido: '%c'\n", c);
                
                // Exibe caractere no OLED
                ssd1306_fill(&ssd, false);
                ssd1306_draw_char(&ssd, c, 60, 30);
                ssd1306_send_data(&ssd);

                // Se for um número entre 0 e 9, exibe no WS2812
                if (c >= '0' && c <= '9') {
                    int num = c - '0';
                    switch (num) {
                        case 0: copy_array(led_buffer, zero); break;
                        case 1: copy_array(led_buffer, one); break;
                        case 2: copy_array(led_buffer, two); break;
                        case 3: copy_array(led_buffer, three); break;
                        case 4: copy_array(led_buffer, four); break;
                        case 5: copy_array(led_buffer, five); break;
                        case 6: copy_array(led_buffer, six); break;
                        case 7: copy_array(led_buffer, seven); break;
                        case 8: copy_array(led_buffer, eight); break;
                        case 9: copy_array(led_buffer, nine); break;
                    }
                    set_one_led(255, 0, 0);
                }
            }
        }

        sleep_ms(100);
    }
}

void gpio_irq_handler(uint gpio, uint32_t events) {
    static uint32_t last_time = 0;
    uint32_t current_time = to_ms_since_boot(get_absolute_time());

    if (current_time - last_time < 300) return;  // Debounce simples
    last_time = current_time;

    if (gpio == BUTTON_A_PIN) {
        green_led_state = !green_led_state;
        gpio_put(GREEN_LED_PIN, green_led_state);
        
        printf("Botão A pressionado. LED Verde %s\n", green_led_state ? "LIGADO" : "DESLIGADO");

        ssd1306_fill(&ssd, false);
        ssd1306_draw_string(&ssd, "LED Verde:", 0, 0);
        ssd1306_draw_string(&ssd, green_led_state ? "LIGADO" : "DESLIGADO", 0, 10);
        ssd1306_send_data(&ssd);
    } 

    if (gpio == BUTTON_B_PIN) {
        blue_led_state = !blue_led_state;
        gpio_put(BLUE_LED_PIN, blue_led_state);
        
        printf("Botão B pressionado. LED Azul %s\n", blue_led_state ? "LIGADO" : "DESLIGADO");

        ssd1306_fill(&ssd, false);
        ssd1306_draw_string(&ssd, "LED Azul:", 0, 0);
        ssd1306_draw_string(&ssd, blue_led_state ? "LIGADO" : "DESLIGADO", 0, 10);
        ssd1306_send_data(&ssd);
    }
}

void copy_array(bool *dest, const bool *src) {
    for (int i = 0; i < NUM_PIXELS; i++) {
        dest[i] = src[i];
    }
}

static inline void put_pixel(uint32_t pixel_grb) {
    pio_sm_put_blocking(pio0, 0, pixel_grb << 8u);
}

static inline uint32_t urgb_u32(uint8_t r, uint8_t g, uint8_t b) {
    return ((uint32_t)(r) << 8) | ((uint32_t)(g) << 16) | (uint32_t)(b);
}

void set_one_led(uint8_t r, uint8_t g, uint8_t b) {
    uint32_t color = urgb_u32(r, g, b);
    for (int i = 0; i < NUM_PIXELS; i++) {
        put_pixel(led_buffer[i] ? color : 0);
    }
}
