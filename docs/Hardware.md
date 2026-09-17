# Hardware Abstraction Layer

## Overview

The Hardware layer provides C++ wrappers around STM32 HAL peripheral drivers. All modules share a consistent design pattern: singleton instances, async DMA/interrupt-driven operations, operation queues, and state machine lifecycle.

Each module is conditionally compiled based on STM32CubeMX HAL defines (`#ifdef HAL_xxx_MODULE_ENABLED`).

## Common Design Pattern

### State Machine

All queued bus modules (UART, I2C, SPI, OneWire, USB CDC) use this operation lifecycle:

```
IDLE → CHECK_FREE → WORK → WAITING → FINISH → CLEAR
```

| State       | Description                                      |
|-------------|--------------------------------------------------|
| `IDLE`      | No operation in progress, ready for next          |
| `CHECK_FREE`| Verify HAL peripheral is ready                   |
| `WORK`      | Start DMA/interrupt transfer                     |
| `WAITING`   | Transfer in progress, waiting for ISR callback   |
| `FINISH`    | Transfer complete, execute user callback          |
| `CLEAR`     | Free allocated buffers, return to IDLE            |

### Operation Queue

Operations are queued in `std::queue<operation>` and processed sequentially. Data buffers are `malloc`'d when enqueued and `free`'d in CLEAR state.

### Timeout Recovery

Each queued operation stores its own timeout. The timeout begins when the operation enters `WORK`, so time spent waiting in the queue does not reduce its limit. The existing recovery behavior of each transport is preserved when its timeout expires.

| Transport | Default timeout |
|-----------|-----------------|
| I2C       | 4 ms            |
| SPI       | 4 ms            |
| OneWire   | 40 ms           |
| UART      | 200 ms          |
| USB CDC   | 50 ms           |

All queueing methods accept `uint32_t timeoutMs` as their last optional argument. This keeps existing calls source-compatible. When overriding the timeout while no completion callback is needed, pass `nullptr` before the timeout:

```cpp
uint8_t value = 0x01;
sys_i2c.transmit(0x70, &value, 1, nullptr, 4U);
```

---

## GPIO

**Header**: `Hardware/GPIO/GPIO.h`  
**Guard**: `#ifdef HAL_GPIO_MODULE_ENABLED`  
**Instance**: Global `GPIO` object (single instance)

### API

```cpp
// Pin setup
void setup(GPIO_TypeDef *GPIOx, uint16_t pin, uint32_t mode);
void setup(GPIO_TypeDef *GPIOx, uint16_t pin, uint32_t mode, uint32_t pullup);

// Output control
void set(GPIO_TypeDef *GPIOx, uint16_t pin, GPIO_PinState state);
void toggle(GPIO_TypeDef *GPIOx, uint16_t pin);

// Input reading
GPIO_PinState readInput(GPIO_TypeDef *GPIOx, uint16_t pin);
GPIO_PinState readOutput(GPIO_TypeDef *GPIOx, uint16_t pin);

// Interrupt support
bool attachInterrupt(GPIO_TypeDef *GPIOx, uint16_t pin, voidCallback_f callback);
bool detachInterrupt(GPIO_TypeDef *GPIOx, uint16_t pin);
uint32_t getInterruptCount(GPIO_TypeDef *GPIOx, uint16_t pin);
```

### Constants

```cpp
#define HIGH  GPIO_PIN_SET
#define LOW   GPIO_PIN_RESET
#define MAX_GPIO_INTERRUPTS 16  // Override by defining MAX_GPIO_INTERRUPTS (or GPIO_MAX_INTERRUPTS)
```

### Usage

```cpp
#define LED   GPIOC, GPIO_PIN_13
#define BTN   GPIOA, GPIO_PIN_0

GPIO.setup(LED, GPIO_MODE_OUTPUT_PP);
GPIO.setup(BTN, GPIO_MODE_INPUT, GPIO_PULLUP);

GPIO.set(LED, HIGH);
GPIO.toggle(LED);
bool pressed = (GPIO.readInput(BTN) == LOW);

GPIO.attachInterrupt(BTN, []() {
    buttonPressed = true;
});
```

---

## UART

**Header**: `Hardware/UART/UART.h`  
**Guard**: `#ifdef HAL_UART_MODULE_ENABLED`  
**Max instances**: `UART_MAX_INSTANCES` (default 2)

### Constructor

