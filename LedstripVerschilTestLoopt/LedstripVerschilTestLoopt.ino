#include <Adafruit_NeoPixel.h>

// Instellingen SK6812 (Pin A0)
#define PIN_SK A0
#define NUM_SK 30

// Instellingen WS2812B (Pin A1)
#define PIN_WS A1
#define NUM_WS 30

// Initialiseer beide strips
// SK6812 gebruikt NEO_GRBW (4 kleuren per led)
Adafruit_NeoPixel stripSK(NUM_SK, PIN_SK, NEO_GRBW + NEO_KHZ800);

// WS2812B gebruikt NEO_GRB (3 kleuren per led)
Adafruit_NeoPixel stripWS(NUM_WS, PIN_WS, NEO_GRB + NEO_KHZ800);

void setup() {
  stripSK.begin();
  stripWS.begin();

  stripSK.setBrightness(255);// max is 255
  stripWS.setBrightness(255);

  stripSK.show();
  stripWS.show();
}


void loop() {
  // Effect op de SK6812 (A0) - inclusief WIT
  colorChase(&stripSK, stripSK.Color(255, 0, 0, 0), 100);
  colorChase(&stripSK, stripSK.Color(0, 255, 0, 0), 100);
  colorChase(&stripSK, stripSK.Color(0, 0, 255, 0), 100);
  colorChase(&stripSK, stripSK.Color(0, 0, 0, 255), 100);
  colorChase(&stripSK, stripSK.Color(255, 255, 255, 255), 100);


  // Effect op de WS2812B (A1) - Alleen RGB
  colorChase(&stripWS, stripWS.Color(255, 0, 0), 100);
  colorChase(&stripWS, stripWS.Color(0, 255, 0), 100);
  colorChase(&stripWS, stripWS.Color(0, 0, 255), 100);
  colorChase(&stripWS, stripWS.Color(255, 255, 255), 100);
}

// Een universele functie die beide soorten strips kan aansturen
void colorChase(Adafruit_NeoPixel *s, uint32_t color, int wait) {
  for (int i = 0; i < s->numPixels(); i++) {
    s->setPixelColor(i, color);
    s->show();
    delay(wait);
    s->setPixelColor(i, 0);
  }
}