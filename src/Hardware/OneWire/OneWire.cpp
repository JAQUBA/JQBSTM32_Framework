/*
 * JQBSTM32 Framework - OneWire Protocol Implementation
 * Copyright (C) 2024 JAQUBA (kjakubowski0492@gmail.com)
 * 
 * Implementation according to Dallas/Maxim 1-Wire specification.
 * 
 * This library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library. If not, see <https://www.gnu.org/licenses/>.
 */

#include "OneWire.h"

OneWire::OneWire(Timer* timer, GPIO_TypeDef* GPIO_Port, uint16_t GPIO_Pin) : OW_Timer(timer), OW_Port(GPIO_Port), OW_Pin(GPIO_Pin) {
	OW_Timer->attachInterrupt(Timer::PeriodElapsedCallback, voidCallback {
		if (ignoreNextTimerEvent) {
			ignoreNextTimerEvent = false;
			return;
		}

		switch(operationProgress) {
			case OPERATION_PROGRESS_IDLE: {
				if(operationState == WAITING) operationState = FINISH;
				break;
			}
			case OPERATION_PROGRESS_RESET: {
				HAL_GPIO_WritePin(OW_Port, OW_Pin, GPIO_PIN_RESET);
				OW_Timer->setPeriod(480);
				operationProgress = OPERATION_PROGRESS_RESET_WAIT_LOW;
				break;
			}
			case OPERATION_PROGRESS_RESET_WAIT_LOW: {
				HAL_GPIO_WritePin(OW_Port, OW_Pin, GPIO_PIN_SET);
				OW_Timer->setPeriod(70);
				operationProgress=OPERATION_PROGRESS_RESET_WAIT_HIGH;
				break;
			}
			case OPERATION_PROGRESS_RESET_WAIT_HIGH: {
				is_device_presence = HAL_GPIO_ReadPin(OW_Port, OW_Pin);
				OW_Timer->setPeriod(410);
				operationProgress=OPERATION_PROGRESS_IDLE;
				break;
			}

			case OPERATION_PROGRESS_WRITE_START: {
				ow_byte = *(currentOperation.pData + ow_byte_index);
				ow_bit_index = 0;
			}
			case OPERATION_PROGRESS_WRITE_BIT: {
				HAL_GPIO_WritePin(OW_Port, OW_Pin, GPIO_PIN_RESET);
				OW_Timer->setPeriod(ow_byte & 0x01?6:60);
				operationProgress=OPERATION_PROGRESS_WRITE_END;
				break;
			}
			case OPERATION_PROGRESS_WRITE_END: {
				HAL_GPIO_WritePin(OW_Port, OW_Pin, GPIO_PIN_SET);
				OW_Timer->setPeriod(ow_byte & 0x01?64:10);
				if (++ow_bit_index < 8) {
					ow_byte >>= 1;
					operationProgress = OPERATION_PROGRESS_WRITE_BIT;
				} else {
					if (++ow_byte_index<currentOperation.Size) {
						operationProgress = OPERATION_PROGRESS_WRITE_START;
						break;
					}
					operationProgress = OPERATION_PROGRESS_IDLE;
				}
				break;
			}

			case OPERATION_PROGRESS_READ_START: {
				ow_bit_index = 0;
				ow_byte = 0;
			}
			case OPERATION_PROGRESS_READ_BIT: {
				HAL_GPIO_WritePin(OW_Port, OW_Pin, GPIO_PIN_RESET);
				OW_Timer->setPeriod(10);
				operationProgress=OPERATION_PROGRESS_READ_WAIT;
				break;
			}
			case OPERATION_PROGRESS_READ_WAIT: {
				HAL_GPIO_WritePin(OW_Port, OW_Pin, GPIO_PIN_SET);
				OW_Timer->setPeriod(9);
				operationProgress=OPERATION_PROGRESS_READ_END;
				break;
			}
			case OPERATION_PROGRESS_READ_END: {
				if (HAL_GPIO_ReadPin(OW_Port, OW_Pin))	ow_byte |= 0x80;
				OW_Timer->setPeriod(55);
				if (++ow_bit_index < 8) {
					ow_byte >>= 1;
					operationProgress = OPERATION_PROGRESS_READ_BIT;
				} else {
					*(currentOperation.pData + ow_byte_index) = ow_byte;
					if(++ow_byte_index<currentOperation.Size) {
						operationProgress = OPERATION_PROGRESS_READ_START;
						break;
					}
					operationProgress = OPERATION_PROGRESS_IDLE;
				}
				break;
			}
		}
	});

	addTaskMain(taskCallback {
		switch(operationState) {
			case IDLE: {
				if(!operations.empty()) {
					currentOperation = operations.front();
					operationState = CHECK_FREE;
				}
				break;
			}
			case CHECK_FREE: {
				if(operationProgress == OPERATION_PROGRESS_IDLE) {
					operationState = WORK;
				}
				break;
			}
			case WORK: {
				operationTimeout = millis() + currentOperation.timeoutMs;
				if(currentOperation.operationType == EoperationType::RESET) {
					is_device_presence = false;
					operationProgress = OPERATION_PROGRESS_RESET;
				} else if(currentOperation.operationType == EoperationType::TRANSMIT) {
					ow_byte_index = 0;
					operationProgress = OPERATION_PROGRESS_WRITE_START;

				} else if(currentOperation.operationType == EoperationType::RECEIVE) {
					ow_byte_index = 0;
					operationProgress = OPERATION_PROGRESS_READ_START;
				}
				operationState = WAITING;
				break;
			}
			case WAITING: {
	   			if (millis() >= operationTimeout) {
					ignoreNextTimerEvent = true;
					operationProgress = OPERATION_PROGRESS_IDLE;
					operationState = FINISH;
				}
				break;
			}
			case FINISH: {
				if(currentOperation.callback_f != nullptr) {
					currentOperation.callback_f(
						currentOperation.pData,
						currentOperation.Size
					);
				}
				operationState = CLEAR;
				break;
			}
			case CLEAR: {
				if(currentOperation.free) free(currentOperation.pData);
				operations.pop();
				operationState = IDLE;
				break;
			}
		}
	});
}

