#include "MemoryBlock.h"

MemoryBlock::MemoryBlock(IExternalMemory *device, uint32_t startingAddress) {
    _extMemory = device;
    _startingAddress = startingAddress;
}
void MemoryBlock::loadBlock(
    uint8_t *pData, 
    uint16_t Size
) {
    _extMemory->readFromMemory(_startingAddress, pData, Size);
}
void MemoryBlock::saveBlock(
    uint8_t *pData, 
    uint16_t Size
) {
    _extMemory->writeToMemory(_startingAddress, pData, Size);
}