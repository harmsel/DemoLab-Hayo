#include <Adafruit_NeoPixel.h>
#include <CapacitiveSensor.h>

///  ++++++++++.     instellingen voor de leds +++++++
int maxHelderheid = 250;
int scanSnelheid = 200;  //loopschelheid van de scanleds
int frequentie = 2;      // Hoe vaak per seconde (Hz) nipperen

///  --------- AANSLUITINGEN AAN DE ARDUINO ------
const int knop = 10;                                //drukknop
const int knopTest = 11;                            //Alleen voor testen
CapacitiveSensor sensCap = CapacitiveSensor(3, 4);  // laatste pin = sensor

// AANTALLEN EN POSITIE OP DE LEDSTRIPJES
#define NUMPIXELS1 12
#define NUMPIXELS2 12
#define NUMPIXELS3 12
#define KNIPPER_LED 0

//#define NUMPIXELS 60 ////VOOR TESTEN
Adafruit_NeoPixel strip1(NUMPIXELS1, A0, NEO_GRBW + NEO_KHZ800);  // strip 1 zijn de twee vertikale strips
Adafruit_NeoPixel strip2(NUMPIXELS2, A1, NEO_GRBW + NEO_KHZ800);  // strip 2 is de onderste strip
Adafruit_NeoPixel strip3(NUMPIXELS3, A2, NEO_GRBW + NEO_KHZ800);  // strip 3 is de bovenste strip

// voor het scannen
int scanPos = 0;       // De huidige positie van de middelste led
int scanRichting = 1;  // 1 is naar rechts/boven, -1 is naar links/onder
unsigned long laatsteStapTijd = 0;
int richtingsWisselTeller = 0;  // bijhouden hoe vaak de scan heen en weer is gegaan

// Diverse globale vars
unsigned long vorigeMillis = 0;
int fase = 1;  // Houdt bij in welke fase we zitten
bool ledAan = false;


void setup() {
  strip1.begin();
  strip1.show();  // Initialize all pixels to 'off'
  strip2.begin();
  strip2.show();  // Initialize all pixels to 'off'
  strip3.begin();
  strip3.show();  // Initialize all pixels to 'off'
  pinMode(knop, INPUT_PULLUP);
  pinMode(knopTest, INPUT_PULLUP);
  Serial.begin(115200);
}

void loop() {
  int knopStand = digitalRead(knop);
  int testKnopStand = digitalRead(knopTest);

  // Start de scan als beide knoppen ingedrukt worden
  if (knopStand == LOW && testKnopStand == LOW && fase == 1) {
    fase = 2;
    richtingsWisselTeller = 0;  // Reset de teller voor de nieuwe scan
    scanPos = 0;                // Start bij het begin
    scanRichting = 1;           // Start de goede kant op
    Serial.println("Scan start, rode LED uit");
  }

  //// ------- DE FASES ------ ////
  if (fase == 1) {  // Rode knipperled
    knipperLed(1);

  } else if (fase == 2) {  // scannen
    scannenHeenWeerVullen(50);

  } else if (fase == 3) {  // alles knipperen
    zetStripAan(strip1);
    zetStripAan(strip2);
    zetStripAan(strip3);
    delay(1000);
    zetStripUit(strip1);
    zetStripUit(strip2);
    zetStripUit(strip3);
    delay(20);
    zetStripAan(strip1);
    zetStripAan(strip2);
    zetStripAan(strip3);
    fadeStripUit(50);  //deze zie je goed als de max helderheid 255 is, dus voor testen is het niet zichtbaar

    delay(1000);
    fase = 1;
  }
}

// ---------------------------========= Effecten =========---------------- //

//// ----- Scan effect waarbij STEEDS DRIE LEDS DIE AAN ZIJN ---
void scannenHeenWeer(int snelheid) {  // Steeds 3 leds
  if (millis() - laatsteStapTijd >= snelheid) {
    laatsteStapTijd = millis();
    strip1.clear();

    for (int i = -1; i <= 1; i++) {
      int huidigeLed = scanPos + i;
      if (huidigeLed >= 0 && huidigeLed < NUMPIXELS1) {
        strip1.setPixelColor(huidigeLed, strip1.Color(255, 255, 255, 255));
      }
    }
    strip1.setBrightness(maxHelderheid);
    strip1.show();

    scanPos += scanRichting;

    // Check of we bij een uiteinde zijn
    if (scanPos >= NUMPIXELS1 - 1 || scanPos <= 0) {
      scanRichting *= -1;       // Draai om
      richtingsWisselTeller++;  // Tel een wissel

      //als hij boven is (dus Numpixels), dan gaat boven aan. Is hij beneden, dan gaat beneden (i = 0) even aan
      if (scanPos <= 0) {  // de onderste aan en uitzetten als hij er is
        zetStripAan(strip2);
        delay(200);
        zetStripUit(strip2);

      } else {  //de bovenledstrip aanzetten als hij boven is
        zetStripAan(strip3);
        delay(200);
        zetStripUit(strip3);
      }


      // Na 4 wissels (heen, terug, heen, terug) is hij 2x volledig rond geweest
      if (richtingsWisselTeller >= 4) {
        fase = 3;        // Stop de scan, start alles aan
        strip1.clear();  // Maak de strip schoon
        strip1.show();
        Serial.println("Scan klaar volgende fase");
      }
    }
  }
}


