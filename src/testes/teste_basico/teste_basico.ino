// TESTE 1: Placa e Comunicação Serial
// Pisca o LED interno (pino 13) e envia mensagem para o PC.

void setup() {
  Serial.begin(9600);
  pinMode(13, OUTPUT);
  Serial.println("Iniciando Teste da Placa...");
}

void loop() {
  digitalWrite(13, HIGH);
  Serial.println("LED LIGADO");
  delay(1000);
  
  digitalWrite(13, LOW);
  Serial.println("LED DESLIGADO");
  delay(1000);
}