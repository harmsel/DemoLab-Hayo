#include <Adafruit_NeoPixel.h>

#define PIN A0
#define NUM 6          // Het aantal leds op je strip
int maxHelderheid = 50; // De maximale sterkte van het ademen (0-255)

// Maak de strip aan (GRBW voor jouw strip)
Adafruit_NeoPixel strip(NUM, PIN, NEO_GRBW + NEO_KHZ800);

void setup() {
  strip.begin();
  strip.setBrightness(255); // We regelen de felheid via de kleuren, niet via de globale brightness
  strip.show(); 
}

void loop() {
  ademEffect(0.002); // 0.002 is een mooie natuurlijke snelheid
}

// ------------------========= Functies =========------------ //

void ademEffect(float snelheid) {
  // Gebruik millis() voor een vloeiende beweging die nooit schokt
  float fase = millis() * snelheid;
  
  // Bereken de intensiteit met een sinus (geeft een waarde tussen 0.0 en 1.0)
  float intensiteit = (sin(fase) + 1.0) / 2.0;
  
  // Bereken de actuele waarde op basis van je maxHelderheid
  int v = intensiteit * maxHelderheid;

  // Kleur instellen
  // strip.Color(R, G, B, W)
  // Nu staat hij op Wit (W-kanaal), verander naar (v, 0, 0, 0) voor Rood
  uint32_t kleur = strip.Color(v, v, v, v); 

  for (int i = 0; i < NUM; i++) {
    strip.setPixelColor(i, kleur);
  }
  
  strip.show();
  
  // Een hele kleine pauze voor de stabiliteit van de controller
  delay(10); 
}