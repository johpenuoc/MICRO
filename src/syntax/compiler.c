#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <math.h>

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

ByteStream mbs_init();
void mbs_reset(ByteStream *bs);

void astream(ByteStream *bs, Instruction isn);

void bytecode_identifier(NodeMaster *node, ByteStream *bs, Genmem *gem, NodeMaster *genm);
void bytecode_args(NodeMaster *node_s, ByteStream *bs, Genmem *gem, NodeMaster *genm);
void bytecode_func(NodeMaster *node_s, ByteStream *bs, Genmem *gem, NodeMaster *genm);
void bytecode_IDlinker_func(NodeMaster *node_s, ByteStream *bs, Genmem *gem, NodeMaster *genm);
void bytecode_IDlinker(NodeMaster *node_s, ByteStream *bs, Genmem *gem, NodeMaster *genm, int *linker_cnt);
//void bytecode_func(NodeMaster *node_s, ByteStream *bs, Genmem *gem, NodeMaster *genm);

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

/*void am_stream(ByteStream *bs, ByteStream sub_bs)
{
    bs->UNION.ms.streamsz += sizeof(sub_bs);
    bs->UNION.ms.stream = realloc(bs->UNION.ms.stream, bs->UNION.ms.streamsz);
    bs->UNION.ms.stream[bs->UNION.ms.streamln] = sub_bs;
    bs->UNION.ms.streamln++;
}*/
void ai_stream(ByteStream *bs, Instruction ins)
{
    /*bs->UNION.is.streamsz += sizeof(ins);
    bs->UNION.is.stream = realloc(bs->UNION.is.stream, bs->UNION.is.streamsz);
    bs->UNION.is.stream[bs->UNION.is.streamln] = ins;
    bs->UNION.is.streamln++;*/
    bs->streamsz += sizeof(ins);
    bs->stream = realloc(bs->stream, bs->streamsz);
    bs->stream[bs->streamln] = ins;
    bs->streamln++;
}
/*void create_mbs(ByteStream *bs)
{
    bs->is_master = true;
    bs->UNION.ms.stream = NULL;
    bs->UNION.ms.streamln = 0;
    bs->UNION.ms.streamsz = 0;
}
void create_ibs(ByteStream *bs)
{
    bs->is_master = false;
    bs->UNION.is.stream = NULL;
    bs->UNION.is.streamln = 0;
    bs->UNION.is.streamsz = 0;
}
void reset_ibs(ByteStream *bs)
{
    bs->is_master = false;
    bs->UNION.is.stream = NULL;
    bs->UNION.is.streamln = 0;
    bs->UNION.is.streamsz = 0;
}
void reset_mbs(ByteStream *bs)
{
    bs->is_master = true;
    bs->UNION.ms.stream = NULL;
    bs->UNION.ms.streamln = 0;
    bs->UNION.ms.streamsz = 0;
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

void bytecode_identifier(NodeMaster *node, ByteStream *bs, Genmem *gem, NodeMaster *genm)
{
    //ByteStream ministream;
    //create_mbs(&ministream);
    
    //ByteStream mministream;
    //create_ibs(&mministream);
    
    char *id = node->UNION.Node.stor;
    Instruction load = CMDcreate_isn(_LOAD, id);
    //ai_stream(&mministream, load);
    //am_stream(&ministream, mministream);
    ai_stream(bs, load);
    //reset_ibs(&mministream);
    
    Instruction push = CMDcreate_isn(_PUSH, "");
    //ai_stream(&mministream, push);
    //am_stream(&ministream, mministream);
    ai_stream(bs, push);
    //am_stream(bs, ministream);
}

void bytecode_args(NodeMaster *node_s, ByteStream *bs, Genmem *gem, NodeMaster *genm)
{
    //ByteStream ministream;
    //create_mbs(&ministream);
    
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
                        }
                }
            else
                {
                    switch (subnode.UNION.Node.type)
                        {
                            case IDENTIFIER:
                                bytecode_identifier(&subnode, bs, gem, genm);
                                
                                //am_stream(bs, ministream);
                                
                                //reset_mbs(&ministream);
                                break;
                        }
                }
        }
}

void bytecode_func(NodeMaster *node_s, ByteStream *bs, Genmem *gem, NodeMaster *genm)
{
    //ByteStream ministream = mbs_init();
    //ByteStream ministream;
    //create_mbs(&ministream);
    
    // we want to load any arguments first, of course
    NodeMaster arg_stack = node_s->UNION.Nodes.nodes[1];
    bytecode_args(&arg_stack, bs, gem, genm);
    
    //ByteStream mministream;
    //create_ibs(&mministream);
    // at index 1 (true index is list[0]), will always be the function name 
    char *func_name = node_s->UNION.Nodes.nodes[0].UNION.Node.stor;
    Instruction call = CMDcreate_isn(_CALL, func_name);
    // PROBLEM HERE
    //ai_stream(&mministream, call);
    //am_stream(&ministream, mministream);
    //am_stream(bs, ministream);
    ai_stream(bs, call);
}

void bytecode_IDlinker_func(NodeMaster *node_s, ByteStream *bs, Genmem *gem, NodeMaster *genm)
{
    //ByteStream ministream;
    //create_mbs(&ministream);
    
    int links = 0;
    bytecode_IDlinker(node_s, bs, gem, genm, &links);
    
    if (gem->types[gem->ln - 1] == FUNCTION_CALL)
        {
            // this means that there was a function at the end
            // of the linker, such as: player.box.move();
            
            //ByteStream mministream;
            //create_ibs(&mministream);
            
            char *seq = malloc(sizeof(char)*(int)log10(links));
            sprintf(seq, "%d", links);
            
            Instruction loadseq = CMDcreate_isn(_LOAD_SEQ, seq);
            //ai_stream(&mministream, loadseq);
            //am_stream(&ministream, mministream);
            ai_stream(bs, loadseq);
            //reset_ibs(&mministream);
            
            // we need to load the function now
            //bytecode_func(&genm->UNION.Nodes.nodes[genm->UNION.Nodes.nodeln - 1], &ministream, gem, genm);
        }
        
    //am_stream(bs, ministream);
}

void bytecode_IDlinker(NodeMaster *node_s, ByteStream *bs, Genmem *gem, NodeMaster *genm, int *linker_cnt)
{
    // if the second index is NODES, then that means the linker has more than
    // one '.'
    // otherwise it means that its a single dot like player.x or something
    
    //printf("master %s %d\n", type_names[node_s->UNION.Nodes.type], node_s->UNION.Nodes.nodeln);
    NodeMaster sublink = node_s->UNION.Nodes.nodes[1];
    //printf("link data 1: %s\n", sublink.UNION.Node.stor);
    
    // this means that there is also a third node which
    NodeMaster subsublink = node_s->UNION.Nodes.nodes[2];
    
    if (subsublink.utype == NODE)
        {
            //ByteStream ministream = mbs_init();
            //ByteStream ministream;
            //create_ibs(&ministream);
            
            Instruction literal = CONcreate_isn(_STRING_, (void *)subsublink.UNION.Node.stor);
            //ai_stream(&ministream, literal);
            //am_stream(bs, ministream);
            ai_stream(bs, literal);
            //reset_ibs(&ministream);
            
            Instruction push = CMDcreate_isn(_PUSH, "");
            //ai_stream(&ministream, push);
            //am_stream(bs, ministream);
            ai_stream(bs, push);
            (*linker_cnt)++;
        }
    else
        {   
            // it could be a function or something
            if (subsublink.UNION.Nodes.type == FUNCTION_CALL)
                {
                    //ByteStream _ministream;
                    //create_ibs(&_ministream);
                    
                    // before we add the function to the LOAD_SEQ, we need
                    // to load in all of the arguments.
                    /*
                        this is because, if we have LOAD_SEQ 3, and then we have CALL, 
                        if those arguments are after CALL, we won't reach them for the 
                        function call. Ontop of the that, we don't want them infront of
                        all of the objects on the stack, because otherwise LOAD_SEQ will
                        hit them BEFORE it hits the objects to load in on each hashmap
                    */
                    //ByteStream _mministream;
                    //create_ibs(&_mministream);
                    NodeMaster args_nodes = subsublink.UNION.Nodes.nodes[1];
                    bytecode_args(&args_nodes, bs, gem, genm);
                    //am_stream(_ministream, _mministream);
                    //am_stream(bs, _ministream);
                    
                    //reset_ibs(&_ministream);
                    //reset_mbs(&_ministream);
                    
                    // we still add it to the LOAD_SEQ
                    // =====================================================
                    NodeMaster node = subsublink.UNION.Nodes.nodes[0];

                    Instruction literal = CONcreate_isn(_STRING_, (void *)node.UNION.Node.stor);
                    //ai_stream(&_ministream, literal);
                    //am_stream(bs, _ministream);
                    ai_stream(bs, literal);
                    //reset_ibs(&_ministream);
                    
                    Instruction push = CMDcreate_isn(_PUSH, "");
                    //ai_stream(&_ministream, push);
                    //am_stream(bs, _ministream);
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
    
    //mbs_reset(&ministream);
    
    if (sublink.utype == NODES)
        {
            bytecode_IDlinker(&sublink, bs, gem, genm, linker_cnt);
        }
    else
        {             
            //ByteStream ministream = mbs_init();
            //ByteStream ministream;
            //create_ibs(&ministream);
            // this means that there is also a third node which
            Instruction _literal = CONcreate_isn(_STRING_, (void *)sublink.UNION.Node.stor);
            //ai_stream(&ministream, _literal);
            //am_stream(bs, ministream);
            ai_stream(bs, _literal);
            //reset_ibs(&ministream);
            
            Instruction push = CMDcreate_isn(_PUSH, "");
            //ai_stream(&ministream, push);
            //am_stream(bs, ministream);
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
    
    //ByteStream value = mbs_init();
    //ByteStream value;
    //create_mbs(&value);
    if (value_n.utype == NODES)
        {
            // ===============================================================
            // THIS WILL ALWAYS BE AN EXPRESSION IF ITS NODES
            // ===============================================================
            
            // THIS IS NEXT UP FOR DEVELOPMENT
            //bytecode_expr()
        }
    else
        {
            // ===============================================================
            // THIS IS IF THE VALUE ON THE RIGHT IN AN ASSIGNMENT IS A SINGLE
            // VALUE, LIKE [name = "john"] OR [number = 10.5]
            // ===============================================================
            
            //ByteStream ministream;
            //create_ibs(&ministream);
            // Casted convert(char *string, enum _DATATYPE_ type)
            char *value_s = value_n.UNION.Node.stor;
            Casted implicit_cast = convert(value_s, type(value_s));
            
            Instruction literal;
            switch (implicit_cast.type)
                {
                    case _INT_:
                        literal = CONcreate_isn(_INT_, (void *)&implicit_cast.data.inum);
                        break;
                    case _FLOAT_:
                        literal = CONcreate_isn(_FLOAT_, (void *)&implicit_cast.data.fnum);
                        break;
                    case _STRING_:
                        literal = CONcreate_isn(_STRING_, (void *)&implicit_cast.data.string);
                        break;
                }
            // append the literal to the stream
            //ai_stream(&ministream, literal);
            //am_stream(&value, ministream);
            ai_stream(bs, literal);
            
            // append the PUSH command to put the literal onto the stack
            //reset_ibs(&ministream);
            Instruction push = CMDcreate_isn(_PUSH, "");
            
            // append the push command to the stream
            //ai_stream(&ministream, push);
            //am_stream(&value, ministream); 
            ai_stream(bs, push);
        }
        
    // we do this one afterwards so that, in bytecode,
    // we 'STO' the value after evaluating the value
    //ByteStream assig;
    //create_mbs(&assig);
    if (assig_n.utype == NODES)
        {
            // ===============================================================
            // THIS WILL ALWAYS BE A LINKER OR AN LIST[INDEX]
            // ===============================================================
            
            switch (assig_n.UNION.Nodes.type)
                {
                    case ID_LINKER:
                        // in the parser.c, ID_LINKER always has a redudant and useless
                        // STACK ID_LINKER at the very type.
                        // it just contains a single other ID_LINKER stack inside.
                        // therefore we skip over it with 'nodes[0]'
                        int links = 0;
                        bytecode_IDlinker(&assig_n.UNION.Nodes.nodes[0], bs, gem, genm, &links);
                        // we then want to sequentially load in those keys into the
                        // respective hashmaps
                        
                        //ByteStream ministream;
                        //create_ibs(&ministream);
                        
                        
                        // thank you stackoverflow
                        char *seq = malloc(sizeof(char)*(int)log10(links));
                        sprintf(seq, "%d", links);
                        
                        Instruction loadseq = CMDcreate_isn(_LOAD_SEQ, seq);
                        //ai_stream(&ministream, loadseq);
                        
                        //am_stream(&assig, ministream);
                        ai_stream(bs, loadseq);
                        
                        // WE DONT CHECK FOR FUNCTION CALLS HERE
                        // it is illegal to say:
                        // player.something.function() = ...
                        // thus, theres no need to check for it!
                        
                        break;
                    case LIST_ACC:
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
            //ByteStream ministream = mbs_init();
            //ByteStream ministream;
            //create_ibs(&ministream);
            
            char *var = assig_n.UNION.Node.stor;
            Instruction variable = CMDcreate_isn(_STO, var);
            
            // append the literal to the stream
            //ai_stream(&ministream, variable);
            //am_stream(bs, ministream);
            ai_stream(bs, variable);
        }
        
    //am_stream(bs, value);
    //am_stream(bs, assig);
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

void traverse3(NodeMaster *stack, ByteStream *bs,  Genmem *gem, NodeMaster *genm)
{
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
                    default:
                        debugp("Hit a default");
                        break;
                }
        }
}