```cpp
UART(UART_HandleTypeDef *pHandler,
     GPIO_TypeDef *dirPort = NULL, uint16_t dirPin = 0);
```

- Without `dirPort/dirPin`: RS232 mode
- With `dirPort/dirPin`: RS485 mode (automatic direction control)

### API

```cpp
static UART *getInstance(UART_HandleTypeDef *pHandler);

// Transmit data asynchronously
void transmit(uint8_t *pData, uint16_t Size,
              dataCallback_f callback = nullptr,
              uint32_t timeoutMs = UART::DEFAULT_TIMEOUT_MS);

// Register receive callback (called when data arrives)
void onReceiveHandler(dataCallback_f onReceive);

// Register transmit-complete callback
void onTransmitHandler(voidCallback_f onTransmit);

// Get bytes available in receive buffer
uint16_t available();
```

### RS485 Direction Control

When `dirPort/dirPin` are provided:
- TX start: direction pin goes HIGH (transmit enable)
- TX complete: direction pin goes LOW (receive enable)

### Usage

```cpp
UART rs485(&huart1, USART1_DIR_GPIO_Port, USART1_DIR_Pin);
UART rs232(&huart3);

rs485.onReceiveHandler([](uint8_t *data, uint16_t size) {
    processData(data, size);
});

uint8_t msg[] = {0x01, 0x03, 0x00};
rs485.transmit(msg, sizeof(msg), [](uint8_t *data, uint16_t size) {
    // Transmit complete
});
```

---

## I2C

**Header**: `Hardware/I2C/I2C.h`  
**Guard**: `#ifdef HAL_I2C_MODULE_ENABLED`  
**Max instances**: `I2C_MAX_INSTANCES` (default 2)

### Constructor

```cpp
I2C(I2C_HandleTypeDef *pHandler);
```

### API

```cpp
static I2C *getInstance(I2C_HandleTypeDef *pHandler);

// Direct transmit/receive
void transmit(uint16_t devAddr, uint8_t *pData, uint16_t size,
        dataCallback_f cb = nullptr,
        uint32_t timeoutMs = I2C::DEFAULT_TIMEOUT_MS);
void receive(uint16_t devAddr, uint8_t *pData, uint16_t size,
          dataCallback_f cb = nullptr,
          uint32_t timeoutMs = I2C::DEFAULT_TIMEOUT_MS);

// Memory-mapped read/write (EEPROM, RTC, etc.)
void readFromMemory(uint16_t devAddr, uint16_t memAddr, uint16_t memAddrSize,
              uint8_t *pData, uint16_t size,
              dataCallback_f cb = nullptr,
              uint32_t timeoutMs = I2C::DEFAULT_TIMEOUT_MS);
void writeToMemory(uint16_t devAddr, uint16_t memAddr, uint16_t memAddrSize,
             uint8_t *pData, uint16_t size,
             dataCallback_f cb = nullptr,
             uint32_t timeoutMs = I2C::DEFAULT_TIMEOUT_MS);
```

### Memory Address Sizes

```cpp
ADDRESS_SIZE_8BIT   // 1 byte address (PCF8583, EEP24C04)
ADDRESS_SIZE_16BIT  // 2 byte address (EEP24C256)
```

### Usage

```cpp
I2C sys_i2c(&hi2c2);

// Read 8 bytes from EEPROM at address 0x100
uint8_t buf[8];
sys_i2c.readFromMemory(0xA0, 0x100, ADDRESS_SIZE_16BIT, buf, 8,
    [](uint8_t *data, uint16_t size) {
        // Read complete
    });
```

---

## SPI

**Header**: `Hardware/SPI/SPI.h`  
**Guard**: `#ifdef HAL_SPI_MODULE_ENABLED`  
**Max instances**: `SPI_MAX_INSTANCES` (default 2)

### Constructor

```cpp
SPI(SPI_HandleTypeDef *pHandler);
```

### API

```cpp
static SPI *getInstance(SPI_HandleTypeDef *pHandler);

void transmit(GPIO_TypeDef *csPort, uint16_t csPin,
              uint8_t *pData, uint16_t size,
              dataCallback_f cb = nullptr,
              uint32_t timeoutMs = SPI::DEFAULT_TIMEOUT_MS);
void receive(GPIO_TypeDef *csPort, uint16_t csPin,
             uint8_t *pData, uint16_t size,
             dataCallback_f cb = nullptr,
             uint32_t timeoutMs = SPI::DEFAULT_TIMEOUT_MS);
void transmitThenReceive(GPIO_TypeDef *csPort, uint16_t csPin,
                         uint8_t *tx, uint16_t txSize,
                         uint8_t *rx, uint16_t rxSize,
                         dataCallback_f cb = nullptr,
                         uint32_t timeoutMs = SPI::DEFAULT_TIMEOUT_MS);
```

