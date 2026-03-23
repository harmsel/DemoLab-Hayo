#include <Adafruit_NeoPixel.h>


// Instellingen
#define PIN          A2    // De datalijn (DI) aangesloten op A0
#define NUMPIXELS   80    // Het aantal leds op je strip

// Initialiseer de strip. 
// Voor WS2813 gebruiken we 'NEO_GRB'.
Adafruit_NeoPixel strip(NUMPIXELS, PIN, NEO_GRBW + NEO_KHZ800);

void setup() {
  strip.begin();           
  strip.show();            // Alles uit bij de start
}

void loop() {
  // 1. Alle leds aan op vol vermogen (Wit gemaakt door RGB)
  // Omdat WS2813 geen aparte W-kanaal heeft, gebruiken we 3 parameters.
  for(int i=0; i<NUMPIXELS; i++) {
    strip.setPixelColor(i, strip.Color(255, 255, 255)); 
  }
  strip.show();
  delay(2000);

  // 2. Alles uit voor 1 seconde
strip.clear();
 strip.show();
delay(1000);


}