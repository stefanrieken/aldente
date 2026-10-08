#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "pasta.h"

intptr_t * argstack;
int argstack_len;
int argstack_size;

#define push_arg(n) argstack[argstack_len++] = n

uint8_t * code;
int pc;
int code_size;

#define fetch_next() code[pc++]


#define N_GROUPS 2
PrimGroupCb prim_group_cb[N_GROUPS] = {
    NULL, // well, we could enlist the core function from below
    int_prim_group_cb
};

// ...groups of lists of strings...
char ** prim_names[N_GROUPS] = {
    NULL, // core commands have no source code representation
    int_prim_names
};

Bytecode bytecode_for_primitive(char * name) {
    for (int group = 1; group< N_GROUPS; group++) {
        int prim=0;
        while (prim_names[group][prim] != NULL) {
            if(strcmp(prim_names[group][prim], name) == 0) return prim | (group << 5);
            prim++;
        }
    }
    return 0; // since first group is core prims, zero is a good null value
}

int main (int argc, char ** argv) {
    // Test expression: * 7 (* 3 2)
    uint8_t foo[] = { bytecode_for_primitive("*"), PUSHB, 7, bytecode_for_primitive("*"), PUSHB, 3, PUSHB, 2, EVAL3, PUSH_RESULT, EVAL, 3, DONE };
    code = foo;
    code_size = 13;
    pc = 0;
    argstack = malloc(sizeof(intptr_t) * 1024);
    argstack_size = 1024;
    argstack_len = 0;

    run_code();
    printf("Result: %d\n", argstack[--argstack_len]);
}
