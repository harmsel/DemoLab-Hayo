#include <elapsedMillis.h>
#include <Adafruit_NeoPixel.h>

// dit heeft de Nano nodig
#ifdef __AVR__
#include <avr/power.h>
#endif

/// LEDSTRIP 
#define NUMPIXELS 200
#define NUMPIXELSWAAK 110

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, A0, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel pixelsWaak = Adafruit_NeoPixel(NUMPIXELSWAAK, A1, NEO_GRB + NEO_KHZ800);

elapsedMillis millisCounterStandby;
