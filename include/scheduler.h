#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <Arduino.h>

struct SchedulerStatistics
{
    uint32_t task1msCount;
    uint32_t task10msCount;
    uint32_t task100msCount;
    uint32_t task1sCount;
    uint16_t task1msMaxDurationUs;
    uint16_t task10msMaxDurationUs;
    uint16_t task100msMaxDurationUs;
    uint16_t task1sMaxDurationUs;
    uint16_t overrunCount;
};

void Scheduler_Init();
void Scheduler_Run();
void Scheduler_Task1ms();
void Scheduler_Task10ms();
void Scheduler_Task100ms();
void Scheduler_Task1s();
bool Scheduler_IsHealthy();
bool Scheduler_HasTask1msRunRecently();
bool Scheduler_HasTask10msRunRecently();
bool Scheduler_HasTask100msRunRecently();
uint16_t Scheduler_GetOverrunCount();
SchedulerStatistics Scheduler_GetStatistics();
void Scheduler_ClearStatistics();

#endif
