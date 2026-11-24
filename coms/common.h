#ifndef TOKEN_H
#define TOKEN_H

#include <stdbool.h>

extern char *ALPHA;
extern char *TERM;
extern char *MODE;

extern char *type_names[];
extern char *stack_type_names[];
extern char *utypes[];
extern char *cmdt_types[];
extern char *isn_types[];
extern char *data_types[];
extern char *mem_types[];

enum _UTYPE_
{
    NODE, NODES
};

enum _COMMAND_
{
    _ADD, _SUB, _MUL, _DIV,
    
    _PUSH, _CALL, _STO, _ID,
    
    _LITERAL, _ID_PUSH, _LOAD,
    
    _SET, _PUSH_KEYL, _HASH_SEQ, _END_HASH_SEQ,
    
    _HASH, _LOAD_SEQ,
    
    SPLIT
};


enum _TYPE_ 
{
    LPAREN, RPAREN, LBRACE, RBRACE,
    LBRACKET, RBRACKET, COMMA, DECIMAL, 
    COLON, SCOLON, ADD, SUB, STAR, DIV, 
    EQUAL, POWER, GTHAN, LTHAN,
    
    AND, OR, NOT, GOTO, UNTIL, IF, ELSE,
    RETURN,
    
    NUMBER, STRING, DEF_FN, DEF_CLS,
    
    EQUIVALENT, GTHAN_OR_EQUAL_TO,
    LTHAN_OR_EQUAL_TO, NOT_EQUAL_TO,
    INHERIT, METADATA, INIT,
    
    ADD_EQ, SUB_EQ, STAR_EQ, DIV_EQ,
    
    IDENTIFIER, COMMENT,
    
    PROGRAM, MEMORY, EXPRESSION, ASSIGNMENT, 
    FUNCTION_CALL, ARGUMENTS, ID_LINKER, FRACTIONAL,
    SCOPE, STATEMENT, CONDITION, GTOET, LTOET,
    LOOP, LIST, LIST_ACC, OPERATION, CLASS, INHERITS,
    META_SCOPE
};

enum _DATATYPE_
{
    _INT_, _STRING_, _FLOAT_
};

enum _MEMTYPE_
{
    STACK, HASH, KEYL
};

typedef struct Token
{
    enum _TYPE_ type;
    char *stor;
    int len;
    int line;
    
} Token;


typedef struct List
{
    void *element;
    int length;
    
} List;

typedef struct TAG
{
    enum _UTYPE_ utype;
    union
    {
        struct
        {
            enum _TYPE_ type;
            char *stor;
        } Node;
        
        struct
        {
            enum _TYPE_ type;
            struct TAG *nodes;
            int nodeln;
        } Nodes;
        
    } UNION;
    
} NodeMaster;

typedef struct
{
    enum _MEMTYPE_ memt;
    union
    {
        int index;
        char *key;
        int keylistid;
    } loc;
    
} MEM;

typedef struct 
{
    enum _COMMAND_ cmdt;
    char *val;
    
} CMD;

typedef struct
{
    enum _DATATYPE_ datat;
    void *val;
    
} CON;

enum _ISNTYPE_
{
    __CMD, __MEM, __CON
};

typedef struct
{
    enum _ISNTYPE_ isnt;
    union
    {
        CMD cmd;
        MEM mem;
        CON con;
    } data;
    
} Instruction;

typedef struct
{
    Instruction *stream;
    int streamln;
    size_t streamsz;
    
} ByteStream;

typedef struct
{
    enum _TYPE_ *types;
    int ln;
    size_t size;
    
} Genmem;


typedef struct
{
    enum _DATATYPE_ type;
    union
    {
        char *string;
        int inum;
        float fnum;
    } data;
    
} Casted;

void debugp(char *s);
bool is_in(char *string, char c, int string_length);
bool only_numbers(char *string, int string_length);
enum _DATATYPE_ is_int_float(char *string);
enum _DATATYPE_ type(char *s);

int cast_to_int(char *string, int stop_id);
int special_cast_to_int(char *string, int stop_id);
float cast_to_float(char *string);
Casted convert(char *string, enum _DATATYPE_ type);

#endif