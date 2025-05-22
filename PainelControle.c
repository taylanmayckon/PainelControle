#include <stdio.h>
#include "pico/stdlib.h"
// Includes do FreeRTOS
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"
// Includes para manipular a GPIO
#include "hardware/adc.h"
#include "hardware/pwm.h"
#include "hardware/i2c.h"
// Includes do Display OLED
#include "ssd1306.h"
#include "font.h"
// Includes da Matriz de LEDs
#include "led_matrix.h"

// Definições
// Botões
#define BUTTON_A 5
#define BUTTON_B 6
// Buzzers
#define BUZZER_A 21
#define BUZZER_B 10
// LED RGB
#define LED_RED 13
#define LED_GREEN 11
#define LED_BLUE 12
// Definições da I2C
#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define endereco 0x3C
// Constantes para a matriz de leds
#define IS_RGBW false
#define LED_MATRIX_PIN 7

// Variáveis da PIO declaradas no escopo global
PIO pio;
uint sm;
// Booleano para o display
bool cor = true;

// Variáveis do PWM (setado para freq. de 312,5 Hz)
uint wrap = 2000;
uint clkdiv = 25;

// TASKS CRIADAS =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-



// FUNÇÃO MAIN =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

int main(){
    stdio_init_all();
    
    // Iniciando as Tasks
    
    vTaskStartScheduler();
    panic_unsupported();
}
