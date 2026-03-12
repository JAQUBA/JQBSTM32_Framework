# Utilities & Interfaces

## Overview

The Util layer provides protocol implementations, data management, and helper functions used across applications. The Interface layer defines abstract contracts for bus communication and external memory access.

---

## Interfaces

### IBus

**Header**: `Interface/IBus.h`

```cpp
class IBus {
    public:
    protected:
};
```

Base interface for all communication bus modules. All Hardware bus drivers (UART, I2C, SPI, CAN, FDCAN, OneWire, USB_CDC) inherit from this interface.

### IExternalMemory

**Header**: `Interface/IExternalMemory.h`

```cpp
class IExternalMemory {
    public:
        virtual void readFromMemory(uint32_t MemAddress, uint8_t *pData, uint16_t Size) = 0;
        virtual void writeToMemory(uint32_t MemAddress, uint8_t *pData, uint16_t Size) = 0;
};
```

Abstract interface for any external memory device. Implemented by:
- `FlashMemory` (internal STM32 flash)
- `PCF8583` (I2C RTC + RAM)
- `EEP24C04` (I2C EEPROM 512B)
- `EEP24C256` (I2C EEPROM 32KB)
- `FM25V05` (SPI FRAM 64KB)

#### Address Size Constants

```cpp
#define ADDRESS_SIZE_8BIT    0x00000001U
#define ADDRESS_SIZE_16BIT   0x00000002U
#define ADDRESS_SIZE_24BIT   0x00000003U
#define ADDRESS_SIZE_32BIT   0x00000004U
#define MEM_ADR_INVALID      0xFFFFFFFFU
```

---

## Modbus RTU

**Header**: `Util/Modbus/Modbus.h`  
**Implementation**: `Util/Modbus/Modbus.cpp`

Industrial communication protocol implementation following the official Modbus specification (www.modbus.org).

### Classes

```cpp
class Modbus {
    void receive(uint8_t *data, uint16_t length, dataCallback_f functionPointer);
    void bind_function(ModbusFunction func, void(*handler)(ModbusFrame *request));
};

class ModbusSlave : public Modbus {
    void setID(uint8_t *slaveID);
};
```

### Function Codes

```cpp
enum ModbusFunction {
    FUNC_3,   // 0x03 — Read Holding Registers
    FUNC_4,   // 0x04 — Read Input Registers
    FUNC_6,   // 0x06 — Write Single Register
    FUNC_10   // 0x10 — Write Multiple Registers
};
```

### Frame Structure

```cpp
struct ModbusFrame {
    ModbusFunction function;   // Function code
    uint16_t address;          // Starting register address
    uint16_t size;             // Number of registers
    uint16_t registers[125];   // Register data (max 125 per spec)
};
```

### Protocol Flow

1. **Receive**: Raw UART data → `modbus.receive(data, len, txCallback)`
2. **Parse**: Verify slave ID, extract function code, address, size
3. **Dispatch**: Call bound handler for the function code
4. **Response**: Build response frame with CRC16, send via callback

### Usage

```cpp
ModbusSlave modbus;
UART rs485(&huart1, DIR_PORT, DIR_PIN);

// Set slave ID (from register bank)
modbus.setID((uint8_t*)opcje.getRegisterPtr(526));

// Bind function handlers
modbus.bind_function(ModbusFunction::FUNC_3, [](ModbusFrame *request) {
    RegisterBank *bank = RegisterBank::find(request->address);
    if (bank) {
        request->size = bank->readRegisters(
            request->registers, request->address, request->size);
    }
});

modbus.bind_function(ModbusFunction::FUNC_6, [](ModbusFrame *request) {
    RegisterBank *bank = RegisterBank::find(request->address);
    if (bank) {
        bank->setRegister(request->address, request->size);
    }
});

// Connect to UART
rs485.onReceiveHandler([](uint8_t *data, uint16_t size) {
    modbus.receive(data, size, [](uint8_t *resp, uint16_t len) {
        rs485.transmit(resp, len);
    });
});
```

---

## RegisterBank

