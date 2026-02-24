//#include <elapsedMillis.h>
#include <Adafruit_NeoPixel.h>

// dit heeft de nano nodig voor de lestrips
#ifdef __AVR__
#include <avr/power.h>
#endif

#define NUMPIXELS 60 //de tafel is 1 meter, dus 60 leds

Adafruit_NeoPixel strip[4] = {

  Adafruit_NeoPixel(NUMPIXELS, A0, NEO_GRB + NEO_KHZ800),
  Adafruit_NeoPixel(NUMPIXELS, A1, NEO_GRB + NEO_KHZ800),
  Adafruit_NeoPixel(NUMPIXELS, A2, NEO_GRB + NEO_KHZ800),
  Adafruit_NeoPixel(NUMPIXELS, A3, NEO_GRB + NEO_KHZ800)
};
int butPins[8] =  {6, 7,  8,  9,  4, 5, 2, 3};

#include "KT403A_Player.h"
#include <SoftwareSerial.h>
SoftwareSerial SoftSerial(12, 11); // Gele draad, witte draad
KT403A<SoftwareSerial> MP3Player;

uint32_t wit = 0xFFFFFF;
uint32_t blauw = 0x0000FF;
uint32_t groen = 0x00FF00;
uint32_t oranje = 0xFF4400;
uint32_t geel = 0xFFBF00;
uint32_t lichtgroen = 0x88FF88;
uint32_t rood = 0xFF0000;
