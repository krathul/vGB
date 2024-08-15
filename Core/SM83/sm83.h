#ifndef SM83_H
#define SM83_H

#include <stdint.h>

union FlagRegister {
    struct {
        unsigned unused : 4;
        unsigned c : 1;
        unsigned h : 1;
        unsigned n : 1;
        unsigned z : 1;
    };

    uint8_t packed;
};

#define SM83_AF_REGISTER union { \
                    struct { \
                        union FlagRegister f; \
                        uint8_t a; \
                    }; \
                    uint16_t af; \
                }

#define SM83_REGISTER_PAIR(HIGH, LOW) union { \
                    struct { \
                        uint8_t LOW; \
                        uint8_t HIGH; \
                    }; \
                    uint16_t HIGH ## LOW; \
                }

struct SM83State {
  struct {
        SM83_AF_REGISTER;
        SM83_REGISTER_PAIR(b, c);
        SM83_REGISTER_PAIR(d, e);
        SM83_REGISTER_PAIR(h, l);
        uint16_t sp;
        uint16_t pc;
    };

  void (*StateInit)(struct SM83State* cpuState);
  void (*StateReset)(struct SM83State* cpuState);
};

void sm83StateReset(struct SM83State*);
void sm83StateInit(struct SM83State*);

#endif
