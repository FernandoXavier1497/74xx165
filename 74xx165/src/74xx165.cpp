#include "74xx165.h"
#include <new>

IC74xx165::IC74xx165(uint8_t dataPin, uint8_t clockPin, uint8_t loadPin,
                     uint8_t numChips, uint8_t cePin)
    : _data(dataPin), _clock(clockPin), _load(loadPin), _ce(cePin),
      _numChips(numChips ? (numChips > 32 ? 32 : numChips) : 1),
      _pulseUs(1), _invert(false), _cur(nullptr), _prev(nullptr) {}

IC74xx165::~IC74xx165() {
  delete[] _cur;
  delete[] _prev;
}

bool IC74xx165::begin() {
  if (!_cur)  _cur  = new (std::nothrow) uint8_t[_numChips];
  if (!_prev) _prev = new (std::nothrow) uint8_t[_numChips];
  if (!_cur || !_prev) return false;

  pinMode(_data, INPUT);
  pinMode(_clock, OUTPUT);
  pinMode(_load, OUTPUT);
  digitalWrite(_clock, LOW);
  digitalWrite(_load, HIGH);
  if (_ce != SEM_PINO) {
    pinMode(_ce, OUTPUT);
    digitalWrite(_ce, LOW);
  }

  // Leitura inicial: evita bordas falsas no primeiro update()
  readHardware(_cur);
  memcpy(_prev, _cur, _numChips);
  return true;
}

void IC74xx165::update() {
  if (!_cur) return;
  memcpy(_prev, _cur, _numChips);
  readHardware(_cur);
}

void IC74xx165::readHardware(uint8_t* dst) {
  // 1) Captura das entradas paralelas (PL em nivel baixo).
  //    O CE so deve mudar com o CP em nivel alto (recomendacao do datasheet).
  if (_ce != SEM_PINO) {
    digitalWrite(_clock, HIGH);
    digitalWrite(_ce, HIGH);
  }
  digitalWrite(_load, LOW);
  pulseDelay();
  digitalWrite(_load, HIGH);
  if (_ce != SEM_PINO) {
    digitalWrite(_ce, LOW);
    digitalWrite(_clock, LOW);
  }
  pulseDelay();

  // 2) Apos o load, o QH ja apresenta a entrada H do CI mais proximo.
  //    Le o bit e depois gera a borda de subida do clock para avancar.
  for (uint8_t chip = 0; chip < _numChips; chip++) {
    uint8_t v = 0;
    for (uint8_t i = 0; i < 8; i++) {
      v = (uint8_t)((v << 1) | (digitalRead(_data) ? 1 : 0));
      digitalWrite(_clock, HIGH);
      pulseDelay();
      digitalWrite(_clock, LOW);
    }
    dst[chip] = v;  // primeiro bit lido (H) acabou no bit7; A no bit0
  }
}

bool IC74xx165::rawBit(const uint8_t* buf, uint16_t pino) const {
  if (!buf || pino >= pinCount()) return false;
  return (buf[pino >> 3] >> (pino & 7)) & 1;
}

bool IC74xx165::read(uint16_t pino) const {
  if (pino >= pinCount()) return false;
  return rawBit(_cur, pino) ^ _invert;
}

uint8_t IC74xx165::readByte(uint8_t chip) const {
  if (!_cur || chip >= _numChips) return 0;
  return _invert ? (uint8_t)~_cur[chip] : _cur[chip];
}

uint32_t IC74xx165::readAll() const {
  uint32_t r = 0;
  for (uint8_t c = 0; c < _numChips && c < 4; c++)
    r |= (uint32_t)readByte(c) << (8 * c);
  return r;
}

bool IC74xx165::changed(uint16_t pino) const {
  return rawBit(_cur, pino) != rawBit(_prev, pino);
}

bool IC74xx165::rose(uint16_t pino) const {
  return read(pino) && !(rawBit(_prev, pino) ^ _invert);
}

bool IC74xx165::fell(uint16_t pino) const {
  return !read(pino) && (rawBit(_prev, pino) ^ _invert);
}

bool IC74xx165::anyChanged() const {
  if (!_cur) return false;
  for (uint8_t i = 0; i < _numChips; i++)
    if (_cur[i] != _prev[i]) return true;
  return false;
}