**Header**: `Util/RegisterBank/RegisterBank.h`  
**Implementation**: `Util/RegisterBank/RegisterBank.cpp`

Register bank management system for Modbus-style register mapping with optional persistent storage.

### Architecture

- Banks are stored in a **global linked list** — all created banks are automatically registered
- `RegisterBank::find(address)` searches all banks to locate the one containing a given global address
- Supports both **volatile** (RAM) and **persistent** (external memory) banks

### Constructors

```cpp
// RAM-only (volatile) bank
RegisterBank(uint16_t regAddress, uint16_t size);

// Persistent bank with external memory backing
RegisterBank(uint16_t regAddress, uint16_t size, MemoryBlock *memoryBlock);
```

### Value Access (Relative Offset)

```cpp
uint16_t getValue(uint16_t offset);                          // Read by offset
void setValue(uint16_t offset, uint16_t value,
             bool instantSave = true);                       // Write by offset
uint16_t *getValuePtr(uint16_t offset);                      // Direct pointer
```

### Register Access (Absolute Address)

```cpp
uint16_t getRegister(uint16_t fullAddress);                  // Read by address
void setRegister(uint16_t fullAddress, uint16_t value,
                 bool instantSave = true);                   // Write by address
uint16_t *getRegisterPtr(uint16_t fullAddress);              // Direct pointer
```

### Bulk Operations

```cpp
uint16_t readRegisters(uint16_t *buffer, uint16_t address, uint16_t size);
    // Returns: number of registers actually read
```

### Static Lookup

```cpp
static RegisterBank *find(uint16_t fullAddress);
    // Returns: bank containing the address, or NULL
```

### Persistence

```cpp
void load();    // Load all values from MemoryBlock
void save();    // Save all values to MemoryBlock
void free_bank();  // Release allocated memory
```

When `instantSave = true` in `setValue()`/`setRegister()`, data is automatically written to backing storage immediately.

### Usage Example

```cpp
FlashMemory flash;

// Persistent banks — backed by flash
RegisterBank kalibracja(0U, 16U, new MemoryBlock(&flash, 0x0801F800));
RegisterBank opcje(512U, 16U, new MemoryBlock(&flash, 0x0801F800 + 32));

// Volatile banks — RAM only
RegisterBank wewy(224U, 4U);
RegisterBank analogi(240U, 20U);

// Access by offset (within bank)
uint16_t threshold = opcje.getValue(2);   // opcje[2] = register 514
opcje.setValue(2, 180);                    // Set and auto-save to flash

// Access by absolute address
uint16_t val = opcje.getRegister(514);    // Same as getValue(2)

// Modbus integration — automatic routing
RegisterBank *bank = RegisterBank::find(514);  // Returns &opcje
bank->readRegisters(buffer, 512, 16);          // Read all 16 registers
```

---

## MemoryBlock

**Header**: `Util/MemoryBlock/MemoryBlock.h`  
**Implementation**: `Util/MemoryBlock/MemoryBlock.cpp`

Thin wrapper around `IExternalMemory` that binds a device to a specific base address.

### Constructor

```cpp
MemoryBlock(IExternalMemory *device, uint32_t startingAddress);
```

### API

```cpp
void loadBlock(uint8_t *pData, uint16_t Size);   // Read from device
void saveBlock(uint8_t *pData, uint16_t Size);   // Write to device
```

All operations are relative to `startingAddress`.

### Usage

```cpp
FlashMemory flash;
MemoryBlock configBlock(&flash, 0x0801F800);

// Used by RegisterBank for persistence
RegisterBank config(0, 16, &configBlock);
// Or standalone:
uint8_t data[32];
configBlock.loadBlock(data, 32);
```

---

## SimpleMenu

**Header**: `Util/Menu/SimpleMenu.h`  
**Implementation**: `Util/Menu/SimpleMenu.cpp`

Hierarchical menu system for embedded user interfaces. Supports submenus, value editing, function callbacks, and dynamically generated lists.

### Constructor Variants

