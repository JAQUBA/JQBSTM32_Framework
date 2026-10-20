#include "Watchdog.h"

#ifdef HAL_IWDG_MODULE_ENABLED

namespace {
struct PrescalerOption {
    uint32_t divisor;
    uint32_t halValue;
};

constexpr PrescalerOption kPrescalerOptions[] = {
    {4U, IWDG_PRESCALER_4},
    {8U, IWDG_PRESCALER_8},
    {16U, IWDG_PRESCALER_16},
    {32U, IWDG_PRESCALER_32},
    {64U, IWDG_PRESCALER_64},
    {128U, IWDG_PRESCALER_128},
    {256U, IWDG_PRESCALER_256}
};
}

Watchdog& Watchdog::instance() {
    static Watchdog watchdog;
    return watchdog;
}

bool Watchdog::selectTiming(uint32_t& prescaler, uint32_t& reload) const {
    if (_config.timeoutMs == 0U || _config.lsiFrequencyHz == 0U || _config.maxReload == 0U) {
        return false;
    }

    for (const PrescalerOption& option : kPrescalerOptions) {
        const uint64_t numerator = (uint64_t)_config.timeoutMs * _config.lsiFrequencyHz;
        const uint64_t denominator = (uint64_t)option.divisor * 1000ULL;
        const uint64_t ticks = (numerator + denominator - 1ULL) / denominator;
        if (ticks == 0ULL || ticks > (uint64_t)_config.maxReload + 1ULL) {
            continue;
        }

        prescaler = option.halValue;
        reload = (uint32_t)(ticks - 1ULL);
        return true;
    }

    return false;
}

bool Watchdog::begin(const Config& config) {
    _config = config;
    _initialized = false;
    _reload = 0U;
    _prescalerDivisor = 0U;
    _resetDetected = (__HAL_RCC_GET_FLAG(RCC_FLAG_IWDGRST) != RESET);
    __HAL_RCC_CLEAR_RESET_FLAGS();

    __HAL_RCC_LSI_ENABLE();
    uint32_t waitLoops = _config.lsiReadyTimeoutLoops;
    while (__HAL_RCC_GET_FLAG(RCC_FLAG_LSIRDY) == RESET && waitLoops-- > 0U) {
    }
    if (__HAL_RCC_GET_FLAG(RCC_FLAG_LSIRDY) == RESET) {
        return false;
    }

    uint32_t prescaler = 0U;
    uint32_t reload = 0U;
    if (!selectTiming(prescaler, reload)) {
        return false;
    }

    _handle.Instance = IWDG;
    _handle.Init.Prescaler = prescaler;
    _handle.Init.Window = IWDG_WINDOW_DISABLE;
    _handle.Init.Reload = reload;
    if (HAL_IWDG_Init(&_handle) != HAL_OK) {
        return false;
    }

    _reload = reload;
    for (const PrescalerOption& option : kPrescalerOptions) {
        if (option.halValue == prescaler) {
            _prescalerDivisor = option.divisor;
            break;
        }
    }
    _initialized = true;
    return true;
}

bool Watchdog::refresh() {
    return _initialized && HAL_IWDG_Refresh(&_handle) == HAL_OK;
}

bool Watchdog::wasReset() const {
    return _resetDetected;
}

bool Watchdog::isReady() const {
    return _initialized;
}

uint32_t Watchdog::reload() const {
    return _reload;
}

uint32_t Watchdog::prescalerDivisor() const {
    return _prescalerDivisor;
}

#endif
