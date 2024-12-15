#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

// Configuración del PCA9685
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();
#define SERVO_MIN 102 // Valor PWM para 0 grados
#define SERVO_MAX 512 // Valor PWM para 140 grados

// Pines
#define NUM_SERVOS 6
int servoAnalogPins[NUM_SERVOS] = {34, 35, 36, 39, 32, 33}; // Pines analógicos para retroalimentación
#define BUTTON1_PIN 25 // Botón para calcular cinemática inversa

// Pines I2C de la ESP32 WROOM 38 pines
#define SDA_PIN 21
#define SCL_PIN 22

// Longitudes de los eslabones del robot (cm)
#define L1 3.2
#define L2 2.1
#define L3 10.05
#define L4 8.2
#define L5 4.3

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

  Serial.println("Sistema listo. Pulsa el botón para que el robot dibuje un cuadrado en el plano X-Z.");
  Serial.println("Rango del robot:");
  Serial.print("Alcance mínimo: ");
  Serial.print(fabs(L3 - L4));
  Serial.println(" cm");
  Serial.print("Alcance máximo: ");
  Serial.print(L3 + L4);
  Serial.println(" cm");
}

void loop() {
  // Verificar si se presiona el botón para iniciar el cálculo
  if (digitalRead(BUTTON1_PIN) == LOW) {
    delay(200); // Antirrebote

    // Coordenadas para dibujar un cuadrado en el plano X-Z
    float points[5][3] = {
        {1, 1, 1},  // Punto inicial
        {5, 1, 1},  // Punto 1 del cuadrado
        {5, 5, 1},  // Punto 2 del cuadrado
        {1, 5, 1},  // Punto 3 del cuadrado
        {1, 1, 1}   // Regreso al punto inicial
    };

    // Dibujar los movimientos
    for (int i = 0; i < 5; i++) {
      float targetX = points[i][0];
      float targetY = points[i][1];
      float targetZ = points[i][2];

      // Calcular cinemática inversa
      float jointAngles[NUM_SERVOS];
      if (calcularCinematicaInversa(targetX, targetY, targetZ, jointAngles)) {
        // Mover los servos a los ángulos calculados
        for (int j = 0; j < NUM_SERVOS; j++) {
          compareAndMoveServo(j, jointAngles[j]);
        }
        delay(1000); // Esperar entre movimientos
      } else {
        Serial.println("Posición fuera del alcance del robot.");
        break;
      }
    }

    Serial.println("Cuadrado completado en el plano.");
  }
}

// Función para calcular la cinemática inversa
bool calcularCinematicaInversa(float px, float py, float pz, float *jointAngles) {
  // Calcular q1 (rotación base)
  jointAngles[0] = atan2(py, px) * 180.0 / M_PI;

  // Distancia en el plano XY
  float d_xy = sqrt(px * px + py * py);

  // Altura relativa desde el eslabón 1
  float h = pz - L1;

  // Distancia efectiva desde la base al efector final
  float d_eff = sqrt(d_xy * d_xy + h * h);

  // Verificar si está dentro del alcance
  if (d_eff > (L3 + L4) || d_eff < fabs(L3 - L4)) {
    return false; // Fuera del alcance físico
  }

  // Calcular q3 usando ley de cosenos
  float cos_q3 = (L3 * L3 + L4 * L4 - d_eff * d_eff) / (2 * L3 * L4);
  jointAngles[2] = acos(cos_q3) * 180.0 / M_PI;

  // Calcular ángulo q2
  float theta = atan2(h, d_xy);
  float cos_phi = (L3 * L3 + d_eff * d_eff - L4 * L4) / (2 * L3 * d_eff);
  float phi = acos(cos_phi);
  jointAngles[1] = (theta + phi) * 180.0 / M_PI;

  // Calcular q4, q5, q6 (simplificado)
  jointAngles[3] = 0; // Ajuste del codo (placeholder)
  jointAngles[4] = 0; // Ajuste de la muñeca (placeholder)
  jointAngles[5] = 0; // Orientación fija inicial (placeholder)

  // Restringir ángulos a 0-140 grados
  for (int i = 0; i < NUM_SERVOS; i++) {
    if (jointAngles[i] < 0) jointAngles[i] = 0;
    if (jointAngles[i] > 140) jointAngles[i] = 140;
  }

  return true;
}

// Función para comparar y mover el servo a un ángulo específico
void compareAndMoveServo(int servoIndex, float targetAngle) {
  int analogValue = analogRead(servoAnalogPins[servoIndex]);
  float currentAngle = map(analogValue, 0, 4095, 0, 180);

  if (currentAngle > 140) {
    currentAngle = 140;
  }

  if (abs(currentAngle - targetAngle) > 2) { // Movimiento solo si hay diferencia significativa
    moveServoToAngle(servoIndex, targetAngle);
    // Leer y mostrar la posición real al llegar al objetivo
    analogValue = analogRead(servoAnalogPins[servoIndex]); // Leer posición real nuevamente
    currentAngle = map(analogValue, 0, 4095, 0, 180);
    Serial.print("Servo ");
    Serial.print(servoIndex);
    Serial.print(" - Objetivo: ");
    Serial.print(targetAngle);
    Serial.print(" grados | Real: ");
    Serial.print(currentAngle);
    Serial.println(" grados");
  } else {
    Serial.print("Servo ");
    Serial.print(servoIndex);
    Serial.println(" ya está en la posición deseada.");
  }
}

// Función para mover el servo a un ángulo específico
void moveServoToAngle(int servoIndex, float angle) {
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
