// TESTE 3: Sensor Ultrassônico e Servo
// Pinos: Trig=A0, Echo=A1 | Servo=9

#include <Servo.h>

const int PIN_TRIG = A0;
const int PIN_ECHO = A1;
const int PIN_SERVO = 9;

Servo servoSensor;

void setup() {
  Serial.begin(9600);
  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);
  
  servoSensor.attach(PIN_SERVO);
  servoSensor.write(90); // Inicia no centro
  delay(1000);
}

void loop() {
  // Disparo do sensor
  digitalWrite(PIN_TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(PIN_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(PIN_TRIG, LOW);

  // Leitura do tempo de resposta
  long duracao = pulseIn(PIN_ECHO, HIGH, 30000); // Timeout de 30ms
  float distancia = (duracao == 0) ? 400.0 : (duracao * 0.0343 / 2.0);

  Serial.print("Distancia: ");
  Serial.print(distancia);
  Serial.println(" cm");

  // Lógica: se aproximar a mão, o servo "vira o rosto"
  if (distancia > 0 && distancia < 20.0) {
    servoSensor.write(150); // Vira para o lado
  } else {
    servoSensor.write(90);  // Volta para o centro
  }
  
  delay(100); // Pequena pausa
}