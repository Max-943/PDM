#include "outputs.h"
#include "board_config.h"
#include <avr/io.h>

namespace
{
    struct Channel { bool request; bool enabled; uint16_t inhibits; };
    Channel channels[2] = {};
    uint16_t globalInhibits = 0;
    uint8_t indexOf(OutputId id) { return static_cast<uint8_t>(id); }
    void writePin(OutputId id, bool on)
    {
        const uint8_t mask = (id == OutputId::OutputPC0) ? _BV(Board::OUTPUT_PC0_BIT) : _BV(Board::OUTPUT_PC1_BIT);
        if (on) PORTC |= mask; else PORTC &= static_cast<uint8_t>(~mask);
    }
}

void Outputs_InitSafe()
{
    PORTC &= static_cast<uint8_t>(~(_BV(Board::OUTPUT_PC0_BIT) | _BV(Board::OUTPUT_PC1_BIT)));
    DDRC |= _BV(Board::OUTPUT_PC0_BIT) | _BV(Board::OUTPUT_PC1_BIT);
    channels[0] = {}; channels[1] = {}; globalInhibits = 0;
}
void Outputs_Task10ms()
{
    for (uint8_t i=0;i<2;++i)
    {
        channels[i].inhibits = globalInhibits;
        channels[i].enabled = channels[i].request && channels[i].inhibits == 0;
        writePin(static_cast<OutputId>(i), channels[i].enabled);
    }
}
void Outputs_Request(OutputId id, bool enabled) { channels[indexOf(id)].request = enabled; }
void Outputs_SetSystemPermission(bool permitted, OutputInhibitReason reason)
{
    const uint16_t mask=static_cast<uint16_t>(reason);
    if (permitted) globalInhibits &= static_cast<uint16_t>(~mask); else globalInhibits |= mask;
}
void Outputs_ForceOff(OutputId id, OutputInhibitReason reason)
{
    Channel &c=channels[indexOf(id)]; c.request=false; c.inhibits |= static_cast<uint16_t>(reason); writePin(id,false); c.enabled=false;
}
void Outputs_ForceAllOff(OutputInhibitReason reason)
{
    globalInhibits |= static_cast<uint16_t>(reason);
    for(uint8_t i=0;i<2;++i){channels[i].request=false; channels[i].enabled=false; writePin(static_cast<OutputId>(i),false);}
}
void Outputs_ClearInhibit(OutputInhibitReason reason) { globalInhibits &= static_cast<uint16_t>(~static_cast<uint16_t>(reason)); }
bool Outputs_IsRequested(OutputId id){return channels[indexOf(id)].request;}
bool Outputs_GetState(OutputId id){return channels[indexOf(id)].enabled;}
uint16_t Outputs_GetInhibitReasons(OutputId id){return channels[indexOf(id)].inhibits;}
OutputStatus Outputs_GetStatus(OutputId id){Channel &c=channels[indexOf(id)]; return {c.request,c.enabled,c.inhibits==0,c.inhibits};}
