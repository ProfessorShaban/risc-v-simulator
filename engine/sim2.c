
#include <stdio.h>
#include <stdlib.h>
#include "sim2.h"
#include "internal.h"


simulator2* create_simulator2(OutputString *outputStringCallback, InputString inputStringCallback)
{
    simulator2 *sim = malloc(sizeof (simulator2));
    sim->sim = create_simulator(outputStringCallback, inputStringCallback);
    return sim;
}

void delete_simulator2(simulator2 *sim)
{
    delete_simulator(sim->sim);
}

unsigned char* get_memory2 (simulator2* sim, unsigned long long address)
{
    return get_memory(sim->sim, address);
}

assembly_instruction** assemble2 (simulator2 *sim, const char *program, int address, int *number_of_instructions)
{
    assembly_instruction **results = assemble(sim->sim, program, address, number_of_instructions);
    sim->line_table = results;
    sim->num_lines = *number_of_instructions;
    return results;
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
//??? do partial assembly and update sim->line_table

    return 0;
}
