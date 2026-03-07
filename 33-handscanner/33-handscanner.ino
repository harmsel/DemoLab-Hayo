#include <elapsedMillis.h>
#include <Adafruit_NeoPixel.h>
#include <CapacitiveSensor.h>

int helderheid = 50;  //max 255

// Ademen variablen
float pulseVal = 0;
bool fadingIn = true;
float pulseSpeed = 3.0;  //snelheid ademen effect

elapsedMillis standByKnipper;
elapsedMillis timer;

const int knop = 11;//drukknop
CapacitiveSensor   sensCap = CapacitiveSensor(3, 4); // laatste pin = sensor

bool eindeScan = true;

#define NUMPIXELS 160 //2,4 meter strip * 60 leds = 144 leds

//#define NUMPIXELS 60 ////VOOR TESTEN
Adafruit_NeoPixel strip(NUMPIXELS, A0, NEO_GRBW + NEO_KHZ800);

int scanSnelheid = 20;//loopschelheid van de scanleds

// START leds van de kanten
int ledLinksBoven = 79;
int ledLinksOnder = 24;
int ledRechtsOnder = 158;
int langeKant = 55; //aantal leds aan de lange kant
int korteKant = 24; //aantal leds korte kant
int knipperLedAdres = 0;

int aantalLedsExtraAan = 3;

int xGlobal = 0;//globale x voor de ademende rode standby led
bool scanGestopt = true; // zo weet je of scannen klaar is

void setup() {
  strip.begin();
  strip.setBrightness(255);
  strip.show(); // Initialize all pixels to 'off'
  pinMode(knop, INPUT_PULLUP);
  Serial.begin(9600);

}

void loop() {
  long start = millis();
  long sens =  sensCap.capacitiveSensor(30);
  //  Serial.println(sens);                  //  sensor output
 Serial.println(digitalRead(knop));

  if (digitalRead(knop) == LOW && scanGestopt) { // de knop is ingedrukt
 // if (digitalRead(knop) == LOW && scanGestopt && sens > 500) { // de knop is ingedrukt
  
    scanGestopt = false;
    strip.setBrightness(255);// nodig om de helderheid door  standByAdem() te resetten

    scannen();
  } else {

  ademEffect(2.5);
  }
  //delay (10);//voor stabiliteit
}

// ------------------========= Functies =========------------ //

/// - ademen
void ademEffect(float snelheid) {
  // We gebruiken millis() als basis voor de tijd. 
  // Hoe hoger de snelheid, hoe sneller de sinus door de fasen loopt.
  float fase = millis() * (snelheid / 1000.0); 
  
  // Bereken intensiteit (0.0 tot 1.0)
  float intensity = (sin(fase) + 1.0) / 2.0; 
  
  // Schaal dit naar de gewenste maximale helderheid (bijv. de variabele 'helderheid')
  int v = intensity * helderheid;

  // Kleur bepalen: Alleen ROOD (R, G, B, W)
  // Wil je wit ademen? Gebruik dan strip.Color(0, 0, 0, v)
  uint32_t kleur = strip.Color(v, 0, 0, 0); 

  // Alleen de eerste LED of de hele strip? 
  // In je vorige vraag wilde je alleen de eerste (knipperLedAdres)
  for (int i = 0; i < NUMPIXELS; i++) {
    strip.setPixelColor(i, kleur);
  }
  
  strip.show();
}

void scannen() {
  // Zijkanten Naar boven scannen
  for (int i = 0; i < langeKant; i++) { // For each pixel...
    int ledL = ledLinksOnder + i;
    int ledR = ledRechtsOnder - i;

    for (int i = 0; i < aantalLedsExtraAan; i++) { // For each pixel...
      strip.setPixelColor(ledL + i, strip.Color(255, 255, 255));
      strip.setPixelColor(ledR - i, strip.Color(255, 255, 255));
    }


    strip.show();
    delay (scanSnelheid);
    strip.setPixelColor(ledL, strip.Color(0, 0, 0));
    strip.setPixelColor(ledR, strip.Color(0, 0, 0));
    strip.show();
  }


  /// -- Bovenrand
  for (int i = 0; i < korteKant; i++) { // breedte van de bovenrand
    strip.setPixelColor(ledLinksBoven + i, strip.Color(255, 255, 255,255)); //Adres led Linksboven
  }
  strip.show();


  // Zijkanten Naar Onder scannen
  for (int i = langeKant - 1; i > 0; i--) { // For each pixel...
    int ledL = ledLinksOnder + i;
    int ledR = ledRechtsOnder - i;

    for (int i = 0; i < aantalLedsExtraAan; i++) { // For each pixel...
      strip.setPixelColor(ledL - i, strip.Color(255, 255, 255));
      strip.setPixelColor(ledR + i, strip.Color(255, 255, 255));
    }

    strip.show();
    delay (scanSnelheid);

  }
  /// -- Alle leds aan
  for (int i = 0; i < NUMPIXELS; i++) { // For each pixel...
    strip.setPixelColor(i, strip.Color(255, 255, 255));
  }
  strip.show();

  /// -- Fade alle leds naar uit
  for (int i = 200; i > 10; i--) { // For each pixel...
    int;
    strip.setBrightness(i);
    strip.show();
    delay(5);
  }

  /// -- Zet Aaan
  for (int i = 0; i < NUMPIXELS; i++) { // For each pixel...
    strip.setBrightness(255);
    strip.setPixelColor(i, strip.Color(255, 255, 255));
  }


  strip.show();
  delay (100);

  /// -- Zet alles
  for (int i = 0; i < NUMPIXELS; i++) { // For each pixel...
    strip.setPixelColor(i, strip.Color(0, 0, 0));
  }
  strip.show();

  scanGestopt = true;
}
