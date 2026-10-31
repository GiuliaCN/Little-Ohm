// TESTE 2: LED RGB e Transistor
// Pinos: R=11, G=3, B=10 | Transistor=2

const int PIN_LED_R = 11;
const int PIN_LED_G = 3;
const int PIN_LED_B = 10;
const int PIN_LEDS_PRESO = 2; // Transistor

void setup() {
  pinMode(PIN_LED_R, OUTPUT);
  pinMode(PIN_LED_G, OUTPUT);
  pinMode(PIN_LED_B, OUTPUT);
  pinMode(PIN_LEDS_PRESO, OUTPUT);
}

void loop() {
  // Teste RGB (Desligando todos antes de ligar o próximo)
  // Obs: Se seu LED for Ânodo Comum, inverta HIGH por LOW e vice-versa nos RGB.
  
  // Vermelho
  digitalWrite(PIN_LED_R, HIGH); digitalWrite(PIN_LED_G, LOW); digitalWrite(PIN_LED_B, LOW);
  delay(1000);
  
  // Verde
  digitalWrite(PIN_LED_R, LOW); digitalWrite(PIN_LED_G, HIGH); digitalWrite(PIN_LED_B, LOW);
  delay(1000);
  
  // Azul
  digitalWrite(PIN_LED_R, LOW); digitalWrite(PIN_LED_G, LOW); digitalWrite(PIN_LED_B, HIGH);
  delay(1000);

  // Desliga RGB e testa o Transistor (LEDs de Emergência)
  digitalWrite(PIN_LED_R, LOW); digitalWrite(PIN_LED_G, LOW); digitalWrite(PIN_LED_B, LOW);
  
  for(int i = 0; i < 5; i++) {
    digitalWrite(PIN_LEDS_PRESO, HIGH); // Aciona transistor
    delay(200);
    digitalWrite(PIN_LEDS_PRESO, LOW);  // Corta transistor
    delay(200);
  }
}