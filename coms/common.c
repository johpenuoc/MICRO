#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

char *ALPHA = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_\"!=+-/*^&|@~";
char *TERM = "(){}[],.:;+-*/=^><!?";
char *MODE = "\"#";

enum _UTYPE_
{
    NODE, NODES
};
const char *utypes[] = {
    "NODE", "NODES"
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
const char *cmdt_types[] = {
    "ADD", "SUB", "MUL", "DIV",
    
    "PUSH", "CALL", "STO", "ID",
    
    "LITERAL", "ID_PUSH", "LOAD",
    
    "SET", "PUSH_KEYL", "HASH_SEQ", "_END_HASH_SEQ",
    
    "HASH", "LOAD_SEQ",
    
    "SPLIT"
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
const char *type_names[] = {
    "LPAREN", "RPAREN", "LBRACE", "RBRACE",
    "LBRACKET", "RBRACKET", "COMMA", "DECIMAL", 
    "COLON", "SCOLON", "ADD", "SUB", "STAR", "DIV", 
    "EQUAL", "POWER", "GTHAN", "LTHAN",
    
    "AND", "OR", "NOT", "GOTO", "UNTIL", "IF", "ELSE",
    "RETURN",
    
    "NUMBER", "STRING", "DEF_FN", "DEF_CLS",
    
    "EQUIVALENT", "GTHAN_OR_EQUAL_TO",
    "LTHAN_OR_EQUAL_TO", "NOT_EQUAL_TO",
    "INHERIT", "METADATA", "INIT",
    
    "ADD_EQ", "SUB_EQ", "STAR_EQ", "DIV_EQ",
    
    "IDENTIFIER", "COMMENT",
    
    "PROGRAM", "MEMORY", "EXPRESSION", "ASSIGNMENT", 
    "FUNCTION_CALL", "ARGUMENTS", "ID_LINKER", "FRACTIONAL",
    "SCOPE", "STATEMENT", "CONDITION", "GTOET", "LTOET",
    "LOOP", "LIST", "LIST_ACC", "OPERATION", "CLASS", "INHERITS",
    "META_SCOPE"

};


enum _DATATYPE_
{
    _INT_, _STRING_, _FLOAT_
};
const char *data_types[] = {
    "INT", "STRING", "FLOAT"
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

// ==============================
// ==============================

// a nodemaster can either be a single node --> [Node] at index one
// or it can hold multiple nodes --> [[Node][[Node_1][Node_2]][Node2]]

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

/*typedef struct
{
    enum _DATATYPE_ dtype;
    union
    {
        char *string;
        int *inum;
        double *fnum;
    } val;
    
} ARG;

typedef struct 
{
    enum _COMMAND_ cmdtype;
    Arg *args;
    int argln;
    size_t argsz;
} CMD;*/


// ===================================

enum _MEMTYPE_
{
    STACK, HASH, KEYL
};
const char *mem_types[] = {
    "STACK", "HASH", "KEYL"
};



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


// CON means 'context'
// like giving a number, 5,
// and saying 'INT' after
enum _ISNTYPE_
{
    __CMD, __MEM, __CON
};
const char *isn_types[] = {
    "__CMD", "__MEM", "__CON"
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

typedef struct __TAG
{
    // this one is only for the main stream. 
    // its only a '2d array'
    struct __TAG *bstream;
    
    Instruction *stream;
    int streamln;
    size_t streamsz;
    
} ByteStream;

// ===================================

// this is a general memory stack
// its going to be used to simply 
// hold certain variables and 'stors'
// from different nodes
typedef struct
{
    enum _TYPE_ *types;
    int ln;
    size_t size;
    
} Genmem;

// ==============================
// ==============================

void debugp(char *s)
{
    printf("DEBUG [%s]\n", s);
}

bool is_in(char *string, char c, int string_length)
{
    for (int i = 0; i < string_length; ++i)
        {
            if (string[i] == c)
                {
                    return true;
                }
        }
    
    return false;
}

bool only_numbers(char *string, int string_length)
{
    // forgot why i even put this here...
    // i should comment wayyyy more often
    //if (string[string_length - 1] == '.') return false;
    
    // this wont stop floats because '.' isnt included in ALPHA
    for (int i = 0; i < string_length; ++i)
        {
            if (is_in(ALPHA, string[i], strlen(ALPHA)))
                {
                    return false;
                }
        }
    
    return true;
}

enum _DATATYPE_ is_int_float(char *string)
{
    int sln = strlen(string);    
    bool possible_float = false;
    for (int i = 0; i < sln; ++i)
        {
            if (is_in(ALPHA, string[i], strlen(ALPHA)))
                {
                    return _STRING_;
                }
            
            if (string[i] == '.') possible_float = true;
        }
        
    if (possible_float) return _FLOAT_;
    else return _INT_;
}

enum _DATATYPE_ type(char *s)
{
    return is_int_float(s);
}

int cast_to_int(char *string, int stop_id)
{
    int num = 0;
    int sln = strlen(string);
    for (int i = 0; i < sln - stop_id; ++i)
        {
            // 506
            /*
            6 * (10^1) = 6
            0 * (10^2) = 0 -- 106 --> 1 * (10^2) --> 106
            */
            int pos = (int)round(pow(10, i));
            int zero_tracker = 0;
            switch (string[sln - i - 1])
                {
                    case '0':
                        num += 1 * pos;
                        zero_tracker += 1 * pos;
                        break;
                    case '1':
                        num += (1 * pos) - zero_tracker;
                        break;
                    case '2':
                        num += (2 * pos) - zero_tracker;
                        break;
                    case '3':
                        num += (3 * pos) - zero_tracker;
                        break;
                    case '4':
                        num += (4 * pos) - zero_tracker;
                        break;
                    case '5':
                        num += (5 * pos) - zero_tracker;
                        break;
                    case '6':
                        num += (6 * pos) - zero_tracker;
                        break;
                    case '7':
                        num += (7 * pos) - zero_tracker;
                        break;
                    case '8':
                        num += (8 * pos) - zero_tracker;
                        break;
                    case '9':
                        num += (9 * pos) - zero_tracker;
                        break;
                    default:
                        perror("Value is not a number");
                        exit(EXIT_FAILURE);
                        return num;                           
                }
        }
        
    return num;
}
int special_cast_to_int(char *string, int stop_id)
{
    int num = 0;
    int sln = strlen(string);
    for (int i = stop_id; i < sln; ++i)
        {
            // 506
            /*
            6 * (10^1) = 6
            0 * (10^2) = 0 -- 106 --> 1 * (10^2) --> 106
            */
            int pos = (int)round(pow(10, i));
            int zero_tracker = 0;
            switch (string[sln - i - 1])
                {
                    case '0':
                        num += 1 * pos;
                        zero_tracker += 1 * pos;
                        break;
                    case '1':
                        num += (1 * pos) - zero_tracker;
                        break;
                    case '2':
                        num += (2 * pos) - zero_tracker;
                        break;
                    case '3':
                        num += (3 * pos) - zero_tracker;
                        break;
                    case '4':
                        num += (4 * pos) - zero_tracker;
                        break;
                    case '5':
                        num += (5 * pos) - zero_tracker;
                        break;
                    case '6':
                        num += (6 * pos) - zero_tracker;
                        break;
                    case '7':
                        num += (7 * pos) - zero_tracker;
                        break;
                    case '8':
                        num += (8 * pos) - zero_tracker;
                        break;
                    case '9':
                        num += (9 * pos) - zero_tracker;
                        break;
                    default:
                        perror("Value is not a number");
                        exit(EXIT_FAILURE);
                        return num;                           
                }
        }
        
    return num;
}

float cast_to_float(char *string)
{
    char dot_id = 0;
    for (int i = 0; i < strlen(string); ++i)
        {
            if (string[i] == '.')
                {
                    dot_id = i + 1;
                    break;
                }
        }
    
    int ten_power = 10;
    int num = cast_to_int(string, dot_id);
    if (num > 9 && num < 100) ten_power = 100;
    else if (num > 99 && num < 1000) ten_power = 1000;
    else if (num > 999 & num < 10000) ten_power = 10000;
    float _num = (float)num / (float)ten_power;
    float __num = (float)special_cast_to_int(string, dot_id + 1);
    
    return __num + _num;
}

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

Casted convert(char *string, enum _DATATYPE_ type)
{
    Casted cast;
    cast.type = type;
    switch (type)
        {
            case _INT_:
                cast.data.inum = cast_to_int(string, 0);
                return cast;
            case _FLOAT_:
                cast.data.fnum = cast_to_float(string);
                return cast;
            case _STRING_:
                cast.data.string = string;
                return cast;
        }
}
