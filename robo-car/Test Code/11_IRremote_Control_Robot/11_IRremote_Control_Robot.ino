/*
  Keyestudio 4WD Mecanum Robot for Arduino
  lesson 11
  IRremote Control Robot
  http://www.keyestudio.com
*/

/****Introduce IR Remote Control header file***/
#include <IRremoteTank.h>

int RECV_PIN = A4;
IRrecv irrecv(RECV_PIN);
decode_results results;

/****JM1Motor controls the pins***/
#define PWM1  3
#define M1    2
/****JM4Motor controls the pins***/
#define PWM4  11
#define M4    8

/****JM2Motor controls the pins***/
#define PWM2  5
#define M2    4
/****JM3Motor controls the pins***/
#define PWM3  6
#define M3    7

/******** Speed compensation30***********/
#define speed1   100          //Righr front motor speed
#define speed4   100          //Righr rear motor speed

#define speed2   (speed1+30)  //Left front motor speed
#define speed3   (speed4+30)  //Left rear motor speed

void setup()
{
  Serial.begin(9600);
  /****All motor control pins are set to output mode***/
  pinMode(M1, OUTPUT);
  pinMode(PWM1, OUTPUT);
  pinMode(M2, OUTPUT);
  pinMode(PWM2, OUTPUT);
  pinMode(M3, OUTPUT);
  pinMode(PWM3, OUTPUT);
  pinMode(M4, OUTPUT);
  pinMode(PWM4, OUTPUT);
  // In case the interrupt driver crashes on setup, give a clue
  // to the user what's going on.
  irrecv.enableIRIn(); // Start the receiver
}

void loop() {
  if (irrecv.decode(&results)) {
    switch (results.value)
    {
      case 0xFF629D: Advance(); break;//
      case 0xFFA857: Back();    break;//
      case 0xFF22DD: Turn_Left();  break;//
      case 0xFFC23D: Turn_Right(); break;//
      case 0xFF02FD: Stop();    break;//OK
      case 0xFF30CF: L_Move(); break;//4
      case 0xFF7A85: R_Move(); break;//6
      case 0xFF6897: LU_Move(); break;//1
      case 0xFF10EF: LD_Move(); break;//7
      case 0xFFB04F: RU_Move(); break;//3
      case 0xFF5AA5: RD_Move(); break;//9
      default: break;
    }
    irrecv.resume(); // Receive the next value
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

/**********The car moves back***********/
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

/**********The car moves on the left***********/
void L_Move(void) {
  digitalWrite(M1, LOW);
  analogWrite(PWM1, speed1);
  digitalWrite(M4, HIGH);
  analogWrite(PWM4, 255 - speed4);

  digitalWrite(M2, HIGH);
  analogWrite(PWM2, 255 - speed2);
  digitalWrite(M3, LOW);
  analogWrite(PWM3, speed3);
}


/**********The car moves on the right***********/
void R_Move(void) {
  digitalWrite(M1, HIGH);
  analogWrite(PWM1, 255 - speed1);
  digitalWrite(M4, LOW);
  analogWrite(PWM4, speed4);

  digitalWrite(M2, LOW);
  analogWrite(PWM2, speed2);
  digitalWrite(M3, HIGH);
  analogWrite(PWM3, 255 - speed3);
}

/**********The car moves on the left front***********/
void LU_Move(void) {
  digitalWrite(M1, LOW);
  analogWrite(PWM1, speed1);
  digitalWrite(M4, LOW);
  analogWrite(PWM4, 0);

  digitalWrite(M2, LOW);
  analogWrite(PWM2, 0);
  digitalWrite(M3, LOW);
  analogWrite(PWM3, speed3);
}


/**********The car moves on the left rear***********/
void LD_Move(void) {
  digitalWrite(M1, LOW);
  analogWrite(PWM1, 0);
  digitalWrite(M4, HIGH);
  analogWrite(PWM4, 255 - speed4);

  digitalWrite(M2, HIGH);
  analogWrite(PWM2, 255 - speed2);
  digitalWrite(M3, LOW);
  analogWrite(PWM3, 0);
}


/**********The car moves on the right front***********/
void RU_Move(void) {
  digitalWrite(M1, LOW);
  analogWrite(PWM1, 0);
  digitalWrite(M4, LOW);
  analogWrite(PWM4, speed4);

  digitalWrite(M2, LOW);
  analogWrite(PWM2, speed2);
  digitalWrite(M3, LOW);
  analogWrite(PWM3, 0);
}


/**********The car moves on the right rear***********/
void RD_Move(void) {
  digitalWrite(M1, HIGH);
  analogWrite(PWM1, 255 - speed1);
  digitalWrite(M4, LOW);
  analogWrite(PWM4, 0);

  digitalWrite(M2, LOW);
  analogWrite(PWM2, 0);
  digitalWrite(M3, HIGH);
  analogWrite(PWM3, 255 - speed3);
}