CS pin is automatically driven LOW during transfer, HIGH after completion.

### Usage

```cpp
SPI spi1(&hspi1);
#define CS_FRAM  GPIOB, GPIO_PIN_6

uint8_t cmd[] = {0x03, 0x00, 0x00};  // Read command
uint8_t data[64];
spi1.transmitThenReceive(CS_FRAM, cmd, 3, data, 64);
```

---

## Timer

**Header**: `Hardware/Timer/Timer.h`  
**Guard**: `#ifdef HAL_TIM_MODULE_ENABLED`  
**Max instances**: `TIMER_MAX_INSTANCES`

### Constructor

```cpp
Timer(TIM_HandleTypeDef *pHandler);
```

### API

```cpp
static Timer *getInstance(TIM_HandleTypeDef *pHandler);

void attachInterrupt(InterruptType type, voidCallback_f callback);
void setPeriod(uint32_t period);
```

### Interrupt Types

```cpp
enum InterruptType {
    PeriodElapsed,      // Timer overflow
    IC_Capture,         // Input capture
    PWM_PulseFinished,  // PWM pulse complete
    Trigger,            // Trigger event
    Error               // Timer error
};
```

### Usage

```cpp
Timer tim3(&htim3);
tim3.attachInterrupt(PeriodElapsed, []() {
    // Called every timer overflow
});
```

---

## PWM

**Header**: `Hardware/PWM/PWM.h`  
**Guard**: `#ifdef HAL_TIM_MODULE_ENABLED`  
**Inherits**: `Timer`

### Constructor

```cpp
PWM(TIM_HandleTypeDef *pHandler, uint32_t channel);
```

### API

```cpp
void set(uint32_t value);    // Set duty cycle (0 to ARR)
uint32_t get();              // Get current duty cycle
```

### Usage

```cpp
PWM motor(&htim2, TIM_CHANNEL_1);
motor.set(500);  // 50% @ ARR=1000
```

---

## Analog / ADC

**Header**: `Hardware/Analog/Analog.h`  
**Guard**: `#ifdef HAL_ADC_MODULE_ENABLED`  
**Max instances**: `ANALOG_MAX_INSTANCES` (default 1)  
**Max channels**: `ANALOG_MAX_CHANNELS` (default 8)

### Constructor

```cpp
Analog(ADC_HandleTypeDef *pHandler, uint16_t vref = 3300);  // vref in mV
```

### API

```cpp
static Analog *getInstance(ADC_HandleTypeDef *pHandler);

uint16_t getRawValue(uint8_t channel);      // Raw (unfiltered) ADC value from DMA buffer
uint16_t getFilteredValue(uint8_t channel); // EMA-filtered ADC value
uint16_t getVoltage(uint8_t channel);       // Voltage in mV (from filtered value)

void setFilterShift(uint8_t shift);                 // Set EMA shift for all channels
void setFilterShift(uint8_t channel, uint8_t shift); // Set EMA shift for one channel

void attachInterrupt(std::function<void(uint16_t*)> callback);
void convCpltCallback();  // Called from DMA ISR
```

### Filter Configuration

Per-channel EMA (Exponential Moving Average) filter. Offset/multiplier calibration belongs
to the application layer (e.g., using RegisterBank).

### Usage

```cpp
Analog adc(&hadc1, 3300);

addTaskMain(taskCallback {
    uint16_t raw = adc.getRawValue(0);
    uint16_t filtered = adc.getFilteredValue(0);
    uint16_t voltage = adc.getVoltage(0);  // mV
}, 100);
```

---

## OneWire

**Header**: `Hardware/OneWire/OneWire.h`  
**Guard**: `#ifdef HAL_TIM_MODULE_ENABLED`

### Constructor

```cpp
OneWire(Timer *timer, GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);
```

Uses a hardware timer for precise µs-level bit-banging on a single GPIO pin.

### API

