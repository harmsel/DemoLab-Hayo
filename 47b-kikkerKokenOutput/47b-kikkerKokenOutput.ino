#include <Adafruit_NeoPixel.h>

int maxHelderheid = 255;
const unsigned long timeoutTijd = 18000;

#define NUM_KNOP_LEDS 25
#define NUM_PAN_LEDS 120

// ===============================
#define GROEPEN 5
int knop[GROEPEN] = {11,10,9,8,7};

#define LEDS_PER_KNOP 5

// OMGEKEERDE VOLGORDE
int startLed[GROEPEN] = {20,15,10,5,0};

int panStart[GROEPEN] = {0,8,16,24,32};
#define LEDS_PER_PAN 8

// ===============================
// EFFECT STRIP CONFIG
// ===============================
#define BUBBLE_PIXELS 72
#define STEAM_PIXELS 111
#define NUM_EFFECT_LEDS (BUBBLE_PIXELS + STEAM_PIXELS)

// ===============================
// STOOM INSTELLINGEN
// ===============================
#define STEAM_PARTICLES 20
#define STEAM_FADE 0.92
#define STEAM_SPEED_MIN 1
#define STEAM_SPEED_MAX 2

#define STEAM_SIZE_MIN 3
#define STEAM_SIZE_MAX 8

float steamPos[STEAM_PARTICLES];
int steamSpeed[STEAM_PARTICLES];
int steamDrift[STEAM_PARTICLES];
int steamBrightness[STEAM_PARTICLES];
int steamSize[STEAM_PARTICLES];

// ===============================
#define BUBBLE_COUNT 8

int bubblePos[BUBBLE_COUNT];
int bubbleSize[BUBBLE_COUNT];
int bubbleLife[BUBBLE_COUNT];

// ===============================
// ROULETTE INSTELLING
// ===============================
#define ROULETTE_INTERVAL 190 // pauze
#define ROULETTE_MAX 30 // 55 de lengte van de roulette korte getal is korter

// ===============================
float angle = 0;
float pulseSpeed = 2;

Adafruit_NeoPixel stripKnoppen(NUM_KNOP_LEDS, A0, NEO_GRBW + NEO_KHZ800);
Adafruit_NeoPixel stripPannen(NUM_PAN_LEDS, A1, NEO_GRBW + NEO_KHZ800);
Adafruit_NeoPixel stripEffect(NUM_EFFECT_LEDS, A2, NEO_GRBW + NEO_KHZ800);

int huidigePan = -1;
int vorigePan = -1;   // 

int keuze = -1;
int fase = 1;

unsigned long laatsteInteractie = 0;
unsigned long laatsteZoek = 0;
unsigned long laatsteBubble = 0;
unsigned long laatsteSteam = 0;

bool zoekActief = false;

int rouletteStappen = 0;
int juistePan = -1;

// ===============================
uint32_t steamWhite(int b){
  return stripEffect.Color(b,b,b,b);
}

// ===============================
int mapKnopNaarPan(int knopIndex){
  return GROEPEN - 1 - knopIndex;
}

// ===============================
void setup(){

  stripKnoppen.begin();
  stripKnoppen.show();

  stripPannen.begin();
  stripPannen.show();

  stripEffect.begin();
  stripEffect.show();

  for(int i=0;i<GROEPEN;i++){
    pinMode(knop[i],INPUT_PULLUP);
  }

  Serial.begin(115200);
  randomSeed(analogRead(A5));

  for(int i=0;i<STEAM_PARTICLES;i++){
    steamPos[i] = BUBBLE_PIXELS + random(0,10);
    steamSpeed[i] = random(STEAM_SPEED_MIN,STEAM_SPEED_MAX+1);
    steamDrift[i] = random(-1,2);
    steamBrightness[i] = random(maxHelderheid/3,maxHelderheid*0.8);
    steamSize[i] = random(STEAM_SIZE_MIN, STEAM_SIZE_MAX);
  }

  for(int i=0;i<BUBBLE_COUNT;i++){
    bubblePos[i] = random(0,BUBBLE_PIXELS);
    bubbleSize[i] = 1;
    bubbleLife[i] = random(3,8);
  }
}

// ===============================
void loop(){

  if(fase == 1){
    bubbleEffect();
    stoomEffect();
    ademEffect();
  }
  else if(fase == 2){
    ledsBijKnoppen();
    roulettePannen();
  }

  leesKnoppen();
  checkTimeout();
}

// ===============================
void leesKnoppen(){

  for(int i=0;i<GROEPEN;i++){

    if(digitalRead(knop[i])==LOW){

      keuze = i;
      fase = 2;

      stripEffect.clear();
      stripEffect.show();

      startRoulette();
      laatsteInteractie = millis();
    }
  }
}

// ===============================
void ledsBijKnoppen(){

  stripKnoppen.clear();

  if(keuze!=-1){

    uint32_t kleur = stripKnoppen.Color(maxHelderheid,maxHelderheid,maxHelderheid,maxHelderheid);

    int start = startLed[keuze];

    for(int i=0;i<LEDS_PER_KNOP;i++){
      stripKnoppen.setPixelColor(start + i, kleur);
    }
  }

  stripKnoppen.show();
}

