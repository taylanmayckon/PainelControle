#include <stdio.h>
#include "pico/stdlib.h"
#include <string.h>
// Includes do FreeRTOS
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"
#include "semphr.h"
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
#define JOYSTICK_BUTTON 22
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
// Máximo de usuários
#define MAX_USERS 99

// Variáveis da PIO declaradas no escopo global
PIO pio;
uint sm;
// Booleano para o display
bool cor = true;
ssd1306_t ssd;  // Inicializa a estrutura do display GLOBALMENTE

// Variáveis do PWM (setado para freq. de 312,5 Hz)
uint wrap = 2000;
uint clkdiv = 25;

// Variável global para a quantidade de usuários
uint users_online = 0;

// Mutex
SemaphoreHandle_t xDisplayMutex;
// Semáforos
SemaphoreHandle_t xSemBinario;
SemaphoreHandle_t xSemContagem;


// FUNÇÕES AUXILIARES
// Função para configurar o PWM e iniciar com 0% de DC
void set_pwm(uint gpio, uint wrap){
    gpio_set_function(gpio, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(gpio);
    pwm_set_clkdiv(slice_num, clkdiv);
    pwm_set_wrap(slice_num, wrap);
    pwm_set_enabled(slice_num, true); 
    pwm_set_gpio_level(gpio, 0);
}

// Função que desenha o frame fixo do display OLED
void DisplayFrame(){
    char string_ocupadas[4];
    sprintf(string_ocupadas, "%d", users_online);
    char string_total[4];
    sprintf(string_total, "%d", MAX_USERS);
    char string_disponivel[4];
    sprintf(string_disponivel, "%d", MAX_USERS-users_online);

    ssd1306_fill(&ssd, false); // Limpa o display

    // Frame que será reutilizado para todos
    ssd1306_rect(&ssd, 0, 0, 128, 64, cor, !cor);
    // Mensagem superior (Nome do projeto e vagas ocupadas/totais)
    ssd1306_rect(&ssd, 0, 0, 128, 12, cor, cor); // Fundo preenchido
    ssd1306_draw_string(&ssd, "BioCarga", 4, 3, true);    
    // Deslocando o texto conforme a quantidade de vagas
    if(users_online >=10){
        ssd1306_draw_string(&ssd, string_ocupadas, 83, 3, true);
    }
    else{
        ssd1306_draw_string(&ssd, string_ocupadas, 91, 3, true);
    }
    ssd1306_draw_string(&ssd, "/", 99, 3, true);
    ssd1306_draw_string(&ssd, string_total, 107, 3, true);
    // Vagas ocupadas
    ssd1306_draw_string(&ssd, "OCUPADAS:", 4, 16, false);
    ssd1306_draw_string(&ssd, string_ocupadas, 95, 16, false);
    // Vagas disponiveis
    ssd1306_draw_string(&ssd, "DISPONIVEL:", 4, 28, false);
    ssd1306_draw_string(&ssd, string_disponivel, 95, 28, false);
    // Mensagem
    ssd1306_line(&ssd, 0, 39, 127, 39, cor); // Divisória
    // Se tiver vagas livres
    if(users_online <= MAX_USERS-2){
        ssd1306_draw_string(&ssd, "ESTACAO LIVRE", 11, 43, false);
        ssd1306_draw_string(&ssd, "CARREGUE JA!", 15, 53, false);
    }
    // Ultima vaga livre
    else if(users_online == MAX_USERS-1){
        ssd1306_draw_string(&ssd, "ULTIMA VAGA!", 15, 43, false);
        ssd1306_draw_string(&ssd, "SEJA RAPIDO! :D", 3, 53, false);
    }
    // Lotado
    else{
        ssd1306_draw_string(&ssd, "LOTADO!", 35, 43, false);
        ssd1306_draw_string(&ssd, "AGUARDE VAGAS", 11, 53, false);
    }
    
    ssd1306_send_data(&ssd); // Envia para o display
}

// Atualiza o LED RGB
void updateLEDRGB(){
    // Vazio
    if(users_online==0){
        pwm_set_gpio_level(LED_RED, 0);
        pwm_set_gpio_level(LED_GREEN, 0);
        pwm_set_gpio_level(LED_BLUE, wrap*0.05);
    }
    // 2 vagas restantes
    else if(users_online <= MAX_USERS-2){
        pwm_set_gpio_level(LED_RED, 0);
        pwm_set_gpio_level(LED_GREEN, wrap*0.05);
        pwm_set_gpio_level(LED_BLUE, 0);
    }
    // 1 vaga restante
    else if(users_online == MAX_USERS-1){
        pwm_set_gpio_level(LED_RED, wrap*0.05);
        pwm_set_gpio_level(LED_GREEN, wrap*0.05);
        pwm_set_gpio_level(LED_BLUE, 0);
    }
    // Lotado
    else{
        pwm_set_gpio_level(LED_RED, wrap*0.05);
        pwm_set_gpio_level(LED_GREEN, 0);
        pwm_set_gpio_level(LED_BLUE, 0);
    }
}

// Beep de lotação
void full_alert_beep(){
    pwm_set_gpio_level(BUZZER_A, wrap*0.05);
    pwm_set_gpio_level(BUZZER_B, wrap*0.05);
    vTaskDelay(pdMS_TO_TICKS(200));
    pwm_set_gpio_level(BUZZER_A, 0);
    pwm_set_gpio_level(BUZZER_B, 0);
}

// Beep de reset (tem que ser chamado 2x dessa forma)
void reset_alert_beep(){
    pwm_set_gpio_level(BUZZER_A, wrap*0.05);
    pwm_set_gpio_level(BUZZER_B, wrap*0.05);
    vTaskDelay(pdMS_TO_TICKS(50));
    pwm_set_gpio_level(BUZZER_A, 0);
    pwm_set_gpio_level(BUZZER_B, 0);
    vTaskDelay(pdMS_TO_TICKS(50));
}

// ISR dos Botões =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Tratamento de interrupções (Só para o Joystick)
uint32_t last_isr_time = 0;
void gpio_irq_handler(uint gpio, uint32_t events){
    uint32_t current_isr_time = to_us_since_boot(get_absolute_time());
    if(current_isr_time-last_isr_time > 200000){ // Debounce
        last_isr_time = current_isr_time;
        
        // Liberando o Semaforo Binario
        BaseType_t xHigherPriorityTaskWoken = pdFALSE; 
        xSemaphoreGiveFromISR(xSemBinario, &xHigherPriorityTaskWoken); // Libera o Semaforo Binario para a Task
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken); // Impede que seja impedido por uma tarefa de maior prioridade
    }
}

