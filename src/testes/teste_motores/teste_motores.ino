// TESTE 4: Motores e Ponte H L298N
// Pinos: ENA=5, ENB=6, IN1=4, IN2=7, IN3=8, IN4=12

const int PIN_ENA = 5;
const int PIN_ENB = 6;
const int PIN_IN1 = 4;
const int PIN_IN2 = 7;
const int PIN_IN3 = 8;
const int PIN_IN4 = 12;

void setup() {
  pinMode(PIN_IN1, OUTPUT); pinMode(PIN_IN2, OUTPUT);
  pinMode(PIN_IN3, OUTPUT); pinMode(PIN_IN4, OUTPUT);
  pinMode(PIN_ENA, OUTPUT); pinMode(PIN_ENB, OUTPUT);
}

void loop() {
  // VELOCIDADE MÁXIMA (255)
  analogWrite(PIN_ENA, 255);
  analogWrite(PIN_ENB, 255);

  // FRENTE
  digitalWrite(PIN_IN1, HIGH); digitalWrite(PIN_IN2, LOW);
  digitalWrite(PIN_IN3, HIGH); digitalWrite(PIN_IN4, LOW);
  delay(2000);

  // PARA
  digitalWrite(PIN_IN1, LOW); digitalWrite(PIN_IN2, LOW);
  digitalWrite(PIN_IN3, LOW); digitalWrite(PIN_IN4, LOW);
  delay(1000);

  // TRÁS
  digitalWrite(PIN_IN1, LOW); digitalWrite(PIN_IN2, HIGH);
  digitalWrite(PIN_IN3, LOW); digitalWrite(PIN_IN4, HIGH);
  delay(2000);

  // PARA
  digitalWrite(PIN_IN1, LOW); digitalWrite(PIN_IN2, LOW);
  digitalWrite(PIN_IN3, LOW); digitalWrite(PIN_IN4, LOW);
  delay(3000); // Espera 3 segundos antes de repetir
}