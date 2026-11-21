#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>

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
    else if (memtype == HASH) isn.data.mem.loc.key = (char *)_index;
    else  isn.data.mem.loc.keylistid = *((int *)_index);
    
    return isn;
}
Instruction CONcreate_isn(enum _DATATYPE_ datatype, void *data)
{
    Instruction isn;
    isn.isnt = __CON;
    isn.data.con.datat = datatype;
    isn.data.con.val = data;
    /*if (datatype == _INT_) isn.data.con.rawdata.val = *((int *)data);
    else if (datatype == _STRING_) isn.data.con.rawdata.inum = *((char *)data);
    else if (datatype == _FLOAT_) isn.data.con.rawdata.inum = *((float *)data);*/
    
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
void traverse(NodeMaster *stack, enum _TYPE_ ptype, ByteStream *bs,  Genmem *gem)
{
    if (stack->utype == NODE)
        {
            printf("Ntype: %s\n", type_names[stack->UNION.Node.type]);
            const enum _TYPE_ STYPE = stack->UNION.Node.type;
            char *val = stack->UNION.Node.stor;
            enum _COMMAND_ ctype;
            
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
                            
                            int p = 1;
                            Instruction memloc1 = MEMcreate_isn(STACK, (void *)&p);
                            astream(&ministream, memloc1);
                            
                            /*char *s = "";
                            Instruction memloc1 = MEMcreate_isn(KEYLIST, (void *)s);
                            astream(&ministream, memloc1);*/
                            
                            char *p2 = "KEYL[-1]";
                            Instruction memloc2 = MEMcreate_isn(HASH, (void *)p2);
                            astream(&ministream, memloc2);
                            
                            // create complete instruction
                            // by appending them to the stream
                            aastream(bs, ministream);
                        }
                        break;
                    case IDENTIFIER:
                        {
                            printf(">> %s\n", type_names[gem->types[gem->ln - 1]]);
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
                                   char *s = stack->UNION.Node.stor;
                                   Instruction memloc = MEMcreate_isn(HASH, (void *)s);
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
                                    
                                    if (gem->types[gem->ln - 1] == ASSIGNMENT)
                                        {
                                             Instruction opcode = CMDcreate_isn(_PUSH_KEYL, "");
                                             astream(&ministream, opcode);
                                             
                                             /*char *s = stack->UNION.Node.stor;
                                             Instruction memloc = MEMcreate_isn(HASH, (void *)s);
                                             astream(&ministream, memloc);*/
                                             
                                             char *s = stack->UNION.Node.stor;
                                             Instruction context = CONcreate_isn(_STRING_, (void *)s);
                                             astream(&ministream, context);
                                        }
                                    else
                                        {
                                    
                                            // LOAD HASH y
                                            
                                            Instruction opcode = CMDcreate_isn(_LOAD, "");
                                            astream(&ministream, opcode);
                                            
                                            char *s = stack->UNION.Node.stor;
                                            Instruction memloc = MEMcreate_isn(HASH, (void *)s);
                                            astream(&ministream, memloc);
                                            
                                        }
                                        
                                    /*Instruction opcode = CMDcreate_isn(_LOAD, "");
                                    astream(&ministream, opcode);
                                    
                                    char *s = stack->UNION.Node.stor;
                                    Instruction memloc = MEMcreate_isn(HASH, (void *)s);
                                    astream(&ministream, memloc);*/
                                    
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
                            
                            // PUSH
                            
                            // similar to the mstack and mini branches
                            ByteStream ministream;
                            ministream.stream = NULL;
                            ministream.streamsz = 0;
                            ministream.streamln = 0;
                            
                            Instruction _opcode = CMDcreate_isn(_PUSH, "");
                            astream(&ministream, _opcode);
                            
                            aastream(bs, ministream);
                            
                            //_MUL mem[top], mem[n - 2]
                            
                            ministream.stream = NULL;
                            ministream.streamsz = 0;
                            ministream.streamln = 0;
                            
                            Instruction opcode = CMDcreate_isn(_MUL, "");
                            astream(&ministream, opcode);
                            int p = 1;
                            Instruction memloc1 = MEMcreate_isn(STACK, (void *)&p);
                            astream(&ministream, memloc1);
                            int p2 = 2;
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
                            /*int p = stack->UNION.Node.stor;
                            Instruction memloc = MEMcreate_isn(STACK, (void *)p);
                            astream(&ministream, memloc);*/
                            int p = 5;
                            Instruction context = CONcreate_isn(_INT_, (void *)&p);
                            astream(&ministream, context);
                            
                            aastream(bs, ministream);                 
                        }
                        break;
                    default:
                        //printf("default: %s\n", type_names[stack->UNION.Nodes.type]);
                        break;
                    
                }
                
            // we append each NODE to gem (general memory)
            /**gemsz += sizeof(*stack);
            gem->UNION.Nodes.nodes = realloc(gem->UNION.Nodes.nodes, *gemsz);
            gem->UNION.Nodes.nodes[gem->UNION.Nodes.nodeln] = *stack;
            gem->UNION.Nodes.nodeln++;*/
            //CMD c = spawn(ctype, 2, val);
            //acmd(cmds, cmdsz, cmdln, spawn(ctype, 2, val));
        }
    else
        {      
            //printf("stack len %d\n", stack->UNION.Nodes.nodeln);
            //(*depth)++;
            /**gemsz += sizeof(*stack);
            gem->UNION.Nodes.nodes = realloc(gem->UNION.Nodes.nodes, *gemsz);
            gem->UNION.Nodes.nodes[gem->UNION.Nodes.nodeln] = *stack;
            gem->UNION.Nodes.nodeln++;*/
            gem->size += sizeof(stack->UNION.Nodes.type);
            gem->types = realloc(gem->types, gem->size);
            gem->types[gem->ln] = stack->UNION.Nodes.type;
            gem->ln++;
            printf("NStype: %s  len: %d\n", type_names[stack->UNION.Nodes.type], stack->UNION.Nodes.nodeln);
            //for (int i = (stack->UNION.Nodes.nodeln - 1); i >= 0; --i)
            for (int i = 0; i < stack->UNION.Nodes.nodeln; ++i)
                {
                    enum _TYPE_ ptype = stack->UNION.Nodes.type;
                    NodeMaster node_s = stack->UNION.Nodes.nodes[i];
                    traverse(&node_s, ptype, bs,  gem);
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
                    printf("%s ", cmdt_types[st[i].data.cmd.cmdt]);
                }
            else if (st[i].isnt == __MEM)
                {
                    if (st[i].data.mem.memt == STACK)
                        {
                            //printf("MEM %d ", mem_types[st[i].data.mem.memt],  st[i].data.mem.loc.index);
                            printf("STACK(-%d), ", st[i].data.mem.loc.index);
                        }
                    /*else if (st[i].data.mem.memt == KEYLIST)
                        {
                            printf("-%d, ", st[i].data.mem.loc.keylistid);
                        }*/
                    else if (st[i].data.mem.memt == HASH)
                        {
                            //printf("%s: %s ", mem_types[st[i].data.mem.memt],  st[i].data.mem.loc.key);
                            printf("HASH(%s) ", st[i].data.mem.loc.key);
                        }
                    //printf("%s, ", mem_types[st[i].data.mem.memt]);
                }
            else
                {
                    void *p = st[i].data.con.val;
                    if (st[i].data.con.datat == _INT_) printf("%d ", (*(int *)p)); 
                    else if (st[i].data.con.datat == _STRING_) printf("%s ", (char *)p);
                    if (st[i].data.con.datat == _FLOAT_) printf("%f ", (*(float *)p));
                    printf("%s, ", data_types[st[i].data.con.datat]);
                }  
             
        }
    printf("\n\n");
}

ByteStream compile(NodeMaster *stack)
{
    /*size_t gemsz = 0;
    NodeMaster gem;
    gem.utype = NODES;
    gem.UNION.Nodes.nodes = NULL;
    gem.UNION.Nodes.nodeln = 0;*/
    Genmem gem;
    gem.types = NULL;
    gem.ln = 0;
    gem.size = 0;
    
    ByteStream bs;
    bs.bstream = NULL;
    bs.streamln = 0;
    bs.streamsz = 0;
    for (int i = stack->UNION.Nodes.nodeln - 1; i >= 0; --i)
        {
            enum _TYPE_ ptype = stack->UNION.Nodes.type;
            NodeMaster node_s = stack->UNION.Nodes.nodes[i];
            traverse(&node_s, PROGRAM, &bs,  &gem);
        }
    //traverse(stack, PROGRAM, &bs,  &gem, &gemsz);
    
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