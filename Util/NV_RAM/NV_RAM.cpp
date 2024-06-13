#include "NV_RAM.h"
#include "../JQBSTM32_Framework/Util/TASK_M/task_m.h"

#define _JMP(x) {_state = x; break;}

enum States {
	I2C_OK,
	I2C_ERR_HAL,
	I2C_ERR_TO_S,
	I2C_ERR_TO_W,
	NIL,
	START_READ16,
	WAIT_COMPL_READ16,
	START_WRITE16,
	WAIT_COMPL_WRITE16
} _state;
       
I2C_HandleTypeDef *_instance;
uint8_t _address;
uint8_t _i2c_state;
uint16_t _pages;
uint16_t _pageSize;
uint16_t _offset;
uint16_t *_data;
uint16_t _size;
uint32_t _timeout;
uint8_t  _status;

TASK_PROCEDURE(task_eep);

TASK_PROC init_eep(void)
{
	return task_eep;
}

TASK_PROCEDURE(task_eep)
{
	if (self->flag)//1mS
	{	self->flag = false;
		if (_timeout>0) _timeout--;
	}
	switch (_state) {
		case START_READ16:{ 
			_i2c_state = HAL_I2C_Mem_Read_DMA(_instance, _address, ((_address*2) + _offset),
				I2C_MEMADD_SIZE_8BIT, (uint8_t*)_data, (_size * 2));

			if (_i2c_state==HAL_OK){
				_timeout=4;
				_JMP(WAIT_COMPL_READ16);
			}
			else if (_i2c_state==HAL_ERROR){
				_status = I2C_ERR_HAL;
				_JMP(NIL);//ERR
			}
			if (_timeout==0){
				_status = I2C_ERR_TO_S;
				_JMP(NIL);//ERR
			}
		}

		case WAIT_COMPL_READ16:{
			if (HAL_I2C_STATE_READY == _instance->State){
				_JMP(NIL);//OK
			}
			if (_timeout==0){
				_status = I2C_ERR_TO_W;
				_JMP(NIL);//ERR
			}
		}

		case START_WRITE16:{ 
			_i2c_state=HAL_I2C_Mem_Write_DMA(_instance, _address, ((_address*2) + _offset),
				I2C_MEMADD_SIZE_8BIT, (uint8_t*)_data, (_size * 2));
			if (_i2c_state==HAL_OK){
				_timeout=4;
				_JMP(WAIT_COMPL_WRITE16);
			}
			else if (_i2c_state==HAL_ERROR){
				_status = I2C_ERR_HAL;
				_JMP(NIL);//ERR
			}
			if (_timeout==0){
				_status = I2C_ERR_TO_S;
				_JMP(NIL);//ERR
			}
		}

		case WAIT_COMPL_WRITE16:{
			if (HAL_I2C_STATE_READY == _instance->State){
				_JMP(NIL);//OK
			}
			if (_timeout==0){
				_status = I2C_ERR_TO_W;
				_JMP(NIL);//ERR
			}
		}

		default: {

		}
	}
}

NV_RAM::NV_RAM(I2C_HandleTypeDef *instance, uint8_t address,
               uint16_t pages, uint16_t pagesize, uint16_t offset){
	_instance = instance;
	_address = address;
	_pages = pages;
	_pageSize = pagesize;
	_offset = offset;
	_state = NIL;
    _i2c_state = HAL_OK;
	_status = I2C_OK;
	init_task_m(init_eep(), 1);

}
void NV_RAM::read16(uint16_t address, uint16_t *data, uint16_t size) {
	_data = data;
	_size = size;
	_timeout = 3;
	_status = I2C_OK;
	_i2c_state = HAL_OK;
	_state=START_READ16;
}

void NV_RAM::write16(uint16_t address, uint16_t *data, uint16_t size) {
	_data = data;
	_size = size;
	_timeout = 3;
	_status = I2C_OK;
	_i2c_state = HAL_OK;
	_state=START_WRITE16;
}