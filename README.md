# Interfaces_RP2040
https://youtube.com/shorts/OibeTe2Twck
# Enunciado
Desenvolver um sistema embarcado utilizando o Raspberry Pi Pico W para interagir com um display OLED SSD1306 e LEDs WS2812, permitindo a exibição de caracteres digitados via comunicação serial e o acionamento de LEDs RGB através de botões físicos.

# Objetivos
Implementar a comunicação com o display OLED SSD1306 via I2C.
Exibir caracteres recebidos via USB Serial no display OLED.
Controlar LEDs WS2812 para representar números digitados.
Acionar LEDs individuais com botões físicos, alterando seu estado ao pressioná-los.

# Descrição do Projeto
O projeto utiliza um Raspberry Pi Pico W para interagir com um display OLED SSD1306, LEDs WS2812 e botões físicos. O sistema permite que caracteres digitados no Serial Monitor sejam exibidos no display OLED e, caso sejam números de 0 a 9, aciona LEDs WS2812 com padrões específicos. Além disso, dois botões físicos permitem alternar o estado de LEDs RGB individuais.

# Funcionalidade do Projeto
Comunicação Serial com OLED

O código inicializa a comunicação USB Serial e aguarda caracteres digitados.
Quando um caractere é recebido, ele é exibido no display OLED SSD1306.
Se o caractere for um número entre 0 e 9, o padrão correspondente é exibido nos LEDs WS2812.
Controle de LEDs WS2812

LEDs endereçáveis WS2812 são configurados para exibir números utilizando padrões predefinidos.
A cor dos LEDs pode ser ajustada via código.
Controle de LEDs RGB via Botões

Dois botões físicos controlam LEDs RGB individuais.
O botão A alterna o estado do LED verde, enquanto o botão B alterna o estado do LED azul.
O estado dos LEDs é atualizado no display OLED.
Tratamento de Interrupções

As interrupções dos botões são tratadas para evitar acionamentos múltiplos (debounce).
Cada botão, ao ser pressionado, altera o estado do LED correspondente e exibe a mudança no display OLED.