//// ----- Scan effect met fases. drie leds zijn aan, aan het einde vult hij
void scannenHeenWeerVullen(int snelheid) {
  if (millis() - laatsteStapTijd >= snelheid) {
    laatsteStapTijd = millis();

    // --- RONDE 1: Heen en Terug (3 leds die bewegen) ---
    if (richtingsWisselTeller < 2) {
      strip1.clear();  // Wis de strip voor het bewegingseffect
      for (int i = -1; i <= 1; i++) {
        int huidigeLed = scanPos + i;
        if (huidigeLed >= 0 && huidigeLed < NUMPIXELS1) {
          strip1.setPixelColor(huidigeLed, strip1.Color(255, 255, 255, 255));
        }
      }
    }

    // --- RONDE 2 HEEN: Vullen (1 led per keer erbij) ---
    else if (richtingsWisselTeller == 2) {
      // We doen GEEN clear(), dus de vorige leds blijven aan
      if (scanPos >= 0 && scanPos < NUMPIXELS1) {
        strip1.setPixelColor(scanPos, strip1.Color(255, 255, 255, 255));
      }
    }

    // --- RONDE 2 TERUG: Leegmaken (1 led per keer weg) ---
    else if (richtingsWisselTeller == 3) {
      // We zetten de huidige led op zwart (gummen)
      if (scanPos >= 0 && scanPos < NUMPIXELS1) {
        strip1.setPixelColor(scanPos, strip1.Color(0, 0, 0, 0));
      }
    }

    strip1.setBrightness(maxHelderheid);
    strip1.show();

    // Positie verplaatsen
    scanPos += scanRichting;

    // Check uiteinden
    if (scanPos >= NUMPIXELS1 || scanPos < 0) {
      scanRichting *= -1;

      // Correctie: zorg dat we niet buiten de strip blijven hangen
      if (scanPos >= NUMPIXELS1) scanPos = NUMPIXELS1 - 1;
      if (scanPos < 0) scanPos = 0;

      richtingsWisselTeller++;
      //als hij boven is (dus Numpixels), dan gaat boven aan. Is hij beneden, dan gaat beneden (i = 0) even aan
      if (scanPos <= 0) {  // de onderste aan en uitzetten als hij er is
        zetStripAan(strip2);
        delay(200);
        zetStripUit(strip2);

      } else {  //de bovenledstrip aanzetten als hij boven is
        zetStripAan(strip3);
        delay(200);
        zetStripUit(strip3);
      }

      // Stop na de wissels (Heen, Terug, Vullen, Gummen)
      if (richtingsWisselTeller >= 4) {
        fase = 3;
        strip1.clear();
        strip1.show();
        Serial.println("Scan cyclus voltooid");
      }
    }
  }
}



// ---------------- ALLES AAN
void zetStripAan(Adafruit_NeoPixel &strip) {
  uint32_t wit = strip.Color(255, 255, 255, 255);

  strip.fill(wit);  // Vul de specifieke strip
  strip.setBrightness(maxHelderheid);
  strip.show();  // Laat het resultaat zien
}

// // ---------------- ALLES UIT
void zetStripUit(Adafruit_NeoPixel &strip) {
  uint32_t uit = strip.Color(0, 0, 0, 0);

  strip.fill(uit);
  strip.show();  // Laat het resultaat zien
}

// // ---------------- FADE OUT
void fadeStripUit(int wachttijd) {

  for (int i = maxHelderheid; i >= 0; i -= 5) {
    strip1.setBrightness(i);
    strip2.setBrightness(i);
    strip3.setBrightness(i);
    strip1.show();
    strip2.show();
    strip3.show();
    delay(wachttijd);  // Hoe hoger de wachttijd, hoe trager de fade
  }
  strip1.clear();             // Zet alle pixels intern op 0 (uit)
  strip2.clear();             // Zet alle pixels intern op 0 (uit)
  strip3.clear();             // Zet alle pixels intern op 0 (uit)
  strip1.setBrightness(255);  // Reset brightness voor de volgende keer dat je de strip aanzet
  strip2.setBrightness(255);  // Reset brightness voor de volgende keer dat je de strip aanzet
  strip3.setBrightness(255);  // Reset brightness voor de volgende keer dat je de strip aanzet
  strip1.show();
  strip2.show();
  strip3.show();
}


/// KNIPPPERLEDJE ROOD
void knipperLed(int snelheid) {

  unsigned long interval = 1000 / (frequentie * snelheid);
  unsigned long huidigeMillis = millis();

  if (huidigeMillis - vorigeMillis >= interval) {
    vorigeMillis = huidigeMillis;  // Onthoud de tijd voor de volgende keer

    // Wissel de status
    ledAan = !ledAan;
    if (ledAan) {
      strip2.setPixelColor(KNIPPER_LED, strip2.Color(255, 0, 0, 0));
    } else {
      strip2.setPixelColor(KNIPPER_LED, strip2.Color(0, 0, 0, 0));
    }
    strip2.setBrightness(maxHelderheid);
    strip2.show();
  }
}
