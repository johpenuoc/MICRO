#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "coms/common.h"

//STO , a , MUL , LOAD , y: MUL , FNCALL , x: LITERAL , 0.5:::

/*
STACK: [ASSIGNMENT]
                NODE: [EQUAL]-------->[=]
                NODE: [IDENTIFIER]--->[a]
                STACK: [EXPRESSION]
                        STACK: [OPERATION]
                                NODE: [STAR]--------->[*]
                                NODE: [IDENTIFIER]--->[y]
                                STACK: [EXPRESSION]
                                        STACK: [OPERATION]
                                                NODE: [STAR]--------->[*]
                                                STACK: [FUNCTION_CALL]
                                                        NODE: [IDENTIFIER]--->[func]
                                                        STACK: [ARGUMENTS]
                                                                STACK: [EXPRESSION]
                                                                        NODE: [IDENTIFIER]--->[x]
                                                NODE: [NUMBER]------->[0.5]
*/

/*void acmd(CMD **cmds, size_t *cmdsz, int *cmdln, CMD obj)
{
    (*cmdsz) += sizeof(obj);
    CMD *tmp = realloc(*cmds, *cmdsz);
    *cmds = tmp;
    (*cmds)[*cmdln] = obj;
    (*cmdln)++;
    
    //printf("%s -> ", cmdt_names[cmds[(*cmdln) - 1].cmdt]);
}

CMD spawn(enum _COMMAND_ cmdtype, int _argln, char *token)
{
    /*CMD cmd;
    cmd.cmdtype = _cmdtype;
    *cmd.argln = 0;
    *cmd.argsz = 0;
    cmd.args = NULL;
    
    CMD cmd;
    cmd.cmdt = cmdtype;
    cmd.argln = _argln;
    cmd.val = malloc(strlen(token) + 1);
    strcpy(cmd.val, token);
    
    return cmd;
}

/*CMD spawn_splitter()
{
    CMD split;
    split.cmdt = SPLIT;
    split.argln = 0;
    split.val = ":";
    
    return split;
}*/

void astream(ByteStream *bs, Instruction isn);

Instruction CMDcreate_isn(enum _COMMAND_ cmdtype, char *_val)
{
    Instruction isn;
    isn.isnt = __CMD;
    isn.data.cmd = (CMD) 
        {
           .cmdt =  cmdtype,
           .val = _val
        };
    
    return isn;
        
}
Instruction MEMcreate_isn(enum _MEMTYPE_ memtype, void *_index)
{
    Instruction isn;
    isn.isnt = __MEM;
    isn.data.mem.memt = memtype;
    if (memtype == STACK) isn.data.mem.loc.index = *((int *)_index);
    else isn.data.mem.loc.key = (char *)_index;
    
    return isn;
}
Instruction CONcreate_isn(enum _DATATYPE_ datatype)
{
    Instruction isn;
    isn.isnt = __CON;
    isn.data.con.datat = datatype;
    
    return isn;
}

void aministream(ByteStream *bs, Instruction *isns, int isnsln)
{
    for (int i = 0; i < isnsln - 1; ++i)
        {
            astream(bs, isns[i]);
        }
}

void astream(ByteStream *bs, Instruction isn)
{
    bs->streamsz += sizeof(isn);
    bs->stream = realloc(bs->stream, bs->streamsz);
    bs->stream[bs->streamln] = isn;
    (bs->streamln)++;
}

void aastream(ByteStream *bs, ByteStream isn)
{
    bs->streamsz += sizeof(isn);
    bs->bstream = realloc(bs->bstream, bs->streamsz);
    bs->bstream[bs->streamln] = isn;
    (bs->streamln)++;
}

