#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "pasta.h"

intptr_t * argstack;
int argstack_len;
int argstack_size;

#define push_arg(n) argstack[argstack_len++] = n

Bytecode * code;
int pc;
int code_len;
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

// I guess signed ints should work as follows:
// 1. Allow for a relatively short representation of e.g. -1, at the cost of, say, 255 or 65535
// 2. Perform sign extension on arg expansion
// 3. Will code ever contain an intptr_t sized int literal or direct pointer?
void add_cmd(Bytecode cmd, intptr_t arg, int arg_size) {
    if (code_len+arg_size+1 > code_size) { code_size += 256; code = realloc(code, code_size); }
    code[code_len++] = cmd;
    while(arg_size > 0) {
        code[code_len++] = arg & 0xFF; // little endian
        arg = arg >> 8;
        arg_size--;
    }
}

void void_cmd(Bytecode cmd) {
    add_cmd(cmd, 0, 0);
}


void byte_cmd(Bytecode cmd, int8_t arg) {
    add_cmd(cmd, arg, 1);
}

void short_cmd(Bytecode cmd, int16_t arg) {
    add_cmd(cmd, arg, 2);
}

// Store a 24-bit literal, save a byte :)
// This keeps unsigned 16-bit values like 65535 from turning into word size
// Of course this requires dedicated byte codes to be useful
void long_cmd(Bytecode cmd, int16_t arg) {
    add_cmd(cmd, arg, 2);
}

// For those days where you just want to push a pointer
void word_cmd(Bytecode cmd, intptr_t arg) {
    add_cmd(cmd, arg, sizeof(intptr_t));
}

int main (int argc, char ** argv) {
    code = NULL;
    code_size = 0;
    code_len = 0;

    // Test expression: * 7 (* 3 2)
    void_cmd(bytecode_for_primitive("*"));
    byte_cmd(PUSHB, 7);
    void_cmd(bytecode_for_primitive("*"));
    byte_cmd(PUSHB, 3);
    byte_cmd(PUSHB, 2);
    void_cmd(EVAL3);
    void_cmd(PUSH_RESULT);
    byte_cmd(EVAL, 3); // variation from before
    void_cmd(DONE);

    printf("Code len: %d\n", code_len);

    pc = 0;
    argstack = malloc(sizeof(intptr_t) * 1024);
    argstack_size = 1024;
    argstack_len = 0;

    run_code();
    printf("Result: %d\n", argstack[--argstack_len]);
}
