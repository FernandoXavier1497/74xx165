# 74xx165

Biblioteca Arduino para o registrador de deslocamento **74HC165** (e 74LS165/74HCT165): transforma 3 pinos do microcontrolador em 8 entradas digitais, e 16, 24, 32... com CIs em cascata.

Funciona em qualquer placa com o core Arduino (Uno, Nano, Mega, ESP32, STM32duino etc.).

## Ligação (1 CI)

| 74HC165 | Pino | Liga em |
|---|---|---|
| PL (load) | 1 | `loadPin` do MCU |
| CP (clock) | 2 | `clockPin` do MCU |
| QH (saída serial) | 9 | `dataPin` do MCU |
| CE (clock enable) | 15 | GND (ou `cePin`, opcional) |
| SER | 10 | GND (último CI da cascata) |
| A, B, C, D | 11, 12, 13, 14 | entradas 0 a 3 |
| E, F, G, H | 3, 4, 5, 6 | entradas 4 a 7 |
| VCC / GND | 16 / 8 | 5 V (ou 3,3 V) / GND |

**Cascata:** o QH (pino 9) do CI seguinte vai no SER (pino 10) do CI anterior. O CI cujo QH vai ao MCU é o **chip 0**. PL, CP (e CE) são compartilhados por todos.

Entrada `n` = `chip * 8 + bit`, onde A = bit 0 e H = bit 7.

## Uso

```cpp
#include <74xx165.h>

IC74xx165 ci(4, 3, 2, 2);   // data, clock, load, nº de CIs [, cePin]

void setup() {
  ci.begin();
  ci.setActiveLow(true);    // botões ao GND com pull-up
}

void loop() {
  ci.update();
  if (ci.rose(0))  { /* entrada 0 foi acionada */ }
  if (ci.fell(9))  { /* entrada 9 foi solta   */ }
  bool b = ci.read(3);
  uint8_t byte0 = ci.readByte(0);
}
```

## API

| Método | Descrição |
|---|---|
| `IC74xx165(data, clock, load, numChips=1, cePin=SEM_PINO)` | construtor |
| `begin()` | configura pinos e faz a leitura inicial |
| `update()` | captura e lê todas as entradas |
| `read(pino)` / `readByte(chip)` / `readAll()` | estado atual |
| `changed(pino)` / `rose(pino)` / `fell(pino)` / `anyChanged()` | bordas entre dois `update()` |
| `setActiveLow(bool)` | lógica invertida |
| `setPulseDelayUs(us)` | largura dos pulsos (padrão 1 µs) |

## Observações

- Use resistores de pull-up/pull-down (ex.: 10 kΩ) nas entradas; entradas flutuantes leem valores aleatórios.
- Botões mecânicos precisam de debounce: um `update()` a cada 5 a 10 ms já ajuda bastante.
- `readAll()` cobre até 32 entradas; acima disso use `read(pino)` ou `readByte(chip)`.

## Licença

MIT
