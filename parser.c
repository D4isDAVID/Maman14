#include "parser.h"

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "strutil.h"
#include "errutil.h"

enum opbitlocations {
	OPBIT_ARE_0,
	OPBIT_ARE_1,
	OPBIT_DEST_2,
	OPBIT_DEST_3,
	OPBIT_SOURCE_4,
	OPBIT_SOURCE_5,
	OPBIT_OPCODE_6,
	OPBIT_OPCODE_7,
	OPBIT_OPCODE_8,
	OPBIT_OPCODE_9,
	OPBIT_PARAM2_10,
	OPBIT_PARAM2_11,
	OPBIT_PARAM1_12,
	OPBIT_PARAM1_13
};

word *encodelabel(int val)
{
	word *w = (word *) alloc(sizeof(w));
	w->field = ENC_RELOCATABLE;
	w->field |= val << 2;
	return w;
}

/* utility function for appending instructions to a linked list */
void addinstructiontolist(void *value, int islabel, int line, struct listnode **instructionptr, int *instructioncount)
{
	instruction *inst = (instruction *) alloc(sizeof(*inst));
	inst->value = value;
	inst->islabel = islabel;
	inst->line = line;
	(*instructionptr)->next = linkedlist_newnode(inst);
	*instructionptr = (*instructionptr)->next;
	(*instructioncount)++;
}

/* uses `encodenum` to encode the num and append it to the list, and converts it to `struct instruction`.
	returns whether the given number was valid or not */
int addnumtoinstructions(char *n, struct listnode **instructionptr, int *instructioncount)
{
	instruction *inst;
	if (!encodenum(n, instructionptr, instructioncount))
		return 0;
	inst = (instruction *) alloc(sizeof(*inst));
	inst->value = (word *) (*instructionptr)->value;
	inst->islabel = 0;
	inst->line = 0;
	((word *) inst->value)->field <<= 2;
	(*instructionptr)->value = inst;
	return 1;
}

enum parsererrno encodeoperation(char *opname, enum symbol opcode, int line, struct listnode **params, struct listnode **instructionptr, int *instructioncount)
{
	struct listnode *paramptr = *params;
	word *first = (word *) alloc(sizeof(*first)), *registers = NULL;
	int addressmethod = 0, methods = 0;

	first->field = 0;
	addinstructiontolist(first, 0, line, instructionptr, instructioncount);

	/* opcode */
	first->field |= (opcode) << OPBIT_OPCODE_6;

	/* address methods + parameters 1 & 2 */
	if (isjumpoperation(opcode)){
		if(!isvalidlabel((char *)paramptr->value))
			return PARSER_EINVALIDLABEL;
		first->field |= (paramptr->next == NULL ? ADDRESS_DIRECT : ADDRESS_JUMP_WITH_PARAMS) << 2;
		addinstructiontolist(strdupl((char *) paramptr->value), 1, line, instructionptr, instructioncount);
		paramptr = paramptr->next;
	}
	while (paramptr != NULL) {
		addressmethod = determineaddressmethod(paramptr->value);
		methods <<= 2;
		methods |= addressmethod;
		if (registers != NULL && addressmethod != ADDRESS_DIRECT_REGISTER) {
			addinstructiontolist(registers, 0, line, instructionptr, instructioncount);
			registers = NULL;
		}
		switch (addressmethod) {
		case ADDRESS_INSTANT:
			if (!addnumtoinstructions(&((char *) paramptr->value)[1], instructionptr, instructioncount)) {
				*params = paramptr;
				return PARSER_EINVALIDNUMBER;
			}
			break;
		case ADDRESS_ERROR:
			*params = paramptr;
			return PARSER_EINVALIDNUMBER;
		case ADDRESS_DIRECT:
			if(!isvalidlabel((char *)paramptr->value))
				return PARSER_EINVALIDLABEL;
			addinstructiontolist(strdupl(paramptr->value), 1, line, instructionptr, instructioncount);
			break;
		case ADDRESS_DIRECT_REGISTER:
			if (registers == NULL) {
				registers = (word *) alloc(sizeof(*registers));
				registers->field = 0;
			}
			registers->field |= atoi(&((char *) paramptr->value)[1]) << (paramptr->next == NULL ? 2 : 8);
			break;
		default:
			break;
		}
		paramptr = paramptr->next;
	}
	if (registers != NULL)
		addinstructiontolist(registers, 0, line, instructionptr, instructioncount);
	first->field |= methods << (isjumpoperation(opcode) ? OPBIT_PARAM2_10 : OPBIT_DEST_2);

	switch (opcode) {
	case OPCODE_LEA:
		if (!(((first->field & (1 << OPBIT_SOURCE_5)) >> OPBIT_SOURCE_5) ^ ((first->field & (1 << OPBIT_SOURCE_4)) >> OPBIT_SOURCE_4)))
			return PARSER_EINVALIDSOURCEPARAM;
	case OPCODE_MOV:
	case OPCODE_ADD:
	case OPCODE_SUB:
	case OPCODE_NOT:
	case OPCODE_CLR:
	case OPCODE_INC:
	case OPCODE_DEC:
	case OPCODE_JMP:
	case OPCODE_BNE:
	case OPCODE_RED:
	case OPCODE_JSR:
		if (!(first->field & ((1 << OPBIT_DEST_3) | (1 << OPBIT_DEST_2))))
			return PARSER_EINVALIDDESTPARAM;
		break;
	default:
		break;
	}

	return PARSER_OK;
}