//CMD* cmds, size_t *cmdsz, int *cmdln
void traverse(NodeMaster *stack, enum _TYPE_ ptype, ByteStream *bs)
{
    if (stack->utype == NODE)
        {
            const enum _TYPE_ STYPE = stack->UNION.Node.type;
            char *val = stack->UNION.Node.stor;
            enum _COMMAND_ ctype;
            
            //printf("%s\n", type_names[STYPE]);
            switch (STYPE)
                {
                    case EQUAL:
                        {
                            ctype = _STO;
                            
                            //_STO ID[top], mem[top]
                            
                            // similar to the mstack and mini branches
                            ByteStream ministream;
                            ministream.stream = NULL;
                            ministream.streamsz = 0;
                            ministream.streamln = 0;
                            
                            Instruction opcode = CMDcreate_isn(_STO, "");
                            astream(&ministream, opcode);
                            int p = 4;
                            Instruction memloc1 = MEMcreate_isn(STACK, (void *)&p);
                            astream(&ministream, memloc1);
                            int p2 = 7;
                            Instruction memloc2 = MEMcreate_isn(STACK, (void *)&p2);
                            astream(&ministream, memloc2);
                            
                            // create complete instruction
                            // by appending them to the stream
                            aastream(bs, ministream);
                        }
                        break;
                    case IDENTIFIER:
                        {
                            if (ptype == FUNCTION_CALL) 
                                {
                                   ctype = _CALL; 
                                   
                                   //_CALL hashmap[FUNC_ID%d]
                                   
                                   // similar to the mstack and mini branches
                                   ByteStream ministream;
                                   ministream.stream = NULL;
                                   ministream.streamsz = 0;
                                   ministream.streamln = 0;
                            
                                   Instruction opcode = CMDcreate_isn(_CALL, "");
                                   astream(&ministream, opcode);
                                   char *p = "FUNC";
                                   Instruction memloc = MEMcreate_isn(HASHMAP, (void *)p);
                                   astream(&ministream, memloc);
                                   
                                   aastream(bs, ministream);
                                }
                            else 
                                {
                                    ctype = _ID;
                                    
                                    //_ID_PUSH _ID
                                    
                                    // similar to the mstack and mini branches
                                    ByteStream ministream;
                                    ministream.stream = NULL;
                                    ministream.streamsz = 0;
                                    ministream.streamln = 0;
                                    
                                    Instruction opcode = CMDcreate_isn(_ID_PUSH, "");
                                    astream(&ministream, opcode);
                                    int p = 6;
                                    Instruction memloc = MEMcreate_isn(STACK, (void *)&p);
                                    astream(&ministream, memloc);
                                    
                                    aastream(bs, ministream);
                                }
                        }
                        break;
                    case ADD:
                        {
                            ctype = _ADD;
                        }
                        break;
                    case SUB:
                        {
                            ctype = _SUB;
                        }
                        break;
                    case DIV:
                        {
                            ctype = _DIV;
                        }
                        break;
                    case STAR:
                        {
                            ctype = _MUL;
                            
                            //_MUL mem[top], mem[n - 2]
                            
                            // similar to the mstack and mini branches
                            ByteStream ministream;
                            ministream.stream = NULL;
                            ministream.streamsz = 0;
                            ministream.streamln = 0;
                            
                            Instruction opcode = CMDcreate_isn(_MUL, "");
                            astream(&ministream, opcode);
                            int p = 8;
                            Instruction memloc1 = MEMcreate_isn(STACK, (void *)&p);
                            astream(&ministream, memloc1);
                            int p2 = 7;
                            Instruction memloc2 = MEMcreate_isn(STACK, (void *)&p2);
                            astream(&ministream, memloc2);
                            
                            aastream(bs, ministream);
                        }
                        break;
                    case NUMBER:
                        {
                            ctype = _LITERAL;
                            
                            //_PUSH _NUMBER, INT
                            
                            // similar to the mstack and mini branches
                            ByteStream ministream;
                            ministream.stream = NULL;
                            ministream.streamsz = 0;
                            ministream.streamln = 0;
                            
                            Instruction opcode = CMDcreate_isn(_PUSH, "");
                            astream(&ministream, opcode);
                            int p = 2;
                            Instruction memloc = MEMcreate_isn(STACK, (void *)&p);
                            astream(&ministream, memloc);
                            Instruction context = CONcreate_isn(_INT_);
                            astream(&ministream, context);
                            
                            aastream(bs, ministream);                 
                        }
                        break;
                    default:
                        //printf("default: %s\n", type_names[stack->UNION.Nodes.type]);
                        break;
                    
                }
            //CMD c = spawn(ctype, 2, val);
            //acmd(cmds, cmdsz, cmdln, spawn(ctype, 2, val));
        }
    else
        {      
            //(*depth)++;
            for (int i = 0; i < stack->UNION.Nodes.nodeln; ++i)
                {
                    enum _TYPE_ ptype = stack->UNION.Nodes.type;
                    NodeMaster node_s = stack->UNION.Nodes.nodes[i];
                    traverse(&node_s, ptype, bs);
                }
            //(*depth)--;
        }
}

void bsprint(ByteStream isn)
{
    Instruction *st = isn.stream;
    for (int i = 0; i < isn.streamln; ++i)
        {
             if (st[i].isnt == __CMD)
                {
                    printf("%s, ", cmdt_types[st[i].data.cmd.cmdt]);
                }
            else if (st[i].isnt == __MEM)
                {
                    if (st[i].data.mem.memt == STACK)
                        {
                            printf("%s[%d], ", mem_types[st[i].data.mem.memt],  st[i].data.mem.loc.index);
                        }
                    else
                        {
                            printf("%s[%s], ", mem_types[st[i].data.mem.memt],  st[i].data.mem.loc.key);
                        }
                    //printf("%s, ", mem_types[st[i].data.mem.memt]);
                }
            else
                {
                    printf("%s, ", data_types[st[i].data.con.datat]);
                }  
             
        }
    printf("\n\n");
}

ByteStream compile(NodeMaster *stack)
{
    ByteStream bs;
    bs.bstream = NULL;
    bs.streamln = 0;
    bs.streamsz = 0;
    traverse(stack, PROGRAM, &bs);
    
    printf("%d BYTECODE:\n", bs.streamln);
    for (int i = bs.streamln - 1; i >= 0; --i)
        {   
            bsprint(bs.bstream[i]);
            /*enum _COMMAND_ cmdname = cmds[i].cmdt;
            //printf("%s ", cmdt_names[cmdname]);
            switch (cmdname)
                {
                    case _STO:
                        printf("_STO ID[top], mem[top]\n"); 
                        break;
                    case _ID:
                        printf("_ID_PUSH _ID%d\n", i);
                        break;
                    case _MUL:
                        printf("_MUL mem[top], mem[n - 2]\n");
                        break;
                    case _CALL:
                        printf("_CALL hashmap[FUNC_ID%d]\n", i);
                        break;
                    case _LITERAL:
                        printf("_PUSH _NUMBER, INT\n");
                        break;
                }
            //printf("%s\n", cmdt_names[cmds[i].cmdt]);
            printf("\n");*/
        }
        
    return bs;
}