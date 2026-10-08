typedef unsigned char Bytecode;

struct Variable;

// One-for-all array type
typedef struct {
    int size;
    int len;
    union {
        Bytecode * code;
        char ** str;
        intptr_t * arg;
        struct Variable * var;
    } vals;
} Array;


extern Array * argstack;

typedef intptr_t (* PrimGroupCb)(Bytecode cmd, int expr_idx);
extern PrimGroupCb prim_group_cb[];

extern char * int_prim_names[];
intptr_t int_prim_group_cb(Bytecode cmd, int exp_idx);

typedef enum CoreCmd {
    PUSH0,
    EVAL1, // These must be properly 3-bit+1 aligned`
    EVAL2,
    EVAL3,
    EVAL4,
    EVAL5,
    EVAL6,
    PUSH1,
    PUSHB,
    PUSH2B,
    PUSH3B,
    PUSHW,
    PUSH_RESULT, // for subexprs
    REF,
    SKIP,
    EVAL,
    DONE, // at end of block, so reset varstack
    N_CMDS
} CoreCmd;

extern Array * code;
extern int pc;

void run_code();
