/* symbols registry */
#include "symbols.h"

#include <stdlib.h>
#include "hashmap.h"

struct hashmap *symbols, *operations;
enum paramamount *paramamounts;

void symbols_prepare(void)
{
	operations = hashmap_new();
	hashmap_setint(operations, "mov", OPCODE_MOV);
	hashmap_setint(operations, "cmp", OPCODE_CMP);
	hashmap_setint(operations, "add", OPCODE_ADD);
	hashmap_setint(operations, "sub", OPCODE_SUB);
	hashmap_setint(operations, "not", OPCODE_NOT);
	hashmap_setint(operations, "clr", OPCODE_CLR);
	hashmap_setint(operations, "lea", OPCODE_LEA);
	hashmap_setint(operations, "inc", OPCODE_INC);
	hashmap_setint(operations, "dec", OPCODE_DEC);
	hashmap_setint(operations, "jmp", OPCODE_JMP);
	hashmap_setint(operations, "bne", OPCODE_BNE);
	hashmap_setint(operations, "red", OPCODE_RED);
	hashmap_setint(operations, "prn", OPCODE_PRN);
	hashmap_setint(operations, "jsr", OPCODE_JSR);
	hashmap_setint(operations, "rts", OPCODE_RTS);
	hashmap_setint(operations, "stop", OPCODE_STOP);
	hashmap_setint(operations, ".data", DIRECTIVE_DATA);
	hashmap_setint(operations, ".string", DIRECTIVE_STRING);
	hashmap_setint(operations, ".entry", DIRECTIVE_ENTRY);
	hashmap_setint(operations, ".extern", DIRECTIVE_EXTERN);

	symbols = hashmap_new();
	hashmap_copy(symbols, operations);

	hashmap_setint(symbols, "r0", REGISTER_ZERO);
	hashmap_setint(symbols, "r1", REGISTER_ONE);
	hashmap_setint(symbols, "r2", REGISTER_TWO);
	hashmap_setint(symbols, "r3", REGISTER_THREE);
	hashmap_setint(symbols, "r4", REGISTER_FOUR);
	hashmap_setint(symbols, "r5", REGISTER_FIVE);
	hashmap_setint(symbols, "r6", REGISTER_SIX);
	hashmap_setint(symbols, "r7", REGISTER_SEVEN);

	paramamounts = (enum paramamount *) malloc(sizeof(*paramamounts) * hashmap_sizeof(operations));
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
}

void symbols_free(void)
{
	hashmap_free(symbols);
	hashmap_free(operations);
	free(paramamounts);
}

/* returns an enum value of the given symbol, or `UNKNOWN_SYMBOL` for non-existent ones */
enum symbol symbols_get(char *op)
{
	int *code = hashmap_getint(symbols, op);
	return code == NULL ? UNKNOWN_SYMBOL : *code;
}

/* returns the parameter amount of the given operation */
enum paramamount symbols_getparamamount(enum symbol op)
{
	if (op >= hashmap_sizeof(operations))
		return PARAM_UNKNOWN;
	return paramamounts[op];
}
