#include "config.h"

int scanSnelheid = 20;//loopschelheid van de scanleds

// START leds van de kanten
int ledLinksBoven = 79;
int ledLinksOnder = 24;
int ledRechtsOnder = 158;
int langeKant = 55; //aantal leds aan de lange kant
int korteKant = 24; //aantal leds korte kant
int knipperLedAdres = 146;

int aantalLedsExtraAan = 3;

int xGlobal = 0;//globale x voor de ademende rode standby led
bool scanGestopt = true; // zo weet je of scannen klaar is

void setup() {
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  pinMode(knop, INPUT_PULLUP);
  Serial.begin(9600);
}

void loop() {
  long start = millis();
  long sens =  sensCap.capacitiveSensor(30);
  //  Serial.println(sens);                  //  sensor output
  //Serial.println(digitalRead(knop));

  if (digitalRead(knop) == LOW && scanGestopt && sens > 500) { // de knop is ingedrukt
    scanGestopt = false;
    strip.setBrightness(255);// nodig om de helderheid door  standByAdem() te resetten

    scannen();
  } else {
    standByAdem(200, 0.08, 50); //Max-helderheid, snelheid, delay
  }
  //delay (10);//voor stabiliteit
}

// ------------------========= Functies =========------------ //

void standByAdem(int maxHelder, float snelheid, int interval ) {
  if (timer >= interval && scanGestopt) {
    timer = 0;
    strip.setPixelColor(knipperLedAdres, strip.Color(255, 0, 0));
    float helderheid =  maxHelder / 2.0 * (1.0 + sin(snelheid * xGlobal));
    strip.setBrightness(helderheid);
    strip.show();
    if (++xGlobal >= 6553) xGlobal = 0;
  }
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
    strip.setPixelColor(ledLinksBoven + i, strip.Color(255, 255, 255)); //Adres led Linksboven
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
