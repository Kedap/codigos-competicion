#include <Arduino.h>
#include <Bluepad32.h>
#include <cmath>

// --- CONSTANTES DE CONTROL ---
// La velocidad máxima es tu principal herramienta para prevenir el
// sobrecalentamiento del driver. Si el robot vuelve a fallar tras un uso
// prolongado, reduce este valor.
const int SPEED_MODE_MAX_PWM = 255;
const int PRECISION_MODE_MAX_PWM = 100;
const int POINT_TURN_MAX_PWM = 200;

const int TRIGGER_THRESHOLD = 100;
const float JOYSTICK_DEAD_ZONE = 0.15;
const float CONTROL_CURVE_EXPONENT = 2.0;
const float TURN_SENSITIVITY = 1.0;

// --- PINES DEL DRIVER Y MOTORES ---
#define STBY 14
#define MOTOR_IZQUIERDO 1
#define MOTOR_DERECHO 2

// Motor Izquierdo
#define AIN1 27
#define AIN2 33
#define PWMA 25
#define PWM_CHANNEL_A 0

// Motor Derecho
#define BIN1 12
#define BIN2 13
#define PWMB 26
#define PWM_CHANNEL_B 1

#define PWM_FREQUENCY 5000
#define PWM_RESOLUTION 8

// --- DECLARACIONES GLOBALES ---
ControllerPtr myControllers[BP32_MAX_GAMEPADS];

// ================================================================
// === FUNCIONES DE CONTROL DEL DRIVER OPTIMIZADAS ===
// ================================================================

void setupMotors() {
  pinMode(STBY, OUTPUT);
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  pinMode(PWMA, OUTPUT);
  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);
  pinMode(PWMB, OUTPUT);

  ledcSetup(PWM_CHANNEL_A, PWM_FREQUENCY, PWM_RESOLUTION);
  ledcSetup(PWM_CHANNEL_B, PWM_FREQUENCY, PWM_RESOLUTION);

  ledcAttachPin(PWMA, PWM_CHANNEL_A);
  ledcAttachPin(PWMB, PWM_CHANNEL_B);

  // Habilitamos el driver UNA SOLA VEZ al inicio.
  digitalWrite(STBY, HIGH);
}

// Función optimizada: solo se ocupa de hacer girar el motor.
void setMotorSpeed(int motorNumber, int speed) {
  int in1_pin, in2_pin, pwm_channel;

  if (motorNumber == MOTOR_IZQUIERDO) {
    in1_pin = AIN1;
    in2_pin = AIN2;
    pwm_channel = PWM_CHANNEL_A;
  } else { // MOTOR_DERECHO
    in1_pin = BIN1;
    in2_pin = BIN2;
    pwm_channel = PWM_CHANNEL_B;
    speed = -speed; // Corregimos la dirección del motor derecho
  }

  speed = constrain(speed, -255, 255);

  if (speed > 0) { // Adelante
    digitalWrite(in1_pin, HIGH);
    digitalWrite(in2_pin, LOW);
  } else if (speed < 0) { // Atrás
    digitalWrite(in1_pin, LOW);
    digitalWrite(in2_pin, HIGH);
  }
  // No hay 'else' para speed == 0, porque la parada se gestiona fuera.

  ledcWrite(pwm_channel, abs(speed));
}

// Nueva función para el freno activo. Es la forma más responsiva de parar.
void brakeMotors() {
  digitalWrite(AIN1, HIGH);
  digitalWrite(AIN2, HIGH);
  ledcWrite(PWM_CHANNEL_A, 0);
  digitalWrite(BIN1, HIGH);
  digitalWrite(BIN2, HIGH);
  ledcWrite(PWM_CHANNEL_B, 0);
}

// ================================================================
// === FUNCIONES AUXILIARES Y LÓGICA PRINCIPAL ===
// ================================================================

void activateKicker() { Serial.println("¡DISPARO ACTIVADO!"); }

float applyControlCurve(float value) {
  float sign = (value < 0) ? -1.0 : 1.0;
  return sign * pow(abs(value), CONTROL_CURVE_EXPONENT);
}

