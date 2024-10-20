#include "Encoder.h"

Encoder *_Encoder_instances[ENCODER_MAX_INSTANCES];
uint8_t _Encoder_instancesNum = 0;

Encoder *Encoder::getInstance(TIM_HandleTypeDef *_pHandler) {
    for (size_t i = 0; i < _Encoder_instancesNum; i++) {
        if(_Encoder_instances[i]->_pHandler->Instance == _pHandler->Instance) return _Encoder_instances[i];
    }
    return nullptr;
}

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim) {Encoder::getInstance(htim)->timInterrupt();}

Encoder::Encoder(TIM_HandleTypeDef *pHandler, StartType startType, uint32_t channel) {
	_pHandler = pHandler;
	_Encoder_instances[_Encoder_instancesNum++] = this;
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
}

void Encoder::timInterrupt() {
	_value+=getDirection()?1:-1;
	if(fnCallback) fnCallback();
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