#include <Adafruit_NeoPixel.h>
int helderheid = 50;  //max 255


#define PIN A0
#define NUM 6

// Knoppen definities
#define BUTTON_START 12
#define BUTTON_1A 11
#define BUTTON_1B 10
#define BUTTON_2A 9
#define BUTTON_2B 8
#define BUTTON_3A 7
#define BUTTON_3B 6
#define BUTTON_4A 5
#define BUTTON_4B 4

Adafruit_NeoPixel strip(NUM, PIN, NEO_GRBW + NEO_KHZ800);

int effectMode = 0;
bool effectActive = true;

// Geheugen voor de keuzes
int set1Choice = 0;
int set2Choice = 0;
int set3Choice = 0;
int set4Choice = 0;

// Timer variabelen
unsigned long lastButtonTime = 0;     // Tijdstip van de laatste knopdruk
const unsigned long timeout = 30000;  // 30 seconden in milliseconden

// Ademen variablen
float pulseVal = 0;
bool fadingIn = true;
float pulseSpeed = 3.0;  //snelheid ademen effect

void setup() {
  pinMode(BUTTON_START, INPUT_PULLUP);
  pinMode(BUTTON_1A, INPUT_PULLUP);
  pinMode(BUTTON_1B, INPUT_PULLUP);
  pinMode(BUTTON_2A, INPUT_PULLUP);
  pinMode(BUTTON_2B, INPUT_PULLUP);
  pinMode(BUTTON_3A, INPUT_PULLUP);
  pinMode(BUTTON_3B, INPUT_PULLUP);
  pinMode(BUTTON_4A, INPUT_PULLUP);
  pinMode(BUTTON_4B, INPUT_PULLUP);

  strip.begin();
  strip.setBrightness(helderheid);
  strip.show();
  Serial.begin(115200);

  lastButtonTime = millis();  // Start de timer
}

void loop() {
  checkButtons();

  // Check of de 30 seconden voorbij zijn
  if (!effectActive && (millis() - lastButtonTime > timeout)) {
    Serial.println("Timeout: Terug naar regenboog...");
    resetToEffects();
  }

  if (effectActive) {
    if (effectMode == 0) {
      ademEffect(70);
    } else if (effectMode == 1) {
      fireStep(100);
    } else {
      fireStep(100);
    }
  }
}


void resetToEffects() {
  effectActive = true;
  set1Choice = 0;
  set2Choice = 0;
  set3Choice = 0;
  set4Choice = 0;
  strip.clear();
  lastButtonTime = millis();  // Reset timer ook bij handmatige reset
}

/// --------- LICHT EFFECTEN  ---------------||||||||----------------------////

/// - ademen
void ademEffect(int ademSnelheid) {
  static float angle = 0;
  float intensity = (sin(angle) + 1.0) / 2.0;  // Bereken de helderheid (0.0 tot 1.0)
  int currentBrightness = intensity * helderheid;

  // Kleur instellen voor alle LED's (hier op wit gezet via het W-kanaal van de GRBW strip)
  // Je kunt dit aanpassen naar strip.Color(currentBrightness, 0, 0, 0) voor rood, etc.
  uint32_t color = strip.Color(currentBrightness, currentBrightness, currentBrightness, currentBrightness);

  for (int i = 0; i < NUM; i++) {
    strip.setPixelColor(i, color);
  }
  strip.show();
  // Verhoog de hoek voor de volgende stap (hoe kleiner de stap, hoe trager het ademen)
  angle += (pulseSpeed / ademSnelheid);
  if (angle > TWO_PI) {
    angle -= TWO_PI;
  }
  delay(10);  // Kleine pauze voor stabiliteit
}



// EFFECT 3: "Color Wipe Bounce" - Een kleur die heen en weer loopt
void bounceStep(int wait) {
  static int pos = 0;
  static int direction = 1;
  static uint32_t color = strip.Color(255, 0, 100, 0);  // Roze/Paars

  strip.clear();
  strip.setPixelColor(pos, color);
  strip.show();

  pos += direction;
  if (pos <= 0 || pos >= NUM - 1) {
    direction *= -1;  // Draai om bij de uiteinden
    // Verander van kleur bij elk keerpunt (optioneel)
    if (pos == 0) color = strip.Color(0, 255, 255, 0);  // Cyaan
    else color = strip.Color(255, 200, 0, 0);           // Goud
  }
  delay(wait * 2);
}

// EFFECT 4: "Theater Chase" - Looplicht effect (wit met kleuren)
void theaterStep(int wait) {
  static int q = 0;
  strip.clear();

  for (int i = 0; i < NUM; i++) {
    if ((i + q) % 3 == 0) {
      strip.setPixelColor(i, strip.Color(0, 0, 0, 255));  // Witte pixel
    } else {
      strip.setPixelColor(i, strip.Color(50, 0, 150, 0));  // Paarse pixel achtergrond
    }
  }
  strip.show();

  q++;
  if (q >= 3) q = 0;
  delay(wait * 1.5);
}

// EFFECT 5: "Fire Flicker" - Warme flikkering zoals een vuurtje
void fireStep(int wait) {
  for (int i = 0; i < NUM; i++) {
    int flicker = random(50, 200);  // Willekeurige helderheid
    // Mix van Rood en Oranje/Geel
    strip.setPixelColor(i, strip.Color(flicker, flicker / 4, 0, 0));
  }
  strip.show();
  delay(wait + random(50));  // Onregelmatige pauze voor realistischer effect
}



/// --------- KNOPPEN ------------------------------------------------////
void checkButtons() {
  // We controleren of er een knop is ingedrukt
  bool pressed = false;

  if (digitalRead(BUTTON_START) == LOW) {
    resetToEffects();
    effectMode++;
    if (effectMode > 1) effectMode = 0;
    pressed = true;
  }

  // SET 1
  if (digitalRead(BUTTON_1A) == LOW) {
    set1Choice = 1;
    effectActive = false;
    pressed = true;
  }
  if (digitalRead(BUTTON_1B) == LOW) {
    set1Choice = 2;
    effectActive = false;
    pressed = true;
  }

  // SET 2
  if (digitalRead(BUTTON_2A) == LOW) {
    set2Choice = 1;
    effectActive = false;
    pressed = true;
  }
  if (digitalRead(BUTTON_2B) == LOW) {
    set2Choice = 2;
    effectActive = false;
    pressed = true;
  }

  // SET 3
  if (digitalRead(BUTTON_3A) == LOW) {
    set3Choice = 1;
    effectActive = false;
    pressed = true;
  }
  if (digitalRead(BUTTON_3B) == LOW) {
    set3Choice = 2;
    effectActive = false;
    pressed = true;
  }

  // SET 4
  if (digitalRead(BUTTON_4A) == LOW) {
    set3Choice = 1;
    effectActive = false;
    pressed = true;
  }
  if (digitalRead(BUTTON_4B) == LOW) {
    set3Choice = 2;
    effectActive = false;
    pressed = true;
  }

  // Als er gedrukt is, reset de timer
  if (pressed) {
    lastButtonTime = millis();
    delay(200);  // Debounce
  }
}

