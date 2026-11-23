#ifndef COMPILER_H
#define COMPILER_H

#include <stdlib.h>

/*void acmd(CMD **cmds, size_t *cmdsz, int *cmdln, CMD obj);
CMD spawn(enum _COMMAND_ cmdtype, int _argln, char *token);*/

Instruction CMDcreate_isn(enum _COMMAND_ cmdtype, char *_val);
Instruction MEMcreate_isn(enum _MEMTYPE_ memtype, void *_index);
Instruction CONcreate_isn(enum _DATATYPE_ datatype, void *data);
void am_stream(ByteStream *bs, ByteStream sub_bs);
void ai_stream(ByteStream *bs, Instruction ins);
void create_mbs(ByteStream *bs);
void create_ibs(ByteStream *bs);
void reset_ibs(ByteStream *bs);

void agem(enum _TYPE_ atype, Genmem *gem);
void agenm(NodeMaster node, NodeMaster *genm);

void bytecode_args(NodeMaster *node_s, ByteStream *bs, Genmem *gem, NodeMaster *genm);
void bytecode_func(NodeMaster *node_s, ByteStream *bs, Genmem *gem, NodeMaster *genm);
void bytecode_IDlinker_func(NodeMaster *node_s, ByteStream *bs, Genmem *gem, NodeMaster *genm);
void bytecode_IDlinker(NodeMaster *node_s, ByteStream *bs, Genmem *gem, NodeMaster *genm, int *linker_cnt);
void bytecode_assignment(NodeMaster *node_s, ByteStream *bs, Genmem *gem, NodeMaster *genm);
void traverse3(NodeMaster *stack, ByteStream *bs,  Genmem *gem, NodeMaster *genm);

ByteStream compile(NodeMaster *stack);

//void bsprint(Instruction isn);
void print_bs(ByteStream *bs);

#endif
