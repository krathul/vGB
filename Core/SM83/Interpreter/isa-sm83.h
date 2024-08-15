#include "Interpreter.h"
#define DEFINE_INSTRUCTION(NAME, BODY) \
    static void NAME(struct Interpreter* interpreter) {\
        BODY; \
    }

// 8-bit load instructions
// ld r,r
#define DEFINE_LD_B_INSTRUCTION(NAME, OPERAND) \
    DEFINE_INSTRUCTION(LD_B_ ## NAME, \
        interpreter->cpuState.b = interpreter->OPERAND;)

#define DEFINE_LD_C_INSTRUCTION(NAME, OPERAND) \
    DEFINE_INSTRUCTION(LD_C_ ## NAME, \
        interpreter->cpuState.c = interpreter->OPERAND;)

#define DEFINE_LD_D_INSTRUCTION(NAME, OPERAND) \
    DEFINE_INSTRUCTION(LD_D_ ## NAME, \
        interpreter->cpuState.d = interpreter->OPERAND;)

#define DEFINE_LD_E_INSTRUCTION(NAME, OPERAND) \
    DEFINE_INSTRUCTION(LD_E_ ## NAME, \
        interpreter->cpuState.e = interpreter->OPERAND;)

#define DEFINE_LD_H_INSTRUCTION(NAME, OPERAND) \
    DEFINE_INSTRUCTION(LD_H_ ## NAME, \
        interpreter->cpuState.h = interpreter->OPERAND;)

#define DEFINE_LD_L_INSTRUCTION(NAME, OPERAND) \
    DEFINE_INSTRUCTION(LD_L_ ## NAME, \
        interpreter->cpuState.l = interpreter->OPERAND;)

#define DEFINE_LD_A_INSTRUCTION(NAME, OPERAND) \
    DEFINE_INSTRUCTION(LD_A_ ## NAME, \
        interpreter->cpuState.a = interpreter->OPERAND;)

