#ifndef SYMBOLS_H
#define SYMBOLS_H

enum opcode {
	OPCODE_MOV,
	OPCODE_CMP,
	OPCODE_ADD,
	OPCODE_SUB,
	OPCODE_NOT,
	OPCODE_CLR,
	OPCODE_LEA,
	OPCODE_INC,
	OPCODE_DEC,
	OPCODE_JMP,
	OPCODE_BNE,
	OPCODE_RED,
	OPCODE_PRN,
	OPCODE_JSR,
	OPCODE_RTS,
	OPCODE_STOP,

	DIRECTIVE_DATA,
	DIRECTIVE_STRING,
	DIRECTIVE_ENTRY,
	DIRECTIVE_EXTERN,

	UNKNOWN_OPERATION
};

enum encoding {
	ENC_ABSOLUTE,
	ENC_EXTERNAL,
	ENC_RELOCATABLE
};

enum addressmethods {
	ADDRESS_INSTANT,
	ADDRESS_DIRECT,
	ADDRESS_JUMP_WITH_PARAMS,
	ADDRESS_DIRECT_REGISTER
};

enum paramamount {
	PARAM_LIST, /* .data */
	PARAM_TWO,
	PARAM_SINGLE,
	PARAM_NONE
};

void symbols_prepare(void);
void symbols_free(void);

enum opcode symbols_getopcode(char *);
enum paramamount symbols_getparamamount(enum opcode);

#endif
