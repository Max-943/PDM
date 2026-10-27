#ifndef CAN_DRIVER_H
#define CAN_DRIVER_H

#include <Arduino.h>

struct CANFrame
{
    uint32_t id;
    uint8_t length;
    uint8_t data[8];
    bool extended;
    bool remote;
};

enum class CANDriverState : uint8_t
{
    Uninitialized,
    Normal,
    ErrorPassive,
    BusOff,
    InitializationFault
};

bool CAN_Init();
void CAN_Process();
bool CAN_GetFrame(CANFrame &frame);
bool CAN_SendStandard(uint16_t id, uint8_t length, const uint8_t *data);
bool CAN_SendExtended(uint32_t id, uint8_t length, const uint8_t *data);
CANDriverState CAN_GetState();
bool CAN_IsHealthy();
bool CAN_IsInterruptAsserted();
uint8_t CAN_GetQueueCount();
uint16_t CAN_GetDroppedFrameCount();
uint32_t CAN_GetReceiveCount();
uint32_t CAN_GetTransmitCount();
uint32_t CAN_GetInterruptCount();
void CAN_RequestRecovery();

#endif
