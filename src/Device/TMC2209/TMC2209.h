#ifndef STEPPER_H_
#define STEPPER_H_

#include "Core.h"



class TMC2209 {
    public:

        enum Direction {
            STEPPER_RIGHT = GPIO_PIN_SET,
            STEPPER_LEFT = GPIO_PIN_RESET
        };
        enum Handler {
            STEPPER_ON_LIMIT,
            STEP
        };

        TMC2209(GPIO_TypeDef* enablePort, uint16_t enablePin,
            GPIO_TypeDef* stepPort, uint16_t stepPin,
            GPIO_TypeDef* directionPort, uint16_t directionPin,
            GPIO_TypeDef* MS1Port, uint16_t MS1Pin,
            GPIO_TypeDef* MS2Port, uint16_t MS2Pin,
            GPIO_TypeDef* zeroPort, uint16_t zeroPin);
        void init();

        void step(int16_t steps = 1);
        void forward();
        void back();

        void reverse();
        void togglePower();
        void enable();
        void disable();
        void direction(Direction direction);

        Direction getDirection();
        
        void setLimit(uint32_t limit);
        void setHandler(Handler handler, void (*fnHandler)(void));

        

        void onLimit(void (*fnHandler)(void));

        uint32_t getLimit();


        void onStep(uint32_t steps, void (*fnHandler)(void));


        uint32_t _limit;
        uint32_t _steps;
    private:
        GPIO_TypeDef* _enablePort; uint16_t _enablePin;
        GPIO_TypeDef* _stepPort; uint16_t _stepPin;
        GPIO_TypeDef* _directionPort; uint16_t _directionPin;
        GPIO_TypeDef* _MS1Port; uint16_t _MS1Pin;
        GPIO_TypeDef* _MS2Port; uint16_t _MS2Pin;
        GPIO_TypeDef* _zeroPort; uint16_t _zeroPin;
        Direction _direction;

        std::list<void (*)()> _onStepHandlers;
        
        void (* _handlers [2])() = {};

        uint32_t _onStepLimit;
};

#endif