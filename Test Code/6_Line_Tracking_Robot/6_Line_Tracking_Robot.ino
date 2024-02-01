/*
  Keyestudio 4WD Mecanum Robot for Arduino
  lesson 6
  Line Tracking Robot
  http://www.keyestudio.com
*/

/****JM1Motor controls pins***/
#define PWM1  3
#define M1    2
/****JM4Motor controls pins***/
#define PWM4  11
#define M4    8

/****JM2Motor controls pins***/
#define PWM2  5
#define M2    4
/****JM3Motor controls pins***/
#define PWM3  6
#define M3    7

/*******Define the pin of Line Tracking Sensor**********/
#define SensorLeft    A0   //input pin of left sensor
#define SensorMiddle  A1   //input pin of middle sensor
#define SensorRight   A2   //input pin of right sensor

uint8_t speed1 = 100;  //Right front motor speed
uint8_t speed4 = 100;  //Right rear motor speed

uint8_t speed2 = 100; //Left front motor speed
uint8_t speed3 = 100; //Left rear motor speed

void setup() {
  Serial.begin(9600); //Start the serial monitor and set baud rate to 9600
  /****All motor control pins are set to output mode***/
  pinMode(M1, OUTPUT);
  pinMode(PWM1, OUTPUT);
  pinMode(M2, OUTPUT);
  pinMode(PWM2, OUTPUT);
  pinMode(M3, OUTPUT);
  pinMode(PWM3, OUTPUT);
  pinMode(M4, OUTPUT);
  pinMode(PWM4, OUTPUT);
  /****All Line Tracking Sensor interface are set to input mode***/
  pinMode(SensorLeft, INPUT);
  pinMode(SensorMiddle, INPUT);
  pinMode(SensorRight, INPUT);

}

void loop() {
  uint8_t SL = digitalRead(SensorLeft);   //Read the value of the left Line Tracking Sensor
  uint8_t SM = digitalRead(SensorMiddle); //Read the value of the left Line Tracking Sensor
  uint8_t SR = digitalRead(SensorRight);  //Read the value of the right Line Tracking Sensor
  if (SM == HIGH) {
    if (SL == LOW && SR == HIGH) {  // black on right, white on left, turn right
      Turn_Right();
    }
    else if (SR == LOW && SL == HIGH) {  // black on left, white on right, turn left
      Turn_Left();
    }
    else {  // white on both sides, going forward
      Advance();
    }
  }
  else {
    if (SL == LOW && SR == HIGH) { // black on right, white on left, turn right
      Turn_Right();
    }
    else if (SR == LOW && SL == HIGH) {  // white on right, black on left, turn left
      Turn_Left();
    }
    else { // all white, stop
      Stop();
    }
  }
}


/**********The car advance***********/
void Advance(void) {
  digitalWrite(M1, LOW);
  analogWrite(PWM1, speed1);
  digitalWrite(M4, LOW);
  analogWrite(PWM4, speed4);

  digitalWrite(M2, LOW);
  analogWrite(PWM2, speed2);
  digitalWrite(M3, LOW);
  analogWrite(PWM3, speed3);
}

/**********The car muves back***********/
void Back(void) {
  digitalWrite(M1, HIGH);
  analogWrite(PWM1, 255 - speed1);
  digitalWrite(M4, HIGH);
  analogWrite(PWM4, 255 - speed4);

  digitalWrite(M2, HIGH);
  analogWrite(PWM2, 255 - speed2);
  digitalWrite(M3, HIGH);
  analogWrite(PWM3, 255 - speed3);
}

/**********The car rotates left***********/
void Turn_Left(void) {
  digitalWrite(M1, LOW);
  analogWrite(PWM1, speed1);
  digitalWrite(M4, LOW);
  analogWrite(PWM4, speed4);

  digitalWrite(M2, HIGH);
  analogWrite(PWM2, 255 - speed2);
  digitalWrite(M3, HIGH);
  analogWrite(PWM3, 255 - speed3);
}

/**********The car rotates right***********/
void Turn_Right(void) {
  digitalWrite(M1, HIGH);
  analogWrite(PWM1, 255 - speed1);
  digitalWrite(M4, HIGH);
  analogWrite(PWM4, 255 - speed4);

  digitalWrite(M2, LOW);
  analogWrite(PWM2, speed2);
  digitalWrite(M3, LOW);
  analogWrite(PWM3, speed3);
}

/**********The car stops***********/
void Stop(void) {
  digitalWrite(M1, LOW);
  analogWrite(PWM1, 0);
  digitalWrite(M2, LOW);
  analogWrite(PWM2, 0);
  digitalWrite(M3, LOW);
  analogWrite(PWM3, 0);
  digitalWrite(M4, LOW);
  analogWrite(PWM4, 0);
}
