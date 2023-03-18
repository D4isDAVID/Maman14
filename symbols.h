/* symbols registry */
#ifndef SYMBOLS_H
#define SYMBOLS_H

enum symbol {
	UNKNOWN_SYMBOL = -1,

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

	REGISTER
};

enum encoding {
	ENC_ABSOLUTE,
	ENC_EXTERNAL, /* externals */
	ENC_RELOCATABLE /* labels */
};

enum addressmethod {
	ADDRESS_ERROR = -1,
	ADDRESS_INSTANT, /* instant number */
	ADDRESS_DIRECT, /* label */
	ADDRESS_JUMP_WITH_PARAMS,
	ADDRESS_DIRECT_REGISTER /* register */
};

enum paramamount {
	PARAM_LIST = -2, /* .data */
	PARAM_UNKNOWN,
	PARAM_NONE,
	PARAM_SINGLE,
	PARAM_TWO,
	PARAM_JUMP
};

enum labelattribute {
	LABEL_ENTRY = 1 << 0,
	LABEL_EXTERNAL = 1 << 1,
	LABEL_DATA = 1 << 2,
	LABEL_INSTRUCTION = 1 << 3
};

void symbols_prepare(void);
void symbols_free(void);

/* returns an enum value of the given symbol, or `UNKNOWN_SYMBOL` for non-existent ones */
enum symbol symbols_get(char *);
/* returns the parameter amount of the given operation */
enum paramamount symbols_getparamamount(enum symbol);
/* returns whether the given string represents an operation */
int isoperation(char *);
/* returns whether the given string represents a directive */
int isdirective(char *);
/* returns whether the given string represents a register */
int isregister(char *);
/* returns whether the given opcode is of a jumping operation */
int isjumpoperation(enum symbol);
/* returns whether the given directive is a data directive */
int isdatadirective(enum symbol);

#endif