```cpp
void reset(uint32_t timeoutMs = OneWire::DEFAULT_TIMEOUT_MS);
void transmit(const uint8_t *pData, uint16_t size,
              dataCallback_f cb = nullptr,
              uint32_t timeoutMs = OneWire::DEFAULT_TIMEOUT_MS);
void receive(uint8_t *pData, uint16_t size,
             dataCallback_f cb = nullptr,
             uint32_t timeoutMs = OneWire::DEFAULT_TIMEOUT_MS);
void transmitThenReceive(const uint8_t *tx, uint16_t txSize,
                         uint8_t *rx, uint16_t rxSize,
                         dataCallback_f cb = nullptr,
                         uint32_t timeoutMs = OneWire::DEFAULT_TIMEOUT_MS);
void transaction(uint8_t romCmd, const uint8_t *address, uint8_t funcCmd,
                         const uint8_t *tx, uint16_t txSize,
                         uint8_t *rx, uint16_t rxSize,
                         dataCallback_f cb = nullptr,
                         bool resetAfter = false,
                         uint32_t timeoutMs = OneWire::DEFAULT_TIMEOUT_MS);
```

### Usage

```cpp
OneWire ow(new Timer(&htim7), GPIOB, GPIO_PIN_12);
// Used internally by DS18B20 driver
```

---

## Encoder

**Header**: `Hardware/Encoder/Encoder.h`  
**Guard**: `#ifdef HAL_TIM_MODULE_ENABLED`  
**Inherits**: `Timer`

### Constructor

```cpp
Encoder(TIM_HandleTypeDef *pHandler, uint32_t channel = TIM_CHANNEL_ALL,
        StartType startType = START_IT);
```

### API

```cpp
bool getDirection();                    // true = forward
int32_t getValue();                     // Current position
void setValue(int32_t value);           // Set position
void setLimits(int32_t min, int32_t max);  // Clamp range
void attachInterrupt(voidCallback_f callback);
```

### Usage

```cpp
Encoder enc(&htim3);
enc.setLimits(0, 100);
enc.attachInterrupt([]() {
    int32_t pos = enc.getValue();
});
```

---

## FlashMemory

**Header**: `Hardware/FlashMemory/FlashMemory.h`  
**Guard**: `#ifdef HAL_FLASH_MODULE_ENABLED`  
**Implements**: `IExternalMemory`

### API

```cpp
void readFromMemory(uint32_t MemAddress, uint8_t *pData, uint16_t Size);
void writeToMemory(uint32_t MemAddress, uint8_t *pData, uint16_t Size);
uint64_t read(uint32_t MemAddress);
void write(uint32_t MemAddress, uint64_t Data);
```

### Usage

```cpp
FlashMemory flash;
// Used with MemoryBlock for RegisterBank persistence
RegisterBank config(512, 16, new MemoryBlock(&flash, 0x0801F800));
```

---

## CAN / FDCAN

### CAN

**Header**: `Hardware/CAN/CAN.h`  
**Guard**: `#ifdef HAL_CAN_MODULE_ENABLED`

```cpp
CAN(CAN_HandleTypeDef *pHandler);
void send(uint32_t identifier, uint8_t *pData, uint32_t length = 8,
          uint32_t rtr_mode = CAN_RTR_DATA);
void onPacket(uint16_t commNumber, dataCallback_f handler);
```

### FDCAN

**Header**: `Hardware/FDCAN/FDCAN.h`  
**Guard**: `#ifdef HAL_FDCAN_MODULE_ENABLED`

Same API pattern as CAN but supports FD frames (up to 64 bytes).

---

## USB_CDC

**Header**: `Hardware/USB_CDC/USB_CDC.h`  
**Guard**: `#ifdef _JQB_USE_USB_CDC`

### API

```cpp
USB_CDC();
static USB_CDC *getInstance();

void send(uint8_t *pData, uint16_t Size,
          dataCallback_f cb = nullptr,
          uint32_t timeoutMs = USB_CDC::DEFAULT_TIMEOUT_MS);
void send(const char *buf, uint32_t timeoutMs = USB_CDC::DEFAULT_TIMEOUT_MS);
void onReceiveHandler(dataCallback_f onReceive);
void onTransmitHandler(voidCallback_f onTransmit);
uint16_t queueSize();
```

### Usage

```cpp
USB_CDC usb;
usb.onReceiveHandler([](uint8_t *data, uint16_t size) {
    processCommand(data, size);
});
usb.send("Hello USB\n");
```
