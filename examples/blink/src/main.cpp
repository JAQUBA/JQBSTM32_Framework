#include "Core.h"
#include "Hardware/GPIO/GPIO.h"

#define LED LED_GPIO_Port, LED_Pin

void init() {
    MX_TIM4_Init();
}

void setup() {
    addTaskMain(taskCallback {
        GPIO.toggle(LED);
    }, 1000);
}