#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include "pasta.h"

typedef enum Primitive {
    PRIM_TIMES
} Primitive;

typedef intptr_t (* PrimCb)(int expr_idx);

intptr_t prim_times(int expr_idx) {
    intptr_t result = 1;
    for (int i=expr_idx+1;i<argstack_len; i++) {
        result *= argstack[i];
    }

    return result;
}


// In reverse order of precedence where possible (not that we use that)
char * int_prim_names[] = {
    "<<", ">>",
    "|", "&", "^",
    "||", "&&",
    "-", "+", "%", "/", "*",
    NULL
};

typedef enum {
    LSL, LSR,
    OR, AND, XOR,
    LOR, LAND,
    MINUS, PLUS, REMAINDER, DIV, TIMES
} IntCb;

intptr_t int_prim_group_cb(Bytecode cmd, int expr_idx) {
    intptr_t result = 0;
    switch(cmd) {
        case LSL:
            result = argstack[expr_idx+1] << argstack[expr_idx+2];
            break;
        case LSR:
            result = argstack[expr_idx+1] >> argstack[expr_idx+2];
            break;
        case OR:
            for (int i=expr_idx+1;i<argstack_len; i++) {
                result |= argstack[i];
            }
            break;
        case AND:
            result = -1; // aka all ones
            for (int i=expr_idx+1;i<argstack_len; i++) {
                result &= argstack[i];
            }
            break;
        case XOR:
            for (int i=expr_idx+1;i<argstack_len; i++) {
                result ^= argstack[i];
            }
            break;
        case LOR:
            for (int i=expr_idx+1;i<argstack_len; i++) {
                result = result || argstack[i];
            }
            break;
        case LAND:
            result = 1;
            for (int i=expr_idx+1;i<argstack_len; i++) {
                result = result && argstack[i];
            }
            break;
        case MINUS:
            result = argstack[expr_idx+1];
            for (int i=expr_idx+2;i<argstack_len; i++) {
                result = result - argstack[i];
            }
            break;
        case PLUS:
            for (int i=expr_idx+1;i<argstack_len; i++) {
                result = result + argstack[i];
            }
            break;
        case REMAINDER:
            result = argstack[expr_idx+1] % argstack[expr_idx+2];
            break;
        case DIV:
            result = argstack[expr_idx+1] ^ argstack[expr_idx+2];
            break;
        case TIMES:
            result = 1;
            for (int i=expr_idx+1;i<argstack_len; i++) {
                result *= argstack[i];
            }
        break;
    }

    return result;
}
