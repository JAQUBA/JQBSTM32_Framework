#include "Encoder.h"


Encoder::Encoder(TIM_HandleTypeDef *pHandler, uint32_t channel, StartType startType): Timer(pHandler) {
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
    Timer::attachInterrupt(Timer::IC_CaptureCallback, voidCallback {
        _value+=getDirection()?1:-1;
        if(fnCallback) fnCallback();
    });
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



void Encoder::attachInterrupt(voidCallback_f callback) {
	if (fnCallback) {
		fnCallback = callback;
	} else {
		fnCallback = callback;
		if (fnCallback) {
		  __HAL_TIM_CLEAR_FLAG(_pHandler, TIM_FLAG_UPDATE);
		  __HAL_TIM_ENABLE_IT(_pHandler, TIM_IT_UPDATE);
		}
	}
}