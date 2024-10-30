#ifndef __ENCODER_H_
#define __ENCODER_H_

#include "../../Core.h"
#ifdef HAL_TIM_MODULE_ENABLED
#include "../Timer/Timer.h"

/**
 * @class Encoder
 * @brief Class for handling encoder operations.
 */
class Encoder : public Timer {
    public:
        /**
         * @brief Constructor for Encoder class.
         * @param pHandler Pointer to TIM_HandleTypeDef structure.
         */
        enum StartType {
            START_POLL = 0,
            START_IT = 1,
            START_DMA = 2
        };
        Encoder(TIM_HandleTypeDef *pHandler, uint32_t channel = TIM_CHANNEL_ALL, StartType startType = START_IT);

        /**
         * @brief Gets the direction of the encoder.
         * @return True if direction is forward, false otherwise.
         */
        bool getDirection();

        /**
         * @brief Gets the current value of the encoder.
         * @return Current encoder value.
         */
        int32_t getValue();

        /**
         * @brief Sets the value of the encoder.
         * @param value New encoder value.
         */
        void setValue(int32_t value);

        /**
         * @brief Sets the limits for the encoder value.
         * @param min Minimum value.
         * @param max Maximum value.
         */
        void setLimits(int32_t min, int32_t max);

        /**
         * @brief Attaches an interrupt callback function.
         * @param callback Function to be called on interrupt.
         */
        void attachInterrupt(voidCallback_f callback);

    private:
        int32_t _value; /**< Current encoder value. */
        int32_t _min = 0; /**< Minimum encoder value. */
        int32_t _max = -1; /**< Maximum encoder value. */
        
        voidCallback_f fnCallback; /**< Callback function for encoder interrupt. */
};

#endif
#endif