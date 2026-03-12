#include <Adafruit_NeoPixel.h>

#define NUMPIXELS 12
#define GROEPEN 4  //dit moeten er dus 4 worden...

int maxHelderheid = 20;
/// ADEM EFFECT
float angle = 0;
float pulseSpeed = 0.3;  //deze aanpassen, op 0.3 gaat het best oke

const unsigned long timeoutTijd = 50000;  // in milliseconden

Adafruit_NeoPixel strip(NUMPIXELS, A0, NEO_GRBW + NEO_KHZ800);
Adafruit_NeoPixel strip1(NUMPIXELS, A1, NEO_GRBW + NEO_KHZ800);

/// KNOPPEN en hun pinnen
#define GROEPEN 4  // aantal keuzes
int knopA[GROEPEN] = { 11, 9, 7, 5 };
int knopB[GROEPEN] = { 10, 8, 6, 4 };

#define BUTTON_START 2  // pin van de startknop


// --======= LEDS EN HUN POSITIE hier staat de ledjes definitie
#define LEDS_PER_KNOP 2
int startA[GROEPEN] = { 0, 4, 10, 30 };  // dit zijn de startposities van de A Leds
int startB[GROEPEN] = { 2, 6, 20, 40 };  // dit zijn de startposities van de B Leds


/// BLINK STATE
int blinkFase = 0;
unsigned long laatsteBlink = 0;
const int blinkInterval = 1000;

/// FASES
int fase = 1;

/// keuze van de knoppen
int keuze[GROEPEN] = { -1, -1, -1, -1 };

//onthouden voor starten van de timout
unsigned long laatsteInteractie = 0;

void setup() {
  strip.begin();
  strip.show();
  strip1.begin();
  strip1.show();
  pinMode(BUTTON_START, INPUT_PULLUP);
  for (int i = 0; i < GROEPEN; i++) {
    pinMode(knopA[i], INPUT_PULLUP);
    pinMode(knopB[i], INPUT_PULLUP);
  }
  Serial.begin(115200);
}

void loop() {
  if (fase == 1) {
    ademEffect();
    Serial.println("Fase 1");
  }
  if (fase == 2) {
    ledstrip1uit();
    updateBlink();
    ledsBijKnoppen();
    Serial.println("Fase 2");
    // zet ledstrip 1 uit
  }
  leesKnoppen();
  checkTimeout();
}

/// ----------------------- KNOPPEN
void leesKnoppen() {
  if (digitalRead(BUTTON_START) == LOW) {  /// STARTKNOP
    fase = 2;
    laatsteInteractie = millis();  // voor de ademtimer
    for (int i = 0; i < GROEPEN; i++) {
      keuze[i] = -1;
    }
    blinkFase = 0;
  }
  for (int i = 0; i < GROEPEN; i++) {  // A EN B KNOPPEN
    if (digitalRead(knopA[i]) == LOW) {
      keuze[i] = 0;
      laatsteInteractie = millis();  // voor de ademtimer
    }
    if (digitalRead(knopB[i]) == LOW) {
      keuze[i] = 1;
      laatsteInteractie = millis();  // voor de ademtimer
    }
  }
}


/// ---------- ledsBijKnoppen A en B die knipperen
void updateBlink() {
  if (millis() - laatsteBlink > blinkInterval) {
    laatsteBlink = millis();
    blinkFase++;
    if (blinkFase > 1) blinkFase = 0;  //als je blinkFase>2 doet gaan de knoppen tussendoor even uit
  }
}

/// ---------- Hiermee zet je de leds bij de knoppen aan en knipperen natuurlijk
void ledsBijKnoppen() {
  strip.clear();
  for (int g = 0; g < GROEPEN; g++) {
    if (keuze[g] == 0) {
      tekenLedBlok(startA[g]);
    } else if (keuze[g] == 1) {
      tekenLedBlok(startB[g]);
    } else {
      if (blinkFase == 0) {
        tekenLedBlok(startA[g]);
      }
      if (blinkFase == 1) {
        tekenLedBlok(startB[g]);
      }
    }
  }
  strip.show();
}

// Functie zodat we centraal de led locaties kunnen definieeeren
void tekenLedBlok(int start) {
  uint32_t kleur = strip.Color(maxHelderheid, maxHelderheid, maxHelderheid, maxHelderheid);
  for (int i = 0; i < LEDS_PER_KNOP; i++) {
    strip.setPixelColor(start + i, kleur);
  }
}

/// timeout, dan gaat hij wer terug naar ademeen
void checkTimeout() {
  if (fase == 2) {
    if (millis() - laatsteInteractie > timeoutTijd) {
      Serial.println("Timeout → terug naar ademstand");
      // keuzes wissen
      for (int i = 0; i < GROEPEN; i++) {
        keuze[i] = -1;
      }
      fase = 1;
    }
  }
}

void ledstrip1uit() {
  strip1.fill(0);  // 0 is de kortste weg naar "alles uit"
  Serial.println("Komt hij hier wel");

  strip.show();
  strip1.show();
}

/// ---------- ADEM EFFECT
void ademEffect() {
  float intensity = (sin(angle) + 1.0) / 2.0;
  int brightness = intensity * maxHelderheid;
  uint32_t color = strip.Color(brightness, brightness, brightness, brightness);
  strip.fill(color);
  strip1.fill(color);

  strip.show();
  strip1.show();
  angle += pulseSpeed / 100.0;
  if (angle > TWO_PI) angle -= TWO_PI;
}