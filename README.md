# Little-Ohm

![Arduino Nano](https://img.shields.io/badge/Arduino_Nano-00979D?style=for-the-badge&logo=arduino&logoColor=white)
![C++](https://img.shields.io/badge/C%2B%2B-00599C?style=for-the-badge&logo=c%2B%2B&logoColor=white)
![Status](https://img.shields.io/badge/Status-Em_Desenvolvimento-green?style=for-the-badge)
![USF](https://img.shields.io/badge/USF-Materiais_Elétricos-blue?style=for-the-badge)

Carrinho com Arduino Nano e sensor ultrassônico para exploração de ambiente, desenvolvido para estudar componentes eletrônicos e suas aplicações. Projeto em equipe realizado no semestre 2026.2 para a disciplina de Materiais Elétricos - Universidade São Francisco (USF).

---
## 🤖 Sobre o Projeto

O **Little-Ohm** é um robô móvel autônomo projetado para desviar de obstáculos e explorar o ambiente. A proposta central do projeto é aplicar de forma prática os conceitos de eletrônica digital e integração de hardware vistos em sala de aula, unindo microcontroladores, sensores, atuadores e componentes eletrônicos básicos (como transistores e LEDs) em um sistema funcional.

### O que o carrinho faz?

* **Exploração Autônoma:** Movimenta-se pelo ambiente de forma independente utilizando motores DC controlados por uma Ponte H (L298N).
* **Visão e Decisão de Rota:** Utiliza um sensor ultrassônico (HC-SR04) acoplado a um servo motor. Ao se deparar com um obstáculo, o robô para, o servo "olha" para a esquerda e para a direita, mede as distâncias e o robô decide virar para o lado que tiver o caminho mais livre.
* **Feedback Visual de Distância (LED RGB):** O carrinho possui um LED RGB que muda de cor dinamicamente para indicar o que ele está "enxergando" à frente:
  * 🟢 **Verde:** Caminho livre.
  * 🟡 **Amarelo:** Obstáculo se aproximando.
  * 🔴 **Vermelho:** Obstáculo muito próximo (momento de desviar).
* **Sistema Anti-Travamento (Modo Emergência):** Caso o robô identifique que está preso (por exemplo, enroscado debaixo de um móvel ou tentando desviar várias vezes sem sair do lugar), um circuito com transistor é acionado para piscar luzes de emergência pedindo "ajuda", seguido de uma manobra de fuga (marcha ré e giro aleatório).

## 🧪 Pasta `teste` (Códigos Modulares)

Para facilitar a identificação de erros de fiação ou problemas de hardware, disponibilizamos uma pasta `teste` contendo códigos isolados para validar cada componente eletrônico do carrinho individualmente antes de executar o código principal.

### Como executar e o que observar em cada teste:

#### 1. Teste Básico (Placa e Comunicação Serial)
* **Como testar:** Faça o upload do código para o Arduino e abra o Monitor Serial na IDE (baud rate: `9600`).
* **O que observar:** O LED interno da placa (ligado ao pino 13) deve piscar a cada 1 segundo. No Monitor Serial, você deve ver as mensagens de "LED LIGADO" e "LED DESLIGADO" aparecendo em sincronia.

#### 2. Teste do LED RGB e Transistor (LEDs de Emergência)
* **Como testar:** Faça o upload do código e observe diretamente os LEDs conectados no carrinho.
* **O que observar:** O LED RGB deve acender alternando as cores de forma nítida (Vermelho -> Verde -> Azul). Em seguida, o RGB apaga e o circuito do transistor é acionado, fazendo os LEDs de emergência piscarem 5 vezes seguidas.

#### 3. Teste do Servo e Sensor Ultrassônico
* **Como testar:** Faça o upload, abra o Monitor Serial (`9600`) e interaja com o robô aproximando e afastando a mão do sensor.
* **O que observar:** O Monitor Serial exibirá a medição da distância em centímetros em tempo real. Fisicamente, se você colocar a mão a menos de 20cm do sensor, o servo motor girará para o lado (como se estivesse "desviando"). Ao tirar a mão, ele volta imediatamente para o centro (90 graus).

#### 4. Teste dos Motores e Ponte H (L298N)
* **Como testar:** **Atenção:** Antes de ligar, apoie o chassi do carrinho em cima de um bloco/caixa para que as rodas fiquem suspensas no ar e não fujam pela mesa. Faça o upload.
* **O que observar:** O teste verifica a alimentação das pilhas e as ligações da Ponte H. As rodas devem girar na direção "Frente" na velocidade máxima por 2 segundos, parar, girar para "Trás" por 2 segundos, e parar por 3 segundos antes de repetir o ciclo.