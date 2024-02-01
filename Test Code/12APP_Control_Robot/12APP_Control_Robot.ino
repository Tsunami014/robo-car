/*
  Keyestudio 4WD Mecanum Robot for Arduino
  lesson 12.2
  APP Control Robot
  http://www.keyestudio.com
*/

#include <Adafruit_NeoPixel.h>
#include <Servo.h>
Servo myservo;    //Define an instance of a servo

//Create a category named rgb_2812 to control 4 LEDs and the pins are connected to D10
Adafruit_NeoPixel rgb_2812 = Adafruit_NeoPixel(4, 10, NEO_GRB + NEO_KHZ800);

/*******Define the pin of Line Tracking Sensor**********/
#define SensorLeft    A0   //input pin of left sensor
#define SensorMiddle  A1   //input pin of middle sensor
#define SensorRight   A2   //input pin of right sensor

#define  LED   A3

/*******Ultrasonic Sensor interface*****/
#define EchoPin  13  //ECHO to D13
#define TrigPin  12  //TRIG to D12

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

/********默认速度补偿30***********/
uint8_t speed1 = 100;      //Right front motor speed
uint8_t speed4 = 100;      //Right rear motor speed

uint8_t speed2 = 130;      //Left front motor speed
uint8_t speed3 = 130;      //Left rear motor speed

String speed1_str, speed2_str, speed3_str, speed4_str; ///string type variable

float distance_M, distance_L, distance_R;
char ble_val;
int color_num = 0;


void setup() {
  Serial.begin(9600);//Start the serial monitor and set baud rate to 9600
  myservo.attach(9);    //The pin of the servo is connected to D9 
  pinMode(LED, OUTPUT);//Configure 7-color LED pins mode for output
  /****All motor control pins are set to output mode***/
  pinMode(M1, OUTPUT);
  pinMode(PWM1, OUTPUT);
  pinMode(M2, OUTPUT);
  pinMode(PWM2, OUTPUT);
  pinMode(M3, OUTPUT);
  pinMode(PWM3, OUTPUT);
  pinMode(M4, OUTPUT);
  pinMode(PWM4, OUTPUT);

  /****All interfaces of the Line Tracking Sensor are set to input mode***/
  pinMode(SensorLeft, INPUT);
  pinMode(SensorMiddle, INPUT);
  pinMode(SensorRight, INPUT);

  pinMode(EchoPin, INPUT);    //The ECHO pin is set to input mode
  pinMode(TrigPin, OUTPUT);   //The TRIG pin is set to output mode

  rgb_2812.begin();   //Start rgb2818
  rgb_2812.setBrightness(255);  //Initial brightness(0~255)
  rgb_2812.show();   //Initialize all NeoPixels to the closed state

}


