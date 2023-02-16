/* symbols registry */
#include "symbols.h"

#include <stdlib.h>
#include "hashmap.h"

struct hashmap *opcodes;
enum paramamount *paramamounts;

void symbols_prepare(void)
{
	opcodes = hashmap_new();
	hashmap_setint(opcodes, "mov", OPCODE_MOV);
	hashmap_setint(opcodes, "cmp", OPCODE_CMP);
	hashmap_setint(opcodes, "add", OPCODE_ADD);
	hashmap_setint(opcodes, "sub", OPCODE_SUB);
	hashmap_setint(opcodes, "not", OPCODE_NOT);
	hashmap_setint(opcodes, "clr", OPCODE_CLR);
	hashmap_setint(opcodes, "lea", OPCODE_LEA);
	hashmap_setint(opcodes, "inc", OPCODE_INC);
	hashmap_setint(opcodes, "dec", OPCODE_DEC);
	hashmap_setint(opcodes, "jmp", OPCODE_JMP);
	hashmap_setint(opcodes, "bne", OPCODE_BNE);
	hashmap_setint(opcodes, "red", OPCODE_RED);
	hashmap_setint(opcodes, "prn", OPCODE_PRN);
	hashmap_setint(opcodes, "jsr", OPCODE_JSR);
	hashmap_setint(opcodes, "rts", OPCODE_RTS);
	hashmap_setint(opcodes, "stop", OPCODE_STOP);
	hashmap_setint(opcodes, ".data", DIRECTIVE_DATA);
	hashmap_setint(opcodes, ".string", DIRECTIVE_STRING);
	hashmap_setint(opcodes, ".entry", DIRECTIVE_ENTRY);
	hashmap_setint(opcodes, ".extern", DIRECTIVE_EXTERN);

	paramamounts = (enum paramamount *) malloc(sizeof(*paramamounts) * hashmap_getsize(opcodes));
	paramamounts[OPCODE_MOV] = PARAM_TWO;
	paramamounts[OPCODE_CMP] = PARAM_TWO;
	paramamounts[OPCODE_ADD] = PARAM_TWO;
	paramamounts[OPCODE_SUB] = PARAM_TWO;
	paramamounts[OPCODE_LEA] = PARAM_TWO;
	paramamounts[OPCODE_NOT] = PARAM_SINGLE;
	paramamounts[OPCODE_CLR] = PARAM_SINGLE;
	paramamounts[OPCODE_INC] = PARAM_SINGLE;
	paramamounts[OPCODE_DEC] = PARAM_SINGLE;
	paramamounts[OPCODE_JMP] = PARAM_SINGLE;
	paramamounts[OPCODE_BNE] = PARAM_SINGLE;
	paramamounts[OPCODE_RED] = PARAM_SINGLE;
	paramamounts[OPCODE_PRN] = PARAM_SINGLE;
	paramamounts[OPCODE_JSR] = PARAM_SINGLE;
	paramamounts[OPCODE_RTS] = PARAM_NONE;
	paramamounts[OPCODE_STOP] = PARAM_NONE;
	paramamounts[DIRECTIVE_DATA] = PARAM_LIST;
	paramamounts[DIRECTIVE_STRING] = PARAM_SINGLE;
	paramamounts[DIRECTIVE_ENTRY] = PARAM_SINGLE;
	paramamounts[DIRECTIVE_EXTERN] = PARAM_SINGLE;
	paramamounts[UNKNOWN_OPERATION] = PARAM_NONE;
}

void symbols_free(void)
{
	hashmap_free(opcodes);
	free(paramamounts);
}

/* returns the opcode of the given operation name, or `UNKNOWN_OPERATION` for non-existent ones */
enum opcode symbols_getopcode(char *op)
{
	int *code = hashmap_getint(opcodes, op);
	return code == NULL ? UNKNOWN_OPERATION : *code;
}

/* returns the parameter amount of the given operation */
enum paramamount symbols_getparamamount(enum opcode op)
{
	return paramamounts[op];
}
