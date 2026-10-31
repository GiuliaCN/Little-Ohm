// Carrinho com Arduino Nano
// - Anda e desvia de obstáculos (HC-SR04 em um servo)
// - LED RGB muda de cor conforme a distância: verde (longe) -> amarelo -> vermelho (perto)
// - Quando fica preso, um transistor aciona vários LEDs piscando
// - LED indica pilha fraca (divisor de tensão em A2)
// Biblioteca: Servo (já vem na Arduino IDE).
// Placa: Arduino Nano. Se o upload falhar, tente Processador: "ATmega328P (Old Bootloader)".

#include <Servo.h>

// ---------- Pinos ----------
const uint8_t PIN_ENA = 5;    // PWM
const uint8_t PIN_ENB = 6;    // PWM
const uint8_t PIN_IN1 = 4;
const uint8_t PIN_IN2 = 7;
const uint8_t PIN_IN3 = 8;
const uint8_t PIN_IN4 = 12;

const uint8_t PIN_SERVO = 9;
const uint8_t PIN_TRIG  = A0;
const uint8_t PIN_ECHO  = A1;

const uint8_t PIN_LED_R = 11;       // PWM
const uint8_t PIN_LED_G = 3;        // PWM
const uint8_t PIN_LED_B = 10;       // só liga/desliga (a lib Servo desativa o PWM no D10)
const uint8_t PIN_LEDS_PRESO = 2;   // base do transistor (via 1k)

const uint8_t PIN_BAT     = A2;     // meio do divisor 10k/10k
const uint8_t PIN_LED_BAT = 13;

// ---------- Ajustes ----------
const int   VELOCIDADE       = 255;   // 0 a 255 (com 4 pilhas os motores recebem só ~4V, então vai no máximo)
const int   VELOCIDADE_GIRO  = 255;
const float DIST_OBSTACULO   = 20.0;  // cm: abaixo disso, desvia
const float DIST_VERDE       = 60.0;  // cm: a partir daqui o RGB fica todo verde
const float DIST_VERMELHO    = 15.0;  // cm: daqui para baixo o RGB fica todo vermelho

const int ANGULO_ESQ    = 150;
const int ANGULO_CENTRO = 90;
const int ANGULO_DIR    = 30;
const int TEMPO_RE_MS   = 350;
const int TEMPO_GIRO_MS = 450;

const bool RGB_ANODO_COMUM = false;   // mude para true se o seu LED RGB for de ânodo comum

const float BAT_FRACA_V   = 4.6;      // 4 pilhas x ~1,15 V (a tensão cai com os motores ligados)
const float BAT_OK_V      = 5.0;      // histerese
const float FATOR_DIVISOR = 2.0;      // (10k + 10k) / 10k

// Detecção de "preso"
const int           N_DESVIOS = 4;        // 4 desvios...
const unsigned long JANELA_MS = 10000;    // ...em menos de 10 s
const unsigned long CHECK_PROGRESSO_MS = 1500;

// ---------- Estado ----------
Servo servoSensor;

bool bateriaFraca = false;
unsigned long ultimaLeituraBat = 0;
unsigned long ultimoPiscar = 0;
bool ledBatEstado = false;

unsigned long tempoDesvios[N_DESVIOS];
int idxDesvio = 0;
int totalDesvios = 0;

float ultimaDist = -1;
unsigned long ultimoCheck = 0;
int contaParado = 0;

// ---------- Motores ----------
void motorEsq(int vel, bool frente) {
  digitalWrite(PIN_IN1, frente ? HIGH : LOW);
  digitalWrite(PIN_IN2, frente ? LOW : HIGH);
  analogWrite(PIN_ENA, vel);
}

void motorDir(int vel, bool frente) {
  digitalWrite(PIN_IN3, frente ? HIGH : LOW);
  digitalWrite(PIN_IN4, frente ? LOW : HIGH);
  analogWrite(PIN_ENB, vel);
}

void parar() {
  digitalWrite(PIN_IN1, LOW); digitalWrite(PIN_IN2, LOW);
  digitalWrite(PIN_IN3, LOW); digitalWrite(PIN_IN4, LOW);
  analogWrite(PIN_ENA, 0);
  analogWrite(PIN_ENB, 0);
}

void andarFrente() { motorEsq(VELOCIDADE, true);  motorDir(VELOCIDADE, true); }
void andarRe()     { motorEsq(VELOCIDADE, false); motorDir(VELOCIDADE, false); }
void girarEsq()    { motorEsq(VELOCIDADE_GIRO, false); motorDir(VELOCIDADE_GIRO, true); }
void girarDir()    { motorEsq(VELOCIDADE_GIRO, true);  motorDir(VELOCIDADE_GIRO, false); }

// ---------- LED RGB ----------
void setRGB(uint8_t r, uint8_t g, bool b) {
  if (RGB_ANODO_COMUM) {
    r = 255 - r;
    g = 255 - g;
    b = !b;
  }
  analogWrite(PIN_LED_R, r);
  analogWrite(PIN_LED_G, g);
  digitalWrite(PIN_LED_B, b ? HIGH : LOW);
}

// Verde (longe) -> amarelo (médio) -> vermelho (perto)
void corPorDistancia(float d) {
  float t = (d - DIST_VERMELHO) / (DIST_VERDE - DIST_VERMELHO);
  t = constrain(t, 0.0, 1.0);
  float r = min(1.0, 2.0 * (1.0 - t));
  float g = min(1.0, 2.0 * t);
  setRGB((uint8_t)(255 * r), (uint8_t)(255 * g), false);
}