int encodenum(char *s, struct listnode **dataptr, int *datacount)
{
	int isnegative;
	word *w;
	if (!isvalidnum(s))
		return 0;
	isnegative = *s == '-';
	w = (word *) alloc(sizeof(*w));
	if (*s == '-' || *s == '+')
		s++;
	w->field = atoi(s) - isnegative;
	if (isnegative)
		w->field = ~w->field;
	(*dataptr)->next = linkedlist_newnode(w);
	*dataptr = (*dataptr)->next;
	(*datacount)++;
	return 1;
}

enum parsererrno encodestring(char *s, struct listnode **dataptr, int *datacount)
{
	struct listnode *tmp;
	char *end;
	word *w;
	int i = 0;
	if (*s != '"')
		return PARSER_EEXPECTEDQUOTES;
	end = strrchr(s, '"') + 1;
	skipwhitespace(end, &i);
	if (s == end-1 || countnonwhitespace(end, &i) > 0)
		return PARSER_EUNFINISHEDSTRING;
	for (s++; s != end; s++) {
		if (!isprint(*s))
			return PARSER_EINVALIDCHAR;
		w = (word *) alloc(sizeof(*w));
		w->field = s == end-1 ? '\0' : *s; /* null terminator if we are at the end of the string */
		tmp = linkedlist_newnode(w);
		if (*dataptr != NULL)
			(*dataptr)->next = tmp;
		*dataptr = tmp;
		(*datacount)++;
	}
	return PARSER_OK;
}

enum parsererrno parseparams(char *line, int *i, int paramamount, struct listnode **head)
{
	int count, ii, isjumpwithparams = 0;
	char *param, *jumpend;
	struct listnode *n = NULL, *tmp;
	if (paramamount == PARAM_UNKNOWN)
		return PARSER_OK;
	if (paramamount == PARAM_JUMP) {
		if (strchr(&line[*i], '(') != NULL) {
			isjumpwithparams = 1;
			jumpend = strchr(&line[*i], ')');
			if (jumpend == NULL)
				return PARSER_EJUMPPARAMS;
			jumpend++;
			ii = 0;
			skipwhitespace(jumpend, &ii);
			if (countnonwhitespace(jumpend, &ii))
				return PARSER_EJUMPPARAMS;
			*(--jumpend) = '\0';
		} else {
			isjumpwithparams = 0;
			paramamount = PARAM_SINGLE;
		}
	}
	while ((count = countnonwhitespace(line, i)) > 0 && paramamount != 0) {
		param = dupluntil(&line[(*i)-count], paramamount == PARAM_JUMP ? '(' : ',');
		*i -= count;
		ii = 0;
		if (countnonwhitespace(param, &ii) == 0)
			return paramamount == PARAM_JUMP ? PARSER_EJUMPPARAMS : PARSER_EUNEXPECTEDCOMMA;
		if ((count = skipwhitespace(param, &ii)) > 0) {
			if (isjumpwithparams || countnonwhitespace(param, &ii) > 0)
				return PARSER_EUNEXPECTEDSPACE;
			param[ii-count] = '\0';
		}
		tmp = linkedlist_newnode(param);
		if (n != NULL)
			(n)->next = tmp;
		n = tmp;
		if (*head == NULL)
			*head = n;
		*i += ii;
		if (line[*i] == (paramamount == PARAM_JUMP ? '(' : ','))
			(*i)++;
		else if (paramamount == PARAM_JUMP)
			return PARSER_EJUMPPARAMS;
		if (skipwhitespace(line, i) > 0 && isjumpwithparams)
			return PARSER_EUNEXPECTEDSPACE;
		paramamount--;
	}
	if (paramamount > 0)
		return PARSER_ENOTENOUGHPARAMS;
	else if (paramamount == 0 && count > 0)
		return PARSER_ETOOMANYPARAMS;
	return PARSER_OK;
}

enum addressmethod determineaddressmethod(char *s)
{
	if (s[0] == '#')
		return isvalidnum(&s[1]) ? ADDRESS_INSTANT : ADDRESS_ERROR;
	else if (isregister(s))
		return ADDRESS_DIRECT_REGISTER;
	return ADDRESS_DIRECT;
}

int isvalidnum(char *s)
{
	if (*s != '-' && *s != '+' && !isdigit(*s))
		return 0;
	for (s++; *s != '\0' && *s != EOF; s++)
		if (!isdigit(*s))
			return 0;
	return 1;
}

int isvalidlabel(char *s)
{
	int count;
	if (!isalpha(*s) || symbols_get(s) != UNKNOWN_SYMBOL)
		return 0;
	for (count = 1, s++; *s != '\0' && *s != EOF; s++, count++)
		if (!isalnum(*s))
			return 0;
	return count <= 30;
}

int isvalidspace(char c)
{
	return c == ' ' || c == '\t';
}

int islineterminator(char c)
{
	return c == '\n' || c == '\0' || c == EOF;
}

int skipwhitespace(char line[], int *i)
{
	int count = 0;
	for (; isvalidspace(line[*i]) && !islineterminator(line[*i]); (*i)++, count++)
		;
	return count;
}

int countnonwhitespace(char line[], int *i)
{
	int count = 0;
	for (; !isvalidspace(line[*i]) && !islineterminator(line[*i]); (*i)++, count++)
		;
	return count;
}

int countuntil(char *s, char c)
{
	int count;
	for (count = 0; s[count] != c && !islineterminator(s[count]); count++)
		;
	return count;
}

char *dupluntil(char *s, char c)
{
	return strndupl(s, countuntil(s, c));
}
