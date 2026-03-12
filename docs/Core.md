# Core & Scheduler

## Overview

The Core module is the heart of the JQBSTM32 Framework. It provides the application entry point, global utility functions, and integrates the cooperative task scheduler with STM32 HAL callbacks.

## Files

| File            | Description                                    |
|-----------------|------------------------------------------------|
| `Core.h`        | Main header — includes all framework modules, declares API |
| `Core.cpp`      | Implementation — `main()`, HAL callbacks, scheduler integration |
| `Scheduler.hpp` | Task scheduler class definition                |
| `Scheduler.cpp` | Task scheduler implementation                  |
| `WProgram.h`    | Arduino compatibility typedefs                 |
| `fl_bit.h`      | Bit manipulation macros                        |

---

## User Entry Points

The framework provides three functions that the user **must** implement:

```cpp
void init();    // Called by Core constructor — initialize CubeMX peripherals (MX_xxx_Init)
void setup();   // Called once before main loop — configure application, register tasks
void loop();    // Called continuously in while(1) — prefer tasks over loop code
```

### Execution Flow

```
Power-on → SystemInit → Core constructor → init() → HAL_Init → setup()
    → while(1) { mainTasks.execute(); loop(); }
```

The `HAL_IncTick()` callback (SysTick ISR) drives both scheduler contexts:
```
SysTick IRQ → interruptTasks.poll() → interruptTasks.execute() → mainTasks.poll()
```

---

## Task Scheduler

### Concepts

The scheduler provides **cooperative multitasking** in two contexts:

| Context      | Function                      | Runs In         | Use For                        |
|-------------|-------------------------------|-----------------|--------------------------------|
| Main tasks  | `addTaskMain(...)`            | Main loop       | I/O, communication, display    |
| ISR tasks   | `addTaskInterrupt(...)`       | SysTick ISR     | Fast timing, flags, counters   |

**Tasks are never preempted** — each must return promptly.

### Task Structure

```cpp
struct taskStruct {
    taskCallback_f functionPointer;  // Lambda or function pointer
    volatile uint32_t _delay;        // Original delay (constant, for repeat)
    volatile uint32_t delay;         // Current countdown (writable)
    bool _single;                    // true = one-shot, removed after execution
    uint16_t _id;                    // Unique task ID (auto-assigned)
};
```

### API

```cpp
// Register periodic task (main loop context)
taskStruct addTaskMain(taskCallback_f func, uint32_t delay = 0,
                       bool single = false, Scheduler::taskTime time = MUL_1MS);

// Register periodic task (interrupt context)
taskStruct addTaskInterrupt(taskCallback_f func, uint32_t delay = 0,
                            bool single = false, Scheduler::taskTime time = MUL_1MS);
```

### Time Multipliers

| Constant    | Value  | Base Unit        |
|-------------|--------|------------------|
| `MUL_100US` | 1      | 100 µs           |
| `MUL_1MS`   | 10     | 1 ms (default)   |
| `MUL_10MS`  | 100    | 10 ms            |
| `MUL_100MS` | 1000   | 100 ms           |
| `MUL_1S`    | 10000  | 1 second         |

### Time Macros

```cpp
#define SECONDS 1000UL
#define MINUTES 60000UL
#define TIME(x, y)  x * y

// Usage:
addTaskMain(myTask, TIME(5, SECONDS));   // Every 5 seconds
addTaskMain(myTask, TIME(2, MINUTES));   // Every 2 minutes
```

### Usage Examples

```cpp
// Lambda task — periodic every 200ms
addTaskMain(taskCallback {
    GPIO.toggle(LED);
    task->delay++;  // Increase delay by 1 tick each cycle
}, 200);

// One-shot task — fires once after 1 second
addTaskMain(taskCallback {
    startupComplete = true;
}, 1000, true);

// Named function task
TASK(sensorRead) {
    temperature = readSensor();
}
addTaskMain(sensorRead, 500);

// ISR-context task — fast polling every 100µs
addTaskInterrupt(taskCallback {
    checkPulse();
}, 1, false, MUL_100US);
```

### Dynamic Rescheduling

Inside a task callback, modify `task->delay` to change the next trigger time:
```cpp
addTaskMain(taskCallback {
    if (condition) {
        task->delay = 50;   // Speed up to 50ms
    } else {
        task->delay = 500;  // Slow down to 500ms
    }
}, 100);
```