/***********main loop***************/
void loop() {
  if (Serial.available())//If the receiving area is not empty
  {
    ble_val = Serial.read();//Read Bluetooth data
    Serial.println(ble_val);//The serial prints the read data
    switch (ble_val)
    {
      /*********************小车行驶************************/
      case 's': Stop();       break;  //Stop
      case 'a': Advance();    break;  //Go ahead
      case 'c': Back();       break;  //retreat
      case 'b': Turn_Left();  break;  //Turn left
      case 'd': Turn_Right(); break;  //Turn right
      case 'k': L_Move();     break;  //Left shift
      case 'h': R_Move();     break;  //Right shift
      case 'l': LU_Move();    break;  //Upper left move
      case 'j': LD_Move();    break;  //lower left move
      case 'g': RU_Move();    break;  //Upper right move
      case 'i': RD_Move();    break;  //lower right move
      case 'e': drift_left(); break;  //Drift
      case 'f': drift_right(); break; //Drift

      case 'p': Tracking();   break;  //Tracking
      case 'q': Follow();     break;  //Follow
      case 'r': Avoidance();  break;  //Obstacle Avoidance


      /*********************The car changes speed*************************/
      case 't': digitalWrite(LED, HIGH);  break;  //Open the 7-color LED
      case 'u': digitalWrite(LED, LOW);   break;  //Close the 7-color LED
      case 'm': color_num++; showColor(); break;  //Switching the color of 2812 LED
      case 'o': closeColor(); break;  //Close the 2812 LED
      case 'n': color_num--; showColor(); break;  //Switching the color of 2812 LED

      /*********************The car changes speed*************************/
      case 'v':    /*Read left front motor M2 speed*/
        Serial.readStringUntil('#');  //Failed to read data the first time
        speed2_str = Serial.readStringUntil('#');  //Failed to read data the first time
        speed2 = String(speed2_str).toInt();     //The speed value is a string and needs to be converted to an integer
        speed2 = map(speed2, 0, 100, 0, 255);  //Mapping from 0 to 100 to 0 to 255
        //Serial.println(speed2);  //Serial debugging
        delay(100); 
        break;
      case 'w':   /*Read left rear M3 motor speed*/
        Serial.readStringUntil('#');
        speed3_str = Serial.readStringUntil('#');
        speed3 = String(speed3_str).toInt();
        speed3 = map(speed3, 0, 100, 0, 255);
        //Serial.println(speed3);
        delay(100);
        break;
      case 'x':   /*Read right front M1 motor speed*/
        Serial.readStringUntil('#');
        speed1_str = Serial.readStringUntil('#');
        speed1 = String(speed1_str).toInt();
        speed1 = map(speed1, 0, 100, 0, 255);
        //Serial.println(speed1);
        delay(100);
        break;
      case 'y':   /*Read right rear M4 motor speed*/
        Serial.readStringUntil('#');
        speed4_str = Serial.readStringUntil('#');
        speed4 = String(speed4_str).toInt();
        speed4 = map(speed4, 0, 100, 0, 255);
        //Serial.println(speed4);
        delay(100);
        break;

      default: break;
    }
  }
}


/*********************Obstacle avoidance*******************************/
void Avoidance(void)
{
  while (1)
  {
    distance_M = Get_Distance();   //Get the distance and save in the distance variable  
    if (distance_M < 20) {//When the distance in front is less than 20cm  
      Stop();  //Robot stop
      delay(500); //Delay 500ms
      myservo.write(180);  //Ultrasonic cradle head turns left
      delay(500); //延时500ms
      distance_L = Get_Distance();  //Assign the left ultrasonic distance to variable a1
      delay(100); //Read values when stable
      myservo.write(0); //Ultrasonic cradle head turns right
      delay(500); //Delay 500ms
      distance_R = Get_Distance(); //Assign the right ultrasonic distance to variable a2
      delay(100);  //Read values when stable

      myservo.write(90);  //Return to the 90 degree position
      delay(500);
      if (distance_L > distance_R) { //When the distance on the left is greater than the distance on the right
        Turn_Left();  //Robot turns left 
        delay(300);  //Turn left 700 ms
      } else {
        Turn_Right(); //Robot turns right
        delay(300);
      }
    }
    else { //If the distance in front is >=20cm, the robot will go ahead
      Advance(); //Go ahead
    }
    ble_val = Serial.read();//Read the data of the bluetooth
    if (ble_val == 's') {
      Stop();
      break;
    }
  }
}

/*********************Follow*******************************/
void Follow(void)
{
  while (1)
  {
    float distance = Get_Distance();  //Get the distance and save in the distance variable 
    if (distance >= 20 && distance <= 40) //Range of advance  
    {
      Advance();
    }
    else if (distance > 10 && distance < 20)  //Range of stop
    {
      Stop();
    }
    else if (distance <= 10)  //Range of fall back
    {
      Back();
    }
    else  //Other cases stop
    {
      Stop();
    }
    ble_val = Serial.read();//Read the data of the bluetooth
    if (ble_val == 's') {
      Stop();
      break;
    }
  }
}