// ===============================
void ademEffect(){

  float intensity = (sin(angle)+1.0)/2.0;
  int brightness = intensity*maxHelderheid;

  uint32_t kleur = stripKnoppen.Color(brightness,brightness,brightness,brightness);

  stripKnoppen.fill(kleur);
  stripKnoppen.show();

  angle += pulseSpeed/100.0;
  if(angle>TWO_PI) angle -= TWO_PI;
}

// ===============================
void startRoulette(){

  zoekActief = true;
  rouletteStappen = 0;

  juistePan = mapKnopNaarPan(keuze);

  vorigePan = -1; // reset zodat eerste keuze vrij is

  stripEffect.clear();
  stripEffect.show();
}

// ===============================
void roulettePannen(){

  if(!zoekActief) return;

  if(millis() - laatsteZoek < ROULETTE_INTERVAL) return;

  laatsteZoek = millis();

  stripPannen.clear();

  if(rouletteStappen < ROULETTE_MAX){

    int nieuwePan;

    do {
      nieuwePan = random(0, GROEPEN);
    } while(nieuwePan == vorigePan);

    huidigePan = nieuwePan;
    vorigePan = huidigePan;

  } else {

    huidigePan = juistePan;
    zoekActief = false;
  }

  tekenPan(huidigePan);
  stripPannen.show();

  rouletteStappen++;
}

// ===============================
void stoomEffect(){

  if(millis()-laatsteSteam < 120) return;
  laatsteSteam = millis();

  for(int i=BUBBLE_PIXELS;i<NUM_EFFECT_LEDS;i++){
    uint32_t c = stripEffect.getPixelColor(i);
    int w = c & 0xFF;
    w *= STEAM_FADE;
    stripEffect.setPixelColor(i,steamWhite(w));
  }

  for(int i=0;i<STEAM_PARTICLES;i++){

    steamPos[i] += steamSpeed[i];

    if(random(5)==0){
      steamPos[i] += steamDrift[i];
    }

    if(steamSize[i] < STEAM_SIZE_MAX && random(10)==0){
      steamSize[i]++;
    }

    if(steamPos[i] >= NUM_EFFECT_LEDS){

      if(random(4)==0){
        steamPos[i] = BUBBLE_PIXELS + random(0,10);
        steamSpeed[i] = random(STEAM_SPEED_MIN,STEAM_SPEED_MAX+1);
        steamDrift[i] = random(-1,2);
        steamBrightness[i] = random(maxHelderheid/3,maxHelderheid*0.8);
        steamSize[i] = random(STEAM_SIZE_MIN, STEAM_SIZE_MAX);
      } else {
        steamPos[i] = NUM_EFFECT_LEDS + random(10,30);
      }
    }

    int p = (int)steamPos[i];

    for(int s = -steamSize[i]; s <= steamSize[i]; s++){

      int led = p + s;

      if(led >= BUBBLE_PIXELS && led < NUM_EFFECT_LEDS){

        float falloff = 1.0 - (abs(s) / (float)steamSize[i]);
        falloff *= falloff;

        int brightness = steamBrightness[i] * falloff;

        stripEffect.setPixelColor(led, steamWhite(brightness));
      }
    }
  }

  stripEffect.show();
}

// ===============================
void bubbleEffect(){

  if(millis()-laatsteBubble < 10) return;
  laatsteBubble = millis();

  for(int i=0;i<BUBBLE_PIXELS;i++){
    uint32_t c = stripEffect.getPixelColor(i);
    int w = c & 0xFF;
    w *= 0.5;
    stripEffect.setPixelColor(i,steamWhite(w));
  }

  for(int i=0;i<BUBBLE_COUNT;i++){

    int pos = bubblePos[i];

    for(int s=0;s<bubbleSize[i];s++){
      int led = pos+s;

      if(led < BUBBLE_PIXELS){
        int b = random(maxHelderheid/2,maxHelderheid+1);
        stripEffect.setPixelColor(led,steamWhite(b));
      }
    }

    bubbleLife[i]--;

    if(bubbleLife[i] <= 0){
      bubblePos[i] = random(0,BUBBLE_PIXELS);
      bubbleSize[i] = random(1,4);
      bubbleLife[i] = random(3,8);
    }
  }

  stripEffect.show();
}

// ===============================
void tekenPan(int panIndex){

  uint32_t kleur = stripPannen.Color(maxHelderheid,maxHelderheid,maxHelderheid,maxHelderheid);

  int start = panStart[panIndex];

  for(int i=0;i<LEDS_PER_PAN;i++){
    stripPannen.setPixelColor(start+i,kleur);
  }
}

// ===============================
void checkTimeout(){

  if(fase==2){

    if(millis()-laatsteInteractie > timeoutTijd){

      keuze = -1;
      fase = 1;

      stripKnoppen.clear();
      stripPannen.clear();

      stripKnoppen.show();
      stripPannen.show();
    }
  }
}