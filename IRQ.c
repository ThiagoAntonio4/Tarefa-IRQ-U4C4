// Bibliotecas que serão utilizadas
#include "pico/stdlib.h"
#include "hardware/clocks.h"
#include "hardware/pio.h"
#include "pio_matrix.pio.h"

// Definição de constantes
const uint gpio_button_a = 5;
const uint gpio_button_b = 6;
const uint gpio_leds = 7;
const uint gpio_green = 11;
const uint gpio_blue = 12;
const uint gpio_red = 13;

// Variáveis globais
uint a = 0;
PIO pio = pio0;
int num_atual = 0;
uint32_t last_time = 0;
double counter[10][25] = { // Matriz de 5x5 para representar os números de 0 a 9

    {0, 1, 1, 1, 0,
     0, 1, 0, 1, 0,
     0, 1, 0, 1, 0,
     0, 1, 0, 1, 0,
     0, 1, 1, 1, 0},

    {0, 0, 1, 0, 0,
     0, 0, 1, 0, 0,
     0, 0, 1, 0, 0,
     0, 0, 1, 0, 0,
     0, 0, 1, 0, 0},

    {0, 1, 1, 1, 0,
     0, 1, 0, 0, 0,
     0, 1, 1, 1, 0,
     0, 0, 0, 1, 0,
     0, 1, 1, 1, 0},

    {0, 1, 1, 1, 0,
     0, 0, 0, 1, 0,
     0, 1, 1, 1, 0,
     0, 0, 0, 1, 0,
     0, 1, 1, 1, 0},

    {0, 1, 0, 0, 0,
     0, 0, 0, 1, 0,
     0, 1, 1, 1, 0,
     0, 1, 0, 1, 0,
     0, 1, 0, 1, 0},

    {0, 1, 1, 1, 0,
     0, 0, 0, 1, 0,
     0, 1, 1, 1, 0,
     0, 1, 0, 0, 0,
     0, 1, 1, 1, 0},

    {0, 1, 1, 1, 0,
     0, 1, 0, 1, 0,
     0, 1, 1, 1, 0,
     0, 1, 0, 0, 0,
     0, 1, 1, 1, 0},

    {0, 1, 0, 0, 0,
     0, 0, 0, 1, 0,
     0, 1, 0, 1, 0,
     0, 1, 0, 1, 0,
     0, 1, 1, 1, 0},

    {0, 1, 1, 1, 0,
     0, 1, 0, 1, 0,
     0, 1, 1, 1, 0,
     0, 1, 0, 1, 0,
     0, 1, 1, 1, 0},

    {0, 1, 0, 0, 0,
     0, 0, 0, 1, 0,
     0, 1, 1, 1, 0,
     0, 1, 0, 1, 0,
     0, 1, 1, 1, 0},

};

// Função para inicializar os pinos
void init_gpios(){
    gpio_init(gpio_button_a);
    gpio_set_dir(gpio_button_a, GPIO_IN);
    gpio_pull_up(gpio_button_a);

    gpio_init(gpio_button_b);
    gpio_set_dir(gpio_button_b, GPIO_IN);
    gpio_pull_up(gpio_button_b);

    gpio_init(gpio_leds);
    gpio_set_dir(gpio_leds, GPIO_OUT);

    gpio_init(gpio_green);
    gpio_set_dir(gpio_green, GPIO_OUT);

    gpio_init(gpio_blue);
    gpio_set_dir(gpio_blue, GPIO_OUT);

    gpio_init(gpio_red);
    gpio_set_dir(gpio_red, GPIO_OUT);
}

// Função para converter valores RGB para um valor de 32 bits
uint32_t rgb_color(double r, double g, double b){
    unsigned char R = r * 255;
    unsigned char G = g * 255;
    unsigned char B = b * 255;
    return (G << 24) | (R << 16) | (B << 8);
}

// Função para tratar a interrupção dos botões
void button_irq_handler(uint gpio, uint32_t events){
    // Implementação de um debounce de 200ms por software 
    uint32_t current_time = to_us_since_boot(get_absolute_time());
    if ((current_time - last_time) > 200000)
    {
        last_time = current_time;

        // Configuração do pio para controle dos leds
        uint offset = pio_add_program(pio, &pio_matrix_program);
        uint sm = pio_claim_unused_sm(pio, true);
        pio_matrix_program_init(pio, sm, offset, gpio_leds);

        if (gpio == gpio_button_a && num_atual < 9) // Se o botão 'a' for pressionado e o número atual for menor que 9
        {
            num_atual++; // Incrementa o número atual
        }
        else if (gpio == gpio_button_b && num_atual > 0) // Se o botão 'b' for pressionado e o número atual for maior que 0
        {
            num_atual--; // Decrementa o número atual
        }

        // Laço que acende os leds de acordo com o número atual
        for (int j = 0; j < 25; j++)
        {
            if (counter[num_atual][j] == 1) // Se o número atual for 1, acende o led
            {
                if (gpio == gpio_button_a) // Se o botão 'a' foi pressionado
                {
                    uint32_t color = rgb_color(0, 255, 0); // Cor verde
                    pio_sm_put_blocking(pio, sm, color); // Acende o led

                }
                else if (gpio == gpio_button_b) // Se o botão 'b' foi pressionado
                {
                    uint32_t color = rgb_color(255, 0, 0); // Cor vermelha
                    pio_sm_put_blocking(pio, sm, color); // Acende o led

                }
            }
            else // Se o número atual for 0, apaga o led
            {
                uint32_t color = rgb_color(0, 0, 0); // Cor preta
                pio_sm_put_blocking(pio, sm, color); // Apaga o led
            }
        }
        pio_remove_program_and_unclaim_sm(&pio_matrix_program, pio, sm, offset); // Desaloca o pio, para evitar vazamento de memória
    }
}

// Função para configurar a interrupção dos botões
void set_irq(){
    // Ativa interrupção para o botão 'a' com uma rotina
    gpio_set_irq_enabled_with_callback(gpio_button_a, GPIO_IRQ_EDGE_FALL, true, &button_irq_handler);

    // Ativa interrupção para o botão 'b' com uma rotina
    gpio_set_irq_enabled_with_callback(gpio_button_b, GPIO_IRQ_EDGE_FALL, true, &button_irq_handler);
}

// Função principal
int main(){
    init_gpios(); // Inicializa os pinos
    set_irq(); // Configura a interrupção dos botões

    uint offset = pio_add_program(pio, &pio_matrix_program);
    uint sm = pio_claim_unused_sm(pio, true);
    pio_matrix_program_init(pio, sm, offset, gpio_leds);

    for (int j = 0; j < 25; j++)
        {
            if (counter[num_atual][j] == 1) // Se o número atual for 1, acende o led
            {
                uint32_t color = rgb_color(0, 0, 255); // Cor vermelha
                pio_sm_put_blocking(pio, sm, color); // Acende o led
            }
            else // Se o número atual for 0, apaga o led
            {
                uint32_t color = rgb_color(0, 0, 0); // Cor preta
                pio_sm_put_blocking(pio, sm, color); // Apaga o led
            }
        }

    // Loop infinito para piscar o led vermelho, 5x por segundo
    while (true)
    {
        gpio_put(gpio_red, 0);
        sleep_ms(100);
        gpio_put(gpio_red, 1);
        sleep_ms(100);
    }
}