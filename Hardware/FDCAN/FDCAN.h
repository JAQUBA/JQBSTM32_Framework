#ifndef __FDCAN_H_
#define __FDCAN_H_

#include "../../Core.h"

#define FDCAN_MAX_INSTANCES 1

class FDCAN {
	public:
		static FDCAN *getInstance(FDCAN_HandleTypeDef *pHandler);
		FDCAN(FDCAN_HandleTypeDef *pHandler);
		
		void onPacket(uint16_t commNumber, void(*handler)(uint8_t *data, uint16_t dataLen));
		void poll();
		void send(uint32_t identifier, uint8_t *data, uint16_t dataLen, uint32_t DataLength = FDCAN_DLC_BYTES_8);

		void RxFifo0Callback();
	private:
		FDCAN_HandleTypeDef *_pInstance;
		FDCAN_FilterTypeDef sFilterConfig;
		
		FDCAN_RxHeaderTypeDef pRxHeader;
		FDCAN_TxHeaderTypeDef pTxHeader;

		uint8_t recv_buff[8] = {0,0,0,0,0,0,0,0};
		bool hasPacket = false;

		struct handlerStruct {
			uint32_t commNumber;
			void(*handler)(uint8_t *data, uint16_t dataLen);
			struct handlerStruct *next;
		};

		struct handlerStruct *handlers;
};
#endif