#ifndef INTERPRETER_TABLE_H
#define INTERPRETER_TABLE_H

#include "sm83.h"

struct Interpreter;

typedef void (*SM83Instruction)(struct Interpreter*);
extern const SM83Instruction sm83InterpreterTable[0x100];

#endif
