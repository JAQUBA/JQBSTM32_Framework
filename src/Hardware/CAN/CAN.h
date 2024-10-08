#ifndef _CAN_H
#define _CAN_H
#include "../../Core.h"
#include "Interface/IBus.h"
#include <unordered_map> 

#ifndef CAN_MAX_INSTANCES
#define CAN_MAX_INSTANCES 1
#endif

class CAN : public IBus{
	public:
		static CAN *getInstance(CAN_HandleTypeDef *pHandler);
		CAN(CAN_HandleTypeDef *pHandler);
		
		void onPacket(uint16_t commNumber, dataCallback_f cHandler);
		void send(uint32_t identifier, uint8_t *pData,
		 		uint32_t DataLength = 8U, uint32_t rtr_mode = CAN_RTR_DATA);
        void txInterrupt();
        void rxInterrupt();
        void errorInterrupt();

	private:
		CAN_HandleTypeDef *_pInstance;
		CAN_FilterTypeDef sFilterConfig;
		
		CAN_RxHeaderTypeDef pRxHeader;
		CAN_TxHeaderTypeDef pTxHeader;

		uint32_t canMailbox;
		uint8_t pData[8] = {0,0,0,0,0,0,0,0};
		bool hasPacket = false;

		struct handler {
			uint32_t commNumber;
			dataCallback_f handler;
		};
		std::list<struct handler> handlers;
};

#endif