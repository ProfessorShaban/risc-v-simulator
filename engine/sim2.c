
#include <stdio.h>
#include <stdlib.h>
#include "sim2.h"


simulator2* create_simulator2(OutputString *outputStringCallback, InputString inputStringCallback)
{
    simulator2 *sim = malloc(sizeof (simulator2));
    sim->sim = create_simulator(outputStringCallback, inputStringCallback);
    sim->line_table = 0;
    sim->symbol_table = 0;
    return sim;
}

void delete_simulator2(simulator2 *sim)
{
    delete_simulator(sim->sim);

// ??? deallocate line table
// ??? deallocate symbol table
}

unsigned char* get_memory2 (simulator2* sim, unsigned long long address)
{
    return get_memory(sim->sim, address);
}

assembly_instruction** assemble2 (simulator2 *sim, const char *program, int address, int *number_of_instructions)
{
    return assemble(sim->sim, program, address, number_of_instructions);
}

assembly_instruction* get_instruction2 (simulator2 *sim, unsigned long long address)
{
    return get_instruction(sim->sim, address);
}

void get_instruction_string2(simulator2 *sim, int address, char* instruction_string)
{
    return get_instruction_string(sim->sim, address, instruction_string);
}

// returns 0 for success, 1 otherwise
int do_partial_assembly(simulator2 *sim, int lineNumber, const char* line)
{
//???

    return 0;
}
