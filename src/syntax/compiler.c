#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <math.h>

#include "coms/common.h"

// this is used purely for tracking commands.
// it is used in the JUMP commands to tell it
// where to jump to (relative) based on a 
// certain condition
int address = 0;


ByteStream mbs_init();
void mbs_reset(ByteStream *bs);

void astream(ByteStream *bs, Instruction isn);

void bytecode_classdef(NodeMaster *stack, ByteStream *bs,  Genmem *gem, NodeMaster *genm, ByteStreamIndex *bsi);
void bytecode_else(NodeMaster *stack, ByteStream *bs,  Genmem *gem, NodeMaster *genm, ByteStreamIndex *bsi, int *pre_ifelse_statement);
void bytecode_fndef(NodeMaster *stack, ByteStream *bs,  Genmem *gem, NodeMaster *genm, ByteStreamIndex *bsi);
void bytecode_if(NodeMaster *stack, ByteStream *bs,  Genmem *gem, NodeMaster *genm, ByteStreamIndex *bsi, int *pre_ifelse_statement);
void bytecode_identifier(NodeMaster *node, ByteStream *bs, Genmem *gem, NodeMaster *genm, ByteStreamIndex *bsi);
void bytecode_args(NodeMaster *node_s, ByteStream *bs, Genmem *gem, NodeMaster *genm, ByteStreamIndex *bsi);
void bytecode_func(NodeMaster *node_s, ByteStream *bs, Genmem *gem, NodeMaster *genm, ByteStreamIndex *bsi);
void bytecode_IDlinker_func(NodeMaster *node_s, ByteStream *bs, Genmem *gem, NodeMaster *genm, ByteStreamIndex *bsi);
void bytecode_IDlinker(NodeMaster *node_s, ByteStream *bs, Genmem *gem, NodeMaster *genm, ByteStreamIndex *bsi, int *linker_cnt);
void bytecode_condition(NodeMaster *stack, ByteStream *bs,  Genmem *gem, NodeMaster *genm, ByteStreamIndex *bsi);
void traverse(NodeMaster *stack, ByteStream *bs,  Genmem *gem, NodeMaster *genm, ByteStreamIndex *bsi);

