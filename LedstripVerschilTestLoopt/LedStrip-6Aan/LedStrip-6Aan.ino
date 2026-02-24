#include <Adafruit_NeoPixel.h>

#define PIN_SK A0
#define NUM_SK 6

#define PIN_WS A1
#define NUM_WS 6

#define BUTTON_PIN1a 11
#define BUTTON_PIN1b 10
#define BUTTON_PIN2a 9
#define BUTTON_PIN2b 8
#define BUTTON_PIN3a 7
#define BUTTON_PIN3b 6
#define BUTTON_PIN4a 5
#define BUTTON_PIN4b 4




#define BLOCK_SIZE 4  

Adafruit_NeoPixel stripSK(NUM_SK, PIN_SK, NEO_GRBW + NEO_KHZ800);
Adafruit_NeoPixel stripWS(NUM_WS, PIN_WS, NEO_GRB + NEO_KHZ800);

void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  stripSK.begin();
  stripWS.begin();

  stripSK.setBrightness(255);
  stripWS.setBrightness(255);

  stripSK.show();
  stripWS.show();
}

void loop() {
  // De animaties draaien constant, tenzij de knop wordt ingedrukt
  movingBlock(stripSK.Color(255, 0, 0, 0), stripWS.Color(255, 0, 0), 400);
  movingBlock(stripSK.Color(0, 255, 0, 0), stripWS.Color(0, 255, 0), 400);
  movingBlock(stripSK.Color(0, 0, 0, 255), stripWS.Color(255, 255, 255), 400);
  movingBlock(stripSK.Color(255, 255, 255, 255), stripWS.Color(255, 255, 255), 400);
}

// Functie die alle leds wit maakt en daar blijft zolang je de knop inhoudt
void makeEverythingWhite() {
  // SK6812: Alleen het witte kanaal (0,0,0,255) of alles (255,255,255,255)
  stripSK.fill(stripSK.Color(0, 0, 0, 255), 0, NUM_SK); 
  stripWS.fill(stripWS.Color(255, 255, 255), 0, NUM_WS);
  
  stripSK.show();
  stripWS.show();

  // Blijf in deze 'wit-modus' zolang de knop is ingedrukt
  while (digitalRead(BUTTON_PIN) == LOW) {
    delay(10); 
  }
}

void movingBlock(uint32_t colorSK, uint32_t colorWS, int wait) {
  for (int i = 0; i < NUM_SK + BLOCK_SIZE; i++) {
    
    // CHECK DE KNOP: Als deze wordt ingedrukt, ga naar de witte modus
    if (digitalRead(BUTTON_PIN) == LOW) {
      makeEverythingWhite();
      return; // Stop de huidige animatie en begin opnieuw na loslaten
    }

    stripSK.clear();
    stripWS.clear();

    for (int j = 0; j < BLOCK_SIZE; j++) {
      int ledPos = i - j;
      if (ledPos >= 0 && ledPos < NUM_SK) {
        stripSK.setPixelColor(ledPos, colorSK);
        stripWS.setPixelColor(ledPos, colorWS);
      }
    }

    stripSK.show();
    stripWS.show();

    // Vervanging van delay(wait) om de knop vaker te kunnen pollen
    for (int d = 0; d < wait; d += 10) {
      if (digitalRead(BUTTON_PIN) == LOW) {
        makeEverythingWhite();
        return;
      }
      delay(10);
    }
  }
}