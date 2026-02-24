#include "config.h"

//================================= ---- INIT ---- =========================================================

// pinnen waar de Knoppen zijn aangeslote
int butPins[12] =      { 2,  5,   6,   9,   11, A3,    //Nano  - Knopjes Vlammen
                         3,  4,   7,  8,    10, 12};  //Nano  - knopjes Waakvlammentjes
int ledVlamVoor[6] =   { 56, 89,  0,  74,   35, 18};   //LED adressen VLAMMEN Voor
int ledVlamAchter[6] = {186, 172, 133, 152, 100,113};   //LED adressen VLAMMEN Achter

int ledWaak[12] =       {0,  27,  36,  63,  81, 99, 
                         9,  18,  54,  45,  72, 90}; //LED adressen van de waakled

// hoeveel leds vanaf de startled gaan er   branden?
int aantalLedsVlam = 11;
int aantalLedsWaak = 3;

uint32_t waakKleur = pixelsWaak.Color(0, 0, 255);
uint32_t vuurKleur = pixelsWaak.Color(255, 0, 0);

int buttReset = A4; //reset knop

int knopgedrukt[12] = {0, 0, 0, 0, 0, 0, 0, 0, 0 , 0, 0, 0}; // voor het bijhouden welke knop is ingedrukt
int aantalKnopGedrukt; //voor bijhouden hoeveel knoppen er ingedrukt zijn, maximaal 6 knoppen

//================================= ---- SETUP ---- =========================================================
void setup() {
  for (int x = 0; x < 12; x++ ) {
    pinMode(butPins[x], INPUT_PULLUP);
  }
  pinMode(buttReset, INPUT_PULLUP);
  Serial.begin(9600);
  standBy();
}

//================================= ---- LOOP ---- =========================================================
void loop() {
  pixels.begin();  pixelsWaak.begin(); pixels.show(); pixelsWaak.show(); //alle leds uitzetten

  // ----- KNOPPEN  ------ vlammen en waakvlammen uitlezen
  for (int x = 0; x < 12; x++) {
    if (digitalRead(butPins[x]) == false && knopgedrukt[x] == false && aantalKnopGedrukt < 6) {
      knopgedrukt[x] = true; //hou bij welke knop gedrukt is

      Serial.println (knopgedrukt[x]);
      millisCounterStandby = 0; //start de standby timer weer op 0
      vlammenAan(x);
      aantalKnopGedrukt++; //er mogen maximaal 6 knoppen worden ingedrukt
    }
  }

  // ----- START KNOP ------ dus alles uit de hele meute uit
  if (digitalRead(buttReset) == LOW) {
    resetVuur();
  }

  // -----  RESET ------- na geen input
  if (millisCounterStandby > 60000) { 
    standBy();
    millisCounterStandby = 0;
  }

  delay (10);// minimaal 10voor stabiliteit,

  
}


//================================= ---- FUNCTIES ---- =========================================================

//// ---   VLAMMEN EN WAAKVLAMMEN AAN
void vlammenAan(int x) {
  waakvlam(ledWaak[x]);
  if (x < 6) {
    meerVlammen(ledVlamVoor[x]);
    meerVlammen(ledVlamAchter[x]);
  }
}

//// ---  DE VERSCHILLENDE VLAMMEN rijen aanzetten
void meerVlammen(int start) {
  for  (int x = start; x < start + aantalLedsVlam; x++) {
    pixels.setPixelColor(x, vuurKleur);
  }
  pixels.show();
}

//// ---   WAAKVLAMMEN NUMMERS
void waakvlam(int start) {
  Serial.print("deze WAAK aan aan:"); Serial.println(start);
  for  (int x = start; x < start + aantalLedsWaak; x++) {
    pixelsWaak.setPixelColor(x, waakKleur);
    pixelsWaak.show();
  }
}

void resetVuur() {
  for (int x = 0; x < 12 ; x++) {
    knopgedrukt[x] = 0;
  }
  for (int x = 0; x < NUMPIXELS; x++) {
    pixels.setPixelColor(x, pixels.Color(0, 0, 0));
  }

  for (int x = 0; x < NUMPIXELSWAAK; x++) {
    pixelsWaak.setPixelColor(x, pixelsWaak.Color(0, 0, 0));
  }
  pixels.show();
  pixelsWaak.show();
  aantalKnopGedrukt = 0; //dan kan de gebruiker weer 6 knoppen indrukken
}



void  standBy() {
  // zo kun je geen knop indrukken totdat je reset drukt
  for (int x = 0; x < 12 ; x++) {
    knopgedrukt[x] = 1;
  }

  for (int x = 0; x < NUMPIXELS; x++) {
    pixels.setPixelColor(x, pixels.Color(0, 0, 0));
  }

  // zet de juiste blauwe leds aan
  for (int i = 0; i < 12; i++) {
    int led = ledWaak[i];

    for (int i = 0; i < aantalLedsWaak; i++) {
      int ledNr =  led + i;
      pixelsWaak.setPixelColor(ledNr, waakKleur);
      Serial.println(led);
    }


  }
  pixels.show();
  pixelsWaak.show();
}