Instruction CMDcreate_isn(enum _COMMAND_ cmdtype, char *_val, enum _FLAG_ _flag)
{
    Instruction isn;
    isn.isnt = __CMD;
    isn.data.cmd = (CMD) 
        {
           .cmdt =  cmdtype,
           .val = _val,
           .flag = _flag
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
    
    return isn;
}

void ai_stream(ByteStream *bs, Instruction ins)
{
    bs->streamsz += sizeof(ins);
    bs->stream = realloc(bs->stream, bs->streamsz);
    bs->stream[bs->streamln] = ins;
    bs->streamln++;
    
    //printf("cmd: %s\n", cmdt_types[ins.data.cmd.cmdt]);
}
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

void agem(enum _TYPE_ atype, Genmem *gem)
{
    gem->size += sizeof(atype);
    gem->types = realloc(gem->types, gem->size);
    gem->types[gem->ln] = atype;
    gem->ln++;
}
void agenm(NodeMaster node, NodeMaster *genm)
{
    genm->UNION.Nodes.nodes = realloc(genm->UNION.Nodes.nodes, sizeof(node));
    genm->UNION.Nodes.nodes[genm->UNION.Nodes.nodeln] = node;
    genm->UNION.Nodes.nodeln++;
}

int search_func(ByteStreamIndex *bsi, char *func_name)
{
    for (int i = 0; i < bsi->len; ++i)
        {
            ByteStream bs = bsi->bytestream[i];
            
            if (strcmp(bsi->table[i], func_name) == 0)
                {
                    debugp("found function in memory");
                    return i;
                }
        }
    
    return -1;
}
/*
STACK: [PROGRAM]
    STACK: [ID_LINKER]
            STACK: [ID_LINKER]
                    NODE: [DECIMAL]------>[.]
                    STACK: [ID_LINKER]
                            NODE: [DECIMAL]------>[.]
                            NODE: [IDENTIFIER]--->[player]
                            NODE: [IDENTIFIER]--->[box]
                    STACK: [FUNCTION_CALL]
                            NODE: [IDENTIFIER]--->[move]
                            STACK: [ARGUMENTS]
                                    STACK: [EXPRESSION]
                                            NODE: [IDENTIFIER]--->[somearg]
                                    STACK: [EXPRESSION]
                                            STACK: [ID_LINKER]
                                                    STACK: [ID_LINKER]
                                                            NODE: [DECIMAL]------>[.]
                                                            NODE: [IDENTIFIER]--->[linkerarg]
                                                            NODE: [IDENTIFIER]--->[links]
                                    STACK: [EXPRESSION]
                                            STACK: [FUNCTION_CALL]
                                                    NODE: [IDENTIFIER]--->[functionarg]
                                                    STACK: [ARGUMENTS]
                                    STACK: [EXPRESSION]
                                            STACK: [FUNCTION_CALL]
                                                    NODE: [IDENTIFIER]--->[functionwithargs]
                                                    STACK: [ARGUMENTS]
                                                            STACK: [EXPRESSION]
                                                                    NODE: [IDENTIFIER]--->[arg1]
*/

void bytecode_listacc(NodeMaster *node, ByteStream *bs, Genmem *gem, NodeMaster *genm, ByteStreamIndex *bsi, int *accesses)
{
    // we first grab all of the indexes, similar to 
    // linkers and functions with arguments
    bytecode_args(&node->UNION.Nodes.nodes[1], bs, gem, genm, bsi);
    *accesses = node->UNION.Nodes.nodes[1].UNION.Nodes.nodeln + 1;

    char *idname = node->UNION.Nodes.nodes[0].UNION.Node.stor;
    //Instruction load = CMDcreate_isn(_LOAD, idname);
    //ai_stream(bs, load);
    Instruction string = CONcreate_isn(_STRING_, idname);
    ai_stream(bs, string);
    
    Instruction push = CMDcreate_isn(_PUSH, "", __NOFLAG__);
    ai_stream(bs, push);
}

void bytecode_identifier(NodeMaster *node, ByteStream *bs, Genmem *gem, NodeMaster *genm, ByteStreamIndex *bsi)
{   
    char *id = node->UNION.Node.stor;
    Instruction load = CMDcreate_isn(_LOAD, id, __NOFLAG__);
    ai_stream(bs, load);
    
    Instruction push = CMDcreate_isn(_PUSH, "", __NOFLAG__);
    ai_stream(bs, push);
}

void bytecode_literal(NodeMaster *node, ByteStream *bs, Genmem *gem, NodeMaster *genm, ByteStreamIndex *bsi)
{   
    
    char *value_s = node->UNION.Node.stor;
    Casted implicit_cast = convert(value_s, type(value_s));
    
    Instruction literal;
    switch (implicit_cast.type)
        {
            case _INT_:
                int *p = malloc(sizeof(implicit_cast.data.inum));
                *p = implicit_cast.data.inum;
                literal = CONcreate_isn(_INT_, (void *)p);
                break;
            case _FLOAT_:
                float *fp = malloc(sizeof(implicit_cast.data.fnum));
                *fp = implicit_cast.data.fnum;
                literal = CONcreate_isn(_FLOAT_, (void *)fp);
                break;
            case _STRING_:
                char *sp = malloc(sizeof(char) * strlen(implicit_cast.data.string));
                strcpy(sp, implicit_cast.data.string);
                literal = CONcreate_isn(_STRING_, (void *)sp);
                break;
            default:
                perror("Idk man...");
                exit(EXIT_FAILURE);
        }
        
    ai_stream(bs, literal);
    
    Instruction push = CMDcreate_isn(_PUSH, "", __NOFLAG__);
    ai_stream(bs, push);
}

void bytecode_args(NodeMaster *node_s, ByteStream *bs, Genmem *gem, NodeMaster *genm, ByteStreamIndex *bsi)
{   
    // there are no arguments, so we simply return
    if (node_s->UNION.Nodes.nodeln < 1) return;
    
    for (int i = 0; i < node_s->UNION.Nodes.nodeln; ++i)
        {
            NodeMaster subnode;
            // every single arguments has EXPRESSION as a parent node
            // so we ignore it and skip past it
            if (node_s->UNION.Nodes.nodes[i].UNION.Nodes.type == EXPRESSION)
                {
                    subnode = node_s->UNION.Nodes.nodes[i].UNION.Nodes.nodes[0];
                }
            else
                {
                    subnode = node_s->UNION.Nodes.nodes[i];
                }
            
            if (subnode.utype == NODES)
                {
                    switch (subnode.UNION.Nodes.type)
                        {
                            case OPERATION:
                                //bytecode_expr();
                                break;
                            case FUNCTION_CALL:
                                bytecode_func(&subnode, bs, gem, genm, bsi);
                                break;
                            case ID_LINKER:
                                // remember, we go straight to index '0' because an ID_LINKER
                                // always has a stupid, redudant ID_LINKER parent stack that 
                                // does nothing but make life a little bit more complicated than it 
                                // has to be 
                                int linker_cnt = 0;
                                bytecode_IDlinker(&subnode.UNION.Nodes.nodes[0], bs, gem, genm, bsi, &linker_cnt);
                                
                                char *seq = malloc(sizeof(char)*(int)log10(linker_cnt));
                                sprintf(seq, "%d", linker_cnt);
                                
                                Instruction loadseq = CMDcreate_isn(_LOAD_SEQ, seq, __NOFLAG__);
                                ai_stream(bs, loadseq);
                                
                                Instruction push = CMDcreate_isn(_PUSH, "", __NOFLAG__);
                                ai_stream(bs, push);
                                
                                Instruction call = CMDcreate_isn(_CALL, "", __NOFLAG__);
                                ai_stream(bs, call);
                                break;
                            case LIST_ACC:
                                int counter = 0;
                                bytecode_listacc(&subnode, bs, gem, genm, bsi, &counter);

                                char *__seq = malloc(sizeof(char)*(int)log10(counter));
                                sprintf(__seq, "%d", counter);
                                Instruction __loadseq = CMDcreate_isn(_LOAD_SEQ, __seq, __NOFLAG__);
                                ai_stream(bs, __loadseq);

                                Instruction __push = CMDcreate_isn(_PUSH, "", __NOFLAG__);
                                ai_stream(bs, __push);
                                
                                break;
                            case LIST:
                                bytecode_args(&subnode, bs, gem, genm, bsi);
                                
                                char *_seq = malloc(sizeof(char)*(int)log10(subnode.UNION.Nodes.nodeln));
                                sprintf(_seq, "%d", subnode.UNION.Nodes.nodeln);
                                Instruction _loadseq = CMDcreate_isn(_LOAD_SEQ, _seq, __NOFLAG__);
                                ai_stream(bs, _loadseq);
                                
                                Instruction _push = CMDcreate_isn(_PUSH, "", __NOFLAG__);
                                ai_stream(bs, _push);
                                
                                break;
                        }
                }
            else
                {
                    switch (subnode.UNION.Node.type)
                        {
                            case IDENTIFIER:
                                bytecode_identifier(&subnode, bs, gem, genm, bsi);
                                break;
                            case NUMBER:
                            case STRING:
                                bytecode_literal(&subnode, bs, gem, genm, bsi);
                                break;
                            default:
                                debugp("HERE??");
                                break;
                        }
                }
        }
}

void bytecode_func(NodeMaster *node_s, ByteStream *bs, Genmem *gem, NodeMaster *genm, ByteStreamIndex *bsi)
{
    // we want to load any arguments first, of course
    NodeMaster arg_stack = node_s->UNION.Nodes.nodes[1];
    bytecode_args(&arg_stack, bs, gem, genm, bsi);
    
    // at index 1 (true index is list[0]), will always be the function name 
    char *func_name = node_s->UNION.Nodes.nodes[0].UNION.Node.stor;
    /*Instruction load = CMDcreate_isn(_LOAD, func_name, __NOFLAG__);
    ai_stream(bs, load);
    Instruction push = CMDcreate_isn(_PUSH, "", __NOFLAG__);
    ai_stream(bs, push);*/
    
    // we fetch the address of the function in the bsi so it
    // can be quickly called later on
    int namefn_index = search_func(bsi, func_name);
    printf("function name is: %s %d\n", func_name, namefn_index);
    // - 1 means we weren't able to find the function in the array
    if (namefn_index < 0)
        {
            perror("Function doesn't exist");
            exit(EXIT_FAILURE);
        }
    //                               'its' just converts an int to string
    char *string = malloc(sizeof(char)*(int)log10(namefn_index + 1));
    sprintf(string, "%d", namefn_index);
    Instruction call = CMDcreate_isn(_CALL, string, __FNCALL__);
    ai_stream(bs, call);
}

void bytecode_IDlinker_func(NodeMaster *node_s, ByteStream *bs, Genmem *gem, NodeMaster *genm, ByteStreamIndex *bsi)
{
    // ==========================================================
    // THIS FUNCTION ACTS AS A WRAPPER FOR THE BYTECODE_IDLINKER
    // BY CONTAINING THE LINKERS AND CONDITIONS ON WHETHER IT IS
    // A FUNCTION CALL OR NOT
    // ==========================================================
    
    int links = 0;
    bytecode_IDlinker(node_s, bs, gem, genm, bsi, &links);
    
    bool non_regular = false;
    if (gem->ln > 0)
        {
            if (gem->types[gem->ln - 1] == FUNCTION_CALL)
                {
                    // this means that there was a function at the end
                    // of the linker, such as: player.box.move();
                    
                    char *seq = malloc(sizeof(char)*(int)log10(links));
                    sprintf(seq, "%d", links);
                    
                    Instruction loadseq = CMDcreate_isn(_LOAD_SEQ, seq, __NOFLAG__);
                    ai_stream(bs, loadseq);
                    
                    // after loading the hashmap keys, we push whats loaded onto the stack
                    Instruction push = CMDcreate_isn(_PUSH, "", __NOFLAG__);
                    ai_stream(bs, push);
                    // we then call it because it is a function
                    Instruction call = CMDcreate_isn(_CALL, "", __NOFLAG__);
                    ai_stream(bs, call);
                    
                    non_regular = true;
                }
        }
    if (!non_regular)
        {
            char *seq = malloc(sizeof(char)*(int)log10(links));
            sprintf(seq, "%d", links);
            
            Instruction loadseq = CMDcreate_isn(_LOAD_SEQ, seq, __NOFLAG__);
            ai_stream(bs, loadseq);
            
            Instruction push = CMDcreate_isn(_PUSH, "", __NOFLAG__);
            ai_stream(bs, push);
        }
}

void bytecode_IDlinker(NodeMaster *node_s, ByteStream *bs, Genmem *gem, NodeMaster *genm, ByteStreamIndex *bsi, int *linker_cnt)
{
    // if the second index is NODES, then that means the linker has more than
    // one '.'
    // otherwise it means that its a single dot like player.x or something
    
    NodeMaster sublink = node_s->UNION.Nodes.nodes[1];
    
    // this means that there is also a third node which
    NodeMaster subsublink = node_s->UNION.Nodes.nodes[2];
    
    if (subsublink.utype == NODE)
        {        
            Instruction literal = CONcreate_isn(_STRING_, (void *)subsublink.UNION.Node.stor);
            ai_stream(bs, literal);
            
            Instruction push = CMDcreate_isn(_PUSH, "", __NOFLAG__);
            ai_stream(bs, push);
            (*linker_cnt)++;
        }
    else
        {   
            // it could be a function or something
            if (subsublink.UNION.Nodes.type == FUNCTION_CALL)
                {                    
                    // before we add the function to the LOAD_SEQ, we need
                    // to load in all of the arguments.
                    /*
                        this is because, if we have LOAD_SEQ 3, and then we have CALL, 
                        if those arguments are after CALL, we won't reach them for the 
                        function call. Ontop of the that, we don't want them infront of
                        all of the objects on the stack, because otherwise LOAD_SEQ will
                        hit them BEFORE it hits the objects to load in on each hashmap
                    */
                    NodeMaster args_nodes = subsublink.UNION.Nodes.nodes[1];
                    bytecode_args(&args_nodes, bs, gem, genm, bsi);
                    
                    // we still add it to the LOAD_SEQ
                    // =====================================================
                    NodeMaster node = subsublink.UNION.Nodes.nodes[0];

                    Instruction literal = CONcreate_isn(_STRING_, (void *)node.UNION.Node.stor);
                    ai_stream(bs, literal);
                    
                    Instruction push = CMDcreate_isn(_PUSH, "", __NOFLAG__);
                    ai_stream(bs, push); 
                    (*linker_cnt)++;
                    // =====================================================
                    
                    // we append it to general memory so that when we return 
                    // out of this recursive function, we can run a CALL command
                    agem(FUNCTION_CALL, gem);
                    
                    // we also append the NodeMaster function mstack into genm
                    // so that we can properly process the function and convert
                    // it into bytecode
                    agenm(subsublink, genm);
                    // ============= THIS IS ASSUMING ITS A FUNCTION CALL =============
                    // we still go through to get the name of the function
                    // so that it can be loaded from the hashmap using 
                    // LOAD_SEQ as normal
                    //bytecode_func(&subsublink.UNION.Nodes.nodes[0], bs, gem, genm);
                }
            
        }
    
    if (sublink.utype == NODES)
        {
            bytecode_IDlinker(&sublink, bs, gem, genm, bsi, linker_cnt);
        }
    else
        {             
            // this means that there is also a third node which
            Instruction _literal = CONcreate_isn(_STRING_, (void *)sublink.UNION.Node.stor);
            ai_stream(bs, _literal);
            
            Instruction push = CMDcreate_isn(_PUSH, "", __NOFLAG__);
            ai_stream(bs, push);
            (*linker_cnt)++;
        }
}

void bytecode_operation(NodeMaster *stack, ByteStream *bs,  Genmem *gem, NodeMaster *genm, ByteStreamIndex *bsi)
{
    NodeMaster operation = stack->UNION.Nodes.nodes[0];
    NodeMaster condition1 = stack->UNION.Nodes.nodes[1];
    NodeMaster condition2 = stack->UNION.Nodes.nodes[2];
    
    enum _TYPE_ oper_type = (operation.utype == NODES) ? operation.UNION.Nodes.type : operation.UNION.Node.type;
    enum _TYPE_ cond1_type = (condition1.utype == NODES) ? condition1.UNION.Nodes.type : condition1.UNION.Node.type; 
    enum _TYPE_ cond2_type = (condition2.utype == NODES) ? condition2.UNION.Nodes.type : condition2.UNION.Node.type;
    
    switch (cond1_type)
        {
            case ID_LINKER:
                bytecode_IDlinker_func(&condition1.UNION.Nodes.nodes[0], bs, gem, genm, bsi);
                
                Instruction variable = CMDcreate_isn(_LOAD, "", __CONSUME__);
                
                break;
            case NUMBER:
                bytecode_literal(&condition1, bs, gem, genm, bsi);
                break;
            case IDENTIFIER:
                bytecode_identifier(&condition1, bs, gem, genm, bsi);
                break;
            case LIST_ACC:
                int accesses = 0;
                bytecode_listacc(&condition1, bs, gem, genm, bsi, &accesses);
                        
                char *seq = malloc(sizeof(char)*(int)log10(accesses));
                sprintf(seq, "%d", accesses);
                
                Instruction loadseq = CMDcreate_isn(_LOAD_SEQ, seq, __NOFLAG__);
                ai_stream(bs, loadseq);
                
                Instruction push = CMDcreate_isn(_PUSH, "", __NOFLAG__);
                ai_stream(bs, push);
                break;
            case FUNCTION_CALL:
                break;  
        }
    
    switch (cond2_type)
        {
            case ID_LINKER:
                bytecode_IDlinker_func(&condition1.UNION.Nodes.nodes[0], bs, gem, genm, bsi);
                
                Instruction variable = CMDcreate_isn(_LOAD, "", __CONSUME__);
                
                break;
            case NUMBER:
                bytecode_literal(&condition2, bs, gem, genm, bsi);
                break;
            case IDENTIFIER:
                bytecode_identifier(&condition2, bs, gem, genm, bsi);
                break;
            case LIST_ACC:
                int accesses = 0;
                bytecode_listacc(&condition2, bs, gem, genm, bsi, &accesses);
                        
                char *seq = malloc(sizeof(char)*(int)log10(accesses));
                sprintf(seq, "%d", accesses);
                
                Instruction loadseq = CMDcreate_isn(_LOAD_SEQ, seq, __NOFLAG__);
                ai_stream(bs, loadseq);
                
                Instruction push = CMDcreate_isn(_PUSH, "", __NOFLAG__);
                ai_stream(bs, push);
                break;
            case FUNCTION_CALL:
                break; 
        }
        
    switch (oper_type)
        {
            case ADD:
                Instruction add = CMDcreate_isn(_ADD, "", __NOFLAG__);
                ai_stream(bs, add);
                {
                    Instruction push = CMDcreate_isn(_PUSH, "", __NOFLAG__);
                    ai_stream(bs, push);
                }
                break;
            case SUB:
                Instruction sub = CMDcreate_isn(_SUB, "", __NOFLAG__);
                ai_stream(bs, sub);
                {
                    Instruction push = CMDcreate_isn(_PUSH, "", __NOFLAG__);
                    ai_stream(bs, push);
                }
                break;
            case STAR:
                Instruction mul = CMDcreate_isn(_MUL, "", __NOFLAG__);
                ai_stream(bs, mul);
                {
                    Instruction push = CMDcreate_isn(_PUSH, "", __NOFLAG__);
                    ai_stream(bs, push);
                }
                break;
            case DIV:
                Instruction div = CMDcreate_isn(_DIV, "", __NOFLAG__);
                ai_stream(bs, div);
                {
                    Instruction push = CMDcreate_isn(_PUSH, "", __NOFLAG__);
                    ai_stream(bs, push);
                }
                break;
        }
}

void bytecode_assignment(NodeMaster *node_s, ByteStream *bs, Genmem *gem, NodeMaster *genm, ByteStreamIndex *bsi)
{
    // this loops through all of the nodes in an
    // ASSIGNMENT mstack
    
    // the operation node is always the first one in an mstack
    NodeMaster oper_n = node_s->UNION.Nodes.nodes[0];
    
    // the assignment node is always the second one in an mstack
    NodeMaster assig_n = node_s->UNION.Nodes.nodes[1]; 
    
    // the value being assigned is always the third one in an mstack
    NodeMaster value_n = node_s->UNION.Nodes.nodes[2];
    
    // we then take both previous values are process them, 
    // as they could be either linkers, lists, or whatever 
    // of the like
    
    // we sit this here first because if the operator happens to be
    // a '+=', then we need to add the previous assig_n value onto
    // the evaluated expression
    enum _COMMAND_ assig_oper = oper_n.UNION.Node.type;
    /*switch (oper_n.UNION.Node.type)
        {
             case EQUAL:
                 break;
             case ADD_EQ:
                 break;
             case SUB_EQ:
                 break;
             case STAR_EQ:
                 break;
             case DIV_EQ:
                 break;
             
             default:
                 perror("Undefined operator type");
                 exit(EXIT_FAILURE);
                 break;   
        }*/
    
    enum _COMMAND_ sto_type = _STO;
    int sto_data = 0;
    enum _FLAG_ sto_flag = __NOFLAG__;
    
    // we check what kind of operator it was so we 
    // can assign the correct flag
    switch (assig_oper)
        {
             case EQUAL:
                 break;
             case ADD_EQ:
                 sto_flag = __ADDEQ__;
                 break;
             case SUB_EQ:
                 sto_flag = __SUBEQ__;
                 break;
             case STAR_EQ:
                 sto_flag = __MULEQ__;
                 break;
             case DIV_EQ:
                 sto_flag = __DIVEQ__;
                 break;
             
             default:
                 perror("Undefined operator type");
                 exit(EXIT_FAILURE);
                 break;   
        }
                
    if (value_n.utype == NODES)
        {
            // ===============================================================
            // THIS WILL ALWAYS BE AN EXPRESSION IF ITS NODES
            // ===============================================================
            
            // THIS IS NEXT UP FOR DEVELOPMENT
            for (int i = 0; i < value_n.UNION.Nodes.nodeln; ++i)
                {
                    NodeMaster subnode = value_n.UNION.Nodes.nodes[i];
                
                    if (subnode.utype == NODES)
                        {
                            // if this is the case, then the subnode will always be
                            // an expression, so we skip it and go straight to the
                            // very first index
                            
                            /*NodeMaster subsubnode = subnode.UNION.Nodes.nodes[0];
                            enum _TYPE_ type;
                            bool is_NODES = false;
                            type = (subsubnode.utype == NODE) ? subsubnode.UNION.Node.type : subsubnode.UNION.Nodes.type;
                            printf("type: %s\n", type_names[type]);*/
                            if (subnode.UNION.Nodes.type == EXPRESSION)
                                {
                                    subnode = subnode.UNION.Nodes.nodes[0];
                                }
                             
                            switch (subnode.UNION.Nodes.type)
                                {
                                    case FUNCTION_CALL:
                                        bytecode_func(&subnode, bs, gem, genm, bsi);
                                        break;
                                    case ID_LINKER:
                                        // remember, that stupid, redundant ID_LINKER
                                        // parent stack, so we use index 0 to bypass it
                                        bytecode_IDlinker_func(&subnode.UNION.Nodes.nodes[0], bs, gem, genm, bsi);
                                        break;
                                    case LIST_ACC:
                                        int accesses = 0;
                                        bytecode_listacc(&subnode, bs, gem, genm, bsi, &accesses);
                                                
                                        char *seq = malloc(sizeof(char)*(int)log10(accesses));
                                        sprintf(seq, "%d", accesses);
                                        
                                        Instruction loadseq = CMDcreate_isn(_LOAD_SEQ, seq, __NOFLAG__);
                                        ai_stream(bs, loadseq);
                                        
                                        Instruction push = CMDcreate_isn(_PUSH, "", __NOFLAG__);
                                        ai_stream(bs, push);
                                        break;
                                    case LIST:
                                        bytecode_args(&subnode, bs, gem, genm, bsi);
                                        sto_type = _STO_SEQ;
                                        sto_data = subnode.UNION.Nodes.nodeln;
                                        break;
                                    case OPERATION:
                                        bytecode_operation(&subnode, bs,  gem, genm, bsi);
                                        break;
                                }     
                        }
                    // i dont believe this is necessarily needed as whenever assigning
                    // a value to a function, if it is not a stand-alone literal, it
                    // will always be contained inside a NODES
                    /*else
                        {
                            switch (subnode.UNION.Node.type)
                                {
                                    case :
                                        break;
                                }
                        }*/
                }
        }
    else
        {
            // ===============================================================
            // THIS IS IF THE VALUE ON THE RIGHT IN AN ASSIGNMENT IS A SINGLE
            // VALUE, LIKE [name = "john"] OR [number = 10.5]
            // ===============================================================
            
            bytecode_literal(&value_n, bs, gem, genm, bsi);
        }
        
    // we do this one afterwards so that, in bytecode,
    // we 'STO' the value after evaluating the value
    if (assig_n.utype == NODES)
        {
            // ===============================================================
            // THIS WILL ALWAYS BE A LINKER OR AN LIST[INDEX] <-- idk what i
            // was thinking when i wrote this. ima leave it here though...
            // ===============================================================
            
            switch (assig_n.UNION.Nodes.type)
                {
                    case ID_LINKER:
                        sto_flag = __CONSUME__;
                        // in the parser.c, ID_LINKER always has a redudant and useless
                        // STACK ID_LINKER at the very type.
                        // it just contains a single other ID_LINKER stack inside.
                        // therefore we skip over it with 'nodes[0]'
                        bytecode_IDlinker_func(&assig_n.UNION.Nodes.nodes[0], bs, gem, genm, bsi);

                        // WE DONT CHECK FOR FUNCTION CALLS HERE
                        // it is illegal to say:
                        // player.something.function() = ...
                        // thus, theres no need to check for it!
                        
                        // given that LOAD_SEQ num loads a value from a hashmap(s)
                        // and then stores it on the stack, we need to consume that
                        // previously loaded item and then store the item before that
                        // into the hashmap.
                        // we do this by using STO CONSUME
                        // this also means that no variable can be named "__CONSUME__" 
                        // in my programming language.
                        
                        Instruction variable = CMDcreate_isn(sto_type, "", sto_flag);
                        
                        // append the literal to the stream
                        ai_stream(bs, variable);
                        
                        // we always check what kind of STO type it is, because if it
                        // is a _STO_SEQ type, then we also need to specify the sequence
                        // length
                        switch (sto_type)
                            {
                                case _STO:
                                    break;
                                case _STO_SEQ:
                                    // _PROVIDE simply 'provides' the previous command
                                    // with some data.
                                    // i know... it's a very messy work-around
                                    char *cseq = malloc(sizeof(char)*(int)log10(sto_data));
                                    sprintf(cseq, "%d", sto_data);
                                    Instruction seq = CMDcreate_isn(_PROVIDE, cseq, __NOFLAG__);
                                    ai_stream(bs, seq);
                                    break;
                            }
                        
                        break;
                    case LIST_ACC:
                        sto_flag = __CONSUME__;
                        bytecode_listacc(&assig_n, bs, gem, genm, bsi, &sto_data);
                        
                        char *seq = malloc(sizeof(char)*(int)log10(sto_data));
                        sprintf(seq, "%d", sto_data);
                        
                        Instruction loadseq = CMDcreate_isn(_LOAD_SEQ, seq, __NOFLAG__);
                        ai_stream(bs, loadseq);
                        
                        Instruction push = CMDcreate_isn(_PUSH, "", __NOFLAG__);
                        ai_stream(bs, push);
                        
                        Instruction _variable = CMDcreate_isn(sto_type, "", sto_flag);
                        ai_stream(bs, _variable);
                        
                        switch (sto_type)
                            {
                                case _STO:
                                    break;
                                case _STO_SEQ:
                                    // _PROVIDE simply 'provides' the previous command
                                    // with some data.
                                    // i know... it's a very messy work-around
                                    char *_cseq = malloc(sizeof(char)*(int)log10(sto_data));
                                    sprintf(_cseq, "%d", sto_data);
                                    Instruction seq = CMDcreate_isn(_PROVIDE, _cseq, __NOFLAG__);
                                    ai_stream(bs, seq);
                                    break;
                            }
                        break;
                    case OPERATION:
                        bytecode_operation(&assig_n, bs,  gem, genm, bsi);
                        break;
                    default:
                        perror("No NODE type exists");
                        exit(EXIT_FAILURE);
                        break;
                }
        }
    else
        {            
            // convert straight into bytecode here
            
            // ===============================================================
            // THIS IS IF THE VALUE ON THE LEFT IN JUST A SINGLE VARIABLE LIKE
            // [name = "john"] OR [number = 10.5], NOT A LINKER
            // ===============================================================
            
            char *var = assig_n.UNION.Node.stor;
            Instruction load = CMDcreate_isn(_LOAD, var, __NOFLAG__);
            Instruction push = CMDcreate_isn(_PUSH, "", __NOFLAG__);
            Instruction op_itself;
            
            // we first see why kind of assignment operator it was
            
            // actually store the variable
            Instruction variable = CMDcreate_isn(sto_type, var, sto_flag);
            
            // append the literal to the stream
            ai_stream(bs, variable);
            
            // we always check what kind of STO type it is, because if it
            // is a _STO_SEQ type, then we also need to specify the sequence
            // length
            switch (sto_type)
                {
                    case _STO:
                        break;
                    case _STO_SEQ:
                        // _PROVIDE simply 'provides' the previous command
                        // with some data.
                        // i know... it's a very messy work-around
                        
                        char *cseq = malloc(sizeof(char)*(int)log10(sto_data));
                        sprintf(cseq, "%d", sto_data);
                        Instruction seq = CMDcreate_isn(_PROVIDE, cseq, __NOFLAG__);
                        ai_stream(bs, seq);
                        break;
                }
                            
        }
}

/* this is just for reference

STACK: [PROGRAM]
        STACK: [ASSIGNMENT]
                NODE: [EQUAL]------->[=]
                STACK: [ID_LINKER]
                        STACK: [ID_LINKER]
                                NODE: [DECIMAL]------>[.]
                                NODE: [IDENTIFIER]--->[player]
                                NODE: [IDENTIFIER]--->[name]
                STACK: [EXPRESSION]
                        STACK: [OPERATION]
                                NODE: [ADD]---------->[+]
                                NODE: [NUMBER]------->[1]
                                NODE: [NUMBER]------->[6]
*/

void bytecode_not(NodeMaster *stack, ByteStream *bs,  Genmem *gem, NodeMaster *genm, ByteStreamIndex *bsi)
{
    // we evaluate the condition inside of the NOT
    NodeMaster *node_s = (stack->utype == NODES) ? stack->UNION.Nodes.nodes : stack;
    bytecode_condition(node_s, bs, gem, genm, bsi);
    
    // we inverse whatever was calculated before it
    // ========= may not need '__CONSUME__' =========  
    Instruction inverse = CMDcreate_isn(_INVERSE, "", __NOFLAG__);//, "__CONSUME__");
    ai_stream(bs, inverse);
    
    Instruction push = CMDcreate_isn(_PUSH, "", __NOFLAG__);
    ai_stream(bs, push);
}

void bytecode_subcondition(NodeMaster *stack, ByteStream *bs,  Genmem *gem, NodeMaster *genm, ByteStreamIndex *bsi)
{
    NodeMaster operation = stack->UNION.Nodes.nodes[0];
    NodeMaster condition1 = stack->UNION.Nodes.nodes[1];
    NodeMaster condition2 = stack->UNION.Nodes.nodes[2];
    
    enum _TYPE_ oper_type = (operation.utype == NODES) ? operation.UNION.Nodes.type : operation.UNION.Node.type;
    enum _TYPE_ cond1_type = (condition1.utype == NODES) ? condition1.UNION.Nodes.type : condition1.UNION.Node.type; 
    enum _TYPE_ cond2_type = (condition2.utype == NODES) ? condition2.UNION.Nodes.type : condition2.UNION.Node.type;
    
    switch (cond1_type)
        {
            case ID_LINKER:
                bytecode_IDlinker_func(&condition1.UNION.Nodes.nodes[0], bs, gem, genm, bsi);
                
                Instruction variable = CMDcreate_isn(_LOAD, "", __CONSUME__);
                
                break;
            case NUMBER:
                bytecode_literal(&condition1, bs, gem, genm, bsi);
                break;
            case IDENTIFIER:
                bytecode_identifier(&condition1, bs, gem, genm, bsi);
                break;
            case LIST_ACC:
                int accesses = 0;
                bytecode_listacc(&condition1, bs, gem, genm, bsi, &accesses);
                        
                char *seq = malloc(sizeof(char)*(int)log10(accesses));
                sprintf(seq, "%d", accesses);
                
                Instruction loadseq = CMDcreate_isn(_LOAD_SEQ, seq, __NOFLAG__);
                ai_stream(bs, loadseq);
                
                Instruction push = CMDcreate_isn(_PUSH, "", __NOFLAG__);
                ai_stream(bs, push);
                break;
            case FUNCTION_CALL:
                break;  
        }
    
    switch (cond2_type)
        {
            case ID_LINKER:
                bytecode_IDlinker_func(&condition1.UNION.Nodes.nodes[0], bs, gem, genm, bsi);
                
                Instruction variable = CMDcreate_isn(_LOAD, "", __CONSUME__);
                
                break;
            case NUMBER:
                bytecode_literal(&condition2, bs, gem, genm, bsi);
                break;
            case IDENTIFIER:
                bytecode_identifier(&condition2, bs, gem, genm, bsi);
            case LIST_ACC:
                int accesses = 0;
                bytecode_listacc(&condition2, bs, gem, genm, bsi, &accesses);
                        
                char *seq = malloc(sizeof(char)*(int)log10(accesses));
                sprintf(seq, "%d", accesses);
                
                Instruction loadseq = CMDcreate_isn(_LOAD_SEQ, seq, __NOFLAG__);
                ai_stream(bs, loadseq);
                
                Instruction push = CMDcreate_isn(_PUSH, "", __NOFLAG__);
                ai_stream(bs, push);
                break;
            case FUNCTION_CALL:
                break; 
        }
        
    enum _COMMAND_ cmd_type;
    switch (oper_type)
        {
            case GTHAN:
                cmd_type = _GT;
                break;
            case LTHAN:
                cmd_type = _LT;
                break;
            case GTHAN_OR_EQUAL_TO:
                cmd_type = _GTET;
                break;
            case LTHAN_OR_EQUAL_TO:
                cmd_type = _LTET;
                break;
            case EQUIVALENT:
                break;
        }
    
    Instruction oper_cmd = CMDcreate_isn(cmd_type, "", __NOFLAG__);
    ai_stream(bs, oper_cmd);   
    Instruction push = CMDcreate_isn(_PUSH, "", __NOFLAG__);
    ai_stream(bs, push);
}

void bytecode_condition(NodeMaster *stack, ByteStream *bs,  Genmem *gem, NodeMaster *genm, ByteStreamIndex *bsi)
{
    // we need to evaluate the condition from the inside
    // out, starting with the most inner parts first.
    // we will achieve this with recursion, as usual 
    
    for (int i = 0; i < stack->UNION.Nodes.nodeln; ++i)
        {
            NodeMaster node = stack->UNION.Nodes.nodes[i];
            
            enum _TYPE_ node_type = (node.utype == NODES) ? node.UNION.Nodes.type : node.UNION.Node.type; 
            switch (node_type)
                {
                    // NODES cases
                    case NOT:
                        bytecode_condition(&node, bs,  gem, genm, bsi);
                        // if its a NOT then we know for a fact that it has
                        // child nodes, so we can safely use Nodes.nodes...
                        bytecode_not(node.UNION.Nodes.nodes, bs, gem, genm, bsi);
                        break;
                    case EXPRESSION:
                        bytecode_condition(&node, bs,  gem, genm, bsi);
                        break;
                    case OPERATION:
                        bytecode_subcondition(&node, bs, gem, genm, bsi);
                        break;
                    case LIST:
                        break;
                }   
        }
}

void bytecode_loop(NodeMaster *stack, ByteStream *bs,  Genmem *gem, NodeMaster *genm, ByteStreamIndex *bsi)
{
    // a condition always has two EXPRESSION child nodes,
    // so we'll just skip over the first one to reach the
    // general outside condition
    //Instruction jump_cmd_condition;
 
    NodeMaster condition_nodes = stack->UNION.Nodes.nodes[0];   
    bytecode_condition(&condition_nodes, bs, gem, genm, bsi);//, &jump_cmd_condition);
    // if the condition is true, we jump to the next line,
    // allowing us to evaluate the if-statement scope
    // if false, we jump past it. if theres an else if or
    // just an else, we jump to that evaulation
    
    // after evaluating the if-scope, we take its total length
    // and then use that to inform the jump_cmd_condition how
    // far to jump ahead if the condition is evaluated as false
    
    // we get the total length of the scope so that we can inform
    // the JUMP command how far back, relatively, to go depending
    // on the condition
    
    Instruction jump_forward = CMDcreate_isn(_JUMP_IF_FALSE, "", __NOFLAG__);
    ai_stream(bs, jump_forward);
    
    int buf = bs->streamln;
     
    NodeMaster scope_nodes = stack->UNION.Nodes.nodes[1];
    traverse(&scope_nodes, bs,  gem, genm, bsi);
    // the number '3' and '4' that are used are to offset the
    // jump commands so that they dont hit the _START_SCOPE 
    // and _END_SCOPE
    address = bs->streamln - buf + 2;

    char *cseq = malloc(sizeof(char)*(int)log10(address - 3));
    sprintf(cseq, "%d", address - 3);
    Instruction jump_backward = CMDcreate_isn(_JUMP_IF_TRUE, cseq, __NOFLAG__);
    ai_stream(bs, jump_backward);
    
    // we need to retrospecively go back and alter _JMP_IF_FALSE so
    // it knows how many lines, relatively, to jump forward
    char *cseq2 = malloc(sizeof(char)*(int)log10(address));
    sprintf(cseq2, "%d", address);
    bs->stream[bs->streamln - (bs->streamln - buf) - 1].data.cmd.val = cseq2;
    
    // JMP_IF_FALSE (jump forward if the condition isnt true to begin with)
    // until-loop scope
    // JMP_IF_TRUE (jump backwards if the condition is still true after looping)
}

/*
STACK: [RETURN]
        NODE: [RETURN]------->[ret]
        STACK: [EXPRESSION]
                STACK: [EXPRESSION]
                        NODE: [IDENTIFIER]--->[arg]
*/

void bytecode_return(NodeMaster *stack, ByteStream *bs,  Genmem *gem, NodeMaster *genm, ByteStreamIndex *bsi)
{
    // the first two are always an EXPRESSION, so we
    // will skip past them
    NodeMaster retval = stack->UNION.Nodes.nodes[0];//.UNION.Nodes.nodes[0];
    // args has a lot of general uses in the context
    // of looping through either one or a few things
    bytecode_args(&retval, bs, gem, genm, bsi);
}

void traverse(NodeMaster *stack, ByteStream *bs,  Genmem *gem, NodeMaster *genm, ByteStreamIndex *bsi)
{
    Instruction scope_start = CMDcreate_isn(_SCOPE_START, "", __NOFLAG__);
    Instruction scope_end = CMDcreate_isn(_SCOPE_END, "", __NOFLAG__);
    
    ai_stream(bs, scope_start);
    for (int i = 0; i < stack->UNION.Nodes.nodeln; ++i)
        {
            NodeMaster node_s = stack->UNION.Nodes.nodes[i];
            // this function ONLY loops through NODES, not NODE's
            enum _TYPE_ stack_type = node_s.UNION.Nodes.type;
            
            switch (stack_type)
                {
                    case ASSIGNMENT:
                        bytecode_assignment(&node_s, bs, gem, genm, bsi);
                        break;
                    case ID_LINKER:
                        // this case COULD be a few things
                        // most likely, itll be a function call such as
                        // player.obj...function();
                        // a linker will never stand alone, such as
                        // player.box.x <-- simply a statement and will 
                        // be ignored by the compiler
                        
                        
                        // we will ASSUME A FUNCTION CALL
                        bytecode_IDlinker_func(&node_s.UNION.Nodes.nodes[0], bs, gem, genm, bsi);
                        break;
                    case FUNCTION_CALL:
                        bytecode_func(&node_s, bs, gem, genm, bsi);
                        break;
                    case SCOPE:                       
                        traverse(node_s.UNION.Nodes.nodes, bs,  gem, genm, bsi);
                        break;
                    case LOOP:
                        bytecode_loop(&node_s, bs,  gem, genm, bsi);
                        break;
                    case IF:
                        int placeholder = 0;
                        bytecode_if(&node_s, bs,  gem, genm, bsi, &placeholder);
                        break;
                    case ELSE:
                        int placeholder2 = 0;
                        bytecode_if(&node_s.UNION.Nodes.nodes[0], bs,  gem, genm, bsi, &placeholder2);
                        break;
                    case DEF_FN:
                        bytecode_fndef(&node_s, bs,  gem, genm, bsi);
                        break;
                    case RETURN:
                        bytecode_return(&node_s, bs,  gem, genm, bsi);
                        break;
                    case DEF_CLS:
                        debugp("hit a class defintion");
                        bytecode_classdef(&node_s, bs,  gem, genm, bsi);
                        break;
                    //case EXPRESSION:
                     //   traverse(node_s.UNION.Nodes.nodes, bs,  gem, genm, bsi);
                    default:
                        printf("default type: %s\n", type_names[stack_type]);
                        perror("Hit a default");
                        exit(EXIT_FAILURE);
                        break;
                }
        }
    ai_stream(bs, scope_end);
}
/* if stack reference

STACK: [IF]
        NODE: [IF]----------->[if]
        STACK: [CONDITION]
                STACK: [EXPRESSION]
                        STACK: [OPERATION]
                                NODE: [GTHAN]-------->[>]
                                NODE: [IDENTIFIER]--->[x]
                                NODE: [NUMBER]------->[5]
        STACK: [SCOPE]
                STACK: [ASSIGNMENT]
                        NODE: [ADD_EQ]------->[+=]
                        NODE: [IDENTIFIER]--->[a]
                        NODE: [NUMBER]------->[1]
                        
*/
void bytecode_else(NodeMaster *stack, ByteStream *bs,  Genmem *gem, NodeMaster *genm, ByteStreamIndex *bsi, int *pre_ifelse_statement)
{
    int buf = bs->streamln;
    
    traverse(stack, bs, gem, genm, bsi);
    
    *pre_ifelse_statement = bs->streamln - buf;
}

void bytecode_if(NodeMaster *stack, ByteStream *bs,  Genmem *gem, NodeMaster *genm, ByteStreamIndex *bsi, int *pre_ifelse_statement)
{
    // an IF stack is always orientated like this
    NodeMaster if_node = stack->UNION.Nodes.nodes[0];
    NodeMaster condition = stack->UNION.Nodes.nodes[1];
    NodeMaster scope = stack->UNION.Nodes.nodes[2];
    
    // we generate the condition
    int pre = bs->streamln;
    bytecode_condition(&condition, bs, gem, genm, bsi);
    int curr_condition_length = bs->streamln - pre;
    
    // this one jumps to the next else-if statement
    Instruction jump_forward = CMDcreate_isn(_JUMP_IF_FALSE, "", __NOFLAG__);
    ai_stream(bs, jump_forward);
    int jiff_address = bs->streamln - 1;
    
    int buf = bs->streamln;
    
    // convert the scope into bytecode
    traverse(&scope, bs, gem, genm, bsi);
    
    int curr_scope_length = bs->streamln - buf;
    
    // this is for any potential parent if-else statement
    // that needs to know the length of this inner scope
    *pre_ifelse_statement = curr_condition_length;
    
    address = bs->streamln - buf + 2;
    char *cseq = malloc(sizeof(char)*(int)log10(address));
    sprintf(cseq, "%d", address);
    bs->stream[bs->streamln - (bs->streamln - buf) - 1].data.cmd.val = cseq;
    
    // this one jumps to either the else statement or after the if statement
    Instruction jump_to_end = CMDcreate_isn(_JUMP, "0", __NOFLAG__);
    ai_stream(bs, jump_to_end);
    int jift_address = bs->streamln - 1; 
    
    // we check if there's a possible else or else-if statement
    buf = bs->streamln;
    int inner_scope_length = 0;
    if (stack->utype == NODES)
        {
            int ln = stack->UNION.Nodes.nodeln;
            for (int i = 0; i < ln; ++i)
                {
                    NodeMaster elseif_node = stack->UNION.Nodes.nodes[3 + i];
                    if (elseif_node.utype == NODES)
                        {
                            if (elseif_node.UNION.Nodes.nodes[0].UNION.Nodes.type == IF)
                                {
                                    bytecode_if(&elseif_node.UNION.Nodes.nodes[0], bs, gem, genm, bsi, &inner_scope_length);
                                }
                            else 
                                {
                                    // hit an else-only block
                                    bytecode_else(&elseif_node.UNION.Nodes.nodes[0], bs, gem, genm, bsi, &inner_scope_length);
                                }
                        }
                }
        }  
    // if the if-statement is evaluated as true, then we need
    // to jump to the very end and skip over all of the other
    // if-else statements, which is what this achieves
    int total_jump = bs->streamln - buf + 1;
    char *cseq3 = malloc(sizeof(char)*(int)log10(total_jump));
    sprintf(cseq3, "%d", total_jump);
    bs->stream[bs->streamln - (bs->streamln - buf) - 1].data.cmd.val = cseq3;
}

/* for reference again obv

STACK: [DEF_FN]
        STACK: [FUNCTION_CALL]
                NODE: [IDENTIFIER]--->[myfunc]
                STACK: [ARGUMENTS]
                        STACK: [EXPRESSION]
                                NODE: [IDENTIFIER]--->[arg]
        STACK: [SCOPE]
                STACK: [ASSIGNMENT]
                        NODE: [ADD_EQ]------->[+=]
                        NODE: [IDENTIFIER]--->[arg]
                        NODE: [NUMBER]------->[1]
                STACK: [RETURN]
                        NODE: [RETURN]------->[ret]
                        STACK: [EXPRESSION]
                                STACK: [EXPRESSION]
                                        NODE: [IDENTIFIER]--->[arg]
                                        
*/

void bytecode_fndef_args(NodeMaster *node_s, ByteStream *bs, Genmem *gem, NodeMaster *genm, ByteStreamIndex *bsi)
{   
    // there are no arguments, so we simply return
    if (node_s->UNION.Nodes.nodeln < 1) return;
    
    // we reverse the order because we're copying in loaded
    // variables/literals from the stack into the newly named
    // argument names, thus this must go in reverse order
    for (int i = node_s->UNION.Nodes.nodeln - 1; i >= 0; --i)
        {
            NodeMaster subnode;
            // every single arguments has EXPRESSION as a parent node
            // so we ignore it and skip past it
            if (node_s->UNION.Nodes.nodes[i].UNION.Nodes.type == EXPRESSION)
                {
                    subnode = node_s->UNION.Nodes.nodes[i].UNION.Nodes.nodes[0];
                }
            else
                {
                    subnode = node_s->UNION.Nodes.nodes[i];
                }
            
            if (subnode.utype == NODES)
                {
                    switch (subnode.UNION.Nodes.type)
                        {
                            case OPERATION:
                                //bytecode_expr();
                                break;
                            case FUNCTION_CALL:
                                bytecode_func(&subnode, bs, gem, genm, bsi);
                                break;
                            case ID_LINKER:
                                // remember, we go straight to index '0' because an ID_LINKER
                                // always has a stupid, redudant ID_LINKER parent stack that 
                                // does nothing but make life a little bit more complicated than it 
                                // has to be 
                                int linker_cnt = 0;
                                bytecode_IDlinker(&subnode.UNION.Nodes.nodes[0], bs, gem, genm, bsi, &linker_cnt);
                                
                                char *seq = malloc(sizeof(char)*(int)log10(linker_cnt));
                                sprintf(seq, "%d", linker_cnt);
                                
                                Instruction loadseq = CMDcreate_isn(_LOAD_SEQ, seq, __NOFLAG__);
                                ai_stream(bs, loadseq);
                                
                                Instruction push = CMDcreate_isn(_PUSH, "", __NOFLAG__);
                                ai_stream(bs, push);
                                
                                Instruction call = CMDcreate_isn(_CALL, "", __NOFLAG__);
                                ai_stream(bs, call);
                                break;
                            case LIST_ACC:
                                int counter = 0;
                                bytecode_listacc(&subnode, bs, gem, genm, bsi, &counter);

                                char *__seq = malloc(sizeof(char)*(int)log10(counter));
                                sprintf(__seq, "%d", counter);
                                Instruction __loadseq = CMDcreate_isn(_LOAD_SEQ, __seq, __NOFLAG__);
                                ai_stream(bs, __loadseq);

                                Instruction __push = CMDcreate_isn(_PUSH, "", __NOFLAG__);
                                ai_stream(bs, __push);
                                
                                break;
                            case LIST:
                                bytecode_args(&subnode, bs, gem, genm, bsi);
                                
                                char *_seq = malloc(sizeof(char)*(int)log10(subnode.UNION.Nodes.nodeln));
                                sprintf(_seq, "%d", subnode.UNION.Nodes.nodeln);
                                Instruction _loadseq = CMDcreate_isn(_LOAD_SEQ, _seq, __NOFLAG__);
                                ai_stream(bs, _loadseq);
                                
                                Instruction _push = CMDcreate_isn(_PUSH, "", __NOFLAG__);
                                ai_stream(bs, _push);
                                
                                break;
                        }
                }
            else
                {
                    switch (subnode.UNION.Node.type)
                        {
                            case IDENTIFIER:
                                bytecode_identifier(&subnode, bs, gem, genm, bsi);
                                break;
                            case NUMBER:
                            case STRING:
                                bytecode_literal(&subnode, bs, gem, genm, bsi);
                                break;
                            default:
                                debugp("HERE??");
                                break;
                        }
                }
        }
}

void bytecode_fndef(NodeMaster *stack, ByteStream *bs,  Genmem *gem, NodeMaster *genm, ByteStreamIndex *bsi)
{
    // =================================================
    // REMEMBER: This function is not 'defining'; it is
    // the actual code to run the function when it's 
    // called. we're just saving it to a dedicated
    // bytestream!
    // =================================================
    
    // first we make room in the 'bsi' so that we can append
    // another function's bytecode to it.
    NodeMaster fndef = stack->UNION.Nodes.nodes[0];
    char *namefn = fndef.UNION.Nodes.nodes[0].UNION.Node.stor;
    NodeMaster args = fndef.UNION.Nodes.nodes[1];
    NodeMaster scope = stack->UNION.Nodes.nodes[1];
    // we want to copy whats into the stack into the
    // arguments, as they have been renamed
    ByteStream fnbs; 
    fnbs.stream = NULL;
    fnbs.streamln = 0;
    fnbs.streamsz = 0;
    
    // load in the arguments (or rather copy from the stack)
    bytecode_fndef_args(&fndef, &fnbs, gem, genm, bsi);
    
    // load in the scope
    //printf("idk mannn %s\n", type_names[scope.UNION.Nodes.type]);
    traverse(&scope, &fnbs, gem, genm, bsi);
    
    // allocate room in the bsi for the fn-bytestream
    bsi->size += sizeof(fnbs);
    bsi->bytestream = realloc(bsi->bytestream, bsi->size);
    // setting the name inside of the table
    bsi->table = realloc(bsi->table, bsi->size);
    bsi->table[bsi->len] = NULL;
    bsi->table[bsi->len] = realloc(bsi->table[bsi->len], sizeof(namefn));
    bsi->table[bsi->len] = namefn;
    // actually appending the fnbs into the bytestream array
    bsi->bytestream[bsi->len] = fnbs;
    
    bsi->len++;  
}

/* for reference, again again, obv

STACK: [DEF_CLS]
        NODE: [IDENTIFIER]--->[Something]
        STACK: [METADATA]
                NODE: [INIT]--------->[_init]
                STACK: [META_SCOPE]
                        STACK: [ID_LINKER]
                                STACK: [ID_LINKER]
                                        NODE: [DECIMAL]------>[.]
                                        NODE: [IDENTIFIER]--->[this]
                                        NODE: [IDENTIFIER]--->[obj1]
                        STACK: [ID_LINKER]
                                STACK: [ID_LINKER]
                                        NODE: [DECIMAL]------>[.]
                                        NODE: [IDENTIFIER]--->[this]
                                        NODE: [IDENTIFIER]--->[obj2]
        STACK: [SCOPE]
                STACK: [DEF_FN]
                        STACK: [FUNCTION_CALL]
                                NODE: [IDENTIFIER]--->[change_obj1]
                                STACK: [ARGUMENTS]
                                        STACK: [EXPRESSION]
                                                NODE: [IDENTIFIER]--->[change_to]
                        STACK: [SCOPE]
                                STACK: [ASSIGNMENT]
                                        NODE: [EQUAL]-------->[=]
                                        STACK: [ID_LINKER]
                                                STACK: [ID_LINKER]
                                                        NODE: [DECIMAL]------>[.]
                                                        NODE: [IDENTIFIER]--->[this]
                                                        NODE: [IDENTIFIER]--->[obj1]
                                        NODE: [IDENTIFIER]--->[change_to]
                                                
*/

void bytecode_classdef(NodeMaster *stack, ByteStream *bs,  Genmem *gem, NodeMaster *genm, ByteStreamIndex *bsi)
{
    ByteStream classbs; 
    classbs.stream = NULL;
    classbs.streamln = 0;
    classbs.streamsz = 0;
    
    // we loop through and check.
    // this means that the meta function doesn't have to be
    // at the top, and it also means you can initiate
    // an empty class if you wanted, for some reason
    // beknown to me
    char *class_name;
    for (int i = 0; i < stack->UNION.Nodes.nodeln; ++i)
        {
            NodeMaster curr_node = stack->UNION.Nodes.nodes[i];
            enum _TYPE_ ntype = (curr_node.utype == NODES) ? curr_node.UNION.Nodes.type : curr_node.UNION.Node.type;
            switch (ntype)
                {
                     case IDENTIFIER:
                         // this is simply where the programmer has typed
                         // the name of the class. 
                         // this is where we create a hashmap of all of
                         // its data, so we call _CLASS_INIT to do so
                         
                         class_name = strdup(curr_node.UNION.Node.stor);
                         Instruction init = CMDcreate_isn(_CLASS_INIT, class_name, __NOFLAG__);
                         ai_stream(&classbs, init);
                         break;
                          
                     case INHERITS:
                         // we basically call the other class and store
                         // it in this class' hashmap
                         char *inherit_classes_name = strdup(curr_node.UNION.Node.stor);
                         
                         // btw, i know it says func... leave me alone, i'm stupid, i get it
                         int namecls_index = search_func(bsi, inherit_classes_name);
                         printf("inherited class name is: %s %d\n", inherit_classes_name, namecls_index);
                         // - 1 means we weren't able to find the function in the array
                         if (namecls_index < 0)
                             {
                                 perror("Inherited Class doesn't exist");
                                 exit(EXIT_FAILURE);
                             }
                        //                               'its' just converts an int to string
                         char *string = malloc(sizeof(char)*(int)log10(namecls_index + 1));
                         sprintf(string, "%d", namecls_index);
                         Instruction call = CMDcreate_isn(_CALL, string, __INHERITS__);
                         ai_stream(&classbs, call);
                         break;
                     case METADATA:
                         enum _TYPE_ metatype = curr_node.UNION.Nodes.nodes[0].UNION.Node.type;
                         NodeMaster meta_scope = curr_node.UNION.Nodes.nodes[1];
                         switch (metatype)
                             {
                                 case INIT:
                                     // _STO_SEQUEL stores any proceeding scope into something
                                     Instruction create_objs = CMDcreate_isn(_STO_SEQUEL, class_name, __GIVETOCLASS__);
                                     traverse(&meta_scope, &classbs, gem, genm, bsi);
                                     break;
                                     
                                 default:
                                     perror("No such metatype exists");
                                     exit(EXIT_FAILURE);
                             }
                         break;
                     case SCOPE:
                         Instruction create_objs = CMDcreate_isn(_STO_SEQUEL, class_name, __GIVETOCLASS__);
                         traverse(&curr_node, &classbs, gem, genm, bsi);
                         break;
                     
                     default:
                         perror("TYPE in class does not exist");
                         exit(EXIT_FAILURE);           
                }
        }
}

ByteStream compile(NodeMaster *stack)
{
    Genmem gem;
    gem.types = NULL;
    gem.ln = 0;
    gem.size = 0;
    
    NodeMaster genm;
    genm.utype = NODES;
    genm.UNION.Nodes.nodes = NULL;
    genm.UNION.Nodes.nodeln = 0;
    
    // functions and classes share the same dedicated bytestream
    ByteStreamIndex bsi;
    bsi.bytestream = NULL;
    bsi.len = 0;
    bsi.size = 0;
    bsi.table = NULL;
    
    ByteStream bs;
    bs.stream = NULL;
    bs.streamln = 0;
    bs.streamsz = 0;
    
    debugp("starting bytecode translation...");
    traverse(stack, &bs, &gem, &genm, &bsi);
    
    printf("\n\n======== COMPLETED BYTECODE TRANSLATION ========\n\n");
    
    for (int i = 0; i < bs.streamln; ++i)
        {
            Instruction isn = bs.stream[i];
                    
             if (isn.isnt == __CMD)
                 {
                     enum _COMMAND_ cmd = isn.data.cmd.cmdt;
                     printf("%s", cmdt_types[cmd]);
                     //printf(" %s, ", isn.data.cmd.val);
                     if (isn.data.cmd.val != "") printf(" %s, ", isn.data.cmd.val);
                     if (isn.data.cmd.flag != __NOFLAG__) printf(" %s, ", flags[isn.data.cmd.flag]);
                     //else printf("\n");
                     printf("\n");
                     //if ((cmd == _PUSH || cmd == _LOAD_SEQ || cmd == _CALL) && isn.data.cmd.val == "") printf("\n");
                     //printf("\n\n");
                     //else if (cmd == _SCOPE_START || cmd == _SCOPE_END || cmd == _JMP_IF_TRUE || cmd == _JMP_IF_FALSE || cmd == _STO_SEQ || cmd == _PROVIDE) printf("\n");
                 }
             else if (isn.isnt == __MEM)
                 {
                     
                 }
             else 
                 {
                     void *p = isn.data.con.val;
                     if (isn.data.con.datat == _INT_) printf("%d ", (*(int *)p)); 
                     else if (isn.data.con.datat == _STRING_) printf("%s ", (char *)p);
                     if (isn.data.con.datat == _FLOAT_) printf("%f ", (*(float *)p));
                     printf("%s, ", data_types[isn.data.con.datat]);
                     printf("\n");
                     
                     
                     //printf("\n\n");
                 } 
             //printf("\n"); 
        }
        
    return bs;
}