/*void print_bs(ByteStream *bs)
{
    if (bs->is_master) 
        { 
            for (int i = 0; i < bs->UNION.ms.streamln; ++i)
                {
                    print_bs(&bs->UNION.ms.stream[i]); 
                }
        }
    else
        {
            for (int i = 0; i < bs->UNION.is.streamln; ++i)
                {
                    Instruction isn = bs->UNION.is.stream[i];
                    
                     if (isn.isnt == __CMD)
                         {
                             enum _COMMAND_ cmd = isn.data.cmd.cmdt;
                             printf("%s %s", cmdt_types[cmd], isn.data.cmd.val);
                            
                             if (cmd == _PUSH || cmd == _LOAD_SEQ) printf("\n");
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
                     printf("\n"); 
                }
        }
}*/

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
    traverse3(stack, &bs, &gem, &genm);
    
    printf("\n\n");
    
    //print_bs(&bs);
    
    for (int i = 0; i < bs.streamln; ++i)
        {
            Instruction isn = bs.stream[i];
                    
             if (isn.isnt == __CMD)
                 {
                     enum _COMMAND_ cmd = isn.data.cmd.cmdt;
                     printf("%s %s", cmdt_types[cmd], isn.data.cmd.val);
                    
                     if (cmd == _PUSH || cmd == _LOAD_SEQ) printf("\n");
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
             printf("\n"); 
        }
        
    return bs;
}