#include <Adafruit_NeoPixel.h>

// Instellingen
#define PIN          A0     // Data pin
#define NUMPIXELS    233     // Aantal leds

// LET OP: RGBW strip → NEO_GRBW
Adafruit_NeoPixel strip(NUMPIXELS, PIN, NEO_GRBW + NEO_KHZ800); // nu dus wel met de SK strip ;-)1

void setup() {
  strip.begin();
  strip.show(); // alles uit bij start
}

void loop() {

  // 1. Alle leds aan volgorde is R, G, B, Wit. Zet gerust de eeste 3 uit om te zien wat het verschil is
  for(int i = 0; i < 8; i++) {
    strip.setPixelColor(i, strip.Color(255, 255, 255, 255)); 
  }
  strip.show();
  delay(2000);



  // 4. Alles uit
  strip.clear();
  strip.show();
  delay(1000);
}
