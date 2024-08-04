#ifndef __ENCODER_H_
#define __ENCODER_H_

#include "../../Core.h"

/**
 * @typedef encoderCallback_f
 * @brief Typedef for encoder callback function.
 */
using encoderCallback_f = std::function<void(void)>;

#ifndef ENCODER_MAX_INSTANCES
#define ENCODER_MAX_INSTANCES 2
#endif

/**
 * @class Encoder
 * @brief Class for handling encoder operations.
 */
class Encoder {
    public:
        /**
         * @brief Constructor for Encoder class.
         * @param pHandler Pointer to TIM_HandleTypeDef structure.
         */
        Encoder(TIM_HandleTypeDef *pHandler);

        /**
         * @brief Gets the instance of the Encoder.
         * @param pHandler Pointer to TIM_HandleTypeDef structure.
         * @return Pointer to Encoder instance.
         */
        static Encoder *getInstance(TIM_HandleTypeDef *pHandler);

        /**
         * @brief Initializes the encoder.
         */
        void init();
        
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
        void attachInterrupt(encoderCallback_f callback);

        /**
         * @brief Handles the timer interrupt.
         */
        void timInterrupt();

    private:
        TIM_HandleTypeDef *_pHandler; /**< Pointer to TIM_HandleTypeDef structure. */

        int32_t _value; /**< Current encoder value. */
        int32_t _min = 0; /**< Minimum encoder value. */
        int32_t _max = -1; /**< Maximum encoder value. */
        
        encoderCallback_f fnCallback; /**< Callback function for encoder interrupt. */
};

#endif