// TASKS CRIADAS =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// Task de entrada
uint32_t last_add_time = 0;
void vEntradaTask(void *params){
    // Iniciando o Botão A para as leituras
    gpio_init(BUTTON_A);
    gpio_set_dir(BUTTON_A, GPIO_IN);
    gpio_pull_up(BUTTON_A);

    bool last_button_state = false; // Ultimo estado do botao

    while(true){
        uint32_t current_time = to_us_since_boot(get_absolute_time()); // Tempo atual
        bool current_button_state = gpio_get(BUTTON_A); // Estado atual do botão
        if(!current_button_state && last_button_state && (current_time - last_add_time) > 200000){ // Pegando a borda de descida com debounce de 200ms
            last_add_time = current_time; // Atualiza o ultimo tempo
            
            // Aguarda o semáforo de contagem
            if(xSemaphoreTake(xSemContagem, portMAX_DELAY) == pdTRUE){
                if(users_online == MAX_USERS){
                    users_online=MAX_USERS; // Limita a quantidade máxima de usuários
                    full_alert_beep(); // Beep longo para indicar que está lotado
                }
                else{
                    users_online++; // Incrementa 1
                }
                
                updateLEDRGB(); // Atualiza o LED RGB

                // Proteção do Mutex para o display
                if(xSemaphoreTake(xDisplayMutex, portMAX_DELAY) == pdTRUE){
                    DisplayFrame(); // Desenha o display
                    xSemaphoreGive(xDisplayMutex); // Libera o Mutex
                }
                xSemaphoreGive(xSemContagem); // Libera o Semaforo de Contagem
            }
        }

        last_button_state = current_button_state; // Atualiza o ultimo estado do botão 
        vTaskDelay(pdMS_TO_TICKS(50)); // Reduz o consumo da CPU da task
    }
}

// Task de saída
uint32_t last_sub_time = 0;
void vSaidaTask(void *params){
    // Iniciando o Botão B para as leituras
    gpio_init(BUTTON_B);
    gpio_set_dir(BUTTON_B, GPIO_IN);
    gpio_pull_up(BUTTON_B);

    bool last_button_state = false; // Ultimo estado do botao

    while(true){
        uint32_t current_time = to_us_since_boot(get_absolute_time()); // Tempo atual
        bool current_button_state = gpio_get(BUTTON_B); // Estado atual do botão
        if(!current_button_state && last_button_state && (current_time - last_sub_time) > 200000){ // Pegando a borda de descida com debounce de 200ms
            last_sub_time = current_time; // Atualiza o ultimo tempo
            
            // Aguarda o semáforo de contagem
            if(xSemaphoreTake(xSemContagem, portMAX_DELAY) == pdTRUE){
                if(users_online==0){
                    users_online=0;
                }
                else{
                    users_online--; // Decrementa 1
                }

                updateLEDRGB(); // Atualiza o LED RGB

                // Proteção do Mutex para o display
                if(xSemaphoreTake(xDisplayMutex, portMAX_DELAY) == pdTRUE){
                    DisplayFrame(); // Desenha o display
                    xSemaphoreGive(xDisplayMutex); // Libera o Mutex
                }

                xSemaphoreGive(xSemContagem); // Libera o Semaforo de Contagem
            }
        }

        last_button_state = current_button_state; // Atualiza o ultimo estado do botão 
        vTaskDelay(pdMS_TO_TICKS(50)); // Reduz o consumo da CPU da task
    }
}

