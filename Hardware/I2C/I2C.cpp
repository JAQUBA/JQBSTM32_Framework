#include "I2C.h"

I2C *_I2C_instances[I2C_MAX_INSTANCES];
uint8_t _I2C_instancesNum = 0;

#define _JMP(x) {_state = x; break;}

I2C *I2C::getInstance(I2C_HandleTypeDef *pHandler) {
    for (size_t i = 0; i < _I2C_instancesNum; i++) {
        if(_I2C_instances[i]->_pHandler->Instance == pHandler->Instance) return _I2C_instances[i];
    }
    return nullptr;
}
void HAL_I2C_MasterRxCpltCallback(I2C_HandleTypeDef *hi2c) {I2C::getInstance(hi2c)->rxInterrupt();}
void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef *hi2c) {I2C::getInstance(hi2c)->txInterrupt();}

I2C::I2C(I2C_HandleTypeDef* pHandler) {
    _pHandler = pHandler;
	_I2C_instances[_I2C_instancesNum++] = this;
	addTaskMain([&](taskStruct *task) {

	}, 0);
}

void I2C::send(uint16_t DevAddress, uint8_t *pData, uint16_t Size, bool Blocking) {
	HAL_I2C_Master_Transmit(_pHandler, DevAddress,  pData,  Size, 1000);
}
void I2C::receive(uint16_t DevAddress, uint8_t *pData, uint16_t Size, bool Blocking) {
    HAL_I2C_Master_Receive(_pHandler, DevAddress, pData, Size, 1000);
}

void I2C::readFromMemory(uint16_t DevAddress, uint16_t MemAddress, uint8_t *pData, uint16_t Size, bool Blocking) {
	HAL_I2C_Mem_Read(_pHandler, DevAddress, MemAddress, I2C_MEMADD_SIZE_8BIT, pData, Size, 1000);
}
void I2C::writeToMemory(uint16_t DevAddress, uint16_t MemAddress, uint8_t *pData, uint16_t Size, bool Blocking) {
	HAL_I2C_Mem_Read(_pHandler, DevAddress, MemAddress, I2C_MEMADD_SIZE_8BIT, pData, Size, 1000);
}

void I2C::txInterrupt() {

}
void I2C::rxInterrupt() {

}


// while (HAL_I2C_GetState(_pInstance) == HAL_I2C_STATE_MEM_BUSY_TX) { }



        

        
        
        // struct _memOperation {
        //     enum {READ, WRITE} operation;
        // };

        // std::list<_memOperation> functions;
// HAL_I2C_Mem_Write(_instance, _address, ((address*2) + _offset), I2C_MEMADD_SIZE_8BIT, (uint8_t*)data, (size * 2), 1000);
// HAL_StatusTypeDef HAL_I2C_Mem_Read(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t MemAddress,
//                                    uint16_t MemAddSize, uint8_t *pData, uint16_t Size, uint32_t Timeout);

	// _pInstance->readFromMemory(_address, address, pData)
	// _pInstance->send();
	// _pInstance->send(address,data,size);
	// HAL_I2C_Master_Receive_IT();
	// HAL_I2c_
	// HAL_SPI_Receive_IT();
	// HAL_I2S_Receive_IT();
	// HAL_UART_Receive_IT();
	// HAL_IRDA_Receive_IT();
	// HAL_I2C_Mem_Read(_pInstance, _address, ((address*2) + _offset), I2C_MEMADD_SIZE_8BIT, (uint8_t*)data, (size * 2), 1000);
	// HAL_I2C_Mem_Read_IT();
    // addTaskMain([&](taskStruct *task) {
		// task->_single=true; //wykona sie tylko raz
		// task->delay = 200; //co 200ms

		// HAL_GPIO_TogglePin(D0_GPIO_Port, D0_Pin);


	   	// switch (_state) {
		// 	case START_READ16:{ 
		// 		_i2c_state = HAL_I2C_Mem_Read_DMA(_instance, _address, ((_address*2) + _offset),
		// 			I2C_MEMADD_SIZE_8BIT, (uint8_t*)_data, (_size * 2));

		// 		_JMP(NIL);

		// 		if (_i2c_state==HAL_OK){
		// 			_timeout=4;
		// 			_JMP(WAIT_COMPL_READ16);
		// 		}
		// 		else if (_i2c_state==HAL_ERROR){
		// 			_status = I2C_ERR_HAL;
		// 			_JMP(NIL);//ERR
		// 		}
		// 		if (_timeout==0){
		// 			_status = I2C_ERR_TO_S;
		// 			_JMP(NIL);//ERR
		// 		}
		// 	}

		// 	case WAIT_COMPL_READ16:{
		// 		if (HAL_I2C_STATE_READY == _instance->State){
		// 			_JMP(NIL);//OK
		// 		}
		// 		if (_timeout==0){
		// 			_status = I2C_ERR_TO_W;
		// 			_JMP(NIL);//ERR
		// 		}
		// 	}

		// 	case START_WRITE16:{ 
		// 		//_i2c_state=HAL_I2C_Mem_Write_DMA(_instance, _address, ((_address*2) + _offset),
		// 			//I2C_MEMADD_SIZE_8BIT, (uint8_t*)_data, (_size * 2));
		// 		_JMP(NIL);

		// 		if (_i2c_state==HAL_OK){
		// 			_timeout=4;
		// 			_JMP(WAIT_COMPL_WRITE16);
		// 		}
		// 		else if (_i2c_state==HAL_ERROR){
		// 			_status = I2C_ERR_HAL;
		// 			_JMP(NIL);//ERR
		// 		}
		// 		if (_timeout==0){
		// 			_status = I2C_ERR_TO_S;
		// 			_JMP(NIL);//ERR
		// 		}
		// 	}

		// 	case WAIT_COMPL_WRITE16:{
		// 		if (HAL_I2C_STATE_READY == _instance->State){
		// 			_JMP(NIL);//OK
		// 		}
		// 		if (_timeout==0){
		// 			_status = I2C_ERR_TO_W;
		// 			_JMP(NIL);//ERR
		// 		}
		// 	}

		// 	default: {

		// 	}
		// }
	// }, 0);
