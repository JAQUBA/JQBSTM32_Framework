#include "Core.h"
#include "tim.h"
#include "Hardware/GPIO/GPIO.h"

#define LED GPIOC, GPIO_PIN_13

void init() {
    MX_TIM4_Init();
    GPIO.setup(LED, GPIO_MODE_AF_PP);
}
void setup() {
    GPIO.set(LED, HIGH);
    addTaskMain(taskCallback {
        GPIO.toggle(LED);
        task->delay++;
    }, 200);
}
