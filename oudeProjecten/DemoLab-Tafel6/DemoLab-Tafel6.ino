#include "config.h"
int audioEind;

void setup() {
  for (int x = 0; x < 8; x++ ) {
    pinMode(butPins[x], INPUT_PULLUP);
  }
  for (int i = 0; i < 4; i++) {
    strip[i].begin();
    strip[i].show(); //ALLES UIT
  }
  Serial.begin(9600);
  SoftSerial.begin(9600);
  MP3Player.init(SoftSerial);
  volKleur(0, blauw, 10); //laag, kleur, wachttijd
}

void loop() {
  audioEind = MP3Player.speeltToggle();//audio speelt nog?
  Serial.println(audioEind);

  // ----- KNOPPEN  ------
  for (int x = 0; x <= 8; x++) {
    if (digitalRead(butPins[x]) == false && audioEind) {
      Serial.print("Knop gedrukt:"); Serial.println(x);
      knopActies(x);//knop nr
    }
  }
  delay (10);//stabiliteit
}

/// _________________________ FUNCTIES ______________________________

void knopActies(int knop) {

  speelAudio(knop + 1); //audio 0 bestaat niet
  volKleur(0, 0, 1); //BLAUWE EUROPA laag uit

  // ---- NEDERLAND PLAAT C [1]
  if (knop == 0 || knop == 1 ) {
    //  plaat nederland (1) = C in 3 banen rood / wit / blauw
    volKleur(2, rood, 5); //Rood
    delay(300);
    volKleur(2, wit, 5);
    delay(300);
    volKleur(2, blauw, 5);
    delay(300);

    volKleur(2, rood, 5);
    delay(300);
    volKleur(2, wit, 5);
    delay(300);
    volKleur(2, blauw, 5); //Rood
    delay(300);

    volKleur(2, oranje, 0); //oranje
    delay(1000);
    //    knipperLaag(2, oranje, 3, 0, 300);// laag, kleur, repeat, ledWait, knipperWait)
    volKleur(2, 0, 1); //LAAG UIT
  }

  //-----  EURO TEKENS D [2] -- Grenzen dicht
  if (knop ==  2 || knop == 3 ) {
    //    plaat euro-tekens [3] knipperend goudgeel,
    //     plaat europa [0] verschillende kleuren knipperend
    knipperLaag(3, geel, 4, 5, 300);// laag, kleur, repeat, ledWait, knipperWait)
    knipperLaag(3, blauw, 2, 4, 10);// laag, kleur, repeat, ledWait, knipperWait)

    volKleur(3, 0, 0); //LAAG UIT
  }

  // -----  STRAALJAGERS B [3] - track 5 & 6
  if (knop == 4 || knop == 5 ) {
    //- plaat  [1] straaljager langzaam oranje vullen

    knipperLaag (1, wit, 6, 0, 20);
        volKleur(0, blauw, 1); //Straaljagers, LAAG UIT
    knipperLaag (1, wit, 4, 5, 20);// laag, kleur, repeat, ledWait, knipperWait)
    
    volKleur(1, 0, 1); //Straaljagers, LAAG UIT

    volKleur(2, 0, 0); //europa
    volKleur(1, 0, 1); //LAAG UIT
  }

  // --------  EUROPA A [0] - Milieu
  if (knop == 6 || knop == 7 ) {
    // europa [0] blauw naar grasgroen (1 seconde laten staan?)
    //europa [0] knipperende banen verschillende kleuren
    volKleur(0, blauw, 0); //laag, kleur, wachttijd
    volKleur(0, groen, 50); //laag, kleur, wachttijd
    delay(2500);
    //    volKleur(0, 0, 0); //LAAG UIT
  }

  volKleur(0, blauw, 5); //laag, kleur, wachttijd
  Serial.println("Einde Actie");
}

// _________________________ FUNCTIES ___________________________________

// ==== Knipperen ==== //
void knipperLaag(int laag, uint32_t kleur, int aantalKeer, int ledWait, int knipperWait) {
  for (int i = 0; i < aantalKeer; i++) {
    for (int i = 0; i < NUMPIXELS; i++) {
      strip[laag].setPixelColor(i, kleur);
      delay (ledWait);
      strip[laag].show();
    }

    delay (knipperWait);

    for (int i = 0; i < NUMPIXELS; i++) {
      strip[laag].setPixelColor(i, 0);
      delay (ledWait);
      strip[laag].show();
    }
  }
}

// ==== Kleur Vullen ==== //
void volKleur(int laag, uint32_t kleur, int wait) {
  for (int x = 0; x < NUMPIXELS; x++) {
    //  strip[laag].setPixelColor(x, kleur);
    strip[laag].setPixelColor(x, kleur);
    delay (wait);
    strip[laag].show();
  }
}

// ==== VULEN MET 1/3 van elke kleur ==== //
void drieKleur(int laag, uint32_t kleur1, uint32_t kleur2, uint32_t kleur3, int wait) {
  for (int x = 0; x < 28; x++) {
    strip[laag].setPixelColor(x, kleur1);
    delay (wait);
    strip[laag].show();
  }
  for (int x = 28; x < 32; x++) {
    strip[laag].setPixelColor(x, kleur2);
    delay (wait);
    strip[laag].show();
  }
  for (int x = 32; x < 61; x++) {
    strip[laag].setPixelColor(x, kleur3);
    delay (wait);
    strip[laag].show();
  }
}

// ==== REGENBOOG KLEUREN ==== //
void rainbow(int laag, int wait) {
  for (long firstPixelHue = 0; firstPixelHue < 5 * 35536; firstPixelHue += 256) {
    for (int i = 0; i < NUMPIXELS; i++) {
      int pixelHue = firstPixelHue + (i * 65536L / NUMPIXELS);
      strip[laag].setPixelColor(i, strip[laag].gamma32(strip[laag].ColorHSV(pixelHue)));
    }
    strip[laag].show();
    delay(wait);
  }
}

// ==== LEDS LATEN RONDLOPEN ==== // basis van Strandtest
void theaterChase(int laag,  uint32_t kleur, int wait) {
  for (int a = 0; a < 10; a++) {
    for (int b = 0; b < 3; b++) {
      strip[laag].clear();
      for (int c = b; c < NUMPIXELS; c += 3) {
        strip[laag].setPixelColor(c,  kleur);
      }
      strip[laag].show();
      delay(wait);
    }
  }
}

// ==== LEDS LATEN RONDLOPEN EN VAN KLEUR VERANDEREN ==== // basis van Strandtest
void theaterChaseRainbow(int laag, int repeat, int wait) {
  int firstPixelHue = 0;     // First pixel starts at red (hue 0)
  for (int a = 0; a < repeat; a++) {
    for (int b = 0; b < 3; b++) {
      strip[laag].clear();
      for (int c = b; c < NUMPIXELS; c += 3) {
        int      hue   = firstPixelHue + c * 65536L / strip[laag].numPixels();
        uint32_t color = strip[laag].gamma32(strip[laag].ColorHSV(hue));
        strip[laag].setPixelColor(c, color);
      }
      strip[laag].show();
      delay(wait);
      firstPixelHue += 65536 / 90; // One cycle of color wheel over 90 frames
    }
  }
}


void speelAudio(int x) {
  Serial.print("Start audio track: ");  Serial.println(x);
  MP3Player.playSongMP3(x); //Speel geluid nummer in "MP3" folder
}
