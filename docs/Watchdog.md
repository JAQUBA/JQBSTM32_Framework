# Watchdog

`Hardware/Watchdog/Watchdog.h` provides a family-independent wrapper around the STM32 HAL independent watchdog.

## Enable the HAL module

The CubeMX project's HAL configuration must contain:

```c
#define HAL_IWDG_MODULE_ENABLED
```

When the module is disabled, the same C++ API remains available as a no-op and `isReady()` returns `false`.

## Usage

```cpp
#include "Hardware/Watchdog/Watchdog.h"

Watchdog& watchdog = Watchdog::instance();
watchdog.begin();

// Call from a health-checked scheduler task.
watchdog.refresh();
```

The default configuration assumes a 32 kHz LSI and a 1 second timeout. The LSI value should be adjusted for the target STM32 family and board when the timeout budget is safety-relevant:

```cpp
Watchdog::Config config(1000U, 32000U, 0x0FFFU, 1000000U);
Watchdog::instance().begin(config);
```

The wrapper selects the smallest available prescaler that can represent the requested timeout and exposes the selected `reload()` and `prescalerDivisor()` values for diagnostics. The watchdog reset flag is captured before reset flags are cleared.
