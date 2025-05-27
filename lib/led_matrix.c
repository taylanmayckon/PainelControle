#include "led_matrix.h"

#define MATRIX_PIN 7
// Quantidade de pixels
#define NUM_PIXELS 25

// Buffer que armazena o frame atual, incluindo cores
Led_frame led_buffer= {{
    {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, 
    {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, 
    {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, 
    {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, 
    {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, 
}};

// FRAMES UTILIZADOS
// Num 0
bool frame_0[NUM_PIXELS] = {
    1, 1, 1, 1, 1,
    1, 0, 0, 0, 1,
    1, 0, 0, 0, 1,
    1, 0, 0, 0, 1,
    1, 1, 1, 1, 1,
};
// Num 1
bool frame_1[NUM_PIXELS] = {
    0, 0, 1, 0, 0,
    0, 0, 1, 0, 0,
    0, 0, 1, 0, 0,
    0, 0, 1, 0, 0,
    0, 1, 1, 1, 0,
};
// Num 2
bool frame_2[NUM_PIXELS] = {
    1, 1, 1, 1, 1,
    0, 0, 0, 0, 1,
    1, 1, 1, 1, 1,
    1, 0, 0, 0, 0,
    1, 1, 1, 1, 1,
};
// Num 3
bool frame_3[NUM_PIXELS] = {
    1, 1, 1, 1, 1,
    0, 0, 0, 0, 1,
    1, 1, 1, 1, 1,
    0, 0, 0, 0, 1,
    1, 1, 1, 1, 1,
};
// Num 4
bool frame_4[NUM_PIXELS] = {
    1, 0, 0, 0, 1,
    1, 0, 0, 0, 1,
    1, 1, 1, 1, 1,
    0, 0, 0, 0, 1,
    0, 0, 0, 0, 1,
};
// Num 5
bool frame_5[NUM_PIXELS] = {
    1, 1, 1, 1, 1,
    1, 0, 0, 0, 0,
    1, 1, 1, 1, 1,
    0, 0, 0, 0, 1,
    1, 1, 1, 1, 1,
};
// Num 6
bool frame_6[NUM_PIXELS] = {
    1, 1, 1, 1, 1,
    1, 0, 0, 0, 0,
    1, 1, 1, 1, 1,
    1, 0, 0, 0, 1,
    1, 1, 1, 1, 1,
};
// Num 7
bool frame_7[NUM_PIXELS] = {
    1, 1, 1, 1, 1,
    0, 0, 0, 0, 1,
    0, 0, 0, 0, 1,
    0, 0, 0, 0, 1,
    0, 0, 0, 0, 1,
};
// Num 8
bool frame_8[NUM_PIXELS] = {
    1, 1, 1, 1, 1,
    1, 0, 0, 0, 1,
    1, 1, 1, 1, 1,
    1, 0, 0, 0, 1,
    1, 1, 1, 1, 1,
};
// Num 9
bool frame_9[NUM_PIXELS] = {
    1, 1, 1, 1, 1,
    1, 0, 0, 0, 1,
    1, 1, 1, 1, 1,
    0, 0, 0, 0, 1,
    0, 0, 0, 0, 1,
};

// Vetor que armazena os ponteiros de cada frame criado anteriormente
bool *all_frames[10] = {
    frame_0,
    frame_1,
    frame_2,
    frame_3,
    frame_4,
    frame_5,
    frame_6,
    frame_7,
    frame_8,
    frame_9
};


static inline void put_pixel(uint32_t pixel_grb){
    pio_sm_put_blocking(pio0, 0, pixel_grb << 8u);
}

// Função que vai transformar valores correspondentes ao padrão RGB em dados binários
uint32_t urgb_u32(double r, double g, double b){
    return ((uint32_t)(r) << 8) | ((uint32_t)(g) << 16) | (uint32_t)(b);
}

// Função que atualiza os Leds do vetor
void set_leds(float intensidade){
    uint32_t color; // Armazena os valores das cores

    // Define todos os LEDs com a cor especificada
    // Faz o processo de virar de cabeça para baixo o arranjo
    for (int i = NUM_PIXELS-1; i >= 0; i--){
        color = urgb_u32(intensidade*led_buffer.led[i].red, intensidade*led_buffer.led[i].green, intensidade*led_buffer.led[i].blue); // Converte as cores para o padrão aceito pela matriz
        put_pixel(color); // Liga o LED com um no buffer
    }
}

// Função genérica para atualiza matriz
void matrix_update_leds(Led_frame *frame, float intensidade){
    // Ordenando corretamente o vetor recebido no buffer

    int j = 0; // Variável para controle do index espelhado
    for(int i=0; i<25; i++){
        if(i>4 && i<10){
            led_buffer.led[i] = frame->led[9-j];
            j++;
        }
        else if(i>14 && i<20){
            led_buffer.led[i] = frame->led[19-j];
            j++;
        }
        else{
            j=0;
            led_buffer.led[i] = frame->led[i];
        }
    }
    set_leds(intensidade);
}

// Atualiza o frame para matriz de leds com o numero desejado
Led_frame update_frame_num(Led_frame *frame, int num, Led_color color){
    // Resgata o frame de booleanos do vetor de ponteiros 
    bool *selected_frame = all_frames[num];
    Led_color empty = {0, 0, 0};

    for(int i=0; i<25; i++){
        if(selected_frame[i]){
            frame->led[i] = color;
        }
        else{
            frame->led[i] = empty;
        }
    }
}