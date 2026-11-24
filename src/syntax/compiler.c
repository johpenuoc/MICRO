#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <math.h>

#include "coms/common.h"


ByteStream mbs_init();
void mbs_reset(ByteStream *bs);

void astream(ByteStream *bs, Instruction isn);

void bytecode_identifier(NodeMaster *node, ByteStream *bs, Genmem *gem, NodeMaster *genm);
void bytecode_args(NodeMaster *node_s, ByteStream *bs, Genmem *gem, NodeMaster *genm);
void bytecode_func(NodeMaster *node_s, ByteStream *bs, Genmem *gem, NodeMaster *genm);
void bytecode_IDlinker_func(NodeMaster *node_s, ByteStream *bs, Genmem *gem, NodeMaster *genm);
void bytecode_IDlinker(NodeMaster *node_s, ByteStream *bs, Genmem *gem, NodeMaster *genm, int *linker_cnt);
void bytecode_condition(NodeMaster *stack, ByteStream *bs,  Genmem *gem, NodeMaster *genm);
void traverse(NodeMaster *stack, ByteStream *bs,  Genmem *gem, NodeMaster *genm);

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
    
    return isn;
}

void ai_stream(ByteStream *bs, Instruction ins)
{
    bs->streamsz += sizeof(ins);
    bs->stream = realloc(bs->stream, bs->streamsz);
    bs->stream[bs->streamln] = ins;
    bs->streamln++;
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

void bytecode_listacc(NodeMaster *node, ByteStream *bs, Genmem *gem, NodeMaster *genm, int *accesses)
{
    // we first grab all of the indexes, similar to 
    // linkers and functions with arguments
    bytecode_args(&node->UNION.Nodes.nodes[1], bs, gem, genm);
    *accesses = node->UNION.Nodes.nodes[1].UNION.Nodes.nodeln + 1;

    char *idname = node->UNION.Nodes.nodes[0].UNION.Node.stor;
    //Instruction load = CMDcreate_isn(_LOAD, idname);
    //ai_stream(bs, load);
    Instruction string = CONcreate_isn(_STRING_, idname);
    ai_stream(bs, string);
    
    Instruction push = CMDcreate_isn(_PUSH, "");
    ai_stream(bs, push);
}

void bytecode_identifier(NodeMaster *node, ByteStream *bs, Genmem *gem, NodeMaster *genm)
{   
    char *id = node->UNION.Node.stor;
    Instruction load = CMDcreate_isn(_LOAD, id);
    ai_stream(bs, load);
    
    Instruction push = CMDcreate_isn(_PUSH, "");
    ai_stream(bs, push);
}

void bytecode_literal(NodeMaster *node, ByteStream *bs, Genmem *gem, NodeMaster *genm)
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
    
    Instruction push = CMDcreate_isn(_PUSH, "");
    ai_stream(bs, push);
}

void bytecode_args(NodeMaster *node_s, ByteStream *bs, Genmem *gem, NodeMaster *genm)
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
                                bytecode_func(&subnode, bs, gem, genm);
                                break;
                            case ID_LINKER:
                                // remember, we go straight to index '0' because an ID_LINKER
                                // always has a stupid, redudant ID_LINKER parent stack that 
                                // does nothing but make life a little bit more complicated than it 
                                // has to be 
                                int linker_cnt = 0;
                                bytecode_IDlinker(&subnode.UNION.Nodes.nodes[0], bs, gem, genm, &linker_cnt);
                                
                                char *seq = malloc(sizeof(char)*(int)log10(linker_cnt));
                                sprintf(seq, "%d", linker_cnt);
                                
                                Instruction loadseq = CMDcreate_isn(_LOAD_SEQ, seq);
                                ai_stream(bs, loadseq);
                                
                                Instruction push = CMDcreate_isn(_PUSH, "");
                                ai_stream(bs, push);
                                
                                Instruction call = CMDcreate_isn(_CALL, "");
                                ai_stream(bs, call);
                                break;
                            case LIST_ACC:
                                int counter = 0;
                                bytecode_listacc(&subnode, bs, gem, genm, &counter);

                                char *__seq = malloc(sizeof(char)*(int)log10(counter));
                                sprintf(__seq, "%d", counter);
                                Instruction __loadseq = CMDcreate_isn(_LOAD_SEQ, __seq);
                                ai_stream(bs, __loadseq);

                                Instruction __push = CMDcreate_isn(_PUSH, "");
                                ai_stream(bs, __push);
                                
                                break;
                            case LIST:
                                bytecode_args(&subnode, bs, gem, genm);
                                
                                char *_seq = malloc(sizeof(char)*(int)log10(subnode.UNION.Nodes.nodeln));
                                sprintf(_seq, "%d", subnode.UNION.Nodes.nodeln);
                                Instruction _loadseq = CMDcreate_isn(_LOAD_SEQ, _seq);
                                ai_stream(bs, _loadseq);
                                
                                Instruction _push = CMDcreate_isn(_PUSH, "");
                                ai_stream(bs, _push);
                                
                                break;
                        }
                }
            else
                {
                    switch (subnode.UNION.Node.type)
                        {
                            case IDENTIFIER:
                                bytecode_identifier(&subnode, bs, gem, genm);
                                break;
                            case NUMBER:
                            case STRING:
                                bytecode_literal(&subnode, bs, gem, genm);
                                break;
                            default:
                                debugp("HERE??");
                                break;
                        }
                }
        }
}

