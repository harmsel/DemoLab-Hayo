#include <Adafruit_NeoPixel.h>
int maxHelderheid = 255;
const unsigned long timeoutTijd = 10000; // na zo veel tijd gaat ademen weer aan


// Lengte ledstrip
#define NUM_KNOP_LEDS 120
#define NUM_PAN_LEDS 120
#define NUM_EFFECT_LEDS 120
#define STEAM_PIXELS 120

// knoppen 
#define GROEPEN 5 // aantal knoppen
int knop[GROEPEN] = {11,10,9,8,7};

// LEDS bij de knoppen
#define LEDS_PER_KNOP 20
int startLed[GROEPEN] = {0,20,40,60,80}; // dit zijn de startleds bij de knoppen

// Leds bij de pannetjes
int panStart[GROEPEN] = {0,20,40,60,80}; //dit zijn de pannetjes
#define LEDS_PER_PAN 20

/// aanpassen effect  stoom
#define STEAM_PARTICLES 20
#define STEAM_FADE 0.82
#define STEAM_SPEED_MIN 1
#define STEAM_SPEED_MAX 5

float steamPos[STEAM_PARTICLES];
int steamSpeed[STEAM_PARTICLES];
int steamDrift[STEAM_PARTICLES];
int steamBrightness[STEAM_PARTICLES];

#define BUBBLE_PIXELS 30 // deel van de strip die bubbels weergeeeft
#define BUBBLE_COUNT 16 // aantal pixels per bubbel - normaal was 3

int bubblePos[BUBBLE_COUNT];
int bubbleSize[BUBBLE_COUNT];
int bubbleLife[BUBBLE_COUNT];

float angle = 0;
float pulseSpeed = 2;


Adafruit_NeoPixel stripKnoppen(NUM_KNOP_LEDS, A0, NEO_GRBW + NEO_KHZ800);
Adafruit_NeoPixel stripPannen(NUM_PAN_LEDS, A1, NEO_GRBW + NEO_KHZ800);
Adafruit_NeoPixel stripEffect(NUM_EFFECT_LEDS, A2, NEO_GRBW + NEO_KHZ800);


/// voor het roulette effect (alternatief voor zoeklicht)


int huidigePan = -1;

int keuze = -1;
int fase = 1;

unsigned long laatsteInteractie = 0;

bool zoekActief = false;
int zoekIndex = 0;

juistePan = GROEPEN - 1 - keuze;

unsigned long laatsteZoek = 0;

unsigned long laatsteBubble = 0;
unsigned long laatsteSteam = 0;

int steamOffset = 0;

//////////////////////////////////////////////////////////
// wit functie voor maximale lichtopbrengst
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
    zoeklicht();
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

      startZoeklicht();

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

  Serial.println(keuze);
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
// ZOEKLICHT
//////////////////////////////////////////////////////////

void startZoeklicht(){
  zoekIndex = 0;
  zoekInterval = 200;
  zoekActief = true;
}

void zoeklicht(){
  if(!zoekActief) return;
  if(millis()-laatsteZoek < zoekInterval) return;
  laatsteZoek = millis();

  stripPannen.clear();
  huidigePan = random(0,GROEPEN);
  tekenPan(huidigePan);
  stripPannen.show();
  //zoekInterval *= 0.96; // dez laat de snelheid oplopen

  
  if(zoekInterval < 30){
    zoekActief = false;
    stripPannen.clear();
    if(keuze!=-1){
      tekenPan(keuze);
    }
    stripPannen.show();
  }
}

//////////////////////////////////////////////////////////
// STOOM EFFECT
//////////////////////////////////////////////////////////

void stoomEffect(){

  if(millis()-laatsteSteam < 60) return;
  laatsteSteam = millis();

  // fade bestaande stoom
  for(int i=BUBBLE_PIXELS;i<NUM_EFFECT_LEDS;i++){

    uint32_t c = stripEffect.getPixelColor(i);
    int w = c & 0xFF;

    w *= STEAM_FADE;

    stripEffect.setPixelColor(i,steamWhite(w));
  }

  // particles bewegen
  for(int i=0;i<STEAM_PARTICLES;i++){

    // random snelheid per stap
    steamPos[i] += steamSpeed[i] + random(-1,2);

    // beetje zijwaartse drift
    if(random(5)==0){
      steamPos[i] += steamDrift[i];
    }

    // particle verdwijnt bovenaan
    if(steamPos[i] >= NUM_EFFECT_LEDS){

      // niet meteen opnieuw laten verschijnen
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

  if(millis()-laatsteBubble < 10) return; //sneheid van de bubbels

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