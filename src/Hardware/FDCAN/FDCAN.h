#ifndef __FDCAN_H_
#define __FDCAN_H_

#include "../../Core.h"
#include "Interface/IBus.h"
#include <unordered_map> 

#ifndef FDCAN_MAX_INSTANCES
#define FDCAN_MAX_INSTANCES 1
#endif

/**
 * @class FDCAN
 * @brief Class for handling FDCAN operations.
 */
class FDCAN : public IBus {
    public:
	/**
	 * @brief Gets the instance of the FDCAN.
	 * @param pHandler Pointer to FDCAN_HandleTypeDef structure.
	 * @return Pointer to FDCAN instance.
	 */
	static FDCAN *getInstance(FDCAN_HandleTypeDef *pHandler);

	/**
	 * @brief Constructor for FDCAN class.
	 * @param pHandler Pointer to FDCAN_HandleTypeDef structure.
	 */
	FDCAN(FDCAN_HandleTypeDef *pHandler);

	/**
	 * @brief Registers a callback for a specific communication number.
	 * @param commNumber Communication number.
	 * @param handler Callback function to handle the data.
	 */
	void onPacket(uint16_t commNumber, dataCallback_f handler);

	/**
	 * @brief Sends data over FDCAN.
	 * @param identifier Message identifier.
	 * @param pData Pointer to data to be sent.
	 * @param Size Size of the data.
	 * @param DataLength Length of the data (default is FDCAN_DLC_BYTES_8).
	 */
	void send(uint32_t identifier, uint8_t *pData, uint16_t Size, uint32_t DataLength = FDCAN_DLC_BYTES_8);

	/**
	 * @brief Handles the receive interrupt.
	 */
	void rxInterrupt();

    private:
	FDCAN_HandleTypeDef *_pInstance; /**< Pointer to FDCAN_HandleTypeDef structure. */
	FDCAN_FilterTypeDef sFilterConfig; /**< Filter configuration structure. */
	
	FDCAN_RxHeaderTypeDef pRxHeader; /**< Receive header structure. */
	FDCAN_TxHeaderTypeDef pTxHeader; /**< Transmit header structure. */

	uint8_t pData[8] = {0,0,0,0,0,0,0,0}; /**< Data buffer. */
	bool hasPacket = false; /**< Flag indicating if a packet has been received. */

	/**
	 * @struct handlerStruct
	 * @brief Structure to hold handler information.
	 */
	struct handlerStruct {
	    uint32_t commNumber; /**< Communication number. */
	    dataCallback_f handler; /**< Callback function. */
	    struct handlerStruct *next; /**< Pointer to the next handler structure. */
	};
};
#endif