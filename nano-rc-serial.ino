// Inclusión de la librería para comunicación serial por software (para el
// HC-05)
#include <SoftwareSerial.h>

// --- Definiciones de Pines ---
// Motor A (por ejemplo, motor derecho)
const int AIN1 = 8; // Pin de Dirección 1 para Motor A (Conectar a IN1A del
                    // TB6612FNG) - Antes D7
const int AIN2 = 7; // Pin de Dirección 2 para Motor A (Conectar a IN2A del
                    // TB6612FNG) - Antes D8
const int PWMA = 9; // Pin PWM para Velocidad del Motor A (Conectar a PWMA del
                    // TB6612FNG) - Mismo D9

// Motor B (por ejemplo, motor izquierdo)
const int BIN1 = 3; // Pin de Dirección 1 para Motor B (Conectar a IN1B del
                    // TB6612FNG) - Antes D4
const int BIN2 = 4; // Pin de Dirección 2 para Motor B (Conectar a IN2B del
                    // TB6612FNG) - Antes D5
const int PWMB = 5; // Pin PWM para Velocidad del Motor B (Conectar a PWMB del
                    // TB6612FNG) - Antes D6

// Pin de Standby para el TB6612FNG
const int STBY_PIN =
    6; // Pin para habilitar/deshabilitar el driver TB6612FNG - Antes D3

// Pines para el Módulo Bluetooth HC-05 (sin cambios solicitados)
const int BT_RX_PIN = 0; // Pin RX del Arduino (Conectar al TXD del HC-05)
const int BT_TX_PIN = 1; // Pin TX del Arduino (Conectar al RXD del HC-05)

// --- Variables Globales ---
SoftwareSerial bluetooth(
    BT_RX_PIN,
    BT_TX_PIN);       // Objeto SoftwareSerial para la comunicación Bluetooth
char comandoRecibido; // Variable para almacenar el comando recibido
int velocidadMotores = 200; // Velocidad por defecto de los motores (0-255)

// --- Función de Configuración (setup) ---
void setup() {
  // Inicializar comunicación serial con el monitor (para depuración)
  Serial.begin(9600);
  Serial.println("Inicializando Robot Mini Sumo RC (Pines Modificados)...");

  // Inicializar comunicación Bluetooth con el HC-05
  bluetooth.begin(9600); // Baud rate común para HC-05 (asegúrate que coincida
                         // con tu módulo)

  // Configurar pines del driver de motores como SALIDA
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  pinMode(PWMA, OUTPUT);
  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);
  pinMode(PWMB, OUTPUT);
  pinMode(STBY_PIN, OUTPUT);

  // Habilitar el driver TB6612FNG
  digitalWrite(STBY_PIN, HIGH);

  // Estado inicial de los motores: detenidos
  detenerMotores();
  Serial.println("Robot Listo.");
}

// --- Función Principal (loop) ---
void loop() {
  // Verificar si hay datos disponibles desde el Bluetooth
  if (bluetooth.available()) {
    comandoRecibido = bluetooth.read(); // Leer el comando entrante
                                        //
    // if (comandoRecibido != 'S') {
    //   Serial.print("Comando recibido: ");
    //   Serial.println(comandoRecibido); // Mostrar comando en el monitor
    //   serial
    // }

    // Procesar el comando recibido
    switch (comandoRecibido) {
    case 'F': // Avanzar (Forward)
      moverAdelante();
      break;
    case 'B': // Retroceder (Backward)
      moverAtras();
      break;
    case 'L': // Girar a la Izquierda (Left)
      girarIzquierda();
      break;
    case 'R': // Girar a la Derecha (Right)
      girarDerecha();
      break;
    case 'S': // Detener (Stop)
      detenerMotores();
      break;
    // Comandos de Velocidad (0-9, mapeados a 0-255)
    // '0' también detiene los motores
    case '0':
      velocidadMotores = 0;
      detenerMotores();
      break;
    case '1':
      velocidadMotores = 200;
      break; // Velocidad baja
    case '2':
      velocidadMotores = 200;
      break;
    case '3':
      velocidadMotores = 200;
      break;
    case '4':
      velocidadMotores = 200;
      break; // Velocidad media
    case '5':
      velocidadMotores = 200;
      break;
    case '6':
      velocidadMotores = 200;
      break;
    case '7':
      velocidadMotores = 200;
      break; // Velocidad alta
    case '8':
      velocidadMotores = 200;
      break;
    case '9':
      velocidadMotores = 200;
      break; // Velocidad máxima
    case 'q':
      velocidadMotores = 200;
      break; // Velocidad máxima
    default:
      Serial.println("Comando desconocido.");
      // Opcional: enviar respuesta por Bluetooth
      // bluetooth.println("Comando no reconocido");
      break;
    }
  }
}

