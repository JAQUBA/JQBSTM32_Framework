#ifndef _CAN_H
#define _CAN_H
#include "../../Core.h"
#include "Interface/IBus.h"
#include <unordered_map> 

#define CAN_MAX_INSTANCES 1

class CAN : public IBus{
	public:
		static CAN *getInstance(CAN_HandleTypeDef *pHandler);
		CAN(CAN_HandleTypeDef *pHandler);
		
		void onPacket(uint16_t commNumber, dataCallback_f handler);
		void send(uint32_t identifier, uint8_t *pData, uint16_t Size, uint32_t DataLength);

        void txInterrupt();
        void rxInterrupt();
        void errorInterrupt();

	private:
		CAN_HandleTypeDef *_pInstance;
		// CAN_FilterTypeDef sFilterConfig;
		
		// CAN_RxHeaderTypeDef pRxHeader;
		// CAN_TxHeaderTypeDef pTxHeader;

		// uint8_t pData[8] = {0,0,0,0,0,0,0,0};
		// bool hasPacket = false;

		// struct handlerStruct {
		// 	uint32_t commNumber;
		// 	dataCallback_f handler;
		// 	struct handlerStruct *next;
		// };

		// struct handlerStruct *handlers;
};

#endif