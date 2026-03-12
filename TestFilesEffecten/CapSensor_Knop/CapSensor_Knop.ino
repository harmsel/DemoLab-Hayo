#include <CapacitiveSensor.h>


const int knop = 2;
CapacitiveSensor cs_4_6 = CapacitiveSensor(4, 6);  // Sensor op pin 6 - 330k tussen 4 & 6

void setup() {
  pinMode(knop, INPUT_PULLUP);
 // cs_4_6.set_CS_AutocaL_Millis(0xFFFFFFFF);  // Zet deze regel aan, dan zet je Autocalibrate uit. Autocalibratie is wel handig omdat de waarde anders zou verlopen. Zonder auto calibratie zet hij elke keer bij opstarten van de arduino de 0 waarde
  Serial.begin(9600);
}

void loop() {

  int knopStand = digitalRead(knop);
  if (knopStand == LOW) {
    long start = millis();
    long capSens = cs_4_6.capacitiveSensor(30);

    Serial.println(capSens);        
  }
}
