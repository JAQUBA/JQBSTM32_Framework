# JQBSTM32 Framework

![Project Status](https://img.shields.io/badge/status-in%20development-yellow)
![License](https://img.shields.io/badge/license-GPL-blue)
![Platform](https://img.shields.io/badge/platform-STM32-green)
![Framework](https://img.shields.io/badge/framework-PlatformIO-orange)

## Opis

JQBSTM32 Framework to zaawansowana biblioteka stworzona do szybkiego i efektywnego rozwoju aplikacji dla mikrokontrolerów STM32. Framework zapewnia kompleksowe narzędzia do zarządzania zasobami sprzętowymi, obsługi urządzeń peryferyjnych oraz optymalizacji kodu.

## Funkcje

### 🔧 Hardware Abstraction Layer (HAL)
- **Analog/ADC** - Obsługa przetworników analogowo-cyfrowych z kalibracją
- **I2C** - Komunikacja I2C z obsługą wielu urządzeń
- **SPI** - Interfejs SPI dla szybkiej komunikacji
- **UART** - Komunikacja szeregowa z obsługą RS232/RS485
- **Timer/PWM** - Zarządzanie timerami i generacja PWM
- **GPIO** - Kontrola portów I/O
- **OneWire** - Protokół OneWire dla czujników temperatury
- **USB CDC** - Komunikacja USB jako wirtualny port szeregowy
- **CAN/FDCAN** - Magistrala CAN dla systemów automotive
- **Flash Memory** - Zarządzanie pamięcią flash wewnętrzną

### 📟 Obsługiwane urządzenia
- **DS18B20** - Cyfrowe czujniki temperatury OneWire
- **EEP24C04/EEP24C256** - Pamięci EEPROM I2C
- **FM25V05** - Pamięć FRAM SPI
- **PCF8583** - Zegar czasu rzeczywistego I2C
- **SED1520** - Kontroler wyświetlacza LCD
- **DFR0646** - Moduł wyświetlacza
- **TMC2209** - Sterownik silnika krokowego

### 🛠️ Narzędzia użytkowe
- **Modbus** - Protokół komunikacyjny Modbus RTU/ASCII
- **RegisterBank** - System banków rejestrów dla danych konfiguracyjnych
- **MemoryBlock** - Zarządzanie blokami pamięci
- **Menu System** - System menu dla interfejsów użytkownika
- **PID Controller** - Regulator PID
- **Math Utils** - Funkcje matematyczne
- **Scheduler** - Zaawansowany scheduler zadań

## Wymagania

### Hardware
- **Mikrokontrolery**: STM32 (wszystkie serie)
- **Narzędzia**: STM32CubeMX dla konfiguracji pinów

### Software
- **Platform**: PlatformIO
- **IDE**: Visual Studio Code z rozszerzeniem PlatformIO
- **Framework**: STM32Cube HAL
- **Kompilator**: GCC ARM

### Systemy operacyjne
- Windows 10/11
- macOS 10.15+
- Linux (Ubuntu 18.04+)

## Instalacja

### 1. Przez PlatformIO Registry
```ini
# platformio.ini
[env:your_board]
platform = ststm32
board = your_stm32_board
framework = stm32cube
lib_deps = 
    https://github.com/JAQUBA/JQBSTM32_Framework.git
```

### 2. Lokalna instalacja
```bash
git clone https://github.com/JAQUBA/JQBSTM32_Framework.git
```

## Szybki start

### 1. Podstawowa konfiguracja
```cpp
#include "Core.h"
#include "Hardware/UART/UART.h"
#include "Device/DS18B20/DS18B20.h"

// Inicjalizacja UART
UART debug(&huart1);

void setup() {
    // Konfiguracja systemu
    
    // Uruchomienie głównej logiki
    logicStart();
}

void loop() {
    // Główna pętla programu
}
```

### 2. Obsługa czujnika DS18B20
```cpp
#include "Hardware/OneWire/OneWire.h"
#include "Device/DS18B20/DS18B20.h"

OneWire ow(new Timer(&htim7), PIN_OW);
DS18B20Manager tempManager(&ow);

void setup() {
    // Dodaj czujnik
    tempManager.addSensor(0, 0x28FF123456789ABC);
    
    // Automatyczny odczyt co sekundę
    tempManager.setAutoReadCallback([](uint8_t id, float temp, bool success) {
        if(success) {
            printf("Temperatura: %.2f°C\n", temp);
        }
    }, 1000);
}
```

### 3. Komunikacja Modbus
```cpp
#include "Util/Modbus/Modbus.h"
#include "Util/RegisterBank/RegisterBank.h"

ModbusSlave modbus;
RegisterBank config(0, 16);

void setup() {
    modbus.setID(1);
    
    modbus.bind_function(ModbusFunction::FUNC_3, [](ModbusFrame *request) {
        RegisterBank *bank = RegisterBank::find(request->address);
        if(bank) {
            request->size = bank->readRegisters(request->registers, 
                                              request->address, request->size);
        }
    });
}
```

## Przykłady

Framework zawiera obszerne przykłady w folderze `examples/`:

- **ADC_MultiChannel** - Obsługa wielokanałowego ADC
- **blink** - Podstawowy przykład migania LED
- **DS18B20** - Kompletny przykład obsługi czujników temperatury
- **GPIO_BasicOperations** - Operacje na portach GPIO
- **I2C_MultiDevice** - Komunikacja z wieloma urządzeniami I2C
- **Memory_Management** - Zarządzanie pamięcią flash
- **SPI_MultiDevice** - Obsługa wielu urządzeń SPI
- **Timer_PWM_Advanced** - Zaawansowane użycie timerów i PWM
- **UART_Communication** - Komunikacja UART z protokołami

## API Reference

### Core System
```cpp
// Dodawanie zadań do schedulera
taskStruct addTaskMain(taskCallback_f func, uint32_t delay = 0, bool single = false);
taskStruct addTaskInterrupt(taskCallback_f func, uint32_t delay = 0, bool single = false);
```

### DS18B20Manager
```cpp
class DS18B20Manager {
public:
    // Dodawanie czujników
    bool addSensor(uint8_t id, uint64_t romCode);
    bool addSensorFromFlash(uint8_t id, uint32_t flashAddress);
    
    // Odczyt temperatury
    void readTemperature(uint8_t id, TemperatureCallback callback);
    float getTemperature(uint8_t id);
    
    // Automatyczny odczyt
    void setAutoReadCallback(TemperatureCallback callback, uint32_t intervalMs);
    void enableAutoRead(uint32_t intervalMs = 1000);
    void disableAutoRead();
};
```

### RegisterBank
```cpp
class RegisterBank {
public:
    RegisterBank(uint16_t startAddress, uint16_t size, MemoryBlock* memory = nullptr);
    
    void setRegister(uint16_t address, uint16_t value);
    uint16_t getRegister(uint16_t address);
    uint16_t* getRegisterPtr(uint16_t address);
    
    uint16_t readRegisters(uint16_t* buffer, uint16_t address, uint16_t count);
    void writeRegisters(uint16_t* buffer, uint16_t address, uint16_t count);
};
```

## Struktura projektu

```
lib/JQBSTM32/
├── library.json          # Metadane biblioteki
├── README.md             # Dokumentacja
├── LICENSE               # Licencja GPL-3.0
├── src/                  # Kod źródłowy
│   ├── Core.h/.cpp       # Rdzeń systemu
│   ├── Scheduler.hpp/.cpp # Scheduler zadań
│   ├── Device/           # Sterowniki urządzeń
│   │   ├── DS18B20/      # Czujniki temperatury
│   │   ├── EEP24C04/     # Pamięci EEPROM
│   │   └── ...
│   ├── Hardware/         # Warstwa HAL
│   │   ├── Analog/       # ADC
│   │   ├── I2C/          # Komunikacja I2C
│   │   ├── UART/         # Komunikacja szeregowa
│   │   └── ...
│   └── Util/             # Narzędzia
│       ├── Modbus/       # Protokół Modbus
│       ├── RegisterBank/ # System rejestrów
│       └── ...
├── examples/             # Przykłady użycia
└── docs/                 # Dokumentacja
```

## Wsparcie techniczne

### Zgłaszanie błędów
Problemy można zgłaszać przez [GitHub Issues](https://github.com/JAQUBA/JQBSTM32_Framework/issues)

### Dyskusje
Pytania i dyskusje: [GitHub Discussions](https://github.com/JAQUBA/JQBSTM32_Framework/discussions)

### Dokumentacja
Szczegółowa dokumentacja dostępna w folderze `docs/` oraz na [GitHub Wiki](https://github.com/JAQUBA/JQBSTM32_Framework/wiki)

## Współpraca

Zapraszamy do współpracy! Zobacz [CONTRIBUTING.md](CONTRIBUTING.md) aby dowiedzieć się jak:
- Zgłaszać błędy
- Proponować nowe funkcje
- Tworzyć pull requesty
- Pisać dokumentację

## Licencja

Ten projekt jest licencjonowany na warunkach licencji GNU General Public License v3.0 lub nowszej. 
Zobacz plik [LICENSE](LICENSE) po szczegółowe informacje.

## Autorzy

- **JAQUBA** - Główny autor i maintainer
  - GitHub: [@JAQUBA](https://github.com/JAQUBA)
  - Email: kjakubowski0492@gmail.com

## Changelog

### v1.0.0-alpha
- Wstępne wydanie framework'a
- Podstawowe sterowniki Hardware Abstraction Layer
- Obsługa czujników DS18B20
- System Modbus RTU/ASCII
- Scheduler zadań
- Przykłady użycia

## Roadmap

### v1.0.0 (Planowane)
- [ ] Stabilne API
- [ ] Kompletna dokumentacja
- [ ] Rozszerzone testy jednostkowe
- [ ] Wsparcie dla większej liczby urządzeń

### v1.1.0 (Przyszłość)
- [ ] Obsługa protokołu Ethernet
- [ ] System plików na SD Card
- [ ] Interfejs graficzny
- [ ] Wsparcie dla STM32H7

---

**Framework JQBSTM32** - Przyspiesz rozwój swoich projektów STM32! 🚀
