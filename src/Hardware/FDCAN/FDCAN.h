#ifndef __FDCAN_H_
#define __FDCAN_H_

#include "../../Core.h"
#include "Interface/IBus.h"
#include <unordered_map> 

#ifndef FDCAN_MAX_INSTANCES
#define FDCAN_MAX_INSTANCES 1
#endif

class FDCAN : public IBus{
	public:
		static FDCAN *getInstance(FDCAN_HandleTypeDef *pHandler);
		FDCAN(FDCAN_HandleTypeDef *pHandler);
		
		void onPacket(uint16_t commNumber, dataCallback_f handler);
		void send(uint32_t identifier, uint8_t *pData, uint16_t Size, uint32_t DataLength = FDCAN_DLC_BYTES_8);

        void rxInterrupt();

	private:
		FDCAN_HandleTypeDef *_pInstance;
		FDCAN_FilterTypeDef sFilterConfig;
		
		FDCAN_RxHeaderTypeDef pRxHeader;
		FDCAN_TxHeaderTypeDef pTxHeader;

		uint8_t pData[8] = {0,0,0,0,0,0,0,0};
		bool hasPacket = false;

		struct handlerStruct {
			uint32_t commNumber;
			dataCallback_f handler;
			struct handlerStruct *next;
		};

		struct handlerStruct *handlers;
};
#endif