#include "Encoder.h"

Encoder *_Encoder_instances[ENCODER_MAX_INSTANCES];
uint8_t _Encoder_instancesNum = 0;

Encoder *Encoder::getInstance(TIM_HandleTypeDef *_instance) {
    for (size_t i = 0; i < _Encoder_instancesNum; i++) {
        if(_Encoder_instances[i]->_pInstance->Instance == _instance->Instance) return _Encoder_instances[i];
    }
    return nullptr;
}

void Encoder::timInterrupt() {
	_value+=getDirection()?1:-1;
	if(fnCallback) fnCallback();
}

Encoder::Encoder(TIM_HandleTypeDef *instance) {
	_pInstance = instance;
	_Encoder_instances[_Encoder_instancesNum++] = this;
}
void Encoder::init() {
	HAL_TIM_Encoder_Start_IT(_pInstance, TIM_CHANNEL_ALL);
}

bool Encoder::getDirection() {
	return !__HAL_TIM_IS_TIM_COUNTING_DOWN(_pInstance);
}

int32_t Encoder::getValue() {
    if(!fnCallback) {
        _value = (int16_t) _pInstance->Instance->CNT;
        if(_value < _min) {
            _value = _min;
            _pInstance->Instance->CNT = _value;
        }
        else if(_value > _max) {
            _value = _max;
            _pInstance->Instance->CNT = _value;
        }
        return _value;
    }
    return _value;
}
void Encoder::setValue(int32_t value) {
    if(!fnCallback) {
        _pInstance->Instance->CNT = value;
        return;
    }
    _value = value;
}
void Encoder::setLimits(int32_t min, int32_t max) {
    _min = min;
    _max = max;
}

void Encoder::attachInterrupt(std::function<void(void)> callback) {
	if (fnCallback) {
		fnCallback = callback;
	} else {
		fnCallback = callback;
		if (fnCallback) {
		  __HAL_TIM_CLEAR_FLAG(_pInstance, TIM_FLAG_UPDATE);
		  __HAL_TIM_ENABLE_IT(_pInstance, TIM_IT_UPDATE);
		}
	}
}