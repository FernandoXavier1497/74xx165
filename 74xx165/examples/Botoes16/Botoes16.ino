// 16 botoes com 2 CIs em cascata. Botoes ligados ao GND, com resistor de
// pull-up (10k) em cada entrada -> usamos logica invertida.
// Cascata: QH do CI 1 -> SER (pino 10) do CI 0; QH do CI 0 -> MCU.
#include <74xx165.h>

IC74xx165 botoes(4, 3, 2, 2);   // data, clock, load, 2 CIs

void setup() {
  Serial.begin(115200);
  botoes.begin();
  botoes.setActiveLow(true);
}

void loop() {
  botoes.update();
  for (uint8_t i = 0; i < botoes.pinCount(); i++) {
    if (botoes.rose(i)) { Serial.print(F("Apertou ")); Serial.println(i); }
    if (botoes.fell(i)) { Serial.print(F("Soltou  ")); Serial.println(i); }
  }
  delay(5);
}
