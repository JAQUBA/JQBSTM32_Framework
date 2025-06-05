#include "Encoder.h"
#ifdef HAL_TIM_MODULE_ENABLED

Encoder* Encoder::_instance = nullptr;

Encoder::Encoder(TIM_HandleTypeDef *pHandler, uint32_t channel, StartType startType): Timer(pHandler) {
    _instance = this;
    switch (startType) {
        case START_POLL: {
            HAL_TIM_Encoder_Start(_pHandler, channel);
            break;
        }
        case START_IT: {
            HAL_TIM_Encoder_Start_IT(_pHandler, channel);
            break;
        }
        case START_DMA: {
            // HAL_TIM_Encoder_Start_DMA(_pHandler, channel, &_value, 1);
            break;
        }
    }
    Timer::attachInterrupt(Timer::IC_CaptureCallback, Encoder::_staticCaptureCallback);
}

bool Encoder::getDirection() {
	return !__HAL_TIM_IS_TIM_COUNTING_DOWN(_pHandler);
}

int32_t Encoder::getValue() {
    if(!fnCallback) {
        _value = (int16_t) _pHandler->Instance->CNT;
        if(_value < _min) {
            _value = _min;
            _pHandler->Instance->CNT = _value;
        }
        else if(_value > _max) {
            _value = _max;
            _pHandler->Instance->CNT = _value;
        }
        return _value;
    }
    return _value;
}
void Encoder::setValue(int32_t value) {
    if(!fnCallback) {
        _pHandler->Instance->CNT = value;
        return;
    }
    _value = value;
}
void Encoder::setLimits(int32_t min, int32_t max) {
    _min = min;
    _max = max;
}



void Encoder::attachInterrupt(void (*callback)()) {
    fnCallback = callback;
}

void Encoder::_staticCaptureCallback() {
    if(_instance) {
        _instance->_value += _instance->getDirection() ? 1 : -1;
        if(_instance->fnCallback) _instance->fnCallback();
    }
}
#endif