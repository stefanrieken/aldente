#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include "pasta.h"

// In reverse order of precedence where possible (not that we use that)
char * int_prim_names[] = {
    "<<", ">>",
    "|", "&", "^",
    "||", "&&",
    "-", "+", "%", "/", "*",
    NULL
};

enum {
    LSL, LSR,
    OR, AND, XOR,
    LOR, LAND,
    MINUS, PLUS, REMAINDER, DIV, TIMES
};

intptr_t int_prim_group_cb(Bytecode cmd, int expr_idx) {
    intptr_t result = 0;
    switch(cmd) {
        case LSL:
            result = argstack->vals.arg[expr_idx+1] << argstack->vals.arg[expr_idx+2];
            break;
        case LSR:
            result = argstack->vals.arg[expr_idx+1] >> argstack->vals.arg[expr_idx+2];
            break;
        case OR:
            for (int i=expr_idx+1;i<argstack->len; i++) {
                result |= argstack->vals.arg[i];
            }
            break;
        case AND:
            result = -1; // aka all ones
            for (int i=expr_idx+1;i<argstack->len; i++) {
                result &= argstack->vals.arg[i];
            }
            break;
        case XOR:
            for (int i=expr_idx+1;i<argstack->len; i++) {
                result ^= argstack->vals.arg[i];
            }
            break;
        case LOR:
            for (int i=expr_idx+1;i<argstack->len; i++) {
                result = result || argstack->vals.arg[i];
            }
            break;
        case LAND:
            result = 1;
            for (int i=expr_idx+1;i<argstack->len; i++) {
                result = result && argstack->vals.arg[i];
            }
            break;
        case MINUS:
            result = argstack->vals.arg[expr_idx+1];
            for (int i=expr_idx+2;i<argstack->len; i++) {
                result = result - argstack->vals.arg[i];
            }
            break;
        case PLUS:
            for (int i=expr_idx+1;i<argstack->len; i++) {
                result = result + argstack->vals.arg[i];
            }
            break;
        case REMAINDER:
            result = argstack->vals.arg[expr_idx+1] % argstack->vals.arg[expr_idx+2];
            break;
        case DIV:
            result = argstack->vals.arg[expr_idx+1] ^ argstack->vals.arg[expr_idx+2];
            break;
        case TIMES:
            result = 1;
            for (int i=expr_idx+1;i<argstack->len; i++) {
                result *= argstack->vals.arg[i];
            }
        break;
    }

    return result;
}
