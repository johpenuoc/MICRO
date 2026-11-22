#ifndef COMPILER_H
#define COMPILER_H

#include <stdlib.h>

/*void acmd(CMD **cmds, size_t *cmdsz, int *cmdln, CMD obj);
CMD spawn(enum _COMMAND_ cmdtype, int _argln, char *token);*/

Instruction CMDcreate_isn(enum _COMMAND_ cmdtype, char *_val);
Instruction MEMcreate_isn(enum _MEMTYPE_ memtype, void *_index);
Instruction CONcreate_isn(enum _DATATYPE_ datatype, void *data);
void aministream(ByteStream *bs, Instruction *isns, int isnsln);
void astream(ByteStream *bs, Instruction isn);
void aastream(ByteStream *bs, ByteStream isn);

void traverse(NodeMaster *stack, enum _TYPE_ ptype, ByteStream *bs,  Genmem *gem);
ByteStream mbs_init();
void mbs_reset(ByteStream *bs);
void traverse2(NodeMaster *stack, ByteStream *bs,  Genmem *gem);

void bytecode_assignment(&node_s, bs, gem);
void traverse3(NodeMaster *stack, ByteStream *bs,  Genmem *gem);

ByteStream compile(NodeMaster *stack);

void bsprint(Instruction isn);

#endif
