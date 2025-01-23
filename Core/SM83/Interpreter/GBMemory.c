#include "GBMemory.h"
#include "Interpreter.h"

#include <stdint.h>
#include <sys/mman.h>

static uint8_t GBLoad8(struct Interpreter* interpreter, uint16_t address) {
    struct GBMemory* gbMemory = (struct GBMemory*)interpreter->memory.m_component;
    switch (address >> 12) {
    case 0x0:
    case 0x1:
    case 0x2:
    case 0x3:
    case 0x4:
    case 0x5:
    case 0x6:
    case 0x7:
        return gbMemory->externalRomRegion[address];
        break;
    case 0x8:
    case 0x9:
        return gbMemory->vRam[address & 0x1FFF];
        break;
    case 0xA:
    case 0xB:
        return gbMemory->externalRam[address & 0x1FFF];
        break;
    case 0xC:
    case 0xD:
        return gbMemory->wRam[address & 0x1FFF];
    case 0xE:
        return gbMemory->wRam[address & 0x1FFF];
    // need to finish it
    }
}

static void GBStore8(struct Interpreter* interpreter, uint16_t address, uint8_t value) {
}

static void GBMemoryInit(struct Interpreter* interpreter, struct InterpreterComponent* component) {
    struct GBMemory* gbMemory = (struct GBMemory*)component;
    // allocate all memory relating to gameboy
    //
    //
    interpreter->memory.load8 = GBLoad8;
    interpreter->memory.store8 = GBStore8;
}

static void GBMemoryDeinit(struct Interpreter* interpreter, struct InterpreterComponent* component) {
    struct GBMemory* gbMemory = (struct GBMemory*)component;
    // free all allocated memories
    //
    //
}

void GBMemoryCreate(struct GBMemory* gbMemory) {
    gbMemory->m_component.init = GBMemoryInit;
    gbMemory->m_component.deinit = GBMemoryDeinit;
}

