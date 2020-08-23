
#ifndef SIM2_H
#define SIM2_H

#include "sim.h"


typedef struct {
    simulator sim;
    assembly_instruction** line_table;
    int num_lines;
} simulator2;


#ifdef FROM_CPP

extern "C" simulator2* create_simulator2(OutputString outputStringCallback, InputString inputStringCallback);
extern "C" void delete_simulator2(simulator2 *sim);
extern "C" assembly_instruction** assemble2 (simulator2 *sim, const char *program, int address, int *number_of_instructions);
extern "C" assembly_instruction* get_instruction2 (simulator2 *sim, unsigned long long address);
extern "C" unsigned char* get_memory2 (simulator2 *sim, unsigned long long address);
extern "C" void get_instruction_string2(simulator2 *sim, int address, char *instruction_string);
extern "C" int do_partial_assembly(simulator2 *sim, int lineNumber, const char* line);
extern "C" int enter_key_hit(simulator2 *sim, int lineNumber, assembly_instruction*** instructions, int* num_instructions);

#endif

#ifndef FROM_CPP

simulator2* create_simulator2(OutputString outputStringCallback, InputString inputStringCallback);
void delete_simulator2(simulator2 *sim);
assembly_instruction** assemble2 (simulator2 *sim, const char *program, int address, int *number_of_instructions);
assembly_instruction* get_instruction2 (simulator2 *sim, unsigned long long address);
unsigned char* get_memory2 (simulator2 *sim, unsigned long long address);
void get_instruction_string2(simulator2 *sim, int address, char *instruction_string);
int do_partial_assembly(simulator2 *sim, int lineNumber, const char* line);
int enter_key_hit(simulator2 *sim, int lineNumber, assembly_instruction*** instructions, int* num_instructions);

#endif

#endif