void bytecode_func(NodeMaster *node_s, ByteStream *bs, Genmem *gem, NodeMaster *genm)
{
    // we want to load any arguments first, of course
    NodeMaster arg_stack = node_s->UNION.Nodes.nodes[1];
    bytecode_args(&arg_stack, bs, gem, genm);
    
    // at index 1 (true index is list[0]), will always be the function name 
    char *func_name = node_s->UNION.Nodes.nodes[0].UNION.Node.stor;
    Instruction load = CMDcreate_isn(_LOAD, func_name);
    ai_stream(bs, load);
    Instruction push = CMDcreate_isn(_PUSH, "");
    ai_stream(bs, push);
    Instruction call = CMDcreate_isn(_CALL, "");
    ai_stream(bs, call);
}

void bytecode_IDlinker_func(NodeMaster *node_s, ByteStream *bs, Genmem *gem, NodeMaster *genm)
{
    // ==========================================================
    // THIS FUNCTION ACTS AS A WRAPPER FOR THE BYTECODE_IDLINKER
    // BY CONTAINING THE LINKERS AND CONDITIONS ON WHETHER IT IS
    // A FUNCTION CALL OR NOT
    // ==========================================================
    
    int links = 0;
    bytecode_IDlinker(node_s, bs, gem, genm, &links);
    
    bool non_regular = false;
    if (gem->ln > 0)
        {
            if (gem->types[gem->ln - 1] == FUNCTION_CALL)
                {
                    // this means that there was a function at the end
                    // of the linker, such as: player.box.move();
                    
                    char *seq = malloc(sizeof(char)*(int)log10(links));
                    sprintf(seq, "%d", links);
                    
                    Instruction loadseq = CMDcreate_isn(_LOAD_SEQ, seq);
                    ai_stream(bs, loadseq);
                    
                    // after loading the hashmap keys, we push whats loaded onto the stack
                    Instruction push = CMDcreate_isn(_PUSH, "");
                    ai_stream(bs, push);
                    // we then call it because it is a function
                    Instruction call = CMDcreate_isn(_CALL, "");
                    ai_stream(bs, call);
                    
                    non_regular = true;
                }
        }
    if (!non_regular)
        {
            char *seq = malloc(sizeof(char)*(int)log10(links));
            sprintf(seq, "%d", links);
            
            Instruction loadseq = CMDcreate_isn(_LOAD_SEQ, seq);
            ai_stream(bs, loadseq);
            
            Instruction push = CMDcreate_isn(_PUSH, "");
            ai_stream(bs, push);
        }
}

void bytecode_IDlinker(NodeMaster *node_s, ByteStream *bs, Genmem *gem, NodeMaster *genm, int *linker_cnt)
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
            
            Instruction push = CMDcreate_isn(_PUSH, "");
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
                    bytecode_args(&args_nodes, bs, gem, genm);
                    
                    // we still add it to the LOAD_SEQ
                    // =====================================================
                    NodeMaster node = subsublink.UNION.Nodes.nodes[0];

                    Instruction literal = CONcreate_isn(_STRING_, (void *)node.UNION.Node.stor);
                    ai_stream(bs, literal);
                    
                    Instruction push = CMDcreate_isn(_PUSH, "");
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
            bytecode_IDlinker(&sublink, bs, gem, genm, linker_cnt);
        }
    else
        {             
            // this means that there is also a third node which
            Instruction _literal = CONcreate_isn(_STRING_, (void *)sublink.UNION.Node.stor);
            ai_stream(bs, _literal);
            
            Instruction push = CMDcreate_isn(_PUSH, "");
            ai_stream(bs, push);
            (*linker_cnt)++;
        }
}

