
#include <stdio.h>
#include <stdlib.h>
#include "sim2.h"


simulator2* create_simulator2(OutputString *outputStringCallback, InputString inputStringCallback)
{
    simulator2 *sim = malloc(sizeof (simulator2));
    sim->sim = create_simulator(outputStringCallback, inputStringCallback);
//    sim->num_lines = 0;
    sim->num_symbols = 0;
//    sim->line_table_size = 10;
    sim->symbol_table_size = 10;

//    sim->line_table = malloc(sizeof(assembly_line) * sim->line_table_size);
    sim->symbol_table = malloc(sizeof(symbol) * sim->symbol_table_size);

    return sim;
}

//void deallocate_line_table(simulator2 *sim) {
//    if (sim->line_table != 0) {
//        for (int i = 0; i < sim->num_lines; i++)
//            free(sim->line_table[i].line_contents);
//        sim->num_lines = 0;
//    }
//}

void deallocate_symbol_table(simulator2 *sim) {
    if (sim->symbol_table != 0) {
        for (int i = 0; i < sim->num_symbols; i++) {

            symbol_reference *current = sim->symbol_table[i].references;
            while (current) {
                symbol_reference *next = current->next;
                free (current);
                current = next;
            }
        }
        sim->num_symbols = 0;
    }
}

void delete_simulator2(simulator2 *sim)
{
    delete_simulator(sim->sim);
//    deallocate_line_table(sim);
//    free(sim->line_table);
    deallocate_symbol_table(sim);
    free(sim->symbol_table);
}

unsigned char* get_memory2 (simulator2* sim, unsigned long long address)
{
    return get_memory(sim->sim, address);
}

assembly_instruction** assemble2 (simulator2 *sim, const char *program, int address, int *number_of_instructions)
{
//    deallocate_line_table(sim);
    deallocate_symbol_table(sim);
    assembly_instruction **results = assemble(sim->sim, program, address, number_of_instructions);

//    if (*results != 0) {
//        for (int i = 0; i < *number_of_instructions; i++) {
//            assembly_instruction *instruction = results[i];
//            int j = 0;
//        }
//    }

    sim->line_table = results;
    sim->num_lines = *number_of_instructions;

    // rebuild symbol table
//???

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
//???

    return 0;
}