// Task de reset
void vResetTask(void *params){
    while(true){
        // Bloqueado até existir a interrupção
        if(xSemaphoreTake(xSemBinario, portMAX_DELAY) == pdTRUE){
            users_online = 0; // Reseta os usuários para 0

            updateLEDRGB(); // Atualiza o LED RGB
            reset_alert_beep(); reset_alert_beep(); // Beep duplo do reset

            // Proteção do Mutex para o display
            if(xSemaphoreTake(xDisplayMutex, portMAX_DELAY) == pdTRUE){
                DisplayFrame(); // Desenha o display
                xSemaphoreGive(xDisplayMutex); // Libera o Mutex
            }
        }

        vTaskDelay(pdMS_TO_TICKS(50)); // Reduz o consumo da CPU da task
    }
}

// Task para controlar a matriz de leds
void vLedMatrixTask(void *params){
    // Inicializando a PIO
    pio = pio0;
    sm = 0;
    uint offset = pio_add_program(pio, &ws2812_program);
    ws2812_program_init(pio, sm, offset, LED_MATRIX_PIN, 800000, IS_RGBW);

    // Cor vermelha
    Led_color green = {0, 255, 0};
    Led_color vazio = {0, 0, 0};

    // Matrizes que receberão valores
    Led_frame matriz_unidade;
    Led_frame matriz_dezena;
    Led_frame empty = {
        vazio, vazio, vazio, vazio, vazio,
        vazio, vazio, vazio, vazio, vazio,
        vazio, vazio, vazio, vazio, vazio,
        vazio, vazio, vazio, vazio, vazio,
        vazio, vazio, vazio, vazio, vazio,
    };

    while(true){
        int unidade = users_online%10;
        int dezena = users_online/10;

        // Pegando os valores para 
        update_frame_num(&matriz_unidade, unidade, green);
        update_frame_num(&matriz_dezena, dezena, green);

        // Exibindo a quantidade de vagas na matriz de leds
        matrix_update_leds(&matriz_dezena, 0.01);
        vTaskDelay(pdMS_TO_TICKS(1000)); 
        matrix_update_leds(&matriz_unidade, 0.01);
        vTaskDelay(pdMS_TO_TICKS(1000)); 
        matrix_update_leds(&empty, 0.01);
        vTaskDelay(pdMS_TO_TICKS(1000)); 
    }
}


// FUNÇÃO MAIN =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
int main(){
    stdio_init_all();

    // Iniciando o LED RGB
    set_pwm(LED_RED, wrap);
    set_pwm(LED_GREEN, wrap);
    set_pwm(LED_BLUE, wrap);
    updateLEDRGB(); // Atualiza o LED RGB

    // Iniciando os buzzers
    set_pwm(BUZZER_A, wrap);
    set_pwm(BUZZER_B, wrap);

    // Iniciando o Botão B para as leituras
    gpio_init(JOYSTICK_BUTTON);
    gpio_set_dir(JOYSTICK_BUTTON, GPIO_IN);
    gpio_pull_up(JOYSTICK_BUTTON);
    // Configurando a interrupção do JOYSTICK_BUTTON
    gpio_set_irq_enabled_with_callback(JOYSTICK_BUTTON, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);

    // Configurando a I2C
    i2c_init(I2C_PORT, 400 * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);                    // Set the GPIO pin function to I2C
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);                    // Set the GPIO pin function to I2C
    gpio_pull_up(I2C_SDA);                                        // Pull up the data line
    gpio_pull_up(I2C_SCL);                                        // Pull up the clock line
    ssd1306_init(&ssd, WIDTH, HEIGHT, false, endereco, I2C_PORT); // Inicializa o display
    ssd1306_config(&ssd);                                         // Configura o display
    ssd1306_send_data(&ssd);                                      // Envia os dados para o display
    // Limpa o display. O display inicia com todos os pixels apagados.
    ssd1306_fill(&ssd, false);
    ssd1306_send_data(&ssd);

    DisplayFrame(users_online); // Desenha o display

    // Criando os semaforos
    xDisplayMutex = xSemaphoreCreateMutex();
    xSemContagem = xSemaphoreCreateCounting(MAX_USERS, 1); // Fila de tamanho MAX_USERS, 1 vaga para as tarefas
    xSemBinario = xSemaphoreCreateBinary();

    
    // Iniciando as Tasks
    xTaskCreate(vLedMatrixTask, "Matriz de LEDs", configMINIMAL_STACK_SIZE + 128, NULL, 1, NULL); // Prioridade baixa
    xTaskCreate(vEntradaTask, "Task de Entrada", configMINIMAL_STACK_SIZE + 128, NULL, 2, NULL); // Prioridade média
    xTaskCreate(vSaidaTask, "Task de Saida", configMINIMAL_STACK_SIZE + 128, NULL, 2, NULL); // Prioridade média
    xTaskCreate(vResetTask, "Task de Reset", configMINIMAL_STACK_SIZE + 128, NULL, 4, NULL); // Maior prioridade

    
    vTaskStartScheduler();
    panic_unsupported();
}
