#define pwma 9
#define ain2 7
#define ain1 8
#define stby 6
#define bin1 3
#define bin2 4
#define pwmb 5

// variables
int estado = '0'; // variable bara almacenar los datos recibidos por el
                  // bluetooth

void setup() {
  Serial.begin(9600);
  pinMode(pwma, OUTPUT); // definimnos como salidas todos los pines para
                         // utilizar el puente h
  pinMode(ain2, OUTPUT);
  pinMode(ain1, OUTPUT);
  pinMode(stby, OUTPUT);
  pinMode(bin1, OUTPUT);
  pinMode(bin2, OUTPUT);
  pinMode(pwmb, OUTPUT);

  digitalWrite(ain2, LOW);
  digitalWrite(ain1, LOW);
  digitalWrite(bin1, LOW);
  digitalWrite(bin2, LOW);
  digitalWrite(stby, 1); // encendemos el standby del puente h tb6612 modo
                         // espera
}

void loop() {

  if (Serial.available() > 0) { // lee los caracteres recibidos por el bluetooth
    estado = Serial.read();
  }

  if (estado == 'F') {
    // adelante
    analogWrite(pwma, 255); // no bajar de 130
    analogWrite(pwmb, 255);
    analogWrite(ain1, 255);
    analogWrite(bin1, 255);
    digitalWrite(ain2, LOW);
    digitalWrite(bin2, LOW);
  }

  if (estado == 'B') {

    analogWrite(ain2, 255);
    analogWrite(bin2, 255);
    digitalWrite(ain1, LOW);
    digitalWrite(bin1, LOW);
    analogWrite(pwma, 255);
    analogWrite(pwmb, 255);
    // atras
  }

  if (estado == 'R') {
    digitalWrite(ain1, LOW);
    analogWrite(bin1, 180);
    analogWrite(ain2, 180);
    digitalWrite(bin2, LOW);
    analogWrite(pwma, 180);
    analogWrite(pwmb, 180);
    // derecha
  }

  if (estado == 'L') {
    analogWrite(ain1, 180);
    digitalWrite(bin1, LOW);
    digitalWrite(ain2, LOW);
    analogWrite(bin2, 180);
    analogWrite(pwma, 180);
    analogWrite(pwmb, 180);
    // izquierda
  }

  if (estado == 'S') {
    // paro
    digitalWrite(ain1, 0);
    digitalWrite(bin1, 0);
    digitalWrite(ain2, 0);
    digitalWrite(bin2, 0);
  }
}