// --- Funciones de Control de Motores ---

/**
 * @brief Mueve el robot hacia adelante.
 * Ambos motores giran en la misma dirección para avanzar.
 */
void moverAdelante() {
  // Serial.println("Moviendo Adelante");
  // Motor A (Derecho) - Adelante
  digitalWrite(AIN1, HIGH);
  digitalWrite(AIN2, LOW);
  analogWrite(PWMA, velocidadMotores);

  // Motor B (Izquierdo) - Adelante
  digitalWrite(BIN1, HIGH);
  digitalWrite(BIN2, LOW);
  analogWrite(PWMB, velocidadMotores);
}

/**
 * @brief Mueve el robot hacia atrás.
 * Ambos motores giran en la misma dirección (opuesta a 'adelante') para
 * retroceder.
 */
void moverAtras() {
  // Serial.println("Moviendo Atras");
  // Motor A (Derecho) - Atrás
  digitalWrite(AIN1, LOW);
  digitalWrite(AIN2, HIGH);
  analogWrite(PWMA, velocidadMotores);

  // Motor B (Izquierdo) - Atrás
  digitalWrite(BIN1, LOW);
  digitalWrite(BIN2, HIGH);
  analogWrite(PWMB, velocidadMotores);
}

/**
 * @brief Hace que el robot gire hacia la izquierda.
 * El motor derecho avanza y el motor izquierdo retrocede (o se detiene/gira más
 * lento).
 */
void girarIzquierda() {
  // Serial.println("Girando Izquierda");
  // Motor A (Derecho) - Adelante
  digitalWrite(AIN1, HIGH);
  digitalWrite(AIN2, LOW);
  analogWrite(PWMA, velocidadMotores);

  // Motor B (Izquierdo) - Atrás
  digitalWrite(BIN1, LOW);
  digitalWrite(BIN2, HIGH);
  analogWrite(PWMB, velocidadMotores);
}

/**
 * @brief Hace que el robot gire hacia la derecha.
 * El motor izquierdo avanza y el motor derecho retrocede (o se detiene/gira más
 * lento).
 */
void girarDerecha() {
  // Serial.println("Girando Derecha");
  // Motor A (Derecho) - Atrás
  digitalWrite(AIN1, LOW);
  digitalWrite(AIN2, HIGH);
  analogWrite(PWMA, velocidadMotores);

  // Motor B (Izquierdo) - Adelante
  digitalWrite(BIN1, HIGH);
  digitalWrite(BIN2, LOW);
  analogWrite(PWMB, velocidadMotores);
}

/**
 * @brief Detiene ambos motores.
 * Utiliza el modo "short brake" del TB6612FNG para una detención más rápida.
 */
void detenerMotores() {
  // Serial.println("Deteniendo Motores");
  // Motor A (Derecho) - Freno
  digitalWrite(AIN1,
               HIGH); // Para freno según datasheet TB6612FNG (Short Brake)
  digitalWrite(AIN2, HIGH); // Para freno
  analogWrite(PWMA, 0);     // Velocidad a 0

  // Motor B (Izquierdo) - Freno
  digitalWrite(BIN1, HIGH); // Para freno
  digitalWrite(BIN2, HIGH); // Para freno
  analogWrite(PWMB, 0);     // Velocidad a 0

  // Alternativa para detener (poner driver en standby):
  // digitalWrite(STBY_PIN, LOW);
  // Si usas esta alternativa, recuerda poner STBY_PIN en HIGH antes de otro
  // movimiento.
}
