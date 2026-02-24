#include <elapsedMillis.h>
#include <Adafruit_NeoPixel.h>
#include <CapacitiveSensor.h>

// dit heeft de Nano Nodig
#ifdef __AVR__
#include <avr/power.h>
#endif

elapsedMillis standByKnipper;
elapsedMillis timer;

const int knop = 2;//drukknop
CapacitiveSensor   sensCap = CapacitiveSensor(3, 4); // laatste pin = sensor

bool eindeScan = true;

#define NUMPIXELS 160 //2,4 meter strip * 60 leds = 144 leds

//#define NUMPIXELS 60 ////VOOR TESTEN
Adafruit_NeoPixel strip(NUMPIXELS, A0, NEO_GRB + NEO_KHZ800);
