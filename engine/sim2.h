
#ifndef SIM2_H
#define SIM2_H

#include "sim.h"


//typedef struct {
//    int valid;
//    unsigned long long address;
//    char *line_contents;
//} assembly_line;

typedef struct symbol_reference {
    int referenced_line;
    struct symbol_reference *next;
} symbol_reference;

typedef struct {
    char *label;
    int declared_line;
    symbol_reference *references;
} symbol;

typedef struct {
    simulator sim;
//    assembly_line *line_table;
    assembly_instruction** line_table;
//    int line_table_size;
    int num_lines;
    symbol *symbol_table;
    int symbol_table_size;
    int num_symbols;
} simulator2;


#ifdef FROM_CPP

extern "C" simulator2* create_simulator2(OutputString outputStringCallback, InputString inputStringCallback);
extern "C" void delete_simulator2(simulator2 *sim);
extern "C" assembly_instruction** assemble2 (simulator2 *sim, const char *program, int address, int *number_of_instructions);
extern "C" assembly_instruction* get_instruction2 (simulator2 *sim, unsigned long long address);
extern "C" unsigned char* get_memory2 (simulator2 *sim, unsigned long long address);
extern "C" void get_instruction_string2(simulator2 *sim, int address, char *instruction_string);
extern "C" int do_partial_assembly(simulator2 *sim, int lineNumber, const char* line);

#endif

#ifndef FROM_CPP

simulator2* create_simulator2(OutputString outputStringCallback, InputString inputStringCallback);
void delete_simulator2(simulator2 *sim);
assembly_instruction** assemble2 (simulator2 *sim, const char *program, int address, int *number_of_instructions);
assembly_instruction* get_instruction2 (simulator2 *sim, unsigned long long address);
unsigned char* get_memory2 (simulator2 *sim, unsigned long long address);
void get_instruction_string2(simulator2 *sim, int address, char *instruction_string);
int do_partial_assembly(simulator2 *sim, int lineNumber, const char* line);

#endif

#endif
