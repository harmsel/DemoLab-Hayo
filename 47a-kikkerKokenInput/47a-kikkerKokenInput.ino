#include <Adafruit_NeoPixel.h>

#define NUMPIXELS 120
#define GROEPEN 4  //Groepen van knoppen en leds

// Kikker-leds op strip A1 — alleen twee kikkers; knipperen alleen als alle keuzes gemaakt zijn
#define KIKKER1_START 44
#define KIKKER1_END 50
#define KIKKER2_START 51
#define KIKKER2_END 79
// Gezamenlijke zone (adem: hier geen licht; knipper: alleen bij alle keuzes)
#define KIKKER_ZONE_START KIKKER1_START
#define KIKKER_ZONE_END KIKKER2_END

int maxHelderheid = 255; //deze op 255 zetten om de maximale helderheid te krijgen
/// ADEM EFFECT
float angle = 0;
float pulseSpeed = 2;  //deze aanpassen, op 2 gaat het best oke

const unsigned long timeoutTijd = 5000;  // voordat hij terug gaat naar de ademstand in milliseconden

Adafruit_NeoPixel strip(NUMPIXELS, A0, NEO_GRBW + NEO_KHZ800);
Adafruit_NeoPixel strip1(NUMPIXELS, A1, NEO_GRBW + NEO_KHZ800);

/// KNOPPEN en hun pinnen
int knopA[GROEPEN] = { 11, 9, 7, 5 }; //deze zijn de pinnen van de knoppen A
int knopB[GROEPEN] = { 10, 8, 6, 4 }; //deze zijn de pinnen van de knoppen B

#define BUTTON_START 2  // pin van de startknop


// --======= LEDS EN HUN POSITIE hier staat de ledjes definitie
#define LEDS_PER_KNOP 11
int startA[GROEPEN] = { 0, 22, 44, 66 };  // dit zijn de startposities van de A Leds
int startB[GROEPEN] = { 11, 33, 55, 77 };  // dit zijn de startposities van de B Leds


/// De A en B knoppen gaan omstebeurt aan en uit
int blinkFase = 0;
unsigned long laatsteBlink = 0;
const int blinkInterval = 1000; //zo lang blijft elke knopserie aan 

// Kikker-effect (A1) knippert sneller dan knoppen
int kikkerBlinkFase = 0;
unsigned long laatsteKikkerBlink = 0;
const int kikkerBlinkInterval = 150; // ms

/// FASES
int fase = 1;

/// keuze van de knoppen
int keuze[GROEPEN] = { -1, -1, -1, -1 };

//onthouden voor starten van de timout
unsigned long laatsteInteractie = 0;

bool alleKeuzesGemaakt() {
  for (int i = 0; i < GROEPEN; i++) {
    if (keuze[i] == -1) return false;
  }
  return true;
}

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
    updateBlink();
    ledsBijKnoppen();
    kikkerEffectA1();
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

/// ---------- KIKKER EFFECT (strip A1): alleen in fase 2; knipperen alleen bij alle keuzes
void kikkerEffectA1() {
  if (millis() - laatsteKikkerBlink > kikkerBlinkInterval) {
    laatsteKikkerBlink = millis();
    kikkerBlinkFase = 1 - kikkerBlinkFase;
  }

  uint32_t kikkerKleur = strip1.Color(maxHelderheid, maxHelderheid, maxHelderheid, maxHelderheid);

  strip1.clear();
  if (!alleKeuzesGemaakt()) {
    strip1.show();
    return;
  }
  // Twee kikkers om en om
  if (kikkerBlinkFase == 0) {
    for (int i = KIKKER1_START; i <= KIKKER1_END; i++) {
      strip1.setPixelColor(i, kikkerKleur);
    }
  } else {
    for (int i = KIKKER2_START; i <= KIKKER2_END; i++) {
      strip1.setPixelColor(i, kikkerKleur);
    }
  }
  strip1.show();
}

/// ---------- ADEM EFFECT
void ademEffect() {
  float intensity = (sin(angle) + 1.0) / 2.0;
  int brightness = intensity * maxHelderheid;
  uint32_t color = strip.Color(brightness, brightness, brightness, brightness);
  strip.fill(color);
  strip1.fill(color);
  // Kikker-zone op A1 blijft uit tijdens ademen
  for (int i = KIKKER_ZONE_START; i <= KIKKER_ZONE_END; i++) {
    strip1.setPixelColor(i, 0);
  }

  strip.show();
  strip1.show();
  angle += pulseSpeed / 100.0;
  if (angle > TWO_PI) angle -= TWO_PI;
}