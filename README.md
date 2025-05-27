# 🤖 BioCarga - BitDogLab 🤖

O BioCarga é a 4º atividade da Fase 2 do EmbarcaTech, ele consiste em um Painel de Controle Interativo com Acesso Concorrente, que gerencia os acessos à uma estação de carregamento de veículos simples elétricos, como patinetes. Ao longo do projeto, foram utilizados recursos de Mutex, Semáforo de Contagem e Semáforo Binário, todos fornecidos através do FreeRTOS.

Desenvolvido por: Taylan Mayckon

---
## 📽️ Link do Video de Demonstração:
[YouTube]()
---

## 📌 **Funcionalidades Implementadas**

- FreeRTOS para geração de diferentes Tasks: Foram geradas quatro tasks para o desenvolvimento do projeto
- Semáforos: Para evitar situações de concorrência ou similares, foram utilizados Mutex, Semáforo de Contagem e Semáforo Binário, para controlar e sincronizar as chamadas de recursos específicos do sistema.
- Botões para interação: Existem três opções de interação, disponibilizadas pelos botoes. O botão A registra um novo acesso ao sistema, o Botão B a saída de usuários e o Botão do Joystick é exclusivo para o administrador, responsável por resetar o sistema, liberando todas as vagas.
- Mensagens interativas: Há um Display OLED que exibe mensagens para os usuários, como quantidade de vagas disponíveis para carga.
- Alerta visual das vagas: No LED RGB, é possível notar quatro cores diferentes ao longo da execução. Azul indica que nenhum usuário está utilizando o sistema no momento, verde que há usuários usando, mas o sistema não está lotado, amarelo indica que há apenas uma vaga restante, e vermelho que nenhuma vaga está disponível.
- Indicativo de vagas: Periodicamente as vagas são exibidas na Matriz de LEDs disponíevl na BitDogLab, exibindo separadamente a dezena e a unidade da quantidade de vagas ocupadas.
- Alertas sonoros para deficientes visuais: Para tornar o sistema mais acessível, quando o usuário tenta acessar o sistema em lotação, são acionados buzzers que emitem um beep curto para indicar que não há vagas disponívels. Alertas sonoros também são emitidos quando o sistema é resetado, sendo um alerta de duplo beep curto.


---

## 🛠 **Recursos Utilizados**

- **FreeRTOS:** é um sistema operacional de código aberto e tempo real (RTOS) projetado para microcontroladores e dispositivos embarcados. Ele permite a criação de diferentes tarefas e faz o gerenciamento das mesmas para serem executadas de forma paralela.
- **Display OLED:** foi utilizado o ssd1306, que possui 128x64 pixels, para informações visuais dos dados lidos.
- **Matriz de LEDs Endereçáveis:** A BitDogLab possui uma matriz de 25 LEDs WS2812, que foi operada com o auxílio de uma máquina de estados para gerar o mapa interativo do sistema.
- **LED RGB:** Utilizado para sinalizar locais bloqueados e nível de bateria, através de sua intensidade.
- **Buzzers:** Emite sons para gerar alertas sonoros para as coletas e locais bloqueados.
- **Botões:** A BitDogLab possui três botões, sendo eles: O botão do Joystick, e dois botões auxilares, A e B.

---

## 📂 **Estrutura do Código**
```
📁 PainelControle/
│
├── 📁 lib/                   # Bibliotecas utilizadas no projeto
│   ├── 📄 FreeRTOSConfig.h   # Arquivos de configuração para o FreeRTOS
│   ├── 📄 font.h             # Fonte utilizada no Display I2C
│   ├── 📄 led_matrix.c       # Funções para manipulação da matriz de LEDs endereçáveis
│   ├── 📄 led_matrix.h       # Cabeçalho para o led_matrix.c
│   ├── 📄 ssd1306.c          # Funções que controlam o Display I2C
│   ├── 📄 ssd1306.h          # Cabeçalho para o ssd1306.c
│   └── 📄 ws2812.pio         # Máquina de estados para operar a matriz de LEDs endereçáveis
│
├── 📄 .gitignore             # Arquivos e diretórios a serem ignorados pelo Git.
├── 📄 CMakeLists.txt         # Configurações para compilar o código corretamente
├── 📄 PainelControle.c       # Código principal do projeto
└── 📄 README.md              # Documentação do projeto.
```

---