// --- LÓGICA DE CONTROL REESTRUCTURADA Y OPTIMIZADA ---
void processGamepad(ControllerPtr ctl) {
  // 1. LEER TODOS LOS INPUTS
  float throttle_input = -ctl->axisY() / 512.0;
  float turn_input = ctl->axisRX() / 512.0;
  bool point_turn_mode_active = (ctl->brake() > TRIGGER_THRESHOLD);
  bool precision_mode_active = (ctl->throttle() > TRIGGER_THRESHOLD);

  // 2. LÓGICA DE PARADA ACTIVA: Si no hay input, frena y no hagas nada más.
  if (abs(throttle_input) < JOYSTICK_DEAD_ZONE &&
      abs(turn_input) < JOYSTICK_DEAD_ZONE) {
    brakeMotors();
    return;
  }

  // 3. PRE-PROCESAR INPUTS (Curva de control, etc.)
  turn_input = -turn_input; // Invertir giro
  float processed_throttle = applyControlCurve(throttle_input);
  float processed_turn = applyControlCurve(turn_input);

  // 4. CALCULAR VELOCIDADES SEGÚN EL MODO ACTIVO
  float leftSpeed = 0, rightSpeed = 0;

  if (point_turn_mode_active) {
    leftSpeed = processed_turn;
    rightSpeed = -processed_turn;
    int motorSpeedL = static_cast<int>(leftSpeed * POINT_TURN_MAX_PWM);
    int motorSpeedR = static_cast<int>(rightSpeed * POINT_TURN_MAX_PWM);
    setMotorSpeed(MOTOR_IZQUIERDO, motorSpeedL);
    setMotorSpeed(MOTOR_DERECHO, motorSpeedR);
  } else {
    float turnEffect = processed_turn * TURN_SENSITIVITY;
    leftSpeed = processed_throttle + turnEffect;
    rightSpeed = processed_throttle - turnEffect;
    float maxMagnitude = max(abs(leftSpeed), abs(rightSpeed));
    if (maxMagnitude > 1.0) {
      leftSpeed /= maxMagnitude;
      rightSpeed /= maxMagnitude;
    }
    int max_speed =
        precision_mode_active ? PRECISION_MODE_MAX_PWM : SPEED_MODE_MAX_PWM;
    int motorSpeedL = static_cast<int>(leftSpeed * max_speed);
    int motorSpeedR = static_cast<int>(rightSpeed * max_speed);
    setMotorSpeed(MOTOR_IZQUIERDO, motorSpeedL);
    setMotorSpeed(MOTOR_DERECHO, motorSpeedR);
  }

  // 5. LÓGICA DE ACCIONES
  if (ctl->a()) {
    activateKicker();
  }
}

// ================================================================
// === FUNCIONES DE BLUEPAD32, SETUP Y LOOP ===
// ================================================================

void onConnectedController(ControllerPtr ctl) {
  bool foundEmptySlot = false;
  for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
    if (myControllers[i] == nullptr) {
      myControllers[i] = ctl;
      foundEmptySlot = true;
      break;
    }
  }
}

void onDisconnectedController(ControllerPtr ctl) {
  bool foundController = false;
  for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
    if (myControllers[i] == ctl) {
      myControllers[i] = nullptr;
      foundController = true;
      break;
    }
  }
}

void setup() {
  Serial.begin(115200);
  Serial.printf("Firmware: %s\n", BP32.firmwareVersion());

  // Llamamos a nuestra función de configuración de motores
  setupMotors();

  // Configuramos Bluepad32
  BP32.setup(&onConnectedController, &onDisconnectedController);
  BP32.forgetBluetoothKeys();
  BP32.enableVirtualDevice(false);
}

void loop() {
  bool dataUpdated = BP32.update();
  if (dataUpdated) {
    for (auto myController : myControllers) {
      if (myController && myController->isConnected() &&
          myController->hasData()) {
        processGamepad(myController);
      }
    }
  }
  delay(10);
}
