// Leitura simples de um 74HC165 (8 entradas) e impressao em binario.
#include <74xx165.h>

const uint8_t PIN_DATA  = 4;  // QH  (pino 9 do CI)
const uint8_t PIN_CLOCK = 3;  // CP  (pino 2)
const uint8_t PIN_LOAD  = 2;  // PL  (pino 1)
// CE (pino 15) ligado ao GND

IC74xx165 ci(PIN_DATA, PIN_CLOCK, PIN_LOAD, 1);

void setup() {
  Serial.begin(115200);
  if (!ci.begin()) {
    Serial.println(F("Falha ao iniciar (memoria)."));
    while (true) {}
  }
}

void loop() {
  ci.update();
  Serial.print(F("Entradas H..A: "));
  for (int8_t i = 7; i >= 0; i--) Serial.print(ci.read(i) ? '1' : '0');
  Serial.println();
  delay(200);
}
