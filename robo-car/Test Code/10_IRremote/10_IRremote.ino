/*
  Keyestudio 4WD Mecanum Robot for Arduino
  lesson 10
  IRremote
  http://www.keyestudio.com
*/

/****JM1Motor controls pins***/
#include <IRremoteTank.h>

int RECV_PIN = A4;
IRrecv irrecv(RECV_PIN);
decode_results results;

void setup()
{
  Serial.begin(9600);
  // In case the interrupt driver crashes on setup, give a clue
  // to the user what's going on.
  Serial.println("Enabling IRin");
  irrecv.enableIRIn(); // Start the receiver
  Serial.println("Enabled IRin");
}

void loop() {
  if (irrecv.decode(&results)) {
    Serial.println(results.value, HEX);
    irrecv.resume(); // Receive the next value
  }
  delay(100);
}