// ---------- Sensor ultrassônico ----------
float medirDistanciaCm() {
  digitalWrite(PIN_TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(PIN_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(PIN_TRIG, LOW);

  unsigned long dur = pulseIn(PIN_ECHO, HIGH, 30000UL);  // timeout de 30 ms
  if (dur == 0) return 400.0;                            // sem eco: considera livre
  return dur * 0.0343 / 2.0;
}

float medirNoAngulo(int angulo) {
  servoSensor.write(angulo);
  delay(400);
  float d = medirDistanciaCm();
  delay(30);
  return d;
}

// ---------- Bateria ----------
float lerTensaoBateria() {
  long soma = 0;
  for (int i = 0; i < 16; i++) {
    soma += analogRead(PIN_BAT);
    delay(2);
  }
  float leitura = soma / 16.0;
  return leitura * (5.0 / 1023.0) * FATOR_DIVISOR;
}

void atualizarBateria() {
  if (millis() - ultimaLeituraBat < 1000) return;
  ultimaLeituraBat = millis();

  float v = lerTensaoBateria();
  Serial.print("Pilhas: ");
  Serial.print(v, 2);
  Serial.println(" V");

  if (!bateriaFraca && v < BAT_FRACA_V) bateriaFraca = true;
  else if (bateriaFraca && v > BAT_OK_V) bateriaFraca = false;
}

void atualizarLedBateria() {
  if (!bateriaFraca) {
    digitalWrite(PIN_LED_BAT, LOW);
    return;
  }
  if (millis() - ultimoPiscar >= 300) {
    ultimoPiscar = millis();
    ledBatEstado = !ledBatEstado;
    digitalWrite(PIN_LED_BAT, ledBatEstado);
  }
}

// ---------- Detecção de "preso" ----------
void resetarDeteccao() {
  idxDesvio = 0;
  totalDesvios = 0;
  ultimaDist = -1;
  contaParado = 0;
  ultimoCheck = millis();
}

// Retorna true se houve N_DESVIOS desvios dentro da janela de tempo
bool registrarDesvio() {
  tempoDesvios[idxDesvio] = millis();
  idxDesvio = (idxDesvio + 1) % N_DESVIOS;
  if (totalDesvios < N_DESVIOS) totalDesvios++;
  if (totalDesvios < N_DESVIOS) return false;

  unsigned long maisAntigo = tempoDesvios[idxDesvio];  // com o buffer cheio, é o mais antigo
  return (millis() - maisAntigo) < JANELA_MS;
}

// Retorna true se andou para a frente e a distância quase não mudou por ~3 s
bool checarSemProgresso(float d) {
  if (millis() - ultimoCheck < CHECK_PROGRESSO_MS) return false;
  ultimoCheck = millis();

  bool parado = (ultimaDist >= 0 && d < 100.0 && fabs(d - ultimaDist) < 1.5);
  contaParado = parado ? contaParado + 1 : 0;
  ultimaDist = d;
  return contaParado >= 2;
}

// ---------- Comportamentos ----------
void desviar() {
  parar();
  setRGB(0, 0, true);   // azul: "pensando"
  delay(150);

  andarRe();
  delay(TEMPO_RE_MS);
  parar();

  float distEsq = medirNoAngulo(ANGULO_ESQ);
  float distDir = medirNoAngulo(ANGULO_DIR);
  servoSensor.write(ANGULO_CENTRO);

  Serial.print("Esq: "); Serial.print(distEsq);
  Serial.print(" cm | Dir: "); Serial.print(distDir);
  Serial.println(" cm");

  if (distEsq >= distDir) girarEsq();
  else                    girarDir();
  delay(TEMPO_GIRO_MS);
  parar();

  delay(400);   // deixa o servo voltar ao centro
  setRGB(0, 0, false);
}

void pedirAjuda() {
  parar();
  Serial.println("Preso! Pedindo ajuda...");

  for (int i = 0; i < 12; i++) {
    digitalWrite(PIN_LEDS_PRESO, HIGH);
    setRGB(255, 0, false);
    delay(150);
    digitalWrite(PIN_LEDS_PRESO, LOW);
    setRGB(0, 0, false);
    delay(150);
  }

  // Depois de pedir ajuda, tenta se soltar sozinho
  andarRe();
  delay(800);
  parar();
  if (random(2)) girarEsq(); else girarDir();
  delay(700);
  parar();

  resetarDeteccao();
}

void setup() {
  Serial.begin(9600);

  pinMode(PIN_IN1, OUTPUT); pinMode(PIN_IN2, OUTPUT);
  pinMode(PIN_IN3, OUTPUT); pinMode(PIN_IN4, OUTPUT);
  pinMode(PIN_ENA, OUTPUT); pinMode(PIN_ENB, OUTPUT);

  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);

  pinMode(PIN_LED_R, OUTPUT);
  pinMode(PIN_LED_G, OUTPUT);
  pinMode(PIN_LED_B, OUTPUT);
  pinMode(PIN_LEDS_PRESO, OUTPUT);
  pinMode(PIN_LED_BAT, OUTPUT);

  parar();
  setRGB(0, 0, false);
  digitalWrite(PIN_LEDS_PRESO, LOW);

  randomSeed(analogRead(A3));   // A3 solto, serve só como fonte de ruído

  servoSensor.attach(PIN_SERVO);
  servoSensor.write(ANGULO_CENTRO);

  resetarDeteccao();
  delay(1000);   // tempo para soltar o carrinho antes de andar
}

void loop() {
  atualizarBateria();
  atualizarLedBateria();

  float d = medirDistanciaCm();
  corPorDistancia(d);

  if (d < DIST_OBSTACULO) {
    desviar();
    ultimaDist = -1;
    contaParado = 0;
    if (registrarDesvio()) pedirAjuda();
  } else {
    andarFrente();
    if (checarSemProgresso(d)) pedirAjuda();
  }

  delay(40);
}
