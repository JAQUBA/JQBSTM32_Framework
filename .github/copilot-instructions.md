# JQBSTM32 Framework – instrukcje dla Copilota

## Zakres
JQBSTM32 to lokalny framework C++ dla STM32Cube/HAL używany przez MS10S. Odpowiadaj po polsku, gdy użytkownik pisze po polsku. Przed zmianą czytaj właściwy nagłówek i implementację modułu; publiczne API musi pozostać zgodne z istniejącymi przykładami.

## Architektura
- `src/Core.cpp/.h`: globalny `Core`, `millis`, `delay`, HAL callbacks i wejścia `init/setup/loop`.
- `src/Scheduler.hpp/.cpp`: kooperatywny scheduler dla kontekstu main i SysTick ISR.
- `src/Hardware/`: GPIO, UART, I2C, SPI, Timer/PWM, Analog, CAN/FDCAN, OneWire, Encoder, FlashMemory, USB_CDC.
- `src/Device/`: sterowniki DS18B20, PCF8583, EEP24Cxx, FM25V05, TMC2209, DFR0646, SED1520.
- `src/Util/`: Modbus, RegisterBank, MemoryBlock, Menu i Math.

## Kontrakty
- `TASK(name)` ma sygnaturę `void name(taskStruct *task)`; `taskCallback`, `dataCallback` i `voidCallback` są makrami lambd.
- `MUL_100US`, `MUL_1MS`, `MUL_10MS`, `MUL_100MS`, `MUL_1S` są bazami czasu schedulerowego.
- HAL busów używa kolejek i callbacków; nie zakładaj synchronicznego zakończenia DMA.
- `RegisterBank` adresuje 16-bitowe rejestry globalnie; `getValue/setValue` są względne, `getRegister/setRegister` absolutne.
- `MemoryBlock` jest granicą trwałości; `save()` wykonuj świadomie, szczególnie dla EEPROM.
- Nie używaj blokującego `delay()` w taskach. Nie dodawaj niekontrolowanego heapu w ścieżkach runtime.

## Jakość i kompatybilność
- Zachowaj istniejący styl 4 spacji, nazwy PascalCase/camelCase i komentarze Doxygen w publicznym API.
- Nie dodawaj zależności ani wyjątków bez wyraźnej potrzeby.
- Dla zmian sprzętowych uwzględnij `#ifdef HAL_*_MODULE_ENABLED` i limity instancji.
- Sprawdź przykłady oraz projekt MS10S po zmianie frameworka; sama kompilacja biblioteki nie wystarcza.

Szczegóły wzorców i procedury walidacji są w [framework-patterns.md](knowledge/framework-patterns.md). Skill `jqbstm32-module-change` służy do zmian modułów frameworka.
