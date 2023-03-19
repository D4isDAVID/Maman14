# [Maman14](https://github.com/ariellush/Maman14)

## Table of Contents

- [General Assumptions](#general-assumptions)
- [Data Structures](#data-structures)
- [Utilities](#utilities)
- [Preassembler](#preassembler)
- [First Phase](#first-phase)
- [Second Phase](#second-phase)

## General Assumptions

- Whitespace = spaces (` `) and tabs (`\t`)
- Newlines = `\n`

## Data Structures

The data structures we use are [Hashmap](#hashmap) and [Linked List](#linked-list).

### Hashmap

Our hashmap implementation is adapted from the K&R book, with some changes.
It contains different functions for getting/setting values of strings and integers, to address the different ways that memory should be allocated for them.

We use hashmap for storing preassembler macros, program labels, and built-in constants.

### Linked List

When a linked list node is created, its value is saved as a void pointer.
Our linked list node implementation contains a `free` field which is used when freeing the list, to be able to correctly free the value.

We use linked list to save the program instructions and data in the order in which they appear.

## Utilities

- `errutil` contains error message utilities.
It has wrappers for functions which may fail (if they fail then a proper error message will be printed, and the program will exit) and utility functions for printing errors and warning.
- `strutil` contains functions for string manipulations.
- `symbols` stores the symbols/built-in constants registry, and contains utility functions related to them.

## Preassembler

Parses macros without any error checking.
Invalid macro definition will result in the definition not being read (see [Macro Definition Structure](#macro-definition-structure)).

Macros will only be parsed after they are defined.

The preassembler treats macro names as "tokens", meaning that for example if we define a macro called `m1`, writing `m1` will be valid but `m1:` will not be considered as a macro.

### Macro Definition Structure

Macro names may not be the same as any built-in constants (such as register names, operation names, etc). To avoid errors, if a built-in constant is used as a name its definition will be read but the macro will not be parsed.

The legal structure of macro definitions is as follows (whitespace doesn't affect the structure):

```
mcr <macro name>
<macro content>
endmcr
```

## First Phase

The first phase gathers any information it can about the code (which are instructions, data, labels, and label attributes). The only thing entrusted to the second phase is the checking of validity of label usage (checking that all labels used are defined).

Instructions is a linked list of every single part of every simple operation, meaning it stores operation names and their parameters in the order that they appear in the program.

Data is a linked list of `.data` and `.string` definitions, which are already encoded.

Labels is a hashmap storing label names and their values.
A label will only have a value when defined (not when defined as an entry or with `.extern`)

Label attributes is a hashmap storing metadata about labels (whether a label stores data or an operation; whether a label is an entry or an external).
A label, when either being defined or being defined as an entry or with `.extern`, will have an attribute associated with it.

## Second Phase

The second phase uses the information gathered in the first phase to first check for the validity of label usage (whether labels that are used are defined). After doing so the encoding is printed into the `.ob` file. In the case that any entries are defined, an `.ent` file is created, and in the case that any externals are define, an `.ext` file is created.
