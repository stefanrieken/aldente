
// Central type definitions for flexibility of future redefinition:

typedef char * UniqueString, * HeapString;
typedef intptr_t SInt;
typedef uintptr_t UInt;

// As long as we retain variable names on the stack,
// we might as well keep this housekeeping header too.
// If nothing else it guards against reallocations.
// Access via functions / macros for if we ever expect it
// to be replaced by a perfectly predictable array
typedef struct {
    int size;
    int len;
    struct Variable * vars;
} Frame, * FrameRef; // in case we ever use something other than a pointer

enum TypeTag {
    TT_NONE,
    TT_SINT,
    TT_UINT,
    TT_BLOCK, // (also) for closures
    TT_USTR,
    // heap allocated types:
    TT_HSTR,
    TT_STRUCT
};

typedef struct Variable {
    UniqueString name;

    // 'Normally' the tag is part of the value, but that creates its own set of
    // challenges. To track it separately is expensive. Compile time analysis
    // can help; or even be the whole goal of this field.
    // For now we only really need to know the type of a struct anonymous object,
    // but as with closures, we can derive that from an (impossible) unique name.
/*
    // Debatable whether we will ever end adding a type.class here,
    // whether as a function or struct. Still, type.tag looks neat.
    union {
        TypeTag tag;
    } type;
*/
    union {
        SInt i;
        UInt u;
        UniqueString str;
        HeapString txt;
        FrameRef frame;
    } val;
} Variable;
