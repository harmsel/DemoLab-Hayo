#include <Adafruit_NeoPixel.h>

// ── Instellingen ──────────────────────────────────────
#define LED_PIN    A1    // SK6812 data-pin
#define NUM_LEDS  100    // 100 pixels
#define VERTRAGING 500   // ms per LED
// ──────────────────────────────────────────────────────

Adafruit_NeoPixel strip(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

unsigned long startTijd;

void setup() {
  Serial.begin(9600);

  strip.begin();
  strip.setBrightness(255);  // Max helderheid

  // Alle LEDs uitzetten voor we beginnen
  strip.clear();
  strip.show();
  delay(100);

  Serial.println("=== SK6812 · 100 pixels · max helderheid ===");
  Serial.println("Alle LEDs uit. Start over 1 seconde...");
  Serial.println("────────────────────────────────────────────");
  delay(1000);

  startTijd = millis();
}

void loop() {

  // ── Vullen: één voor één aan, eerder aan = blijft aan ─
  for (int i = 0; i < NUM_LEDS; i++) {

    // Volle witte kleur (R+G+B max = zuiver wit op SK6812)
    strip.setPixelColor(i, strip.Color(255, 255, 255));
    strip.show();

    unsigned long ms  = millis() - startTijd;
    unsigned long sec = ms / 1000;
    unsigned long ms3 = ms % 1000;

    Serial.println(i);
    delay(VERTRAGING);
  }

  // ── Alle 100 LEDs branden — timer loopt door ──────────
  Serial.println("────────────────────────────────────────────");
  Serial.println("Alle 100 LEDs aan op volle helderheid!");
  Serial.println("────────────────────────────────────────────");

  while (true) {
    unsigned long ms  = millis() - startTijd;
    unsigned long sec = ms / 1000;
    unsigned long ms3 = ms % 1000;

    Serial.print("[ALLE AAN]  t=");
    Serial.print(sec);
    Serial.print(".");
    if (ms3 < 100) Serial.print("0");
    if (ms3 < 10)  Serial.print("0");
    Serial.println(ms3);

    delay(500);
  }
}