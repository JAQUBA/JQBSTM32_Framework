/*
 * JQBSTM32 Framework - OneWire Protocol Header
 * Copyright (C) 2024 JAQUBA (kjakubowski0492@gmail.com)
 * 
 * Implementation according to Dallas/Maxim 1-Wire specification.
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef ONEWIRE_H
#define ONEWIRE_H

#include "../../Core.h"
#include "Interface/IBus.h"
#include "Hardware/Timer/Timer.h"

class OneWire : public IBus {
	public:
		OneWire(Timer* timer, GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);

		// Resets 1-Wire bus
		void reset();

		// Sends data through 1-Wire
		void transmit(
			const uint8_t* pData, uint16_t size,
			dataCallback_f callbackFn = nullptr
		);

		// Receives data through 1-Wire
		void receive(
			uint8_t* pData, uint16_t size,
			dataCallback_f callbackFn = nullptr
		);

		// Sequence: first transmit, then receive
		void transmitThenReceive(
			const uint8_t* pData_tx, uint16_t txSize,
			uint8_t* pData_rx, uint16_t rxSize,
			dataCallback_f callbackFn = nullptr
		);

		// Full transaction on 1-Wire bus
		void transaction(
			uint8_t romCommand,
			const uint8_t* address,
			uint8_t functionCommand,
			const uint8_t* pData_tx = nullptr,
			uint16_t txSize = 0,
			uint8_t* pData_rx = nullptr,
			uint16_t rxSize = 0,
			dataCallback_f callbackFn = nullptr,
			bool resetAfterTransaction = false
		);
		uint16_t queueSize();

	private:
		Timer* OW_Timer;
		GPIO_TypeDef* OW_Port;
		uint16_t OW_Pin; 

		uint32_t operationTimeout;
		bool     is_device_presence;

		uint8_t  ow_byte;
		uint8_t  ow_byte_index;
		uint8_t  ow_bit_index;
		
		enum {
			OPERATION_PROGRESS_IDLE,

			OPERATION_PROGRESS_RESET,
			OPERATION_PROGRESS_RESET_WAIT_LOW,
			OPERATION_PROGRESS_RESET_WAIT_HIGH,

			OPERATION_PROGRESS_WRITE_START,
			OPERATION_PROGRESS_WRITE_BIT,
			OPERATION_PROGRESS_WRITE_END,

			OPERATION_PROGRESS_READ_START,
			OPERATION_PROGRESS_READ_BIT,
			OPERATION_PROGRESS_READ_WAIT,
			OPERATION_PROGRESS_READ_END
		} operationProgress = OPERATION_PROGRESS_IDLE;   

		enum {
			IDLE,
			CHECK_FREE,
			WORK,
			WAITING,
			CLEAR,
			FINISH
		} operationState = IDLE;

		enum EoperationType {
			RESET,
			RECEIVE,
			TRANSMIT
		};
			
		struct operation {
			EoperationType  operationType;
			uint8_t         *pData;
			uint16_t        Size;
			bool            free = false;
			dataCallback_f  callback_f = nullptr;
		} currentOperation;
		
		std::queue<operation> operations;
};

#endif // ONEWIRE_H