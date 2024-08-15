#include "sm83.h"
#include <stdint.h>

void
sm83StateReset(struct SM83State* cpuState)
{
    cpuState->af = 0;
    cpuState->bc = 0;
    cpuState->de = 0;
    cpuState->hl = 0;
    cpuState->pc = 0;
    cpuState->sp = 0;
}

void
sm83StateInit(struct SM83State* cpuState)
{
    cpuState->af = 0;
    cpuState->bc = 0;
    cpuState->de = 0;
    cpuState->hl = 0;
    cpuState->pc = 0;
    cpuState->sp = 0;
}