```cpp
// Root menu with fixed submenus
SimpleMenu(int numItems, SimpleMenu *submenus);

// Function callback only
SimpleMenu(void (*callback)());

// Dynamic list + callback
SimpleMenu(bool (*listCB)(int index), void (*callback)());

// Dynamic list + submenus
SimpleMenu(bool (*listCB)(int index), SimpleMenu *submenus);

// Named submenu
SimpleMenu(String name, int numItems, SimpleMenu *submenus);

// Named value editor (no limits)
SimpleMenu(String name, int *value);

// Named value editor (with min/max)
SimpleMenu(String name, int *value, int min, int max);

// Named function item
SimpleMenu(String name, void (*callback)());

// Named dynamic list + submenus
SimpleMenu(String name, bool (*listCB)(int index), SimpleMenu *submenus);

// Named dynamic list + callback
SimpleMenu(String name, bool (*listCB)(int index), void (*callback)());
```

### Initialization

```cpp
void begin();
void begin(void (*displayCB)(SimpleMenu *menu));
void begin(void (*displayCB)(SimpleMenu *menu), void (*valueCB)(SimpleMenu *menu));
void begin(SimpleMenu *top, void (*displayCB)(SimpleMenu *menu),
           void (*valueCB)(SimpleMenu *menu));
void setFunctions(void (*displayCB)(SimpleMenu *menu),
                  void (*valueCB)(SimpleMenu *menu));
```

### Navigation

```cpp
void home();               // Go to top level
void select();             // Enter/activate current item
void back();               // Go back to parent
void returned();           // Exit value editing
void up();                 // Previous item
void down();               // Next item
void index(int idx);       // Jump to specific index
```

### Properties

```cpp
SimpleMenu *next();           // Get next item
SimpleMenu *next(int index);  // Get item at index
int getValue();               // Current value (for value items)
bool hasValue();              // Is this a value editor?
int getIndex();               // Current selection index
String name;                  // Display text
bool selectMenu;              // Selection flag
int menuLocation;              // Position in hierarchy
```

### Usage

```cpp
int brightness = 50;
int contrast = 30;

SimpleMenu settingsItems[] = {
    SimpleMenu("Brightness", &brightness, 0, 100),
    SimpleMenu("Contrast", &contrast, 0, 100),
    SimpleMenu("Reset", []() { brightness = 50; contrast = 30; })
};

SimpleMenu mainItems[] = {
    SimpleMenu("Settings", 3, settingsItems),
    SimpleMenu("Info", []() { showInfo(); })
};

SimpleMenu root(2, mainItems);

root.begin([](SimpleMenu *menu) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(menu->name);
    lcd.flush();
}, [](SimpleMenu *menu) {
    // Value changed callback
    saveSettings();
});

// In encoder callback:
void onEncoderCW()  { root.down(); }
void onEncoderCCW() { root.up(); }
void onEncoderBtn() { root.select(); }
```

---

## Math Utilities

### CRC16

**Header**: `Util/Math/CRC16.h`  
**Implementation**: `Util/Math/CRC16.c`

```cpp
uint16_t CRC16(const uint8_t *wsk, uint16_t len);
```

Standard **Modbus CRC-16** (polynomial 0x8005, initial 0xFFFF). Used internally by the Modbus module for frame validation.

### BCD Conversion

```cpp
uint8_t bcdToDec(uint8_t val);   // BCD → decimal (0x23 → 23)
uint8_t decToBcd(uint8_t val);   // Decimal → BCD (23 → 0x23)
```

Used with RTC chips (PCF8583) that store time in BCD format.

### Random Numbers

**Header**: `Util/Math/Random.h`  
**Implementation**: `Util/Math/Random.c`

```cpp
float random_float(float min, float max);  // [min, max)
int random_int(int min, int max);          // [min, max]
```

Based on standard `rand()`. Consider seeding with `srand()` for non-deterministic behavior.

### Time Constants

**Header**: `Util/Math/Time.h`

```cpp
#define SECONDS  1000UL     // 1000 ms
#define MINUTES  60000UL    // 60000 ms
#define TIME(x, y)  x * y   // TIME(5, SECONDS) = 5000
```
