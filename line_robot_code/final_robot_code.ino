#include "Adafruit_BluefruitLE_SPI.h"
#include <Adafruit_MotorShield.h>

#define VERBOSE_MODE                   false

#define BLUEFRUIT_SPI_CS               8
#define BLUEFRUIT_SPI_IRQ              7
#define BLUEFRUIT_SPI_RST              4

#define MOTOR_A_TERMINAL 1
#define MOTOR_B_TERMINAL 2
#define MOTOR_C_TERMINAL 3
#define MOTOR_D_TERMINAL 4

#define SENSOR_1_PIN A0
#define SENSOR_2_PIN A1
#define SENSOR_3_PIN 9
// #define SENSOR_4_PIN 9

#define LIGHT_1_PIN 10
#define GLOB_DELAY 10

#define TIME1 10
#define RATE1 260 // RATE INVERSE 100, 300, 350, 260

// Button numbers
#define UP '5'
#define DOWN '6'
#define RIGHT '8'
#define LEFT '7'

// Bluetooth setup
Adafruit_BluefruitLE_SPI ble(BLUEFRUIT_SPI_CS, BLUEFRUIT_SPI_IRQ, BLUEFRUIT_SPI_RST);

int glob_fire_count = 0;
bool bluetooth_mode = false;
bool sharp_turn = false;

void error(const __FlashStringHelper*err) {
  Serial.println(err);
  while (1);
}

Adafruit_MotorShield AFMS = Adafruit_MotorShield();

Adafruit_DCMotor *MOTOR_A = AFMS.getMotor(MOTOR_A_TERMINAL);
Adafruit_DCMotor *MOTOR_B = AFMS.getMotor(MOTOR_B_TERMINAL);
Adafruit_DCMotor *MOTOR_C = AFMS.getMotor(MOTOR_C_TERMINAL);
Adafruit_DCMotor *MOTOR_D = AFMS.getMotor(MOTOR_D_TERMINAL);

bool is_black(int sensor_pin, bool digital = false);

