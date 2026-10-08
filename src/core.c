#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "pasta.h"

#define push_arg(n) argstack[argstack_len++] = n

#define fetch_next() code[pc++]

intptr_t lookup(intptr_t x) { return x; } // TODO implement vars

void run_code() {
    int result = 0;
    int n;

    uint8_t cmd = code[pc++];
    while (cmd != DONE && pc <= code_len) {
        if (cmd < N_CMDS) {
            n = 0; // To aid the fall-throughs below
            switch(cmd) {
                // Big heap of overly clever fall-throughs:
                case PUSHW:
                    n += fetch_next() << 24;
                case PUSH3B:
                    n += fetch_next() << 16;
                case PUSH2B:
                    n += fetch_next() << 8;
                case PUSHB:
                    n += fetch_next();
                case PUSH0:
                    push_arg(n);
                    break;

                case PUSH1:
                    push_arg(1);
                    break;
                case PUSH_RESULT:
                    push_arg(result);
                    break;
                case REF:
                    push_arg(lookup(fetch_next()));
                    break;
                case SKIP:
                    n = fetch_next(); // TODO probably more than 8 bits?
                    pc += n; // TODO += or = ?
                    break;
                case EVAL1:
                case EVAL2:
                case EVAL3:
                case EVAL4:
                case EVAL5:
                case EVAL6: // there's still room for an EVAL7 if we want
                    n = cmd & 0b111; // and fall through:
                case EVAL:
                    if (n == 0) n = fetch_next();
                    int expr_idx = argstack_len-n;
                    int prim = argstack[expr_idx];
                    result = prim_group_cb[prim >> 5](prim & 0b00011111, expr_idx);
                    argstack_len = expr_idx; // reduce argstack after eval
                    break;
            }
        } else push_arg(cmd); // push idx not resolved pointer; this keeps argstack word size flexible
        cmd = code[pc++];
    }

    // Only push the last return value from an expression sequence
    push_arg(result);
}
