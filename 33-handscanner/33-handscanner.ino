#include <Adafruit_NeoPixel.h>
#include <CapacitiveSensor.h>


/// INSTELLINGEN
int maxHelderheid = 20;
int scanSnelheid = 100;   // lager getal = snellere scan
int frequentie = 2;       // knipperfrequentie rode LED

#define NUMPIXELS1 12
#define NUMPIXELS2 12
#define NUMPIXELS3 12

#define KNIPPER_LED 0     // positie rode LED

/// SENSOR EN KNOP
const int knop = 2;
CapacitiveSensor cs_4_6 = CapacitiveSensor(4,6);

long capSens;
bool sensorActief = false;   // voorkomt meerdere triggers

/// LEDSTRIPS
////////////////////////////////////////////////////

Adafruit_NeoPixel strip1(NUMPIXELS1, A0, NEO_GRBW + NEO_KHZ800);
Adafruit_NeoPixel strip2(NUMPIXELS2, A1, NEO_GRBW + NEO_KHZ800);
Adafruit_NeoPixel strip3(NUMPIXELS3, A2, NEO_GRBW + NEO_KHZ800);


/// SCAN VARIABELEN
int scanPos = 0;
int scanRichting = 1;

unsigned long laatsteStapTijd = 0;

int richtingsWisselTeller = 0;

/// OVERIGE VARIABELEN
unsigned long vorigeMillis = 0;

int fase = 1;

bool ledAan = false;


/// FUNCTIE VOOR WIT LICHT
uint32_t wit(Adafruit_NeoPixel &strip, int b){
  return strip.Color(0,0,0,b);
}

/// SETUP
void setup(){

  strip1.begin();
  strip1.show();

  strip2.begin();
  strip2.show();

  strip3.begin();
  strip3.show();

  pinMode(knop,INPUT_PULLUP);

  Serial.begin(115200);

  // voorkomt automatische recalibratie capsensor
  cs_4_6.set_CS_AutocaL_Millis(0xFFFFFFFF);
}


/// LOOP
void loop(){

  int knopStand = digitalRead(knop);

  // capsensor meten
  capSens = cs_4_6.capacitiveSensor(30);

  Serial.println(capSens);

/// START SCAN (KNOP + SENSOR)
  if(capSens > 100 && knopStand == LOW && !sensorActief && fase == 1){

    sensorActief = true;

    // rode LED direct uitzetten
    strip2.setPixelColor(KNIPPER_LED, strip2.Color(0,0,0,0));
    strip2.show();

    fase = 2;

    richtingsWisselTeller = 0;
    scanPos = 0;
    scanRichting = 1;
  }

  // reset trigger wanneer sensor losgelaten wordt
  if(capSens < 50){
    sensorActief = false;
  }

////////////////////////////////////////////////////
//// FASES
////////////////////////////////////////////////////

  if(fase == 1){
    // rode LED knippert
    knipperLed(1);
  }
  else if(fase == 2){
    // scan effect
    scannenHeenWeerVullen(scanSnelheid);
  }
  else if(fase == 3){

    // alle strips aan
    zetStripAan(strip1);
    zetStripAan(strip2);
    zetStripAan(strip3);
    delay(500);

    // alles uit
    zetStripUit(strip1);
    zetStripUit(strip2);
    zetStripUit(strip3);
    delay(20);

    // nog een keer aan
    zetStripAan(strip1);
    zetStripAan(strip2);
    zetStripAan(strip3);
    fadeStripUit(50);
    delay(1000);

    // systeem reset
    fase = 1;
    ledAan = false;
  }

}

//// SCAN EFFECT
void scannenHeenWeerVullen(int snelheid){
  if(millis() - laatsteStapTijd >= snelheid){
    laatsteStapTijd = millis();
    if(richtingsWisselTeller < 2){
      strip1.clear();
      int helderMidden = 255;
      int helderZijkant = 80;
      int helderBuiten = 20;
      if(scanPos >=0 && scanPos < NUMPIXELS1)
        strip1.setPixelColor(scanPos, wit(strip1, helderMidden));
      if(scanPos-1 >=0)
        strip1.setPixelColor(scanPos-1, wit(strip1, helderZijkant));
      if(scanPos+1 < NUMPIXELS1)
        strip1.setPixelColor(scanPos+1, wit(strip1, helderZijkant));
      if(scanPos-2 >=0)
        strip1.setPixelColor(scanPos-2, wit(strip1, helderBuiten));
      if(scanPos+2 < NUMPIXELS1)
        strip1.setPixelColor(scanPos+2, wit(strip1, helderBuiten));
    }

    else if(richtingsWisselTeller == 2){
      if(scanPos >=0 && scanPos < NUMPIXELS1)
        strip1.setPixelColor(scanPos, wit(strip1,255));
    }

    else if(richtingsWisselTeller == 3){
      if(scanPos >=0 && scanPos < NUMPIXELS1)
        strip1.setPixelColor(scanPos, wit(strip1,0));
    }

    strip1.setBrightness(maxHelderheid);
    strip1.show();
    scanPos += scanRichting;

////////////////////////////////////////////////////
//// RICHTING WISSEL

    if(scanPos >= NUMPIXELS1 || scanPos < 0){
      scanRichting *= -1;
      if(scanPos >= NUMPIXELS1) scanPos = NUMPIXELS1-1;
      if(scanPos < 0) scanPos = 0;
      richtingsWisselTeller++;
      if(scanPos <= 0){
        zetStripAan(strip2);
        delay(200);
        zetStripUit(strip2);
      }

      else{
        zetStripAan(strip3);
        delay(200);
        zetStripUit(strip3);
      }

      if(richtingsWisselTeller >= 4){
        fase = 3;
        strip1.clear();
        strip1.show();
      }
    }
  }
}

////////////////////////////////////////////////////
//// STRIP AAN
////////////////////////////////////////////////////

void zetStripAan(Adafruit_NeoPixel &strip){
  uint32_t witLed = wit(strip,255);
  strip.fill(witLed);
  strip.setBrightness(maxHelderheid);
  strip.show();
}

////////////////////////////////////////////////////
//// STRIP UIT
////////////////////////////////////////////////////

void zetStripUit(Adafruit_NeoPixel &strip){
  strip.clear();
  strip.show();
}

////////////////////////////////////////////////////
//// FADE UIT
////////////////////////////////////////////////////

void fadeStripUit(int wachttijd){
  for(int i=maxHelderheid;i>=0;i-=5){

    strip1.setBrightness(i);
    strip2.setBrightness(i);
    strip3.setBrightness(i);

    strip1.show();
    strip2.show();
    strip3.show();

    delay(wachttijd);
  }

  strip1.clear();
  strip2.clear();
  strip3.clear();

  strip1.setBrightness(255);
  strip2.setBrightness(255);
  strip3.setBrightness(255);

  strip1.show();
  strip2.show();
  strip3.show();
}

////////////////////////////////////////////////////
//// KNIPPER LED
////////////////////////////////////////////////////

void knipperLed(int snelheid){
  unsigned long interval = 1000 / (frequentie * snelheid);
  unsigned long huidigeMillis = millis();
  if(huidigeMillis - vorigeMillis >= interval){
    vorigeMillis = huidigeMillis;
    ledAan = !ledAan;
    if(ledAan)
      strip2.setPixelColor(KNIPPER_LED, strip2.Color(255,0,0,0));
    else
      strip2.setPixelColor(KNIPPER_LED, strip2.Color(0,0,0,0));

    strip2.setBrightness(maxHelderheid);
    strip2.show();
  }
}