void OneWire::reset(uint32_t timeoutMs) {
	if (operations.size() >= 8U) return;
	operation operation;
	operation.operationType = EoperationType::RESET;
	operation.timeoutMs = timeoutMs;
	operation.free = false;
	operations.push(operation);
}
void OneWire::transmit(
	const uint8_t* pData, uint16_t size,
	dataCallback_f callbackFn,
	uint32_t timeoutMs
){
	if (pData == nullptr || size == 0U) return;
	if (operations.size() >= 8U) return;
	operation operation;
	operation.operationType = EoperationType::TRANSMIT;
	operation.timeoutMs = timeoutMs;
	operation.pData = (uint8_t*) malloc(size);
	if (operation.pData == nullptr) return;
	memcpy(operation.pData, pData, size);
	operation.Size = size;
	operation.free = true;
	operation.callback_f = callbackFn;
	operations.push(operation);
}

void OneWire::receive(
	uint8_t* pData, uint16_t size,
	dataCallback_f callbackFn,
	uint32_t timeoutMs
){
	if (pData == nullptr || size == 0U) return;
	if (operations.size() >= 8U) return;
	operation operation;
	operation.operationType = EoperationType::RECEIVE;
	operation.timeoutMs = timeoutMs;
	operation.pData = pData;
	operation.Size = size;
	operation.free = false;
	operation.callback_f = callbackFn;
	operations.push(operation);
}

void OneWire::transmitThenReceive(
	const uint8_t* pData_tx, uint16_t txSize,
	uint8_t* pData_rx, uint16_t rxSize,
	dataCallback_f callbackFn,
	uint32_t timeoutMs
){
	if (txSize > 0U && pData_tx == nullptr) return;
	if (rxSize > 0U && pData_rx == nullptr) return;
	const uint8_t requiredOperations = (txSize > 0U ? 1U : 0U) + (rxSize > 0U ? 1U : 0U);
	if (requiredOperations == 0U || operations.size() > 8U - requiredOperations) return;

	if (txSize > 0U) {
		operation operation;
		operation.operationType = EoperationType::TRANSMIT;
		operation.timeoutMs = timeoutMs;
		operation.pData = (uint8_t*) malloc(txSize);
		if (operation.pData == nullptr) return;
		memcpy(operation.pData, pData_tx, txSize);
		operation.Size = txSize;
		operation.free = true;
		operations.push(operation);
	}

	if (rxSize > 0U) {
		operation operation;
		operation.operationType = EoperationType::RECEIVE;
		operation.timeoutMs = timeoutMs;
		operation.pData = pData_rx;
		operation.Size = rxSize;
		operation.free = false;
		operation.callback_f = callbackFn;
		operations.push(operation);
	}
}

