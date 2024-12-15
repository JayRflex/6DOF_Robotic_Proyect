#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

// Configuración del PCA9685
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();
#define SERVO_MIN 102 // Valor PWM para 0 grados
#define SERVO_MAX 512 // Valor PWM para 140 grados

// Pines
#define NUM_SERVOS 6
int servoAnalogPins[NUM_SERVOS] = {34, 35, 36, 39, 32, 33}; // Pines analógicos para retroalimentación
#define BUTTON1_PIN 25       // Botón 1 para guardar posiciones
#define BUTTON2_PIN 26       // Botón 2 para reproducir posiciones
#define BUTTON3_PIN 27       // Botón 3 para borrar posiciones

// Pines I2C de la ESP32 WROOM 38 pines
#define SDA_PIN 21
#define SCL_PIN 22

// Variables globales
int savedPositions[NUM_SERVOS][10]; // Array para guardar posiciones en grados para cada servo
int currentPositionIndex[NUM_SERVOS] = {0, 0, 0, 0, 0, 0}; // Índice actual para guardar posiciones
bool playback = false;                // Indica si está reproduciendo las posiciones
int measuredPositions[NUM_SERVOS][10]; // Array para guardar mediciones reales

void setup() {
  Serial.begin(115200);

  // Inicializar comunicación I2C en pines específicos
  Wire.begin(SDA_PIN, SCL_PIN);

  pwm.begin();
  pwm.setPWMFreq(50); // Frecuencia de los servos (50 Hz)

  for (int i = 0; i < NUM_SERVOS; i++) {
    pinMode(servoAnalogPins[i], INPUT);
  }
  pinMode(BUTTON1_PIN, INPUT_PULLUP);
  pinMode(BUTTON2_PIN, INPUT_PULLUP);
  pinMode(BUTTON3_PIN, INPUT_PULLUP);

  // Inicializar memoria
  for (int i = 0; i < NUM_SERVOS; i++) {
    for (int j = 0; j < 10; j++) {
      savedPositions[i][j] = -1;
      measuredPositions[i][j] = -1;
    }
  }

  Serial.println("Sistema listo. Usa los botones para interactuar.");
}

void loop() {
  // Leer y mostrar la posición actual de cada servo
  for (int i = 0; i < NUM_SERVOS; i++) {
    int analogValue = analogRead(servoAnalogPins[i]);
    int servoAngle = map(analogValue, 0, 4095, 0, 180);

    // Ajustar el ángulo si excede el límite de 140 grados
    if (servoAngle > 140) {
      servoAngle = 140;
    }

    Serial.print("Servo ");
    Serial.print(i);
    Serial.print(" - Posición actual (grados): ");
    Serial.println(servoAngle);
  }

  // Mostrar valores de posiciones guardadas
  Serial.println("Posiciones guardadas:");
  for (int i = 0; i < NUM_SERVOS; i++) {
    Serial.print("Servo ");
    Serial.print(i);
    Serial.print(": ");
    for (int j = 0; j < 10; j++) {
      if (savedPositions[i][j] != -1) {
        Serial.print(savedPositions[i][j]);
        Serial.print(" ");
      }
    }
    Serial.println();
  }

  // Verificar si se presiona el botón 1 para guardar posiciones
  if (digitalRead(BUTTON1_PIN) == LOW) {
    delay(200); // Antirrebote
    for (int i = 0; i < NUM_SERVOS; i++) {
      if (currentPositionIndex[i] < 10) {
        int analogValue = analogRead(servoAnalogPins[i]);
        int servoAngle = map(analogValue, 0, 4095, 0, 180);

        if (servoAngle > 140) {
          servoAngle = 140;
        }

        savedPositions[i][currentPositionIndex[i]] = servoAngle;
        Serial.print("Servo ");
        Serial.print(i);
        Serial.print(" - Posición guardada en el índice ");
        Serial.print(currentPositionIndex[i]);
        Serial.print(": ");
        Serial.println(servoAngle);
        currentPositionIndex[i]++;
      } else {
        Serial.print("Memoria llena para el servo ");
        Serial.println(i);
      }
    }
  }

  // Verificar si se presiona el botón 2 para reproducir posiciones
  if (digitalRead(BUTTON2_PIN) == LOW) {
    delay(200); // Antirrebote
    playback = true;
    Serial.println("Reproduciendo posiciones...");
    for (int j = 0; j < 10; j++) {
      for (int i = NUM_SERVOS - 1; i >= 0; i--) {
        if (savedPositions[i][j] != -1) {
          compareAndMoveServo(i, savedPositions[i][j], j);
          delay(200); // Esperar 0.2 segundos entre movimientos de cada servo
        }
      }
    }
    playback = false;

    // Apagar los servos después de la secuencia
    turnOffServos();

    // Imprimir tabla de posiciones objetivo y reales al final de la secuencia
    Serial.println("\nTabla de posiciones:");
    for (int i = 0; i < NUM_SERVOS; i++) {
      Serial.print("Servo ");
      Serial.print(i);
      Serial.println(":");
      for (int j = 0; j < 10; j++) {
        if (savedPositions[i][j] != -1) {
          Serial.print("Posición objetivo: ");
          Serial.print(savedPositions[i][j]);
          Serial.print(" - Posición real: ");
          Serial.println(measuredPositions[i][j]);
        }
      }
    }
  }

  // Verificar si se presiona el botón 3 para borrar posiciones
  if (digitalRead(BUTTON3_PIN) == LOW) {
    delay(200); // Antirrebote
    clearMemory();
    Serial.println("Memoria borrada. Puedes guardar nuevas posiciones.");
  }

  delay(3000); // Retraso de 3 segundos para muestreo
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

// Función para comparar y mover el servo a un ángulo específico
void compareAndMoveServo(int servoIndex, int targetAngle, int positionIndex) {
  int analogValue = analogRead(servoAnalogPins[servoIndex]);
  int currentAngle = map(analogValue, 0, 4095, 0, 180);

  if (currentAngle > 140) {
    currentAngle = 140;
  }

  if (abs(currentAngle - targetAngle) > 2) { // Movimiento solo si hay diferencia significativa
    moveServoToAngle(servoIndex, targetAngle);
    // Leer y guardar la posición real al llegar al objetivo
    analogValue = analogRead(servoAnalogPins[servoIndex]); // Leer posición real nuevamente
    currentAngle = map(analogValue, 0, 4095, 0, 180);
    measuredPositions[servoIndex][positionIndex] = currentAngle;
    Serial.print("real_servo");
    Serial.print(servoIndex);
    Serial.print(":");
    Serial.println(currentAngle);
  } else {
    Serial.print("Servo ");
    Serial.print(servoIndex);
    Serial.println(" ya está en la posición deseada.");
  }

  // Graficar posición objetivo
  Serial.print("objetivo_servo");
  Serial.print(servoIndex);
  Serial.print(":");
  Serial.println(targetAngle);
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

// Función para borrar la memoria de posiciones
void clearMemory() {
  for (int i = 0; i < NUM_SERVOS; i++) {
    for (int j = 0; j < 10; j++) {
      savedPositions[i][j] = -1;
      measuredPositions[i][j] = -1;
    }
    currentPositionIndex[i] = 0;
  }
}