void setup() {

  //Motor shield Setup code
  
  // Initialise the bluetooth module
  if ( !ble.begin(VERBOSE_MODE) ) {
    error(F("Couldn't find Bluefruit, make sure it's in CoMmanD mode & check wiring?"));
  }


  // Perform a factory reset to make sure everything is in a known state
    if ( !ble.factoryReset() ){
      error(F("Couldn't factory reset"));
    }

  //Disable command echo from Bluefruit so commands aren't sent back for confirmation
  ble.echo(false);


  //ble.info() can be used to get info about the bluefruit module


  // prevents module from sending extensive debug info
  ble.verbose(false);  
 
  //Wait for connection
  while (! ble.isConnected()) {
      delay(500);
  }

  // Set module to DATA mode
  ble.setMode(BLUEFRUIT_MODE_DATA);

  

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

  pinMode(LIGHT_1_PIN, OUTPUT);
  // pinMode(SENSOR_4_PIN, INPUT);

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

  delay(GLOB_DELAY);
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

// Clockwise
void cw(int force) {

  MOTOR_A->setSpeed((force));
  MOTOR_A->run(BACKWARD);

  MOTOR_B->setSpeed(force);
  MOTOR_B->run(FORWARD);

  MOTOR_C->setSpeed((force));
  MOTOR_C->run(BACKWARD);

  MOTOR_D->setSpeed(force);
  MOTOR_D->run(FORWARD);

  // MOTOR_A->setSpeed(0);
  // MOTOR_A->run(RELEASE);

  // MOTOR_B->setSpeed(force);
  // MOTOR_B->run(FORWARD);

  // MOTOR_C->setSpeed(0);
  // MOTOR_C->run(RELEASE);

  // MOTOR_D->setSpeed(force);
  // MOTOR_D->run(FORWARD);
}

// Counter
void ccw(int force) {
  MOTOR_A->setSpeed(force);
  MOTOR_A->run(FORWARD);

  MOTOR_B->setSpeed((force));
  MOTOR_B->run(BACKWARD);

  MOTOR_C->setSpeed(force);
  MOTOR_C->run(FORWARD);

  MOTOR_D->setSpeed((force));
  MOTOR_D->run(BACKWARD);
  // MOTOR_A->setSpeed(force);
  // MOTOR_A->run(FORWARD);

  // MOTOR_B->setSpeed(0);
  // MOTOR_B->run(RELEASE);

  // MOTOR_C->setSpeed(force);
  // MOTOR_C->run(FORWARD);

  // MOTOR_D->setSpeed(0);
  // MOTOR_D->run(RELEASE);

}

void smart_turn_left() {
  bool first = true;

  for(int i = 0; is_black(SENSOR_1_PIN) && !is_black(SENSOR_2_PIN); ) {
    Serial.println("COUNTER CLOCKWISING");

    if (i < (255/RATE1)) {
      ++i;
    }
    
    Serial.print("Smart turn left value (backward): ");
    Serial.println(i);

    MOTOR_A->setSpeed(255);
    MOTOR_A->run(FORWARD);

    MOTOR_B->setSpeed(i*RATE1);
    MOTOR_B->run(BACKWARD);

    MOTOR_C->setSpeed(255);
    MOTOR_C->run(FORWARD);

    MOTOR_D->setSpeed(i*RATE1);
    MOTOR_D->run(BACKWARD);

    delay(10);

  }

}

void smart_turn_right() {
  bool first = true;

  // If need to turn right
  for(int i = 0; is_black(SENSOR_2_PIN) && !is_black(SENSOR_1_PIN); ) {
    // 
    if (i < (255/RATE1)) {
      ++i;
    }
    Serial.print("Smart turn right value (backward): ");
    Serial.println(i);
    
    MOTOR_A->setSpeed(i*RATE1);
    MOTOR_A->run(BACKWARD);

    MOTOR_B->setSpeed(255);
    MOTOR_B->run(FORWARD);

    MOTOR_C->setSpeed(i*RATE1);
    MOTOR_C->run(BACKWARD);

    MOTOR_D->setSpeed(255);
    MOTOR_D->run(FORWARD);
 
    Serial.println("CLOCKWISE");
    delay(10);

  }


}

// void turn_left_st(int force1, int force2) { // force1 -> left wheels (semi-tank)

//   MOTOR_A->setSpeed(force1);
//   MOTOR_A->run(FORWARD);

//   MOTOR_B->setSpeed(force2);
//   MOTOR_B->run(BACKWARD);

//   MOTOR_C->setSpeed(force1);
//   MOTOR_C->run(FORWARD);

//   MOTOR_D->setSpeed(force2);
//   MOTOR_D->run(BACKWARD);
// }

void turn_right(int force1, int force2) { // force1 -> right wheels (semi-tank)

  MOTOR_A->setSpeed(force2);
  MOTOR_A->run(FORWARD);

  MOTOR_B->setSpeed(force1);
  MOTOR_B->run(FORWARD);

  MOTOR_C->setSpeed(force2);
  MOTOR_C->run(FORWARD);

  MOTOR_D->setSpeed(force1);
  MOTOR_D->run(FORWARD);
}

void turn_left(int force1, int force2) { // force1 -> right wheels (semi-tank)
  MOTOR_A->setSpeed(force1);
  MOTOR_A->run(FORWARD);

  MOTOR_B->setSpeed(force2);
  MOTOR_B->run(FORWARD);

  MOTOR_C->setSpeed(force1);
  MOTOR_C->run(FORWARD);

  MOTOR_D->setSpeed(force2);
  MOTOR_D->run(FORWARD);

}

bool is_black(int sensor_pin, bool digital = false) {
  if (digital) {
    if (digitalRead(sensor_pin) == 1) {
      return true;
    }
    else {
      return false;
    }
  }

  const int threshold = 500;
  int value = analogRead(sensor_pin);
  if (value > threshold) {
    return true;
  }
  else {
    return false;
  }

}

void ramp_mode(int speed, int force1, int force2){
  int time = 300;
  // milliseconds /= 5;
  digitalWrite(LIGHT_1_PIN, HIGH);

  for (int count = 0; count < time; ++count) {
    forward(speed);
  //   if (is_black(SENSOR_1_PIN) && is_black(SENSOR_2_PIN)) {
  //   forward(speed);
  // }

  // else if (!is_black(SENSOR_1_PIN) && !is_black(SENSOR_2_PIN)) {
  //   forward(speed);
  //   // delay(20);

  // }
  // // Sensor 1 sees black
  // else if (is_black(SENSOR_1_PIN)) { // Turn left
  //   turn_left(force1, force2);
  //   // turn_left_st(turn_fast, turn_slow); // left
  //   Serial.println("Ramp mode: move left");
  // }
  // else if (is_black(SENSOR_2_PIN)) { // Turn right
  //     turn_right(force1, force2);
  //     // cw(speed);
  //     // turn_right_st(turn_fast, turn_slow); // right
  //     Serial.println("Ramp mode: move right");
  // }
    delay(10);
  }
  analogWrite(LIGHT_1_PIN, LOW);
}

void tank_fire() {

  ++glob_fire_count;

  if (glob_fire_count > 5) {
    if (digitalRead(LIGHT_1_PIN) == HIGH) {
      digitalWrite(LIGHT_1_PIN, LOW);
    }
    else {
      digitalWrite(LIGHT_1_PIN, HIGH);
    }
    glob_fire_count = 0;
  }
  delay(1);

}

char getInput(char button_arr[]){

  char variable1 = ble.read();
  if (variable1 == '!') {
    char variable2 = ble.read();
    if (variable2 == 'B') {
        char button = ble.read();
        char released = ble.read();
        char dummy = ble.read();

        button_arr[0] = '!';
        button_arr[1] = 'B';
        button_arr[2] = button;
        button_arr[3] = released;
        button_arr[4] = dummy;

        for (int i = 0; i < 5; ++i) {
          Serial.print(i);
          Serial.print(" is printing: ");
          Serial.println(button_arr[i]);
        }
        // Serial.println(button);
        // Serial.println(released);


    } 
  }
  else {
      return variable1;
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

  char button;

  // 0, 1 is waste. 2 is the button. 3 is released or not 
  char button_arr[5] = {'x', 'x', 'x', 'x', 'x'};

  // get controller input as a single char
  if (ble.available() ){
    // Serial.println("I'm avaliable. ");
    button = getInput(button_arr);
    Serial.print("BUTTON READS: ");
    Serial.println(button);
    // delay(500);
  }

  const int speed = 255;
  const int turn_fast = 255;
  const int turn_slow = 180;

  tank_fire();

  // Bluetooth mode
  if (button_arr[2] == '1') { // Button 1 turns on bluetooth nmode 
    bluetooth_mode = true;
    Serial.println("BLUETOOTH ON");
    // delay(1000);
  }
  else if (button_arr[2] == '2') { // Button 2 turns off bluetooth mode 
    bluetooth_mode = false;
    Serial.println("BLUETOOTH OFF");
    // delay(1000);
  }

  else if (button_arr[2] == '4') { // Button 3 turns on sharp_turn mode 
    sharp_turn = true;
    Serial.println("SHARPTURN ON");
    // delay(1000);
  }

  else if (button_arr[2] == '3') { // Button 3 turns on sharp_turn mode 
    sharp_turn = false;
    Serial.println("SHARPTURN OFF");
    // delay(1000);
  }

  if (bluetooth_mode) {
    if (button_arr[3] == '0') { // Released
      stop();
    } 
    else {
      if (button_arr[2] == UP) {
        forward(255);
        Serial.println("forward with controller");
      }
      else if (button_arr[2] == DOWN) {
        backward(255);
        Serial.println("backwards with controller");
      }

      else if (button_arr[2] == RIGHT) {
        cw(180);
        Serial.println("right with controller");
      }
      else if (button_arr[2] == LEFT) {
        ccw(180);
        Serial.println("left with controller");
      }
    }

  }

  else {
    // digitalWrite(LIGHT_1_PIN, HIGH);
    if (!is_black(SENSOR_3_PIN, true)) {
      // ramp_mode(speed, turn_fast, turn_slow);
      // Serial.println("Yooo");
    }

    else if (is_black(SENSOR_1_PIN) && is_black(SENSOR_2_PIN)) {
      // stop(); // change back
      forward(speed);
      // delay(30);

    }
    else if (!is_black(SENSOR_1_PIN) && !is_black(SENSOR_2_PIN)) {
      forward(speed);
      // delay(30);
      // delay(20);

    }
    // Sensor 1 sees black
    else if (is_black(SENSOR_1_PIN)) { // Turn left
      if (sharp_turn) {
        ccw(speed);
        delay(5);
      }
      else {
        smart_turn_left();
      }
      // delay(5);
      // Serial.println("Counter Clockwise");
    }
    else if (is_black(SENSOR_2_PIN)) { // Turn right
        if (sharp_turn) {
          cw(speed);
          delay(5);
        }
        else {
          smart_turn_right();
        }
        // delay(5);
        // Serial.println("Clockwise");
    }
    // Serial.println(analogRead(SENSOR_1_PIN));
    // Serial.println(analogRead(SENSOR_2_PIN));
    // Serial.println("\n");
  }
  delay(5);

}


