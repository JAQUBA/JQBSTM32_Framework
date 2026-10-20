---
name: jqbstm32-module-change
description: "Use when changing JQBSTM32 framework modules: Scheduler/Core, GPIO, UART, I2C, SPI, Timer, Analog, OneWire, device drivers, Modbus, RegisterBank or MemoryBlock."
---

# Zmiana modułu JQBSTM32

1. Znajdź publiczny nagłówek i odpowiadający mu `.cpp`; sprawdź użycia w `examples/` oraz projekcie MS10S.
2. Zachowaj model asynchroniczny: kolejka, stan operacji, timeout i callback. Ustal, kto posiada każdy bufor i kiedy można go zwolnić.
3. Nie wykonuj blokującego `delay()` ani ciężkiej pracy w ISR. Sprawdź wpływ na `HAL_IncTick`, `mainTasks` i limity instancji.
4. Dla `RegisterBank` zachowaj semantykę adresów względnych/absolutnych i jawne `save()` dla pamięci trwałej.
5. Utrzymaj kompatybilność z konfiguracją `HAL_*_MODULE_ENABLED` i opcjonalnymi `custom_using`/flagami PlatformIO.
6. Waliduj najpierw wąsko, potem zbuduj projekt nadrzędny: `pio run -e MS10S`.

Nie zmieniaj API publicznego tylko po to, by uprościć jeden call site. Jeśli kontrakt musi się zmienić, zaktualizuj przykłady i instrukcje frameworka w tym samym zadaniu.
