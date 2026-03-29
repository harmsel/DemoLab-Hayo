#include <Adafruit_NeoPixel.h>

#define NUMPIXELS 120
#define GROEPEN 4  //Groepen van knoppen en leds

// Kikker-leds op strip A1 — na alle keuzes: allemaal ademen (zelfde als globale adem)
#define KIKKER1_EXTRA_START 19  // tegelijk met kikker 1 Kikker bovenin
#define KIKKER1_EXTRA_END 21    // Kikker bovenin

#define KIKKER1_START 44  // kikker onderin start bij led 44
#define KIKKER1_END 50
#define KIKKER2_START 51  // kikker 2 start bij led 51
#define KIKKER2_END 79


// Zone 44–79: tijdens rust geen adem op A1; na alle keuzes ademen die + extra 20–21
#define KIKKER_ZONE_START KIKKER1_START
#define KIKKER_ZONE_END KIKKER2_END

int maxHelderheid = 255;  //deze op 255 zetten om de maximale helderheid te krijgen
// Zachter ademen vóór start (fase 1); na startknop blijft fase 2 op maxHelderheid
const int maxHelderheidAdemFase1 = 90;
/// ADEM EFFECT
float angle = 0;
float pulseSpeed = 2;  //deze aanpassen, op 2 gaat het best oke

const unsigned long timeoutTijd = 5000;  // voordat hij terug gaat naar de ademstand in milliseconden

Adafruit_NeoPixel strip(NUMPIXELS, A0, NEO_GRBW + NEO_KHZ800);
Adafruit_NeoPixel strip1(NUMPIXELS, A1, NEO_GRBW + NEO_KHZ800);

/// KNOPPEN en hun pinnen
int knopA[GROEPEN] = { 11, 9, 7, 5 };  //deze zijn de pinnen van de knoppen A
int knopB[GROEPEN] = { 10, 8, 6, 4 };  //deze zijn de pinnen van de knoppen B

#define BUTTON_START 2  // pin van de startknop


// --======= LEDS EN HUN POSITIE hier staat de ledjes definitie
#define LEDS_PER_KNOP 11
// Startknop-leds op strip A1 — vast aan tijdens ademen tot ergens A of B is ingedrukt; ademen nooit
#define STARTKNOP_LED_START 24
#define STARTKNOP_LED_END 33

int startA[GROEPEN] = { 0, 22, 44, 66 };   // dit zijn de startposities van de A Leds
int startB[GROEPEN] = { 11, 33, 55, 77 };  // dit zijn de startposities van de B Leds


/// FASES
int fase = 1;

/// keuze van de knoppen
int keuze[GROEPEN] = { -1, -1, -1, -1 };

//onthouden voor starten van de timout
unsigned long laatsteInteractie = 0;

// Startknop-leds: vast max aan tijdens ademen; na eerste A/B false (uit bij adem), weer true na timeout→fase1
bool startknopLedsVastAan = true;

bool alleKeuzesGemaakt() {
  for (int i = 0; i < GROEPEN; i++) {
    if (keuze[i] == -1) return false;
  }
  return true;
}

void updateAdemHoek() {
  angle += pulseSpeed / 100.0;
  if (angle > TWO_PI) angle -= TWO_PI;
}

int ademHelderheidMetMax(int maxBr) {
  float intensity = (sin(angle) + 1.0) / 2.0;
  return (int)(intensity * maxBr);
}

int ademHelderheid() {
  return ademHelderheidMetMax(maxHelderheid);
}

void tekenLedBlokHelderheid(int start, int helderheid) {
  uint32_t kleur = strip.Color(helderheid, helderheid, helderheid, helderheid);
  for (int i = 0; i < LEDS_PER_KNOP; i++) {
    strip.setPixelColor(start + i, kleur);
  }
}

