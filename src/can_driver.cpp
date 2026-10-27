#include "can_driver.h"
#include "board_config.h"
#include "pdm_settings.h"
#include "leds.h"
#include <SPI.h>
#include <mcp_can.h>

namespace
{
    MCP_CAN controller(Board::CAN_CS);
    CANFrame queue[Settings::CAN_RX_QUEUE_LENGTH];
    uint8_t head=0,tail=0,count=0;uint16_t dropped=0;uint32_t rx=0,tx=0;volatile bool irq=false;volatile uint32_t irqCount=0;CANDriverState state=CANDriverState::Uninitialized;
    void isr(){irq=true;++irqCount;}
    bool push(const CANFrame&f){if(count>=Settings::CAN_RX_QUEUE_LENGTH){++dropped;return false;}queue[head]=f;head=(head+1)%Settings::CAN_RX_QUEUE_LENGTH;++count;return true;}
    void drain()
    {
        while(controller.checkReceive()==CAN_MSGAVAIL)
        {
            unsigned long id=0;byte len=0,data[8]={0};if(controller.readMsgBuf(&id,&len,data)!=CAN_OK)break;
            CANFrame f={};f.extended=(id&0x80000000UL)!=0;f.remote=(id&0x40000000UL)!=0;f.id=id&(f.extended?0x1FFFFFFFUL:0x7FFUL);f.length=len>8?8:len;for(uint8_t i=0;i<f.length;++i)f.data[i]=data[i];push(f);++rx;LEDs_NotifyCANActivity();
        }
    }
}
bool CAN_Init()
{
    pinMode(Board::CAN_INT,INPUT_PULLUP);pinMode(Board::CAN_CS,OUTPUT);digitalWrite(Board::CAN_CS,HIGH);SPI.begin();
    if(controller.begin(MCP_ANY,CAN_500KBPS,MCP_16MHZ)!=CAN_OK){state=CANDriverState::InitializationFault;return false;}
    controller.setMode(MCP_NORMAL);attachInterrupt(digitalPinToInterrupt(Board::CAN_INT),isr,FALLING);state=CANDriverState::Normal;return true;
}
void CAN_Process(){bool pending;noInterrupts();pending=irq;irq=false;interrupts();if(pending||digitalRead(Board::CAN_INT)==LOW)drain();}
bool CAN_GetFrame(CANFrame&f){if(count==0)return false;f=queue[tail];tail=(tail+1)%Settings::CAN_RX_QUEUE_LENGTH;--count;return true;}
bool CAN_SendStandard(uint16_t id,uint8_t len,const uint8_t*data){if(id>0x7FF||len>8||(len&&data==nullptr))return false;byte r=controller.sendMsgBuf(id,0,len,const_cast<byte*>(data));if(r==CAN_OK){++tx;return true;}return false;}
bool CAN_SendExtended(uint32_t id,uint8_t len,const uint8_t*data){if(id>0x1FFFFFFFUL||len>8||(len&&data==nullptr))return false;byte r=controller.sendMsgBuf(id,1,len,const_cast<byte*>(data));if(r==CAN_OK){++tx;return true;}return false;}
CANDriverState CAN_GetState(){return state;}
bool CAN_IsHealthy(){return state==CANDriverState::Normal;}
bool CAN_IsInterruptAsserted(){return digitalRead(Board::CAN_INT)==LOW;}
uint8_t CAN_GetQueueCount(){return count;}
uint16_t CAN_GetDroppedFrameCount(){return dropped;}
uint32_t CAN_GetReceiveCount(){return rx;}
uint32_t CAN_GetTransmitCount(){return tx;}
uint32_t CAN_GetInterruptCount(){uint32_t v;noInterrupts();v=irqCount;interrupts();return v;}
void CAN_RequestRecovery(){controller.setMode(MCP_NORMAL);state=CANDriverState::Normal;}
