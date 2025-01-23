#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "SM83/sm83.h"
#include "InterpreterTable.h"

#include <stdint.h>

struct InterpreterComponent {
    void (*init)(struct Interpreter*, struct InterpreterComponent*);
    void (*deinit)(struct Interpreter*, struct InterpreterComponent*);
};

struct InterpreterMemory {
    struct InterpreterComponent* m_component;
    uint8_t (*load8)(struct Interpreter*, uint16_t address);
    void (*store8)(struct Interpreter*, uint16_t address, uint8_t value);
};

enum InterpreterCoreState {
    FETCH,
    READ_PC,
    MEMORY_LOAD,
    MEMORY_STORE,
    STALL
};

struct Interpreter {
    unsigned isRunning;

    struct SM83State cpuState;
    int32_t cycles;
    int32_t tMultiplier;

    struct InterpreterMemory memory;
    enum InterpreterCoreState coreState;
    uint16_t temp; // comes handy
    uint8_t bus;
    uint16_t memIndex;
    SM83Instruction instruction;

    void (*SingleStep)(struct Interpreter*);
    void (*SingleStepInner)(struct Interpreter*);
    void (*Shutdown)(struct Interpreter*);
};

void initInterpreter(struct Interpreter*);
void runInterpreter(struct Interpreter*);


#endif