/*********************Line Tracking*******************************/
void Tracking(void) {   //Tracking black line
  while (1)
  {
    uint8_t SL = digitalRead(SensorLeft);   //Read the value of the left Line Tracking Sensor
    uint8_t SM = digitalRead(SensorMiddle); //Read the value of the intermediate Line Tracking Sensor
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
    ble_val = Serial.read();//Read the data of the bluetooth
    if (ble_val == 's') {
      Stop();
      break;
    }
  }
}

/*********************Altrasonic detects the distance *******************************/
float Get_Distance(void) {    //Altrasonic detects the distance 
  float dis;
  digitalWrite(TrigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(TrigPin, HIGH); //Give the TRIG a high level at least 10 µ s to trigger
  delayMicroseconds(10);
  digitalWrite(TrigPin, LOW);
  dis = pulseIn(EchoPin, HIGH) / 58.2; //Work out the distance
  delay(50);
  return dis;
}

/*********************RGB2812 display*******************************/
void showColor() {
  //  Serial.print("color num:"); //Serial debugging
  //  Serial.println(color_num);
  //  There are only 7 colors, you can add them yourself
  if (color_num > 6)color_num = 0;
  if (color_num < 0)color_num = 6;
  switch (color_num) {
    case  0:
      for (int i = 0; i < 4; i++) {
        rgb_2812.setPixelColor(i, 255, 0, 0);  //The iLED is red
      }
      rgb_2812.show();                        //Display
      break;
    case  1:
      for (int i = 0; i < 4; i++) {
        rgb_2812.setPixelColor(i, 255, 80, 0); //The iLED is orange
      }
      rgb_2812.show();                        //Display
      break;
    case  2:
      for (int i = 0; i < 4; i++) {
        rgb_2812.setPixelColor(i, 255, 255, 0); //The iLED is yellow
      }
      rgb_2812.show();                        //Display
      break;
    case  3:
      for (int i = 0; i < 4; i++) {
        rgb_2812.setPixelColor(i, 0, 255, 0);   //The iLED is green
      }
      rgb_2812.show();                        //Display
      break;
    case  4:
      for (int i = 0; i < 4; i++) {
        rgb_2812.setPixelColor(i, 0, 0, 255);   //The iLED is blue
      }
      rgb_2812.show();                        //Display
      break;
    case  5:
      for (int i = 0; i < 4; i++) {
        rgb_2812.setPixelColor(i, 0, 255, 255); //The iLED is indigo
      }
      rgb_2812.show();                        //Display
      break;
    case  6:
      for (int i = 0; i < 4; i++) {
        rgb_2812.setPixelColor(i, 160, 32, 240);//The iLED is purple
      }
      rgb_2812.show();                        //Display
      break;
    default : break;
  }
}

/*********************Close RGB2812*******************************/
void closeColor() {                  //Close aii 2818 LEDs
  for (int i = 0; i < 4; i++) {
    rgb_2812.setPixelColor(i, 0, 0, 0);  //The iLED goes off
    rgb_2812.show();                 //Execute display
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

/**********The car back***********/
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

/**********The car rotates on the left***********/
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

/**********The car rotates on the right***********/
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

/**********The car moves to the left***********/
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


/**********The car moves to the right***********/
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

/**********The car moves to the front***********/
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


/**********The car moves back***********/
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


/**********The car moves to the right front***********/
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


/**********The car moves to the right rear***********/
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

/**********The car drifts left***********/
void drift_left(void) {
  digitalWrite(M1, LOW);
  analogWrite(PWM1, 0);
  digitalWrite(M4, LOW);
  analogWrite(PWM4, speed4);

  digitalWrite(M2, LOW);
  analogWrite(PWM2, 0);
  digitalWrite(M3, HIGH);
  analogWrite(PWM3, 255 - speed3);
}


/**********The car drifts right***********/
void drift_right(void) {
  digitalWrite(M1, LOW);
  analogWrite(PWM1, 0);
  digitalWrite(M4, HIGH);
  analogWrite(PWM4, 255 - speed4);

  digitalWrite(M2, LOW);
  analogWrite(PWM2, 0);
  digitalWrite(M3, LOW);
  analogWrite(PWM3, speed3);
}
