#ifndef GBMEMORY_H
#define GBMEMORY_H

#include "Interpreter.h"
#include "SM83/sm83.h"

struct GBMemory {
    struct InterpreterComponent m_component;
    char* externalRomRegion;
    char* vRam;
    char* externalRam;
    char* wRam;
};

void GBMemoryCreate(struct GBMemory*);

#endif
