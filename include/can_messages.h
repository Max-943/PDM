#ifndef CAN_MESSAGES_H
#define CAN_MESSAGES_H

#include <Arduino.h>

struct PDMCommand
{
    bool efuse1Requested;
    bool efuse2Requested;
    bool outputPC0Requested;
    bool outputPC1Requested;
    bool emergencyOffRequested;
    uint8_t rollingCounter;
    uint8_t receivedCRC;
    uint32_t lastValidMessageMs;
    bool valid;
    bool counterValid;
    bool crcValid;
};

struct PDMFaultResetCommand
{
    bool resetEFuse1;
    bool resetEFuse2;
    bool resetSystem;
    uint16_t resetKey;
    bool valid;
};

void CANMessages_Init();
void CANMessages_ProcessReceived();
void CANMessages_Task10ms();
void CANMessages_SendStatus();
void CANMessages_SendCurrents();
void CANMessages_SendSensors();
void CANMessages_SendFaults();
void CANMessages_SendDiagnostics();
const PDMCommand &CANMessages_GetCommand();
bool CANMessages_HasValidCommand();
bool CANMessages_HasTimedOut();
uint32_t CANMessages_GetLastValidCommandTimeMs();
uint8_t CANMessages_GetLastCommandCounter();
uint8_t CANMessages_CalculateCRC8(const uint8_t *data, uint8_t length);
void CANMessages_InvalidateCommand();

#endif
