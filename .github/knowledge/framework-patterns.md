# JQBSTM32: wzorce frameworka

## Core i scheduler
Globalny `Core _core` w `src/Core.cpp` wykonuje `HAL_Init()`, `SystemClock_Config()`, konfiguruje SysTick 100 us i dopiero potem wywołuje `init()`. `main()` wywołuje `setup()`, odmierza ticki dla `mainTasks`, wykonuje gotowe taski i `loop()`. `HAL_IncTick()` odpala `interruptTasks` oraz sygnalizuje pracę main schedulera.

Scheduler jest kooperatywny. `task->delay` można zmienić dla dynamicznego następnego uruchomienia; `task->_delay` przechowuje bazę. Task ISR nie może używać blokujących operacji, ciężkich alokacji ani API zależnego od main context.

## Moduły HAL
Typowy moduł ma singleton/limity instancji, kolejkę operacji, stany IDLE/CHECK_FREE/WORK/WAITING/FINISH/CLEAR i callback zakończenia. Najpierw sprawdź konkretną implementację, bo szczegóły różnią się między UART, I2C, SPI, OneWire i Analog.

Bufory przekazywane do operacji DMA muszą żyć do callbacku. Callbacki mogą być wywołane później, więc nie przekazuj wskaźników do lokalnych buforów stosu, jeśli implementacja nie wykonuje operacji natychmiast.

## RegisterBank i MemoryBlock
Bank RAM tworzy rejestry ulotne. Bank z `MemoryBlock` ma osobne `load()`/`save()` i nie zapisuje automatycznie, chyba że `setValue/setRegister` dostanie `instantSave=true`. `find(address)` zwraca bank obejmujący adres; zakresy banków nie powinny się nakładać.

## Modbus
`ModbusSlave::receive()` parsuje ramkę i wywołuje handler związany przez `bind_function`. Handlery są przechowywane per instancja Modbus, więc niezależne kanały nie nadpisują się wzajemnie. `ModbusFrame::registers` ma miejsce na 125 rejestrów. Handler musi poprawnie ustawić `request->size` dla odczytu. Dla funkcji zapisu interpretacja pól wynika z `Modbus.h`, nie z nazwy pola.
Biblioteka obsługuje FC03, FC06 i FC16; broadcast jest dozwolony tylko dla
zapisów i nie otrzymuje odpowiedzi. `Modbus::statistics()` udostępnia
liczniki diagnostyczne bez alokacji dynamicznej.

## Walidacja zmian
Po zmianie modułu:
1. Zbuduj przykład lub projekt używający modułu.
2. Zbuduj `pio run -e MS10S` w repozytorium nadrzędnym.
3. Dla zmian scheduler/busów sprawdź timeouty, callbacki, kolejkę i ścieżkę błędu.
4. Dla zmian pamięci sprawdź rozmiar, adresowanie i liczbę zapisów EEPROM.
