/* utility parsing functions */
#include "parser.h"

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "strutil.h"
#include "errutil.h"

/* utility function for appending instructions to a linked list */
void addinstructiontolist(void *value, int islabel, struct listnode **instructionptr, int *instructioncount)
{
	instruction *inst = (instruction *) alloc(sizeof(*inst));
	inst->value = value;
	inst->islabel = islabel;
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
	((word *) inst->value)->field <<= 2;
	(*instructionptr)->value = inst;
	return 1;
}

/* encodes the given operation and its parameters and appends it to the linked list */
enum parsererrno encodeoperation(char *opname, enum symbol opcode, struct listnode **params, struct listnode **instructionptr, int *instructioncount)
{
	struct listnode *paramptr = *params;
	word *first = (word *) alloc(sizeof(*first)), *registers = NULL;
	int addressmethod = 0, methods = 0;

	first->field = 0;
	addinstructiontolist(first, 0, instructionptr, instructioncount);

	/* opcode */
	first->field |= (opcode) << 6;
	
	
	/* address methods + parameters 1 & 2 */
	if (isjumpoperation(opcode)){
		if(!isvalidlabel((char *)paramptr->value))
			return PARSER_EINVALIDLABEL;
		first->field |= (paramptr->next == NULL ? ADDRESS_DIRECT : ADDRESS_JUMP_WITH_PARAMS) << 2;
		addinstructiontolist(strdupl((char *) paramptr->value), 1, instructionptr, instructioncount);
		paramptr = paramptr->next;
	}
	while (paramptr != NULL) {
		addressmethod = determineaddressmethod(paramptr->value);
		methods <<= 2;
		methods |= addressmethod;
		if (registers != NULL && addressmethod != ADDRESS_DIRECT_REGISTER) {
			addinstructiontolist(registers, 0, instructionptr, instructioncount);
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
			addinstructiontolist(strdupl(paramptr->value), 1, instructionptr, instructioncount);
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
		addinstructiontolist(registers, 0, instructionptr, instructioncount);
	first->field |= methods << (isjumpoperation(opcode) ? 10 : 2);

	return PARSER_OK;
}

/* encodes the given string as a number and appends it to the linked list.
	returns whether the function was given a valid number or not */
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

/* encodes the given string as an array of characters and appends it to the linked list.
	returns a parser error number */
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

/* parses parameters in the given string from `i` and appends them to the given linked list.
	returns a parser error number */
enum parsererrno parseparams(char *line, int *i, int paramamount, struct listnode **head)
{
	int count, ii;
	char *param, *jumpend;
	struct listnode *n = NULL, *tmp;
	if (paramamount == PARAM_UNKNOWN)
		return PARSER_OK;
	if (paramamount == PARAM_JUMP) {
		if (strchr(&line[*i], '(') != NULL) {
			jumpend = strchr(&line[*i], ')');
			if (jumpend == NULL)
				return PARSER_EJUMPPARAMS;
			jumpend++;
			ii = 0;
			skipwhitespace(jumpend, &ii);
			if (countnonwhitespace(jumpend, &ii))
				return PARSER_EJUMPPARAMS;
			*(--jumpend) = '\0';
		} else
			paramamount = PARAM_SINGLE;
	}
	while ((count = countnonwhitespace(line, i)) > 0 && paramamount != 0) {
		param = dupluntil(&line[(*i)-count], paramamount == PARAM_JUMP ? '(' : ',');
		*i -= count;
		ii = 0;
		if (countnonwhitespace(param, &ii) == 0)
			return paramamount == PARAM_JUMP ? PARSER_EJUMPPARAMS : PARSER_EUNEXPECTEDCOMMA;
		if ((count = skipwhitespace(param, &ii)) > 0) {
			if (paramamount == PARAM_JUMP || countnonwhitespace(param, &ii) > 0)
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
		if (skipwhitespace(line, i) > 0 && paramamount == PARAM_JUMP)
			return PARSER_EUNEXPECTEDSPACE;
		paramamount--;
	}
	if (paramamount > 0)
		return PARSER_ENOTENOUGHPARAMS;
	else if (paramamount == 0 && count > 0)
		return PARSER_ETOOMANYPARAMS;
	return PARSER_OK;
}

/* returns the address method of the given string */
enum addressmethod determineaddressmethod(char *s)
{
	if (s[0] == '#')
		return isvalidnum(&s[1]) ? ADDRESS_INSTANT : ADDRESS_ERROR;
	else if (isregister(s))
		return ADDRESS_DIRECT_REGISTER;
	return ADDRESS_DIRECT;
}

/* returns whether the given string is a valid number (operator `+`/`-` followed by digits `0-9`) */
int isvalidnum(char *s)
{
	if (*s != '-' && *s != '+' && !isdigit(*s))
		return 0;
	for (s++; *s != '\0' && *s != EOF; s++)
		if (!isdigit(*s))
			return 0;
	return 1;
}

/* returns whether the given string is a valid label (30 characters long and starts with a letter followed by letters or digits) */
int isvalidlabel(char *s)
{
	int count;
	if (!isalpha(*s) || symbols_get(s)!=UNKNOWN_SYMBOL)
		return 0;
	for (count = 0, s++; *s != '\0' && *s != EOF && count <= 30; s++, count++)
		if (!isalnum(*s))
			return 0;
	return count <= 30;
}

/* returns whether the given character is either a space or a tab */
int isvalidspace(char c)
{
	return c == ' ' || c == '\t';
}

/* returns whether the given character terminates the line in any way */
int islineterminator(char c)
{
	return c == '\n' || c == '\0' || c == EOF;
}

/* skips over spaces and tabs in the given line at the given position, and returns their amount */
int skipwhitespace(char line[], int *i)
{
	int count = 0;
	for (; isvalidspace(line[*i]) && !islineterminator(line[*i]); (*i)++, count++)
		;
	return count;
}

/* skips over non-space, non-tab, and non-newline characters in the given line at the given position,
	and returns their amount */
int countnonwhitespace(char line[], int *i)
{
	int count = 0;
	for (; !isvalidspace(line[*i]) && !islineterminator(line[*i]); (*i)++, count++)
		;
	return count;
}

/* duplicates the given string until the given character */
char *dupluntil(char *s, char c)
{
	int count;
	for (count = 0; s[count] != c && !islineterminator(s[count]); count++)
		;
	return strndupl(s, count);
}
word *encodelabel(int val)
{
	word *w = (word *) alloc(sizeof(w));
	w->field = ENC_RELOCATABLE;
	w->field |= val << 2;
	return w;
}
