/*
  Keyestudio 4WD Mecanum Robot for Arduino
  lesson 1
  Color LED
  http://www.keyestudio.com
*/
#define  LED   A3

void setup() {
  pinMode(LED, OUTPUT);//Configure 7-color LED pin mode for output
}

void loop() {
  digitalWrite(LED, HIGH);//Turn on the 7-color LED flash
  delay(5000);            //Delay 5000 ms
  digitalWrite(LED, LOW); //Turn off the 7-color LED flash
  delay(1000);            //Delay 1000 ms
}
