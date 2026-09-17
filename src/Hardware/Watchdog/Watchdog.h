#pragma once

#include "../../Core.h"

#ifdef HAL_IWDG_MODULE_ENABLED

/**
 * @brief Portable STM32 independent watchdog wrapper.
 *
 * The wrapper uses the STM32 HAL IWDG API shared by the supported families.
 * The LSI frequency is configurable because its tolerance and nominal value
 * differ between MCU families and board designs.
 */
class Watchdog {
public:
    struct Config {
        Config(
            uint32_t timeoutMs = 1000U,
            uint32_t lsiFrequencyHz = 32000U,
            uint32_t maxReload = 0x0FFFU,
            uint32_t lsiReadyTimeoutLoops = 1000000U
        )
            : timeoutMs(timeoutMs),
              lsiFrequencyHz(lsiFrequencyHz),
              maxReload(maxReload),
              lsiReadyTimeoutLoops(lsiReadyTimeoutLoops) {}

        uint32_t timeoutMs;
        uint32_t lsiFrequencyHz;
        uint32_t maxReload;
        uint32_t lsiReadyTimeoutLoops;
    };

    static Watchdog& instance();

    bool begin(const Config& config = Config());
    bool refresh();
    bool wasReset() const;
    bool isReady() const;
    uint32_t reload() const;
    uint32_t prescalerDivisor() const;

private:
    Watchdog() = default;

    bool selectTiming(uint32_t& prescaler, uint32_t& reload) const;

    IWDG_HandleTypeDef _handle = {};
    Config _config = {};
    bool _initialized = false;
    bool _resetDetected = false;
    uint32_t _reload = 0U;
    uint32_t _prescalerDivisor = 0U;
};

#else

class Watchdog {
public:
    struct Config {
        Config(
            uint32_t timeoutMs = 1000U,
            uint32_t lsiFrequencyHz = 32000U,
            uint32_t maxReload = 0x0FFFU,
            uint32_t lsiReadyTimeoutLoops = 1000000U
        )
            : timeoutMs(timeoutMs),
              lsiFrequencyHz(lsiFrequencyHz),
              maxReload(maxReload),
              lsiReadyTimeoutLoops(lsiReadyTimeoutLoops) {}

        uint32_t timeoutMs;
        uint32_t lsiFrequencyHz;
        uint32_t maxReload;
        uint32_t lsiReadyTimeoutLoops;
    };

    static Watchdog& instance() {
        static Watchdog watchdog;
        return watchdog;
    }

    bool begin(const Config& = Config()) { return false; }
    bool refresh() { return false; }
    bool wasReset() const { return false; }
    bool isReady() const { return false; }
    uint32_t reload() const { return 0U; }
    uint32_t prescalerDivisor() const { return 0U; }

private:
    Watchdog() = default;
};

#endif
