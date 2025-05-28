#include <SoftwareSerial.h>

// Pines motores
const int AIN1 = 8;
const int AIN2 = 7;
const int PWMA = 9;

const int BIN1 = 3;
const int BIN2 = 4;
const int PWMB = 5;

const int STBY_PIN = 6;

// Bluetooth
const int BT_RX_PIN = 0;
const int BT_TX_PIN = 1;
SoftwareSerial bluetooth(BT_RX_PIN, BT_TX_PIN);

char comandoRecibido;
int velocidadObjetivo = 200;
int velocidadActual = 0;

// --- SETUP ---
void setup() {
  Serial.begin(9600);
  bluetooth.begin(9600);

  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  pinMode(PWMA, OUTPUT);
  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);
  pinMode(PWMB, OUTPUT);
  pinMode(STBY_PIN, OUTPUT);

  digitalWrite(STBY_PIN, HIGH);
  detenerMotores();

  // Aumentar frecuencia PWM (pines 5, 6, 9, 10)
  TCCR0B = TCCR0B & 0b11111000 | 0x01; // PWM 5 y 6 -> ~62kHz
  TCCR1B = TCCR1B & 0b11111000 | 0x01; // PWM 9 y 10 -> ~31kHz

  Serial.println("Robot mejorado listo.");
}

// --- LOOP ---
void loop() {
  if (bluetooth.available()) {
    comandoRecibido = bluetooth.read();

    switch (comandoRecibido) {
    case 'F':
      moverAdelante();
      break;
    case 'B':
      moverAtras();
      break;
    case 'L':
      girarIzquierda();
      break;
    case 'R':
      girarDerecha();
      break;
    case 'S':
      detenerMotores();
      break;

    case '0':
      velocidadObjetivo = 0;
      detenerMotores();
      break;
    case '1':
      velocidadObjetivo = 100;
      break;
    case '2':
      velocidadObjetivo = 120;
      break;
    case '3':
      velocidadObjetivo = 140;
      break;
    case '4':
      velocidadObjetivo = 160;
      break;
    case '5':
      velocidadObjetivo = 180;
      break;
    case '6':
      velocidadObjetivo = 200;
      break;
    case '7':
      velocidadObjetivo = 220;
      break;
    case '8':
      velocidadObjetivo = 240;
      break;
    case '9':
      velocidadObjetivo = 255;
      break;
    case 'q':
      velocidadObjetivo = 255;
      break;
    }
  }

  // Transición suave hacia la velocidad objetivo
  actualizarVelocidad();
}

// --- CONTROL SUAVE ---
void actualizarVelocidad() {
  if (velocidadActual < velocidadObjetivo)
    velocidadActual += 5;
  else if (velocidadActual > velocidadObjetivo)
    velocidadActual -= 5;
  delay(5); // Ajusta para suavidad
}

// --- MOVIMIENTOS ---
void moverAdelante() {
  digitalWrite(AIN1, HIGH);
  digitalWrite(AIN2, LOW);
  digitalWrite(BIN1, HIGH);
  digitalWrite(BIN2, LOW);
  analogWrite(PWMA, velocidadActual);
  analogWrite(PWMB, velocidadActual);
}

void moverAtras() {
  digitalWrite(AIN1, LOW);
  digitalWrite(AIN2, HIGH);
  digitalWrite(BIN1, LOW);
  digitalWrite(BIN2, HIGH);
  analogWrite(PWMA, velocidadActual);
  analogWrite(PWMB, velocidadActual);
}

void girarIzquierda() {
  digitalWrite(AIN1, HIGH);
  digitalWrite(AIN2, LOW);
  digitalWrite(BIN1, LOW);
  digitalWrite(BIN2, HIGH);
  analogWrite(PWMA, velocidadActual);
  analogWrite(PWMB, velocidadActual);
}

void girarDerecha() {
  digitalWrite(AIN1, LOW);
  digitalWrite(AIN2, HIGH);
  digitalWrite(BIN1, HIGH);
  digitalWrite(BIN2, LOW);
  analogWrite(PWMA, velocidadActual);
  analogWrite(PWMB, velocidadActual);
}

void detenerMotores() {
  digitalWrite(AIN1, HIGH);
  digitalWrite(AIN2, HIGH);
  digitalWrite(BIN1, HIGH);
  digitalWrite(BIN2, HIGH);
  analogWrite(PWMA, 0);
  analogWrite(PWMB, 0);
  velocidadActual = 0;
}