### Performance Monitoring

```cpp
uint32_t getSystemUptime();        // Total uptime in ms
uint32_t getTaskOverruns();        // Tasks that exceeded 10ms execution
uint32_t getMaxExecutionTime();    // Worst-case task execution time
void resetPerformanceCounters();   // Reset all counters
```

---

## Global Utility Functions

```cpp
uint32_t millis();           // System uptime in milliseconds (volatile, ISR-safe)

void delay(volatile uint32_t ms);     // Blocking delay — avoid in tasks!
void delay_us(volatile uint32_t us);  // Microsecond blocking delay

// Linear interpolation with overflow protection
long map(long x, long in_min, long in_max, long out_min, long out_max);
```

---

## Bit Manipulation Macros (`fl_bit.h`)

### Flag-Level Operations (mask-based)

```cpp
bf_set(flag, mask)    // flag |= mask       — Set bits in mask
bf_clr(flag, mask)    // flag &= ~mask      — Clear bits in mask
bf_is(flag, mask)     // (flag & mask) != 0 — Test any bit in mask
bf_mask(flag, mask)   // flag & mask        — Extract masked bits
```

### Bit-Level Operations (index-based)

```cpp
bfb_set(ptr, bit)    // Set single bit by index (0-based)
bfb_clr(ptr, bit)    // Clear single bit by index
bfb_tog(ptr, bit)    // Toggle single bit by index
bfb_iss(ptr, bit)    // Test single bit by index → returns bool
```

### Logic & Math

```cpp
XOR(A, B)    // Logical XOR: (!A != !B)
ABS(X)       // Absolute value: ((X) > 0 ? (X) : -(X))
TRUE         // 1
FALSE        // 0
HIGH         // GPIO_PIN_SET
LOW          // GPIO_PIN_RESET
```

---

## Arduino Compatibility (`WProgram.h`)

Provides Arduino-like type aliases for embedded C++ compatibility:

```cpp
typedef bool boolean;
typedef uint8_t byte;
// Includes: <stdint.h>, <stdbool.h>, <string.h>, <stdlib.h>, <stdio.h>, <functional>
```

---

## HAL Callback Integration

Core.cpp automatically handles STM32 HAL callbacks and dispatches them to framework modules:

| HAL Callback                        | Dispatched To                |
|-------------------------------------|------------------------------|
| `HAL_IncTick()`                     | Scheduler poll + execute     |
| `HAL_UART_TxCpltCallback()`        | `UART::txInterrupt()`       |
| `HAL_UART_RxCpltCallback()`        | `UART::rxInterrupt()`       |
| `HAL_I2C_MasterTxCpltCallback()`   | `I2C::txInterrupt()`        |
| `HAL_I2C_MasterRxCpltCallback()`   | `I2C::rxInterrupt()`        |
| `HAL_I2C_MemTxCpltCallback()`      | `I2C::txInterrupt()`        |
| `HAL_I2C_MemRxCpltCallback()`      | `I2C::rxInterrupt()`        |
| `HAL_I2C_ErrorCallback()`          | `I2C::errorInterrupt()`     |
| `HAL_SPI_TxRxCpltCallback()`       | `SPI::txRxInterrupt()`      |
| `HAL_SPI_TxCpltCallback()`         | `SPI::txInterrupt()`        |
| `HAL_SPI_RxCpltCallback()`         | `SPI::rxInterrupt()`        |
| `HAL_TIM_PeriodElapsedCallback()`  | `Timer::interrupt()`        |
| `HAL_TIM_IC_CaptureCallback()`     | `Timer::interrupt()`        |
| `HAL_TIM_PWM_PulseFinishedCallback()` | `Timer::interrupt()`     |
| `HAL_GPIO_EXTI_Callback()`         | `HardwareGPIO::interrupt()` |
| `HAL_ADC_ConvCpltCallback()`       | `Analog::convCpltCallback()` |
| `HAL_CAN_RxFifo0MsgPendingCallback()` | `CAN::rxInterrupt()`    |
| `HAL_FDCAN_RxFifo0Callback()`      | `FDCAN::rxInterrupt()`     |