void OneWire::transaction(
	uint8_t romCommand,
	const uint8_t* address,
	uint8_t functionCommand,
	const uint8_t* pData_tx,
	uint16_t txSize,
	uint8_t* pData_rx,
	uint16_t rxSize,
	dataCallback_f callbackFn,
	bool resetAfterTransaction,
	uint32_t timeoutMs
){
	if (txSize > 0U && pData_tx == nullptr) return;
	if (rxSize > 0U && pData_rx == nullptr) return;

	const uint16_t commandSize = (address == NULL) ? 2U : 10U;
	if (txSize > static_cast<uint16_t>(UINT16_MAX - commandSize)) return;

	const uint8_t requiredOperations = (uint8_t)(2U + (rxSize > 0U ? 1U : 0U) + (resetAfterTransaction ? 1U : 0U));
	if (operations.size() > 8U - requiredOperations) return;

	const uint16_t size = (uint16_t)(commandSize + txSize);
	uint8_t *payload = (uint8_t*) malloc(size);
	if (payload == nullptr) return;

	payload[0] = romCommand;
	if (address == NULL) {
		payload[1] = functionCommand;
		if (txSize > 0U) memcpy(payload + 2U, pData_tx, txSize);
	} else {
		memcpy(payload + 1U, address, 8U);
		payload[9] = functionCommand;
		if (txSize > 0U) memcpy(payload + 10U, pData_tx, txSize);
	}

	operation operation;
	operation.operationType = EoperationType::RESET;
	operation.timeoutMs = timeoutMs;
	operation.free = false;
	operations.push(operation);

	operation.operationType = EoperationType::TRANSMIT;
	operation.pData = payload;
	operation.Size = size;
	operation.free = true;
	operation.callback_f = nullptr;
	operations.push(operation);

	if (rxSize > 0U) {
		operation operationRx;
		operationRx.operationType = EoperationType::RECEIVE;
		operationRx.timeoutMs = timeoutMs;
		operationRx.pData = pData_rx;
		operationRx.Size = rxSize;
		operationRx.free = false;
		operationRx.callback_f = callbackFn;
		operations.push(operationRx);
	}

	if (resetAfterTransaction) {
		operation operationReset;
		operationReset.operationType = EoperationType::RESET;
		operationReset.timeoutMs = timeoutMs;
		operationReset.free = false;
		operations.push(operationReset);
	}
}       

uint16_t OneWire::queueSize() {
	return operations.size();
}

bool OneWire::isDevicePresent() {
	return !is_device_presence; // Inverted logic - LOW means device present
}

bool OneWire::isBusy() {
	return operationState != IDLE || operationProgress != OPERATION_PROGRESS_IDLE;
}

void OneWire::clearQueue() {
	while(!operations.empty()) {
		if(operations.front().free && operations.front().pData != nullptr) {
			free(operations.front().pData);
		}
		operations.pop();
	}
}

uint8_t OneWire::calculateCRC8(const uint8_t* data, uint8_t length) {
	uint8_t crc = 0;
	
	for(uint8_t i = 0; i < length; i++) {
		uint8_t inbyte = data[i];
		for(uint8_t j = 0; j < 8; j++) {
			uint8_t mix = (crc ^ inbyte) & 0x01;
			crc >>= 1;
			if(mix) crc ^= 0x8C; // Dallas/Maxim CRC8 polynomial
			inbyte >>= 1;
		}
	}
	
	return crc;
}

bool OneWire::validateCRC(const uint8_t* data, uint8_t length, uint8_t expectedCRC) {
	return calculateCRC8(data, length) == expectedCRC;
}