// ld r,n
#define DEFINE_LD_R_N_INSTRUCTION(NAME) \
    DEFINE_INSTRUCTION(LD_ ## NAME ## _N, \
        interpreter->coreState = READ_PC; \
        interpreter->instruction = LD_ ## NAME ## _BUS;)

// ld r,(HL)
#define DEFINE_LD_R_HL_INSTRUCTION(NAME) \
    DEFINE_INSTRUCTION(LD_ ## NAME ## _HL, \
        interpreter->coreState = MEMORY_LOAD; \
        interpreter->memIndex = interpreter->cpuState.hl; \
        interpreter->instruction = LD_ ## NAME ## _BUS;)

// ld (HL),r
#define DEFINE_LD_HL_INSTRUCTION(NAME, OPERAND) \
    DEFINE_INSTRUCTION(LD_HL_ ## NAME, \
        interpreter->coreState = MEMORY_STORE; \
        interpreter->memIndex = interpreter->cpuState.hl; \
        interpreter->bus = interpreter->OPERAND; \
        interpreter->instruction = NOP;)

// ld (HL),n
#define DEFINE_LD_HL_N_INSTRUCTION \
    DEFINE_INSTRUCTION(LD_HL_N, \
        interpreter->coreState = READ_PC; \
        interpreter->instruction = LD_HL_BUS;)

// ld A,(BC)
#define DEFINE_LD_A_BC_INSTRUCTION \
    DEFINE_INSTRUCTION(LD_A_BC, \
        interpreter->coreState = MEMORY_LOAD; \
        interpreter->memIndex = interpreter->cpuState.bc; \
        interpreter->instruction = LD_A_BUS;)

// ld A,(DE)
#define DEFINE_LD_A_DE_INSTRUCTION \
    DEFINE_INSTRUCTION(LD_A_DE, \
        interpreter->coreState = MEMORY_LOAD; \
        interpreter->memIndex = interpreter->cpuState.de; \
        interpreter->instruction = LD_A_BUS;)

// ld A,(nn)
#define DEFINE_LD_A_NN_STEP2 \
    DEFINE_INSTRUCTION(LD_A_NN_STEP2, \
        interpreter->memIndex |= interpreter->bus << 8; \
        interpreter->coreState = MEMORY_LOAD; \
        interpreter->instruction = LD_A_BUS;)

#define DEFINE_LD_A_NN_STEP1 \
    DEFINE_INSTRUCTION(LD_A_NN_STEP1, \
        interpreter->memIndex = interpreter->bus; \
        interpreter->coreState = READ_PC; \
        interpreter->instruction = LD_A_NN_STEP2;)

#define DEFINE_LD_A_NN_INSTRUCTION \
    DEFINE_LD_A_NN_STEP2; \
    DEFINE_LD_A_NN_STEP1; \
    DEFINE_INSTRUCTION(LD_A_NN, \
        interpreter->coreState = READ_PC; \
        interpreter->instruction = LD_A_NN_STEP1;)

// ld (BC),A
#define DEFINE_LD_BC_A_INSTRUCTION \
    DEFINE_INSTRUCTION(LD_BC_A, \
        interpreter->bus = interpreter->cpuState.a; \
        interpreter->memIndex = interpreter->cpuState.bc; \
        interpreter->coreState = MEMORY_STORE; \
        interpreter->instruction = NOP;)

// ld (DE),A
#define DEFINE_LD_DE_A_INSTRUCTION \
    DEFINE_INSTRUCTION(LD_DE_A, \
        interpreter->bus = interpreter->cpuState.a; \
        interpreter->memIndex = interpreter->cpuState.de; \
        interpreter->coreState = MEMORY_STORE; \
        interpreter->instruction = NOP;)

// ld (nn),A
#define DEFINE_LD_NN_A_STEP2 \
    DEFINE_INSTRUCTION(LD_NN_A_STEP2, \
        interpreter->memIndex = interpreter->memIndex << 8 | interpreter->bus; \
        interpreter->coreState = MEMORY_STORE; \
        interpreter->bus = interpreter->cpuState.a; \
        interpreter->instruction = NOP;)

#define DEFINE_LD_NN_A_STEP1 \
    DEFINE_INSTRUCTION(LD_NN_A_STEP1, \
        interpreter->memIndex = interpreter->bus; \
        interpreter->coreState = READ_PC; \
        interpreter->instruction = LD_NN_A_STEP2;)

#define DEFINE_LD_NN_A_INSTRUCTION \
    DEFINE_LD_NN_A_STEP2; \
    DEFINE_LD_NN_A_STEP1; \
    DEFINE_INSTRUCTION(LD_NN_A, \
        interpreter->coreState = READ_PC; \
        interpreter->instruction = LD_NN_A_STEP1;)

// ld A,(FF00+n)
#define DEFINE_LDH_A_N_STEP1 \
    DEFINE_INSTRUCTION(LDH_A_N_STEP1, \
        interpreter->memIndex = 0xFF00 + interpreter->bus; \
        interpreter->coreState = MEMORY_LOAD; \
        interpreter->instruction = LD_A_BUS;)

#define DEFINE_LDH_A_N_INSTRUCTION \
    DEFINE_LDH_A_N_STEP1; \
    DEFINE_INSTRUCTION(LDH_A_N, \
        interpreter->coreState = READ_PC; \
        interpreter->instruction = LDH_A_N_STEP1;)

// ld (FF00+n),A
#define DEFINE_LDH_N_A_STEP1 \
    DEFINE_INSTRUCTION(LDH_A_N_STEP1, \
        interpreter->memIndex = 0xFF00 + interpreter->bus; \
        interpreter->coreState = MEMORY_STORE; \
        interpreter->bus = interpreter->cpuState.a; \
        interpreter->instruction = NOP;)

#define DEFINE_LDH_N_A_INSTRUCTION \
    DEFINE_LDH_N_A_STEP1; \
    DEFINE_INSTRUCTION(LDH_N_A, \
        interpreter->coreState = READ_PC; \
        interpreter->instruction = LDH_N_A_STEP1;)

// ld A,(FF00+C)
#define DEFINE_LDH_A_C_INSTRUCTION \
    DEFINE_INSTRUCTION(LDH_A_C, \
        interpreter->memIndex = 0xFF00 + interpreter->cpuState.c; \
        interpreter->coreState = MEMORY_LOAD; \
        interpreter->instruction = LD_A_BUS;)

// ld (FF00+C),A
#define DEFINE_LDH_C_A_INSTRUCTION \
    DEFINE_INSTRUCTION(LDH_C_A, \
        interpreter->memIndex = 0xFF00 + interpreter->cpuState.c; \
        interpreter->bus = interpreter->cpuState.a; \
        interpreter->coreState = MEMORY_STORE; \
        interpreter->instruction = NOP;)

// ldi A,(HL)
#define DEFINE_LDI_A_HL_INSTRUCTION \
    DEFINE_INSTRUCTION(LDI_A_HL, \
        interpreter->memIndex = interpreter->cpuState.hl; \
        interpreter->cpuState.hl += 1; \
        interpreter->coreState = MEMORY_LOAD; \
        interpreter->instruction = LD_A_BUS;)

// ldi (HL),A
#define DEFINE_LDI_HL_A_INSTRUCTION \
    DEFINE_INSTRUCTION(LDI_HL_A, \
        interpreter->memIndex = interpreter->cpuState.hl; \
        interpreter->cpuState.hl += 1; \
        interpreter->bus = interpreter->cpuState.a; \
        interpreter->coreState = MEMORY_STORE; \
        interpreter->instruction = NOP;)

// ldd A,(HL)
#define DEFINE_LDD_A_HL_INSTRUCTION \
    DEFINE_INSTRUCTION(LDD_A_HL, \
    interpreter->memIndex = interpreter->cpuState.hl; \
    interpreter->cpuState.hl -= 1; \
    interpreter->coreState = MEMORY_LOAD; \
    interpreter->instruction = LD_A_BUS;)

// ldd (HL),A
#define DEFINE_LDD_HL_A_INSTRUCTION \
    DEFINE_INSTRUCTION(LDD_HL_A, \
        interpreter->memIndex = interpreter->cpuState.hl; \
        interpreter->cpuState.hl -= 1; \
        interpreter->bus = interpreter->cpuState.a; \
        interpreter->coreState = MEMORY_STORE; \
        interpreter->instruction = NOP;)

// ld rr,nn
#define DEFINE_LD_RR_NN_STEP1(NAME, L) \
    DEFINE_INSTRUCTION(LD_ ## NAME ## _NN_STEP1, \
        interpreter->L = interpreter->bus; \
        interpreter->coreState = READ_PC; \
        interpreter->instruction = LD_ ## NAME ## _NN_STEP2;)

#define DEFINE_LD_RR_NN_INSTRUCTION(NAME, H, L) \
    DEFINE_INSTRUCTION(LD_ ## NAME ## _NN_STEP2, interpreter->H = interpreter->bus;) \
    DEFINE_LD_RR_NN_STEP1(NAME, L); \
    DEFINE_INSTRUCTION(LD_ ## NAME ## _NN, \
        interpreter->coreState = READ_PC; \
        interpreter->instruction = LD_ ## NAME ## _NN_STEP1;)

#define DEFINE_LD_SP_NN_INSTRUCTION \
    DEFINE_INSTRUCTION(LD_SP_NN_STEP2, interpreter->cpuState.sp |= interpreter->bus << 8;) \
    DEFINE_LD_RR_NN_STEP1(SP, cpuState.sp); \
    DEFINE_INSTRUCTION(LD_SP_NN, \
        interpreter->coreState = READ_PC; \
        interpreter->instruction = LD_SP_NN_STEP1;)

// ld nn,sp
#define DEFINE_LD_NN_SP_STEP3 \
    DEFINE_INSTRUCTION(LD_NN_SP_STEP3, \
        interpreter->bus = interpreter->cpuState.sp >> 8; \
        interpreter->coreState = MEMORY_STORE; \
        interpreter->memIndex += 1; \
        interpreter->instruction = NOP;)

#define DEFINE_LD_NN_SP_STEP2 \
    DEFINE_INSTRUCTION(LD_NN_SP_STEP2, \
        interpreter->memIndex |= interpreter->bus << 8; \
        interpreter->coreState = MEMORY_STORE; \
        interpreter->bus = interpreter->cpuState.sp; \
        interpreter->instruction = LD_NN_SP_STEP3;)

#define DEFINE_LD_NN_SP_STEP1 \
    DEFINE_INSTRUCTION(LD_NN_SP_STEP1, \
        interpreter->memIndex = interpreter->bus; \
        interpreter->coreState = READ_PC; \
        interpreter->instruction = LD_NN_SP_STEP2;)

#define DEFINE_LD_NN_SP_INSTRUCTION \
    DEFINE_LD_NN_SP_STEP3; \
    DEFINE_LD_NN_SP_STEP2; \
    DEFINE_LD_NN_SP_STEP1; \
    DEFINE_INSTRUCTION(LD_NN_SP, \
        interpreter->coreState = READ_PC; \
        interpreter->instruction = LD_NN_SP_STEP1;)


// ld sp,hl
#define DEFINE_LD_SP_HL \
    DEFINE_INSTRUCTION(LD_SP_HL, \
        interpreter->cpuState.sp = interpreter->cpuState.hl; \
        interpreter->coreState = STALL;)

// push rr
#define DEFINE_PUSH_RR_STEP1(NAME, L) \
    DEFINE_INSTRUCTION(PUSH_ ## NAME ## _STEP1, \
        interpreter->cpuState.sp -= 1; \
        interpreter->memIndex = interpreter->cpuState.sp; \
        interpreter->bus = interpreter->L; \
        interpreter->coreState = MEMORY_STORE; \
        interpreter->instruction = PUSH_ ## NAME ## _STEP2;)


#define DEFINE_PUSH_RR_INSTRUCTION(NAME, H, L) \
    DEFINE_INSTRUCTION(PUSH_ ## NAME ## _STEP2, interpreter->coreState = STALL;) \
    DEFINE_PUSH_RR_STEP1(NAME, L); \
    DEFINE_INSTRUCTION(PUSH_ ## NAME, \
        interpreter->cpuState.sp -= 1; \
        interpreter->memIndex = interpreter->cpuState.sp; \
        interpreter->bus = interpreter->H; \
        interpreter->coreState = MEMORY_STORE; \
        interpreter->instruction = PUSH_ ## NAME ## _STEP1)

// pop rr
#define DEFINE_POP_RR_STEP1(H, L, h, l) \
    DEFINE_INSTRUCTION(POP_ ## H ## L ## _STEP1, \
        interpreter->l = interpreter->bus; \
        interpreter->cpuState.f.packed &= 0xF0; \
        interpreter->memIndex = interpreter->cpuState.sp; \
        interpreter->coreState = MEMORY_LOAD; \
        interpreter->instruction = LD_ ## H ## _BUS;)

#define DEFINE_POP_RR_INSTRUCTION(H, L, h, l) \
    DEFINE_POP_RR_STEP1(H, L, h, l); \
    DEFINE_INSTRUCTION(POP_ ## H ## L, \
        interpreter->memIndex = interpreter->cpuState.sp; \
        interpreter->cpuState.sp +=1 ; \
        interpreter->coreState = MEMORY_LOAD; \
        interpreter->instruction = POP_ ## H ## L ## _STEP1;)

// add
#define DEFINE_ADD_A_R_INSTRUCTION(NAME, OPERAND) \
    DEFINE_INSTRUCTION(ADD_A_ ## NAME, \
        int sum  = interpreter->cpuState.a + interpreter->OPERAND; \
        interpreter->cpuState.f.n = 0; \
        interpreter->cpuState.f.h = (interpreter->cpuState.a & 0x0F) + (interpreter->OPERAND & 0x0F) >= 0x10; \
        interpreter->cpuState.f.c = sum >= 0x100; \
        interpreter->cpuState.a = sum; \
        interpreter->cpuState.f.z = !interpreter->cpuState.a;)

#define DEFINE_ADD_A_N_INSTRUCTION \
    DEFINE_ADD_A_R_INSTRUCTION(N_STEP1, bus); \
    DEFINE_INSTRUCTION(ADD_A_N, \
        interpreter->coreState = READ_PC; \
        interpreter->instruction = ADD_A_N_STEP1;)

#define DEFINE_ADD_A_HL_INSTRUCTION \
    DEFINE_ADD_A_R_INSTRUCTION(HL_STEP1, bus); \
    DEFINE_INSTRUCTION(ADD_A_HL, \
        interpreter->coreState = MEMORY_LOAD; \
        interpreter->memIndex = interpreter->cpuState.hl; \
        interpreter->instruction = ADD_A_HL_STEP1;)

// adc
#define DEFINE_ADC_A_R_INSTRUCTION(NAME, OPERAND) \
    DEFINE_INSTRUCTION(ADC_A_ ## NAME, \
        int sum  = interpreter->cpuState.a + interpreter->OPERAND + interpreter->cpuState.f.c; \
        interpreter->cpuState.f.n = 0; \
        interpreter->cpuState.f.h = (interpreter->cpuState.a & 0x0F) + (interpreter->OPERAND & 0x0F) + interpreter->cpuState.f.c >= 0x10; \
        interpreter->cpuState.f.c = sum >= 0x100; \
        interpreter->cpuState.a = sum; \
        interpreter->cpuState.f.z = !interpreter->cpuState.a;)

#define DEFINE_ADC_A_N_INSTRUCTION \
    DEFINE_ADC_A_R_INSTRUCTION(N_STEP1, bus); \
    DEFINE_INSTRUCTION(ADC_A_N, \
        interpreter->coreState = READ_PC; \
        interpreter->instruction = ADC_A_N_STEP1;)

#define DEFINE_ADC_A_HL_INSTRUCTION \
    DEFINE_ADC_A_R_INSTRUCTION(HL_STEP1, bus); \
    DEFINE_INSTRUCTION(ADC_A_HL, \
        interpreter->coreState = MEMORY_LOAD; \
        interpreter->memIndex = interpreter->cpuState.hl; \
        interpreter->instruction = ADC_A_HL_STEP1;)

// sub
#define DEFINE_SUB_A_R_INSTRUCTION(NAME, OPERAND) \
    DEFINE_INSTRUCTION(SUB_A_ ## NAME, \
        int diff  = interpreter->cpuState.a - interpreter->OPERAND; \
        interpreter->cpuState.f.n = 1; \
        interpreter->cpuState.f.h = (interpreter->cpuState.a & 0x0F) - (interpreter->OPERAND & 0x0F) < 0; \
        interpreter->cpuState.f.c = diff < 0; \
        interpreter->cpuState.a = diff; \
        interpreter->cpuState.f.z = !interpreter->cpuState.a;)

#define DEFINE_SUB_A_N_INSTRUCTION \
    DEFINE_SUB_A_R_INSTRUCTION(N_STEP1, bus); \
    DEFINE_INSTRUCTION(SUB_A_N, \
        interpreter->coreState = READ_PC; \
        interpreter->instruction = SUB_A_N_STEP1;)

#define DEFINE_SUB_A_HL_INSTRUCTION \
    DEFINE_SUB_A_R_INSTRUCTION(HL_STEP1, bus); \
    DEFINE_INSTRUCTION(SUB_A_HL, \
        interpreter->coreState = MEMORY_LOAD; \
        interpreter->memIndex = interpreter->cpuState.hl; \
        interpreter->instruction = SUB_A_HL_STEP1;)

// sbc
#define DEFINE_SBC_A_R_INSTRUCTION(NAME, OPERAND) \
    DEFINE_INSTRUCTION(SBC_A_ ## NAME, \
        int diff  = interpreter->cpuState.a - interpreter->OPERAND - interpreter->cpuState.f.c; \
        interpreter->cpuState.f.n = 1; \
        interpreter->cpuState.f.h = (interpreter->cpuState.a & 0x0F) - (interpreter->OPERAND & 0x0F) - interpreter->cpuState.f.c < 0; \
        interpreter->cpuState.f.c = diff < 0; \
        interpreter->cpuState.a = diff; \
        interpreter->cpuState.f.z = !interpreter->cpuState.a;)

#define DEFINE_SBC_A_N_INSTRUCTION \
    DEFINE_SBC_A_R_INSTRUCTION(N_STEP1, bus); \
    DEFINE_INSTRUCTION(SBC_A_N, \
        interpreter->coreState = READ_PC; \
        interpreter->instruction = SBC_A_N_STEP1;)

#define DEFINE_SBC_A_HL_INSTRUCTION \
    DEFINE_SBC_A_R_INSTRUCTION(HL_STEP1, bus); \
    DEFINE_INSTRUCTION(SBC_A_HL, \
        interpreter->coreState = MEMORY_LOAD; \
        interpreter->memIndex = interpreter->cpuState.hl; \
        interpreter->instruction = SBC_A_HL_STEP1;)

// jmp
// jp nn
#define DEFIN_JP_NN_STEP2 \
    DEFINE_INSTRUCTION(JP_NN_STEP2, \
        interpreter->temp |= interpreter->bus << 8; \
        interpreter->cpuState.pc = interpreter->temp; \
        interpreter->coreState = STALL;)

#define DEFINE_JP_NN_STEP1 \
    DEFINE_INSTRUCTION(JP_NN_STEP1, \
        interpreter->coreState = READ_PC; \
        interpreter->temp = interpreter->bus; \
        interpreter->instruction = JP_NN_STEP2;)

#define DEFINE_JP_NN_INSTRUCTION \
    DEFIN_JP_NN_STEP2; \
    DEFINE_JP_NN_STEP1; \
    DEFINE_INSTRUCTION(JP_NN, \
        interpreter->coreState = READ_PC; \
        interpreter->instruction = JP_NN_STEP1;)

// jp HL
#define DEFINE_JP_HL_INSTRUCTION \
    DEFINE_INSTRUCTION(JP_HL, \
        interpreter->cpuState.pc = interpreter->cpuState.hl;)

// jp cc,nn
#define DEFINE_JP_CC_NN_STEP2(CONDITION, VALUE) \
    DEFINE_INSTRUCTION(JP_ ## CONDITION ## _NN_STEP2, \
        interpreter->temp |= interpreter->bus << 8; \
        if(VALUE) { \
            interpreter->cpuState.pc = interpreter->temp; \
            interpreter->coreState = STALL; \
        })

#define DEFINE_JP_NN_CC_STEP1(CONDITION) \
    DEFINE_INSTRUCTION(JP_ ## CONDITION ## _NN_STEP1, \
        interpreter->coreState = READ_PC; \
        interpreter->temp = interpreter->bus; \
        interpreter->instruction = JP_ ## CONDITION ## _NN_STEP2;)

#define DEFINE_JP_NN_CC_INSTRUCTION(CONDITION, VALUE) \
    DEFINE_JP_CC_NN_STEP2(CONDITION, VALUE); \
    DEFINE_JP_NN_CC_STEP1(CONDITION); \
    DEFINE_INSTRUCTION(JP_ ## CONDITION ## _NN, \
        interpreter->coreState = READ_PC; \
        interpreter->instruction = JP_ ## CONDITION ## _NN_STEP1;)

#define DEFINE_JP_NN_CONDITIONAL_INSTRUCTION \
    DEFINE_JP_NN_CC_INSTRUCTION(Z, interpreter->cpuState.f.z); \
    DEFINE_JP_NN_CC_INSTRUCTION(NZ, !interpreter->cpuState.f.z); \
    DEFINE_JP_NN_CC_INSTRUCTION(C, interpreter->cpuState.f.c); \
    DEFINE_JP_NN_CC_INSTRUCTION(NC, !interpreter->cpuState.f.c);

// jr nn
#define DEFINE_JR_NN_STEP1 \
    DEFINE_INSTRUCTION(JR_NN_STEP1, \
        interpreter->cpuState.pc += interpreter->bus; \
        interpreter->coreState = STALL;)

#define DEFINE_JR_NN_INSTRUCTION \
    DEFINE_JR_NN_STEP1; \
    DEFINE_INSTRUCTION(JR_NN, \
        interpreter->coreState = READ_PC; \
        interpreter->instruction = JR_NN_STEP1;)

// jr cc,nn
#define DEFINE_JR_CC_NN_STEP1(CONDITION, CHECK) \
    DEFINE_INSTRUCTION(JR_ ## CONDITION ## _NN_STEP1, \
        if(CHECK) { \
            interpreter->cpuState.pc += interpreter->bus; \
            interpreter->coreState = STALL; \
        })

#define DEFINE_JR_CC_NN_INSTRUCTION(CONDITION, CHECK) \
    DEFINE_JR_CC_NN_STEP1(CONDITION, CHECK); \
    DEFINE_INSTRUCTION(JR_ ## CONDITION ## _NN, \
        interpreter->coreState = READ_PC; \
        interpreter->instruction = JR_ ## CONDITION ## _NN_STEP1;)

#define DEFINE_JR_CONDITIONAL_NN_INSTRUCTION \
    DEFINE_JR_CC_NN_INSTRUCTION(Z, interpreter->cpuState.f.z); \
    DEFINE_JR_CC_NN_INSTRUCTION(NZ, !interpreter->cpuState.f.z); \
    DEFINE_JR_CC_NN_INSTRUCTION(C, interpreter->cpuState.f.c); \
    DEFINE_JR_CC_NN_INSTRUCTION(NC, !interpreter->cpuState.f.c);

// call nn
#define DEFINE_CALL_NN_STEP3 \
    DEFINE_INSTRUCTION(CALL_NN_STEP4, interpreter->coreState = STALL;) \
    DEFINE_INSTRUCTION(CALL_NN_STEP3, \
        interpreter->cpuState.sp --; \
        interpreter->memIndex = interpreter->cpuState.sp; \
        interpreter->coreState = MEMORY_STORE; \
        interpreter->bus = interpreter->cpuState.pc; \
        interpreter->cpuState.pc = interpreter->temp; \
        interpreter->instruction = CALL_NN_STEP4;)

#define DEFINE_CALL_NN_STEP2 \
    DEFINE_INSTRUCTION(CALL_NN_STEP2, \
        interpreter->temp |= interpreter->bus << 8; \
        interpreter->coreState = MEMORY_STORE; \
        interpreter->bus = interpreter->cpuState.pc >> 8; \
        interpreter->cpuState.sp --; \
        interpreter->memIndex = interpreter->cpuState.sp; \
        interpreter->instruction = CALL_NN_STEP3;)

#define DEFINE_CALL_NN_STEP1 \
    DEFINE_INSTRUCTION(CALL_NN_STEP1, \
        interpreter->temp = interpreter->bus; \
        interpreter->coreState = READ_PC; \
        interpreter->instruction = CALL_NN_STEP2;)

#define DEFINE_CALL_NN_INSTRUCTION \
    DEFINE_CALL_NN_STEP3 \
    DEFINE_CALL_NN_STEP2 \
    DEFINE_CALL_NN_STEP1 \
    DEFINE_INSTRUCTION(CALL_NN, \
        interpreter->coreState = READ_PC; \
        interpreter->instruction = CALL_NN_STEP1;)

// call cc,nn
#define DEFINE_CALL_CC_NN_STEP3 \
    DEFINE_INSTRUCTION(CALL_CC_NN_STEP3, \
        interpreter->cpuState.sp --; \
        interpreter->memIndex = interpreter->cpuState.sp; \
        interpreter->coreState = MEMORY_STORE; \
        interpreter->bus = interpreter->cpuState.pc; \
        interpreter->cpuState.pc = interpreter->temp; \
        interpreter->instruction = CALL_CC_NN_STEP4;)

#define DEFINE_CALL_CC_NN_STEP2(CONDITION, CHECK) \
    DEFINE_INSTRUCTION(CALL_ ## CONDITION ## _NN_STEP2, \
        interpreter->temp |= interpreter->bus << 8; \
        if(CHECK) { \
            interpreter->coreState = MEMORY_STORE; \
            interpreter->bus = interpreter->cpuState.pc >> 8; \
            interpreter->cpuState.sp --; \
            interpreter->memIndex = interpreter->cpuState.sp; \
            interpreter->instruction = CALL_CC_NN_STEP3; \
        })

#define DEFINE_CALL_CC_NN_STEP1(CONDITION) \
    DEFINE_INSTRUCTION(CALL_ ## CONDITION ## _NN_STEP1, \
        interpreter->temp = interpreter->bus; \
        interpreter->coreState = READ_PC; \
        interpreter->instruction = CALL_ ## CONDITION ## _NN_STEP2;)

#define DEFINE_CALL_CC_NN_INSTRUCTION(CONDITION, CHECK) \
    DEFINE_CALL_CC_NN_STEP2(CONDITION, CHECK); \
    DEFINE_CALL_CC_NN_STEP1(CONDITION); \
    DEFINE_INSTRUCTION(CALL_ ## CONDITION ## _NN, \
        interpreter->coreState = READ_PC; \
        interpreter->instruction = CALL_ ## CONDITION ## _NN_STEP1;)

#define DEFINE_CALL_CONDITIONAL_NN_INSTRUCTION \
    DEFINE_INSTRUCTION(CALL_CC_NN_STEP4, interpreter->coreState = STALL;) \
    DEFINE_CALL_CC_NN_STEP3; \
    DEFINE_CALL_CC_NN_INSTRUCTION(Z, interpreter->cpuState.f.z);

// ret
#define DEFINE_RET_STEP2 \
    DEFINE_INSTRUCTION(RET_STEP1, \
        interpreter->cpuState.pc |= interpreter->bus << 8; \
        interpreter->memIndex = interpreter->cpuState.sp; \
        interpreter->coreSate = STALL;)

#define DEFINE_RET_STEP1 \
    DEFINE_INSTRUCTION(RET_STEP1, \
        interpreter->cpuState.pc = interpreter->bus; \
        interpreter->memIndex = interpreter->cpuState.sp; \
        interpreter->cpuState.sp ++; \
        interpreter->coreState = MEMORY_LOAD; \
        interpreter->instruction = RET_STEP2;)

#define DEFINE_RET_INSTRUCTION \
    DEFINE_INSTRUCTION(RET, \
        interpreter->memIndex = interpreter->cpuState.sp; \
        interpreter->cpuState.sp ++; \
        interpreter->coreState = MEMORY_LOAD; \
        interpreter->instruction = RET_STEP1;)

// ret cc