void bytecode_assignment(NodeMaster *node_s, ByteStream *bs, Genmem *gem, NodeMaster *genm)
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
    switch (oper_n.UNION.Node.type)
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
        }
    
    enum _COMMAND_ sto_type = _STO;
    int sto_data = 0;
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
                             
                            switch (subnode.UNION.Nodes.type)
                                {
                                    case FUNCTION_CALL:
                                        bytecode_func(&subnode, bs, gem, genm);
                                        break;
                                    case ID_LINKER:
                                        // remember, that stupid, redundant ID_LINKER
                                        // parent stack, so we use index 0 to bypass it
                                        bytecode_IDlinker_func(&subnode.UNION.Nodes.nodes[0], bs, gem, genm);
                                        break;
                                    /*case LIST_ACC:
                                        bytecode_args(&subnode, bs, gem, genm);
                                        break;*/
                                    case LIST:
                                        bytecode_args(&subnode, bs, gem, genm);
                                        sto_type = _STO_SEQ;
                                        sto_data = subnode.UNION.Nodes.nodeln;
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
            
            char *value_s = value_n.UNION.Node.stor;
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
                        debugp("hit default");
                        break;
                }
            // append the literal to the stream
            ai_stream(bs, literal);
            
            // append the PUSH command to put the literal onto the stack
            Instruction push = CMDcreate_isn(_PUSH, "");
            
            // append the push command to the stream 
            ai_stream(bs, push);
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
                        // in the parser.c, ID_LINKER always has a redudant and useless
                        // STACK ID_LINKER at the very type.
                        // it just contains a single other ID_LINKER stack inside.
                        // therefore we skip over it with 'nodes[0]'
                        bytecode_IDlinker_func(&assig_n.UNION.Nodes.nodes[0], bs, gem, genm);

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
                        char *var = "__CONSUME__";
                        Instruction variable = CMDcreate_isn(sto_type, var);
                        
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
                                    Instruction seq = CMDcreate_isn(_PROVIDE, cseq);
                                    ai_stream(bs, seq);
                                    break;
                            }
                        
                        break;
                    case LIST_ACC:
                        bytecode_listacc(&assig_n, bs, gem, genm, &sto_data);
                        
                        char *seq = malloc(sizeof(char)*(int)log10(sto_data));
                        sprintf(seq, "%d", sto_data);
                        
                        Instruction loadseq = CMDcreate_isn(_LOAD_SEQ, seq);
                        ai_stream(bs, loadseq);
                        
                        Instruction push = CMDcreate_isn(_PUSH, "");
                        ai_stream(bs, push);
                        
                        char *_var = "__CONSUME__";
                        Instruction _variable = CMDcreate_isn(sto_type, _var);
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
                                    Instruction seq = CMDcreate_isn(_PROVIDE, _cseq);
                                    ai_stream(bs, seq);
                                    break;
                            }
                        break;
                    default:
                        perror("No NODE type exists");
                        exit(EXIT_FAILURE);
                        break;
                }
        }
    else
        {
            // buf [move(somearg, 107, world.centre(y.h));]
            
            // convert straight into bytecode here
            
            // ===============================================================
            // THIS IS IF THE VALUE ON THE LEFT IN JUST A SINGLE VARIABLE LIKE
            // [name = "john"] OR [number = 10.5], NOT A LINKER
            // ===============================================================
            
            char *var = assig_n.UNION.Node.stor;
            Instruction variable = CMDcreate_isn(sto_type, var);
            
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
                        Instruction seq = CMDcreate_isn(_PROVIDE, cseq);
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

void bytecode_not(NodeMaster *stack, ByteStream *bs,  Genmem *gem, NodeMaster *genm)
{
    // we evaluate the condition inside of the NOT
    NodeMaster *node_s = (stack->utype == NODES) ? stack->UNION.Nodes.nodes : stack;
    bytecode_condition(node_s, bs, gem, genm);
    
    // we inverse whatever was calculated before it
    Instruction inverse = CMDcreate_isn(_INVERSE, "");
    ai_stream(bs, inverse);
}

void bytecode_condition(NodeMaster *stack, ByteStream *bs,  Genmem *gem, NodeMaster *genm)
{
    // we need to evaluate the condition from the inside
    // out, starting with the most inner parts first.
    // we will achieve this with recursion, as usual 
    
    NodeMaster node = stack->UNION.Nodes.nodes[0];
    
    enum _TYPE_ node_type = (node.utype == NODES) ? node.UNION.Nodes.type : node.UNION.Node.type; 
    switch (node_type)
        {
            case NOT:
                // if its a NOT then we know for a fact that it has
                // child nodes, so we can safely use Nodes.nodes...
                bytecode_not(node.UNION.Nodes.nodes, bs, gem, genm);
                break;
            case EXPRESSION:
                break;
            case OPERATION:
                break;
        }
}

void bytecode_loop(NodeMaster *stack, ByteStream *bs,  Genmem *gem, NodeMaster *genm)
{
    // a condition always has two EXPRESSION child nodes,
    // so we'll just skip over the first one to reach the
    // general outside condition
    //Instruction jump_cmd_condition;
 
    NodeMaster condition_nodes = stack->UNION.Nodes.nodes[0];   
    bytecode_condition(&condition_nodes, bs, gen, genm);//, &jump_cmd_condition);
    // if the condition is true, we jump to the next line,
    // allowing us to evaluate the if-statement scope
    // if false, we jump past it. if theres an else if or
    // just an else, we jump to that evaulation
     
    NodeMaster scope_nodes = stack->UNION.Nodes.nodes[1];
    
    // after evaluating the if-scope, we take its total length
    // and then use that to inform the jump_cmd_condition how
    // far to jump ahead if the condition is evaluated as false
}

void traverse(NodeMaster *stack, ByteStream *bs,  Genmem *gem, NodeMaster *genm)
{
    Instruction scope_start = CMDcreate_isn(_SCOPE_START, "");
    Instruction scope_end = CMDcreate_isn(_SCOPE_END, "");
    
    ai_stream(bs, scope_start);
    for (int i = 0; i < stack->UNION.Nodes.nodeln; ++i)
        {
            NodeMaster node_s = stack->UNION.Nodes.nodes[i];
            // this function ONLY loops through NODES, not NODE's
            enum _TYPE_ stack_type = node_s.UNION.Nodes.type;
            
            switch (stack_type)
                {
                    case ASSIGNMENT:
                        bytecode_assignment(&node_s, bs, gem, genm);
                        break;
                    case ID_LINKER:
                        // this case COULD be a few things
                        // most likely, itll be a function call such as
                        // player.obj...function();
                        // a linker will never stand alone, such as
                        // player.box.x <-- simply a statement and will 
                        // be ignored by the compiler
                        
                        
                        // we will ASSUME A FUNCTION CALL
                        bytecode_IDlinker_func(&node_s.UNION.Nodes.nodes[0], bs, gem, genm);
                        break;
                    case FUNCTION_CALL:
                        bytecode_func(&node_s, bs, gem, genm);
                        break;
                    case SCOPE:                       
                        traverse(node_s.UNION.Nodes.nodes, bs,  gem, genm);
                        break;
                    default:
                        perror("Hit a default. Line 754 at the time of 24/11.");
                        exit(EXIT_FAILURE);
                        break;
                }
        }
    ai_stream(bs, scope_end);
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
    
    ByteStream bs;
    //create_ibs(&bs);
    bs.stream = NULL;
    bs.streamln = 0;
    bs.streamsz = 0;
    traverse(stack, &bs, &gem, &genm);
    
    printf("\n\n");
    
    for (int i = 0; i < bs.streamln; ++i)
        {
            Instruction isn = bs.stream[i];
                    
             if (isn.isnt == __CMD)
                 {
                     enum _COMMAND_ cmd = isn.data.cmd.cmdt;
                     printf("%s", cmdt_types[cmd]);
                     if (isn.data.cmd.val != "") printf(" %s, ", isn.data.cmd.val);
                     else printf("\n");
                     
                     if ((cmd == _PUSH || cmd == _LOAD_SEQ || cmd == _CALL) && isn.data.cmd.val == "") printf("\n");
                     //else if (cmd == _SCOPE_START || cmd == _SCOPE_END) printf("\n");
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
                 } 
             //printf("\n"); 
        }
        
    return bs;
}