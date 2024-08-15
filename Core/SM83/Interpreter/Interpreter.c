#include "Interpreter.h"
#include "sm83.h"
#include "InterpreterTable.h"

static void
sm83SingleStep(struct Interpreter* interpreter)
{
    interpreter->cycles += interpreter->tMultiplier;
    enum InterpreterCoreState state = interpreter->coreState;
    switch(state) {
        case FETCH:
            interpreter->bus = interpreter->memory.load8(interpreter, interpreter->cpuState.pc);
            interpreter->instruction = sm83InterpreterTable[interpreter->bus];
            interpreter->cpuState.pc++;
            break;
        case READ_PC:
            interpreter->bus = interpreter->memory.load8(interpreter, interpreter->cpuState.pc);
            interpreter->cpuState.pc++;
            break;
        case MEMORY_LOAD:
            interpreter->bus = interpreter->memory.load8(interpreter, interpreter->memIndex);
            break;
        case MEMORY_STORE:
            interpreter->memory.store8(interpreter, interpreter->memIndex, interpreter->bus);
        case STALL:
            interpreter->instruction = sm83InterpreterTable[0];
    }
    interpreter->SingleStepInner(interpreter);
}


static void
sm83SingleStepInner(struct Interpreter* interpreter)
{
    interpreter->coreState = FETCH;
    interpreter->instruction(interpreter);
}

static void
initCpu(struct SM83State* cpuState)
{
   cpuState->StateInit = &sm83StateInit;
   cpuState->StateReset = &sm83StateReset;
}

static void
initMemory(struct InterpreterMemory* memory)
{

}

void
initInterpreter(struct Interpreter* interpreter)
{
    interpreter->isRunning = 1;
    initCpu(&interpreter->cpuState);
    initMemory(&interpreter->memory);

    interpreter->cpuState.StateInit(&interpreter->cpuState);
    interpreter->cycles = 0;
    interpreter->tMultiplier = 4;
    interpreter->coreState = FETCH;

}


void
runInterpreter(struct Interpreter* interpreter)
{
    while(interpreter->isRunning) {
        interpreter->SingleStep(interpreter);
    }
}
