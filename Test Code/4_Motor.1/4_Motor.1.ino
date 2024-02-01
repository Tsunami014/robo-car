/*
  Keyestudio 4WD Mecanum Robot for Arduino
  lesson 4.1
  Motor
  http://www.keyestudio.com
*/

/****JM1Motor controls pins***/
#define PWM1  3 //Speed control pin
#define M1    2 //Direction control pin


void setup() {
  /****Set all motor control pins to output mode***/
  pinMode(M1, OUTPUT);
  pinMode(PWM1, OUTPUT);
}

void loop() {
  digitalWrite(M1, LOW);  //Motor rotates clockwise
  analogWrite(PWM1, 100); //Set the speed
  delay(2000);  //Wait for 2 seconds
  
  digitalWrite(M1, LOW);
  analogWrite(PWM1, 0); //Stop when the speed is 0
  delay(1000);
  
  digitalWrite(M1, HIGH);  //Motor rotates counterclockwise
  analogWrite(PWM1, 100);  //Set the speed
  delay(2000);

  digitalWrite(M1, LOW);
  analogWrite(PWM1, 0); //Stop when the speed is 0
  delay(1000);
}
