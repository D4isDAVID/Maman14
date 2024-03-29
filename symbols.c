#include "symbols.h"

#include <stdlib.h>
#include "hashmap.h"
#include "errutil.h"

struct hashmap *operations, *directives, *registers;
enum paramamount *paramamounts;
int paramamountsize;

void symbols_prepare(void)
{
	/* operation definitions */
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

	/* directive definitions */
	directives = hashmap_new();
	hashmap_setint(directives, ".data", DIRECTIVE_DATA);
	hashmap_setint(directives, ".string", DIRECTIVE_STRING);
	hashmap_setint(directives, ".entry", DIRECTIVE_ENTRY);
	hashmap_setint(directives, ".extern", DIRECTIVE_EXTERN);

	/* register definitions */
	registers = hashmap_new();
	hashmap_setint(registers, "r0", REGISTER);
	hashmap_setint(registers, "r1", REGISTER);
	hashmap_setint(registers, "r2", REGISTER);
	hashmap_setint(registers, "r3", REGISTER);
	hashmap_setint(registers, "r4", REGISTER);
	hashmap_setint(registers, "r5", REGISTER);
	hashmap_setint(registers, "r6", REGISTER);
	hashmap_setint(registers, "r7", REGISTER);

	/* parameter amount definitions */
	paramamountsize = operations->size + directives->size;
	paramamounts = (enum paramamount *) alloc(sizeof(*paramamounts) * paramamountsize);
	paramamounts[OPCODE_JMP] = PARAM_JUMP;
	paramamounts[OPCODE_BNE] = PARAM_JUMP;
	paramamounts[OPCODE_JSR] = PARAM_JUMP;
	paramamounts[OPCODE_MOV] = PARAM_TWO;
	paramamounts[OPCODE_CMP] = PARAM_TWO;
	paramamounts[OPCODE_ADD] = PARAM_TWO;
	paramamounts[OPCODE_SUB] = PARAM_TWO;
	paramamounts[OPCODE_LEA] = PARAM_TWO;
	paramamounts[OPCODE_NOT] = PARAM_SINGLE;
	paramamounts[OPCODE_CLR] = PARAM_SINGLE;
	paramamounts[OPCODE_INC] = PARAM_SINGLE;
	paramamounts[OPCODE_DEC] = PARAM_SINGLE;
	paramamounts[OPCODE_RED] = PARAM_SINGLE;
	paramamounts[OPCODE_PRN] = PARAM_SINGLE;
	paramamounts[OPCODE_RTS] = PARAM_NONE;
	paramamounts[OPCODE_STOP] = PARAM_NONE;
	paramamounts[DIRECTIVE_STRING] = PARAM_UNKNOWN;
	paramamounts[DIRECTIVE_DATA] = PARAM_LIST;
	paramamounts[DIRECTIVE_ENTRY] = PARAM_SINGLE;
	paramamounts[DIRECTIVE_EXTERN] = PARAM_SINGLE;
}

void symbols_free(void)
{
	hashmap_free(operations);
	hashmap_free(directives);
	hashmap_free(registers);
	free(paramamounts);
}

enum symbol symbols_get(char *op)
{
	int *code = hashmap_getint(operations, op);
	if (code == NULL)
		code = hashmap_getint(directives, op);
	if (code == NULL)
		code = hashmap_getint(registers, op);
	return code == NULL ? UNKNOWN_SYMBOL : *code;
}

enum paramamount symbols_getparamamount(enum symbol op)
{
	if (op < 0 || op >= paramamountsize)
		return PARAM_UNKNOWN;
	return paramamounts[op];
}

int isoperation(char *s)
{
	return hashmap_getint(operations, s) != NULL;
}

int isdirective(char *s)
{
	return hashmap_getint(directives, s) != NULL;
}

int isregister(char *s)
{
	return hashmap_getint(registers, s) != NULL;
}

int isjumpoperation(enum symbol o)
{
	return o == OPCODE_JMP || o == OPCODE_BNE || o == OPCODE_JSR;
}

int isdatadirective(enum symbol o)
{
	return o == DIRECTIVE_DATA || o == DIRECTIVE_STRING;
}
