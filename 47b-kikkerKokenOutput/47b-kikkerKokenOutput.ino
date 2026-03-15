#include <Adafruit_NeoPixel.h>

int maxHelderheid = 255;
const unsigned long timeoutTijd = 10000;

#define NUM_KNOP_LEDS 120
#define NUM_PAN_LEDS 120
#define NUM_EFFECT_LEDS 120

#define GROEPEN 5
int knop[GROEPEN] = {11,10,9,8,7};

#define LEDS_PER_KNOP 20
int startLed[GROEPEN] = {0,20,40,60,80};

int panStart[GROEPEN] = {0,20,40,60,80};
#define LEDS_PER_PAN 20

#define STEAM_PARTICLES 20
#define STEAM_FADE 0.82
#define STEAM_SPEED_MIN 1
#define STEAM_SPEED_MAX 5

float steamPos[STEAM_PARTICLES];
int steamSpeed[STEAM_PARTICLES];
int steamDrift[STEAM_PARTICLES];
int steamBrightness[STEAM_PARTICLES];

#define BUBBLE_PIXELS 30
#define BUBBLE_COUNT 16

int bubblePos[BUBBLE_COUNT];
int bubbleSize[BUBBLE_COUNT];
int bubbleLife[BUBBLE_COUNT];

float angle = 0;
float pulseSpeed = 2;

Adafruit_NeoPixel stripKnoppen(NUM_KNOP_LEDS, A0, NEO_GRBW + NEO_KHZ800);
Adafruit_NeoPixel stripPannen(NUM_PAN_LEDS, A1, NEO_GRBW + NEO_KHZ800);
Adafruit_NeoPixel stripEffect(NUM_EFFECT_LEDS, A2, NEO_GRBW + NEO_KHZ800);

int huidigePan = -1;

int keuze = -1;
int fase = 1;

unsigned long laatsteInteractie = 0;
unsigned long laatsteZoek = 0;
unsigned long laatsteBubble = 0;
unsigned long laatsteSteam = 0;

bool zoekActief = false;

int rouletteStappen = 0;
#define ROULETTE_MAX 25
int juistePan = -1;

int zoekInterval = 120;

//////////////////////////////////////////////////////////
// wit functie
//////////////////////////////////////////////////////////

uint32_t steamWhite(int b){
  return stripEffect.Color(b,b,b,b);
}

//////////////////////////////////////////////////////////
// SETUP
//////////////////////////////////////////////////////////

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
    steamBrightness[i] = random(maxHelderheid/2,maxHelderheid);
  }

  for(int i=0;i<BUBBLE_COUNT;i++){
    bubblePos[i] = random(0,BUBBLE_PIXELS);
    bubbleSize[i] = 1;
    bubbleLife[i] = random(3,8);
  }
}

//////////////////////////////////////////////////////////
// LOOP
//////////////////////////////////////////////////////////

void loop(){

  bubbleEffect();
  stoomEffect();

  if(fase==1){
    ademEffect();
  }
  else if(fase==2){
    ledsBijKnoppen();
    roulettePannen();
  }

  leesKnoppen();
  checkTimeout();
}

//////////////////////////////////////////////////////////
// KNOPPEN
//////////////////////////////////////////////////////////

void leesKnoppen(){

  for(int i=0;i<GROEPEN;i++){

    if(digitalRead(knop[i])==LOW){

      keuze = i;
      fase = 2;

      startRoulette();

      laatsteInteractie = millis();
    }
  }
}

//////////////////////////////////////////////////////////
// LEDS BIJ KNOPPEN
//////////////////////////////////////////////////////////

void ledsBijKnoppen(){

  stripKnoppen.clear();

  if(keuze!=-1){

    uint32_t kleur = stripKnoppen.Color(maxHelderheid,maxHelderheid,maxHelderheid,maxHelderheid);

    for(int i=0;i<LEDS_PER_KNOP;i++){
      stripKnoppen.setPixelColor(startLed[keuze]+i,kleur);
    }
  }

  stripKnoppen.show();
}

//////////////////////////////////////////////////////////
// ADEM EFFECT
//////////////////////////////////////////////////////////

