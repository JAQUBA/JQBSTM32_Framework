#ifndef __FDCAN_H_
#define __FDCAN_H_

#include "Core.h"

#define FDCAN_MAX_INSTANCES 1

class FDCAN {
	public:
		FDCAN(FDCAN_HandleTypeDef *instance);
		void init();

		static FDCAN *getInstance(FDCAN_HandleTypeDef *_instance);

		void onPacket(uint16_t commNumber, void(*handler)(uint8_t *data, uint16_t dataLen));
		void poll();

		void handleMessage();
	private:
		FDCAN_HandleTypeDef *_pInstance;

		FDCAN_RxHeaderTypeDef pRxHeader;
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