/// Alleen knop-leds op A0: A- en B-blok per groep dezelfde adem-helderheid
void ademKnopLedsA0() {
  int br = ademHelderheidMetMax(maxHelderheidAdemFase1);
  strip.clear();
  for (int g = 0; g < GROEPEN; g++) {
    tekenLedBlokHelderheid(startA[g], br);
    tekenLedBlokHelderheid(startB[g], br);
  }
  strip.show();
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
  updateAdemHoek();
  if (fase == 1) {
    ademEffect();
    Serial.println("Fase 1");
  }
  if (fase == 2) {
    ledsBijKnoppen();
    kikkerEffectA1();
    Serial.println("Fase 2");
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
  }

  // de knoppen AB zijn pas actief na het indrukken van start
  if (fase == 2) {
    for (int i = 0; i < GROEPEN; i++) {
      if (digitalRead(knopA[i]) == LOW) {
        keuze[i] = 0;  //0 Dat is keuze A
        laatsteInteractie = millis();
        startknopLedsVastAan = false;
      }
      if (digitalRead(knopB[i]) == LOW) {
        keuze[i] = 1;  //1 Dat is keuze B
        laatsteInteractie = millis();
        startknopLedsVastAan = false;
      }
    }
  }
}


  /// ---------- Knop-leds fase 2: gekozen = vast; nog geen keuze = ademen (A en B tegelijk, geen knipperen)
  void ledsBijKnoppen() {
    int brAdem = ademHelderheid();
    strip.clear();
    for (int g = 0; g < GROEPEN; g++) {
      if (keuze[g] == 0) {
        tekenLedBlok(startA[g]);
      } else if (keuze[g] == 1) {
        tekenLedBlok(startB[g]);
      } else {
        tekenLedBlokHelderheid(startA[g], brAdem);
        tekenLedBlokHelderheid(startB[g], brAdem);
      }
    }
    strip.show();
  }

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
        startknopLedsVastAan = true;
      }
    }
  }

  void ledstrip1uit() {
    strip1.fill(0);  // 0 is de kortste weg naar "alles uit"
    Serial.println("Komt hij hier wel");

    strip.show();
    strip1.show();
  }

  /// ---------- KIKKER EFFECT (strip A1): fase 2; als alle A/B gekozen: alle kikker-leds ademen tegelijk
  void kikkerEffectA1() {
    strip1.clear();
    if (!alleKeuzesGemaakt()) {
      strip1.show();
      return;
    }
    int br = ademHelderheid();
    uint32_t kleur = strip1.Color(br, br, br, br);
    for (int i = KIKKER1_EXTRA_START; i <= KIKKER1_EXTRA_END; i++) {
      strip1.setPixelColor(i, kleur);
    }
    for (int i = KIKKER1_START; i <= KIKKER1_END; i++) {
      strip1.setPixelColor(i, kleur);
    }
    for (int i = KIKKER2_START; i <= KIKKER2_END; i++) {
      strip1.setPixelColor(i, kleur);
    }
    strip1.show();
  }

  /// ---------- ADEM EFFECT (fase 1): knop-leds op A0 ademen; A1 ademt behalve kikker-zone; startknop vast of uit (nooit ademen)
  void ademEffect() {
    int brFase1 = ademHelderheidMetMax(maxHelderheidAdemFase1);
    uint32_t color = strip.Color(brFase1, brFase1, brFase1, brFase1);
    ademKnopLedsA0();
    strip1.fill(color);

    // Kikker 2 blijft uit in fase 1; kikker 1 moet juist mee ademen.
    for (int i = KIKKER2_START; i <= KIKKER2_END; i++) {
      strip1.setPixelColor(i, 0);
    }

    int brKikker1 = ademHelderheidMetMax(maxHelderheid);
    uint32_t kikker1Kleur = strip1.Color(brKikker1, brKikker1, brKikker1, brKikker1);
    for (int i = KIKKER1_EXTRA_START; i <= KIKKER1_EXTRA_END; i++) {
      strip1.setPixelColor(i, kikker1Kleur);
    }
    for (int i = KIKKER1_START; i <= KIKKER1_END; i++) {
      strip1.setPixelColor(i, kikker1Kleur);
    }

    uint32_t startKleur = strip1.Color(maxHelderheid, maxHelderheid, maxHelderheid, maxHelderheid);
    for (int i = STARTKNOP_LED_START; i <= STARTKNOP_LED_END; i++) {
      strip1.setPixelColor(i, startknopLedsVastAan ? startKleur : 0);
    }
    strip1.show();
  }