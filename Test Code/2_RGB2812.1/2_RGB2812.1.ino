/*
  Keyestudio 4WD Mecanum Robot for Arduino
  lesson 2.1
  RGB2812
  http://www.keyestudio.com
*/
#include <Adafruit_NeoPixel.h>

//Create a category named rgb_2812 to control 4 LEDs and the pins are connected to D10
Adafruit_NeoPixel rgb_2812 = Adafruit_NeoPixel(4, 10, NEO_GRB + NEO_KHZ800);

void setup() {
  rgb_2812.begin();   //Activate rgb2818
  rgb_2812.setBrightness(100);  //Set the initial brightness to 100,(0~255)
  rgb_2812.show();   //Initialize all NeoPixels to the closed state


  rgb_2812.setPixelColor(0, 255, 0, 0);//The first LED is bright red
  rgb_2812.setPixelColor(1, 0, 255, 0);//The second LED is bright green
  rgb_2812.setPixelColor(2, 0, 0, 255);//The third LED is bright blue
  rgb_2812.setPixelColor(3, 255, 255, 255);//The fourth LED is bright white
  rgb_2812.show();    //Refresh display 
}

void loop() {
}
