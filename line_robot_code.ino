#include <Adafruit_MotorShield.h>

#define MOTOR_A_TERMINAL 1
#define MOTOR_B_TERMINAL 2
#define MOTOR_C_TERMINAL 3
#define MOTOR_D_TERMINAL 4

#define SENSOR_1_PIN A0
#define SENSOR_2_PIN A1
#define SENSOR_3_PIN 9
#define SENSOR_4_PIN 9

Adafruit_MotorShield AFMS = Adafruit_MotorShield();

Adafruit_DCMotor *MOTOR_A = AFMS.getMotor(MOTOR_A_TERMINAL);
Adafruit_DCMotor *MOTOR_B = AFMS.getMotor(MOTOR_B_TERMINAL);
Adafruit_DCMotor *MOTOR_C = AFMS.getMotor(MOTOR_C_TERMINAL);
Adafruit_DCMotor *MOTOR_D = AFMS.getMotor(MOTOR_D_TERMINAL);

void setup() {
  // Turn on the motorshield and PWM driver
  AFMS.begin();

  // Reset motor A
  MOTOR_A->setSpeed(0);
  MOTOR_A->run(RELEASE);

  // Reset motor B
  MOTOR_B->setSpeed(0);
  MOTOR_B->run(RELEASE);

  // Reset motor C
  MOTOR_C->setSpeed(0);
  MOTOR_C->run(RELEASE);

  // Reset motor D
  MOTOR_D->setSpeed(0);
  MOTOR_D->run(RELEASE);

  Serial.begin(9600);
  
  pinMode(SENSOR_1_PIN, INPUT);
  pinMode(SENSOR_2_PIN, INPUT);
  pinMode(SENSOR_3_PIN, INPUT);
  pinMode(SENSOR_4_PIN, INPUT);

}

void forward(int force) {
  MOTOR_A->setSpeed(force);
  MOTOR_A->run(FORWARD);

  MOTOR_B->setSpeed(force);
  MOTOR_B->run(FORWARD);

  MOTOR_C->setSpeed(force);
  MOTOR_C->run(FORWARD);

  MOTOR_D->setSpeed(force);
  MOTOR_D->run(FORWARD);
}

void stop() {
  MOTOR_A->setSpeed(0);
  MOTOR_A->run(RELEASE);

  MOTOR_B->setSpeed(0);
  MOTOR_B->run(RELEASE);

  MOTOR_C->setSpeed(0);
  MOTOR_C->run(RELEASE);

  MOTOR_D->setSpeed(0);
  MOTOR_D->run(RELEASE);
}

void backward(int force) {
  MOTOR_A->setSpeed(force);
  MOTOR_A->run(BACKWARD);

  MOTOR_B->setSpeed(force);
  MOTOR_B->run(BACKWARD);

  MOTOR_C->setSpeed(force);
  MOTOR_C->run(BACKWARD);

  MOTOR_D->setSpeed(force);
  MOTOR_D->run(BACKWARD);
}

// Clockwise
void cw(int force) {

  MOTOR_A->setSpeed(force);
  MOTOR_A->run(BACKWARD);

  MOTOR_B->setSpeed(force);
  MOTOR_B->run(FORWARD);

  MOTOR_C->setSpeed(force);
  MOTOR_C->run(BACKWARD);

  MOTOR_D->setSpeed(force);
  MOTOR_D->run(FORWARD);
}

// Counter
void ccw(int force) {
   MOTOR_A->setSpeed(force);
  MOTOR_A->run(FORWARD);

  MOTOR_B->setSpeed(force);
  MOTOR_B->run(BACKWARD);

  MOTOR_C->setSpeed(force);
  MOTOR_C->run(FORWARD);

  MOTOR_D->setSpeed(force);
  MOTOR_D->run(BACKWARD);
}

bool is_black(int sensor_pin) {
  const int threshold = 500;
  int value = analogRead(sensor_pin);
  if (value > threshold) {
    return true;
  }
  else {
    return false;
  }

}

void loop() {
  // forward(100);
  // delay(1000);
  // backward(100);
  // delay(1000);
  // ccw(255);
  // delay(1000);
  // cw(255);
  // delay(1000);

  const int speed = 255;
  if (is_black(SENSOR_1_PIN) && is_black(SENSOR_2_PIN)) {
    stop();
  }
  else if (!is_black(SENSOR_1_PIN) && !is_black(SENSOR_2_PIN)) {
    forward(speed);
    // delay(20);

  }
  // Sensor 1 sees black
  else if (is_black(SENSOR_1_PIN)) {
    ccw(speed);
    Serial.println("Counter Clockwise");
  }
  else if (is_black(SENSOR_2_PIN)) {
      cw(speed);
      Serial.println("Clockwise");
  }
  delay(10);

  Serial.println(analogRead(SENSOR_1_PIN));
  Serial.println(analogRead(SENSOR_2_PIN));
  Serial.println("\n");
}


