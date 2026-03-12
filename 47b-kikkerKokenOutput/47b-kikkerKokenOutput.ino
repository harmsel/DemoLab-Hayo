#include <Adafruit_NeoPixel.h>
int maxHelderheid = 20;

#define NUM_KNOP_LEDS 120    // totaal aantal leds op deze strip
#define NUM_PAN_LEDS 120     // totaal aantal leds op deze strip
#define NUM_EFFECT_LEDS 120  // totaal aantal leds op deze strip


#define STEAM_PIXELS 120  // tot hier is het stoom effect

#define GROEPEN 5  // hoeveel knoppen en groepjes ledjes zijn er
#define LEDS_PER_KNOP 2


int panStart[GROEPEN] = { 0, 20, 40, 60, 80 };


#define LEDS_PER_PAN 2
int huidigePan = -1;

/// stoom
#define STEAM_PARTICLES 12
#define STEAM_FADE 0.82
#define STEAM_SPEED_MIN 1  // hier kun je mee rommelen
#define STEAM_SPEED_MAX 2  // hier kun je mee rommelen

float steamPos[STEAM_PARTICLES];
int steamSpeed[STEAM_PARTICLES];
int steamDrift[STEAM_PARTICLES];
int steamBrightness[STEAM_PARTICLES];


#define BUBBLE_PIXELS 4  // onderste deel waar bubbels ontstaan
#define BUBBLE_COUNT 2   // aantal actieve bubbels

int bubblePos[BUBBLE_COUNT];
int bubbleSize[BUBBLE_COUNT];
int bubbleLife[BUBBLE_COUNT];

/// adem effect
float angle = 0;
float pulseSpeed = 2;  // voor langere strips getal hoger maken. 2 werkt oke

/// timeout
const unsigned long timeoutTijd = 5000;

/// LEDSTRIPS
Adafruit_NeoPixel stripKnoppen(NUM_KNOP_LEDS, A0, NEO_GRBW + NEO_KHZ800);
Adafruit_NeoPixel stripPannen(NUM_PAN_LEDS, A1, NEO_GRBW + NEO_KHZ800);
Adafruit_NeoPixel stripEffect(NUM_EFFECT_LEDS, A2, NEO_GRBW + NEO_KHZ800);

/// pinnen van de KNOPPEN
int knop[GROEPEN] = { 11, 10, 9, 8, 7 };

/// start Led positie van de Ledstrip voor knopverlichting
int startLed[GROEPEN] = { 0, 2, 4, 6 };

/// keuze
int keuze = -1;

/// fase
int fase = 1;

unsigned long laatsteInteractie = 0;

/// zoeklicht
bool zoekActief = false;
int zoekIndex = 0;
int zoekInterval = 200;
unsigned long laatsteZoek = 0;

//bubbels
unsigned long laatsteBubble = 0;

/// stoom animatie
unsigned long laatsteSteam = 0;
int steamOffset = 0;

////////////// ////////////// ////////////// ////////////// ////////////// ////////
//////////////         SETUP         ////////////////////////////////////////////
////////////// ////////////// ////////////// ////////////// ////////////// ////////
void setup() {
  stripKnoppen.begin();
  stripKnoppen.show();
  stripPannen.begin();
  stripPannen.show();
  stripEffect.begin();
  stripEffect.show();
  for (int i = 0; i < GROEPEN; i++) {
    pinMode(knop[i], INPUT_PULLUP);
  }
  Serial.begin(115200);


  /// stoom
  randomSeed(analogRead(A5));  // lees pin A5 voor random waarden
  for (int i = 0; i < STEAM_PARTICLES; i++) {
    steamPos[i] = BUBBLE_PIXELS + random(0, 10);
    steamSpeed[i] = random(STEAM_SPEED_MIN, STEAM_SPEED_MAX);
    steamDrift[i] = random(-1, 2);
    steamBrightness[i] = random(maxHelderheid / 2, maxHelderheid);
  }

  /// bubbels
  for (int i = 0; i < BUBBLE_COUNT; i++) {
    bubblePos[i] = random(0, BUBBLE_PIXELS);
    bubbleSize[i] = 1;
    bubbleLife[i] = random(3, 8);
  }
}

////////////// ////////////// ////////////// ////////////// ////////////// ////////
//////////////            LOOP IE LOOP          ////////////////////////////////
////////////// ////////////// ////////////// ////////////// ////////////// ////////
void loop() {
  /// EFFECTEN LOPEN ALTIJD
  bubbleEffect();
  stoomEffect();

  if (fase == 1) {
    ademEffect();
  } else if (fase == 2) {
    ledsBijKnoppen();
    zoeklicht();
  }

  leesKnoppen();
  checkTimeout();
}

////////////////////////////////////////////////////////////
/// KNOPPEN uitlezen en acties starten


void leesKnoppen() {
  for (int i = 0; i < GROEPEN; i++) {
    if (digitalRead(knop[i]) == LOW) {
      keuze = i;
      fase = 2;
      startZoeklicht();
      laatsteInteractie = millis();
    }
  }
}

////////////////////////////////////////////////////////////
/// hier bepaal je welke leds bij de knoppen aangaan
void ledsBijKnoppen() {
  stripKnoppen.clear();
  if (keuze != -1) {
    uint32_t kleur = stripKnoppen.Color(maxHelderheid, maxHelderheid, maxHelderheid, maxHelderheid);
    for (int i = 0; i < LEDS_PER_KNOP; i++) {
      stripKnoppen.setPixelColor(startLed[keuze] + i, kleur);
    }
  }
  stripKnoppen.show();
  Serial.println(keuze);
}

