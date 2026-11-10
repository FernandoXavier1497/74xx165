/*
 * 74xx165 - biblioteca Arduino para registradores de deslocamento
 * paralelo-serial 74HC165 / 74LS165 / 74HCT165 (e compativeis).
 *
 * Suporta varios CIs em cascata (daisy chain), deteccao de borda
 * (apertou/soltou) e logica invertida (botoes com pull-up).
 *
 * Licenca MIT
 */
#ifndef IC74XX165_H
#define IC74XX165_H

#include <Arduino.h>

class IC74xx165 {
 public:
  static const uint8_t SEM_PINO = 0xFF;

  // dataPin  -> pino do MCU ligado ao QH (pino 9) do CI mais proximo do MCU
  // clockPin -> CP (pino 2)
  // loadPin  -> PL (pino 1, ativo em nivel baixo)
  // numChips -> quantidade de CIs em cascata (1 a 32)
  // cePin    -> CE (pino 15, opcional). Se nao usar, ligue o CE no GND.
  IC74xx165(uint8_t dataPin, uint8_t clockPin, uint8_t loadPin,
            uint8_t numChips = 1, uint8_t cePin = SEM_PINO);
  ~IC74xx165();

  // Configura os pinos e faz a primeira leitura. Retorna false se faltou memoria.
  bool begin();

  // Captura as entradas paralelas e le todos os bits. Chame a cada ciclo do loop().
  void update();

  // Leitura (usa o ultimo update()). Entrada 0 = A do CI 0, 7 = H do CI 0,
  // 8 = A do CI 1, e assim por diante.
  bool    read(uint16_t pino) const;
  uint8_t readByte(uint8_t chip) const;   // bit0 = A ... bit7 = H
  uint32_t readAll() const;               // ate 32 entradas (chip 0 = byte menos significativo)

  // Eventos entre os dois ultimos update()
  bool changed(uint16_t pino) const;
  bool rose(uint16_t pino) const;         // foi de 0 para 1 (apos inversao)
  bool fell(uint16_t pino) const;         // foi de 1 para 0 (apos inversao)
  bool anyChanged() const;

  // true = entradas em logica invertida (ex.: botao ligado ao GND com pull-up:
  // read() retorna true quando apertado).
  void setActiveLow(bool activeLow) { _invert = activeLow; }

  // Largura dos pulsos em microssegundos (padrao 1). Aumente com fios longos/clock alto.
  void setPulseDelayUs(uint8_t us) { _pulseUs = us; }

  uint16_t pinCount() const { return (uint16_t)_numChips * 8; }
  uint8_t  chipCount() const { return _numChips; }

 private:
  IC74xx165(const IC74xx165&) = delete;
  IC74xx165& operator=(const IC74xx165&) = delete;

  void readHardware(uint8_t* dst);
  bool rawBit(const uint8_t* buf, uint16_t pino) const;
  inline void pulseDelay() const { if (_pulseUs) delayMicroseconds(_pulseUs); }

  uint8_t  _data, _clock, _load, _ce;
  uint8_t  _numChips;
  uint8_t  _pulseUs;
  bool     _invert;
  uint8_t* _cur;
  uint8_t* _prev;
};

#endif
