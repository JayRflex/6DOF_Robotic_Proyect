#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

// Configuración del PCA9685
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();
#define SERVO_MIN 102 // Valor PWM para 0 grados
#define SERVO_MAX 512 // Valor PWM para 140 grados

// Pines
#define NUM_SERVOS 6
#define BUTTON1_PIN 25 // Botón 1 para iniciar secuencia
#define BUTTON2_PIN 26 // Botón 2 (sin uso)
#define BUTTON3_PIN 27 // Botón 3 (sin uso)

// Pines I2C de la ESP32 WROOM 38 pines
#define SDA_PIN 21
#define SCL_PIN 22

// Secuencia de posiciones objetivo
int targetPositions[NUM_SERVOS][9] = {
    {89, 87, 88, 87, 87, 87, 87, 87, 87}, // Servo 0
    {11, 46, 47, 47, 47, 47, 47, 47, 10}, // Servo 1
    {12, 13, 12, 12, 12, 12, 12, 12, 68}, // Servo 2
    {33, 74, 88, 88, 88, 89, 88, 88, 31}, // Servo 3
    {29, 29, 100, 24, 24, 99, 31, 110, 23}, // Servo 4
    {14, 9, 140, 140, 0, 0, 0, 0, 99}      // Servo 5
};

void setup() {
  Serial.begin(115200);

  // Inicializar comunicación I2C en pines específicos
  Wire.begin(SDA_PIN, SCL_PIN);

  pwm.begin();
  pwm.setPWMFreq(50); // Frecuencia de los servos (50 Hz)

  pinMode(BUTTON1_PIN, INPUT_PULLUP);
  pinMode(BUTTON2_PIN, INPUT_PULLUP);
  pinMode(BUTTON3_PIN, INPUT_PULLUP);

  Serial.println("Sistema listo. Pulsa el botón 1 para iniciar la secuencia.");
}

void loop() {
  // Verificar si se presiona el botón 1 para iniciar la secuencia
  if (digitalRead(BUTTON1_PIN) == LOW) {
    delay(200); // Antirrebote
    executeSequence();
  }
}

// Función para ejecutar la secuencia de posiciones
void executeSequence() {
  Serial.println("Iniciando secuencia de posiciones...");

  for (int j = 0; j < 9; j++) { // Iterar sobre las posiciones objetivo
    for (int i = 0; i < NUM_SERVOS; i++) { // Iterar sobre los servos
      int targetAngle = targetPositions[i][j];
      moveServoToAngle(i, targetAngle);
      delay(200); // Tiempo entre movimientos de cada servo
    }
  }

  turnOffServos(); // Apagar los servos al terminar
  Serial.println("Secuencia completada. Servos apagados.");
}

// Función para mover el servo a un ángulo específico
void moveServoToAngle(int servoIndex, int angle) {
  if (angle >= 0 && angle <= 140) {
    int pwmValue = map(angle, 0, 140, SERVO_MIN, SERVO_MAX);
    pwm.setPWM(servoIndex, 0, pwmValue); // Canal correspondiente al servo
    Serial.print("Servo ");
    Serial.print(servoIndex);
    Serial.print(" - Moviendo a: ");
    Serial.print(angle);
    Serial.println(" grados");
  } else {
    Serial.print("Servo ");
    Serial.print(servoIndex);
    Serial.println(" - Ángulo fuera de rango.");
  }
}

// Función para apagar los servos
void turnOffServos() {
  for (int i = 0; i < NUM_SERVOS; i++) {
    pwm.setPWM(i, 0, 0); // Enviar señal PWM de 0 para desenergizar
    Serial.print("Servo ");
    Serial.print(i);
    Serial.println(" apagado.");
  }
}