void ademEffect(){

  float intensity = (sin(angle)+1.0)/2.0;

  int brightness = intensity*maxHelderheid;

  uint32_t kleur = stripKnoppen.Color(brightness,brightness,brightness,brightness);

  stripKnoppen.fill(kleur);
  stripKnoppen.show();

  angle += pulseSpeed/100.0;

  if(angle>TWO_PI) angle -= TWO_PI;
}

//////////////////////////////////////////////////////////
// START ROULETTE
//////////////////////////////////////////////////////////

void startRoulette(){

  zoekActief = true;
  rouletteStappen = 0;

  juistePan = GROEPEN - 1 - keuze;
}

//////////////////////////////////////////////////////////
// ROULETTE EFFECT
//////////////////////////////////////////////////////////

void roulettePannen(){

  if(!zoekActief) return;
  if(millis()-laatsteZoek < zoekInterval) return;

  laatsteZoek = millis();

  stripPannen.clear();

  if(rouletteStappen < ROULETTE_MAX){

    huidigePan = random(0,GROEPEN);

  }
  else{

    huidigePan = juistePan;
    zoekActief = false;
  }

  tekenPan(huidigePan);
  stripPannen.show();

  rouletteStappen++;
}

//////////////////////////////////////////////////////////
// STOOM EFFECT
//////////////////////////////////////////////////////////

void stoomEffect(){

  if(millis()-laatsteSteam < 60) return;
  laatsteSteam = millis();

  for(int i=BUBBLE_PIXELS;i<NUM_EFFECT_LEDS;i++){

    uint32_t c = stripEffect.getPixelColor(i);
    int w = c & 0xFF;

    w *= STEAM_FADE;

    stripEffect.setPixelColor(i,steamWhite(w));
  }

  for(int i=0;i<STEAM_PARTICLES;i++){

    steamPos[i] += steamSpeed[i] + random(-1,2);

    if(random(5)==0){
      steamPos[i] += steamDrift[i];
    }

    if(steamPos[i] >= NUM_EFFECT_LEDS){

      if(random(4)==0){

        steamPos[i] = BUBBLE_PIXELS + random(0,10);

        steamSpeed[i] = random(STEAM_SPEED_MIN,STEAM_SPEED_MAX+1);
        steamDrift[i] = random(-1,2);
        steamBrightness[i] = random(maxHelderheid/2,maxHelderheid);

      }
      else{
        steamPos[i] = NUM_EFFECT_LEDS + random(10,30);
      }
    }

    int p = (int)steamPos[i];

    if(p >= BUBBLE_PIXELS && p < NUM_EFFECT_LEDS){

      int b = steamBrightness[i];

      stripEffect.setPixelColor(p,steamWhite(b));

      if(p-1 >= BUBBLE_PIXELS)
        stripEffect.setPixelColor(p-1,steamWhite(b*0.5));

      if(p-2 >= BUBBLE_PIXELS)
        stripEffect.setPixelColor(p-2,steamWhite(b*0.25));
    }
  }

  stripEffect.show();
}

//////////////////////////////////////////////////////////
// BUBBLES
//////////////////////////////////////////////////////////

void bubbleEffect(){

  if(millis()-laatsteBubble < 10) return;
  laatsteBubble = millis();

  for(int i=0;i<BUBBLE_PIXELS;i++){

    uint32_t c = stripEffect.getPixelColor(i);

    int r = (c>>24)&0xFF;
    int g = (c>>16)&0xFF;
    int b = (c>>8)&0xFF;
    int w = c & 0xFF;

    r *= 0.5;
    g *= 0.5;
    b *= 0.5;
    w *= 0.5;

    stripEffect.setPixelColor(i,stripEffect.Color(r,g,b,w));
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

//////////////////////////////////////////////////////////
// PANNEN
//////////////////////////////////////////////////////////

void tekenPan(int panIndex){

  uint32_t kleur = stripPannen.Color(maxHelderheid,maxHelderheid,maxHelderheid,maxHelderheid);

  int start = panStart[panIndex];

  for(int i=0;i<LEDS_PER_PAN;i++){
    stripPannen.setPixelColor(start+i,kleur);
  }
}

//////////////////////////////////////////////////////////
// TIMEOUT
//////////////////////////////////////////////////////////

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