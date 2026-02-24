#include <Adafruit_NeoPixel.h>

int helderheid = 150;
#define PIN_SK A0
#define NUM_SK 6

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

Adafruit_NeoPixel stripSK(NUM_SK, PIN_SK, NEO_GRBW + NEO_KHZ800);

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

  stripSK.begin();
  stripSK.setBrightness(helderheid);
  stripSK.show();
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
    if (effectMode == 0) fireStep(100);
    else twinkleStep(80);
  } else {
    updateBreathing();
  }
}

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

void resetToEffects() {
  effectActive = true;
  set1Choice = 0;
  set2Choice = 0;
  set3Choice = 0;
  set4Choice = 0;
  stripSK.clear();
  lastButtonTime = millis();  // Reset timer ook bij handmatige reset
}


/// - ademen ///
void updateBreathing() {
  if (fadingIn) {
    pulseVal += pulseSpeed;
    if (pulseVal >= 255) {
      pulseVal = 255;
      fadingIn = false;
    }
  } else {
    pulseVal -= pulseSpeed;
    if (pulseVal <= 0) {
      pulseVal = 0;
      fadingIn = true;
    }
  }

  stripSK.clear();
  if (set1Choice == 1) stripSK.setPixelColor(0, stripSK.Color(0, 0, 0, (int)pulseVal));
  else if (set1Choice == 2) stripSK.setPixelColor(1, stripSK.Color(0, 0, 0, (int)pulseVal));

  if (set2Choice == 1) stripSK.setPixelColor(2, stripSK.Color(0, 0, 0, (int)pulseVal));
  else if (set2Choice == 2) stripSK.setPixelColor(3, stripSK.Color(0, 0, 0, (int)pulseVal));

  if (set3Choice == 1) stripSK.setPixelColor(4, stripSK.Color(0, 0, 0, (int)pulseVal));
  else if (set3Choice == 2) stripSK.setPixelColor(5, stripSK.Color(0, 0, 0, (int)pulseVal));

  stripSK.show();
  delay(15);
}

// --- EFFECTEN ---
void rainbowStep(int wait) {
  static uint16_t hue = 0;
  for (int i = 0; i < NUM_SK; i++) {
    uint32_t color = stripSK.ColorHSV(hue + (i * 65536 / NUM_SK));
    stripSK.setPixelColor(i, stripSK.gamma32(color));
  }
  stripSK.show();
  hue += 256;
  delay(wait);
}

void twinkleStep(int wait) {
  int i = random(NUM_SK);
  stripSK.setPixelColor(i, stripSK.Color(random(255), random(255), random(255), random(255)));
  stripSK.show();
  delay(wait);
  stripSK.setPixelColor(i, 0);
  stripSK.show();
}

// --- EXTRA EFFECTEN ---

// EFFECT 3: "Color Wipe Bounce" - Een kleur die heen en weer loopt
void bounceStep(int wait) {
  static int pos = 0;
  static int direction = 1;
  static uint32_t color = stripSK.Color(255, 0, 100, 0); // Roze/Paars

  stripSK.clear();
  stripSK.setPixelColor(pos, color);
  stripSK.show();
  
  pos += direction;
  if (pos <= 0 || pos >= NUM_SK - 1) {
    direction *= -1; // Draai om bij de uiteinden
    // Verander van kleur bij elk keerpunt (optioneel)
    if(pos == 0) color = stripSK.Color(0, 255, 255, 0); // Cyaan
    else color = stripSK.Color(255, 200, 0, 0); // Goud
  }
  delay(wait * 2); 
}

// EFFECT 4: "Theater Chase" - Looplicht effect (wit met kleuren)
void theaterStep(int wait) {
  static int q = 0;
  stripSK.clear();
  
  for (int i = 0; i < NUM_SK; i++) {
    if ((i + q) % 3 == 0) {
      stripSK.setPixelColor(i, stripSK.Color(0, 0, 0, 255)); // Witte pixel
    } else {
      stripSK.setPixelColor(i, stripSK.Color(50, 0, 150, 0)); // Paarse pixel achtergrond
    }
  }
  stripSK.show();
  
  q++;
  if (q >= 3) q = 0;
  delay(wait * 1.5);
}

// EFFECT 5: "Fire Flicker" - Warme flikkering zoals een vuurtje
void fireStep(int wait) {
  for (int i = 0; i < NUM_SK; i++) {
    int flicker = random(50, 200); // Willekeurige helderheid
    // Mix van Rood en Oranje/Geel
    stripSK.setPixelColor(i, stripSK.Color(flicker, flicker/4, 0, 0)); 
  }
  stripSK.show();
  delay(wait + random(50)); // Onregelmatige pauze voor realistischer effect
}