////////////////////////////////////////////////////////////
/// ADEM EFFECT
void ademEffect() {
  float intensity = (sin(angle) + 1.0) / 2.0;
  int brightness = intensity * maxHelderheid;
  uint32_t kleur = stripKnoppen.Color(brightness, brightness, brightness, brightness);
  stripKnoppen.fill(kleur);
  stripKnoppen.show();
  angle += pulseSpeed / 100.0;
  if (angle > TWO_PI) angle -= TWO_PI;
}

////////////////////////////////////////////////////////////
/// ZOEKLICHT

void startZoeklicht() {

  zoekIndex = 0;
  zoekInterval = 200;
  zoekActief = true;
}
void zoeklicht() {
  if (!zoekActief) return;
  if (millis() - laatsteZoek < zoekInterval) return;
  laatsteZoek = millis();
  stripPannen.clear();
  // willekeurig pannetje kiezen
  huidigePan = random(0, GROEPEN);
  // pannetje tekenen
  tekenPan(huidigePan);
  stripPannen.show();

  // versnellen
  zoekInterval *= 0.90;

  if (zoekInterval < 30) {
    zoekActief = false;
    stripPannen.clear();

    if (keuze != -1) {
      tekenPan(keuze); /////// HIER NOG AANGEVEN BIJ WELKE KNOPKEUZE 1 T/M 5 WELKE PANKEUZE HOORT
    }

    stripPannen.show();
  }
}


////////////////////////////////////////////////////////////
/// STOOM EFFECT
void stoomEffect() {
  if (millis() - laatsteSteam < 60) return;
  laatsteSteam = millis();
  // fade oude pixels
  for (int i = BUBBLE_PIXELS; i < NUM_EFFECT_LEDS; i++) {

    uint32_t c = stripEffect.getPixelColor(i);
    int r = (c >> 24) & 0xFF;
    int g = (c >> 16) & 0xFF;
    int b = (c >> 8) & 0xFF;
    int w = c & 0xFF;

    r *= STEAM_FADE;
    g *= STEAM_FADE;
    b *= STEAM_FADE;
    w *= STEAM_FADE;
    stripEffect.setPixelColor(i, stripEffect.Color(r, g, b, w));
  }

  // stoomdeeltjes
  for (int i = 0; i < STEAM_PARTICLES; i++) {
    steamPos[i] += steamSpeed[i];
    if (random(10) == 0) {
      steamPos[i] += steamDrift[i];
    }
    if (steamPos[i] >= NUM_EFFECT_LEDS) {
      steamPos[i] = BUBBLE_PIXELS + random(0, 6);
      steamSpeed[i] = random(STEAM_SPEED_MIN, STEAM_SPEED_MAX);
      steamDrift[i] = random(-1, 2);
      steamBrightness[i] = random(maxHelderheid / 2, maxHelderheid);
    }

    int p = (int)steamPos[i];
    if (p >= BUBBLE_PIXELS && p < NUM_EFFECT_LEDS) {
      int b = min(steamBrightness[i], maxHelderheid);
      stripEffect.setPixelColor(
        p,
        stripEffect.Color(b, b, b, b));
    }
  }

  stripEffect.show();
}

////////////////////////////////////////////////////////////
/// Bubbels onderin de ledstrip
////////////////////////////////////////////////////////////

void bubbleEffect() {

  if (millis() - laatsteBubble < 80) return;
  laatsteBubble = millis();

  // oude bubbels vervagen
  for (int i = 0; i < BUBBLE_PIXELS; i++) {

    uint32_t c = stripEffect.getPixelColor(i);

    int r = (c >> 24) & 0xFF;
    int g = (c >> 16) & 0xFF;
    int b = (c >> 8) & 0xFF;
    int w = c & 0xFF;

    r *= 0.5;
    g *= 0.5;
    b *= 0.5;
    w *= 0.5;

    stripEffect.setPixelColor(i, stripEffect.Color(r, g, b, w));
  }

  // bubbels tekenen
  for (int i = 0; i < BUBBLE_COUNT; i++) {
    int pos = bubblePos[i];
    for (int s = 0; s < bubbleSize[i]; s++) {
      int led = pos + s;
      if (led < BUBBLE_PIXELS) {
        int b = random(maxHelderheid / 2, maxHelderheid);
        stripEffect.setPixelColor(
          led,
          stripEffect.Color(b, b, b, b));
      }
    }
    bubbleLife[i]--;
    if (bubbleLife[i] <= 0) {
      bubblePos[i] = random(0, BUBBLE_PIXELS);
      bubbleSize[i] = random(1, 4);
      bubbleLife[i] = random(3, 8);
    }
  }
  stripEffect.show();
}

///// de ledjes van de pannetjes aanzetten pannetjes
void tekenPan(int panIndex) {
  uint32_t kleur = stripPannen.Color(maxHelderheid, maxHelderheid, maxHelderheid, maxHelderheid);

  int start = panStart[panIndex];

  for (int i = 0; i < LEDS_PER_PAN; i++) {
    stripPannen.setPixelColor(start + i, kleur);
  }
}


////////////////////////////////////////////////////////////
/// TIMEOUT
////////////////////////////////////////////////////////////

void checkTimeout() {
  if (fase == 2) {
    if (millis() - laatsteInteractie > timeoutTijd) {
      keuze = -1;
      fase = 1;
      stripKnoppen.clear();
      stripPannen.clear();
      stripKnoppen.show();
      stripPannen.show();
    }
  }
}