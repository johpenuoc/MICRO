#ifndef COMPILER_H
#define COMPILER_H

#include <stdlib.h>

extern int address;

Instruction CMDcreate_isn(enum _COMMAND_ cmdtype, char *_val);
Instruction MEMcreate_isn(enum _MEMTYPE_ memtype, void *_index);
Instruction CONcreate_isn(enum _DATATYPE_ datatype, void *data, enum _FLAG_ _flag);

void ai_stream(ByteStream *bs, Instruction ins);;
void agem(enum _TYPE_ atype, Genmem *gem);
void agenm(NodeMaster node, NodeMaster *genm);
void createLITERAL(NodeMaster *value_n, ByteStream *bs);

void bytecode_listacc(NodeMaster *node, ByteStream *bs, Genmem *gem, NodeMaster *genm, ByteStreamIndex *bsi, int *accesses);
void bytecode_identifier(NodeMaster *node, ByteStream *bs, Genmem *gem, NodeMaster *genm, ByteStreamIndex *bsi);
void bytecode_literal(NodeMaster *node, ByteStream *bs, Genmem *gem, NodeMaster *genm, ByteStreamIndex *bsi);
void bytecode_args(NodeMaster *node_s, ByteStream *bs, Genmem *gem, NodeMaster *genm, ByteStreamIndex *bsi);
void bytecode_func(NodeMaster *node_s, ByteStream *bs, Genmem *gem, NodeMaster *genm, ByteStreamIndex *bsi);
void bytecode_IDlinker_func(NodeMaster *node_s, ByteStream *bs, Genmem *gem, NodeMaster *genm, ByteStreamIndex *bsi);
void bytecode_IDlinker(NodeMaster *node_s, ByteStream *bs, Genmem *gem, NodeMaster *genm, ByteStreamIndex *bsi, int *linker_cnt);
void bytecode_operation(NodeMaster *stack, ByteStream *bs,  Genmem *gem, NodeMaster *genm, ByteStreamIndex *bsi);
void bytecode_assignment(NodeMaster *node_s, ByteStream *bs, Genmem *gem, NodeMaster *genm, ByteStreamIndex *bsi);
void bytecode_not(NodeMaster *stack, ByteStream *bs,  Genmem *gem, NodeMaster *genm, ByteStreamIndex *bsi);
void bytecode_subcondition(NodeMaster *stack, ByteStream *bs,  Genmem *gem, NodeMaster *genm, ByteStreamIndex *bsi);
void bytecode_condition(NodeMaster *stack, ByteStream *bs,  Genmem *gem, NodeMaster *genm, ByteStreamIndex *bsi);//, Instruction *jumpcmd);
void bytecode_loop(NodeMaster *stack, ByteStream *bs,  Genmem *gem, NodeMaster *genm, ByteStreamIndex *bsi);
void bytecode_return(NodeMaster *stack, ByteStream *bs,  Genmem *gem, NodeMaster *genm, ByteStreamIndex *bsi);
void traverse(NodeMaster *stack, ByteStream *bs,  Genmem *gem, NodeMaster *genm, ByteStreamIndex *bsi);
void bytecode_else(NodeMaster *stack, ByteStream *bs,  Genmem *gem, NodeMaster *genm, ByteStreamIndex *bsi, int *pre_ifelse_statement);
void bytecode_if(NodeMaster *stack, ByteStream *bs,  Genmem *gem, NodeMaster *genm, ByteStreamIndex *bsi, int *pre_ifelse_statement);
void bytecode_fndef_args(NodeMaster *node_s, ByteStream *bs, Genmem *gem, NodeMaster *genm, ByteStreamIndex *bsi);
void bytecode_fndef(NodeMaster *stack, ByteStream *bs,  Genmem *gem, NodeMaster *genm, ByteStreamIndex *bsi);

ByteStream compile(NodeMaster *stack);

#endif
