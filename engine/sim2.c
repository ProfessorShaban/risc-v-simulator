
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sim2.h"
#include "internal.h"
#include "utils.h"


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
int enter_key_hit(simulator2 *sim, int lineNumber, assembly_instruction*** instructions, int* num_instructions)
{
    // insert an entry into the line table
    assembly_instruction *instruction = malloc (sizeof (assembly_instruction));
    instruction -> address = 0;
    instruction -> source_line[0] = 0;
    instruction -> source_line_number = lineNumber;
    instruction -> error = 0;
    instruction -> error_message = 0;
    for (int i = sim->num_lines - 1; i >= lineNumber; i--) {

        // move lines down
        sim->line_table[i+1] = sim->line_table[i];

        // also, adjust line numbers in subsequent line table records
        sim->line_table[i+1]->source_line_number++;
    }
    sim->line_table[lineNumber] = instruction;
    sim->num_lines++;

    (*num_instructions)++;

    // expand line table if needed
    if (*num_instructions % 10 == 0) {
        int newSize = *num_instructions + 10;
        sim->line_table = realloc (sim->line_table, sizeof (assembly_instruction) * newSize);

        *instructions = sim->line_table;
    }

    return 0;
}

// returns 0 for success, 1 otherwise
int do_partial_assembly(simulator2 *sim, int lineNumber, const char* line)
{
    // do partial assembly

    // if this line isn't there previously, fail and do full assembly
    if (sim->line_table == 0 || sim->num_lines <= lineNumber)
        return 1;

    assembly_instruction *old_instruction = sim->line_table[lineNumber];
    int oldAddress = old_instruction->address;
    simulator_internal *simi = (simulator_internal *) sim->sim;

    // find starting address for this line, if its address is 0. Work backward and find the previous line with a valid address, then add one word
    int address = 1000;
    for (int i = lineNumber - 1; i >= 0; i--) {
        if (sim->line_table[i]->address > 0) {
            address = sim->line_table[i]->address + 4;
            break;
        }
    }

    // if either the prior version, or the new version of the line contain a label definition, fail and do full assembly
    if (strchr(old_instruction->source_line, ':') != 0 || strchr(line, ':') != 0)
        return 1;

    // save existing 4 bytes
    int existingBytes[4];
    existingBytes[0] = existingBytes[1] = existingBytes[2] = existingBytes[3] = 0;
    if (old_instruction->error == 0 && address > 0)
        for (int i = 0; i < 4; i++)
            existingBytes[i] = simi->memory[address+i];

    assembly_instruction *instruction = assemble_line (sim->sim, address, line, lineNumber, old_instruction /* reuse_instruction */);

    // if line isn't empty, and not a proper instruction, consider it an instruction with invalid contents
    if (instruction != 0 && instruction->address == 0 && !empty_line(line))
        instruction->address = address;

    // if instruction is null, and existing instruction is null, nothing to do except replace existing instruction with new one
    if (instruction != 0 && instruction->address == 0 && oldAddress == 0)
        return 0;

    // if instruction is not null, and existing instruction is not null, replace existing instruction with new instruction
    if (instruction != 0 && instruction->address != 0 && oldAddress != 0)
        return 0;

    // if instruction is not null, and existing instruction is null, shift everything below this line down (line inserted)
    if (instruction != 0 && instruction->address != 0 && oldAddress == 0) {
        // move all subsequent instructions down by one word
        for (int i = lineNumber + 1; i < sim->num_lines; i++)
            if (sim->line_table[i]->address != 0)
                sim->line_table[i]->address += 4;

        int lastAddress = 9999;
        for (int i = sim->num_lines - 1; i >= 0; i--) {
            if (sim->line_table[i]->address != 0) {
                lastAddress = sim->line_table[i]->address + 4;
                break;
            }
        }

        // move instructions in memory down by one word
        if (lastAddress != 9999)
            for (unsigned long long i = lastAddress; i > sim->line_table[lineNumber]->address + 4; i --) {
                char byte = simi->memory[i-4];
                simi->memory[i] = byte;
            }

        // restore original instruction, to be directly after the new instruction
        for (int j = 0; j < 4; j++)
            simi->memory[sim->line_table[lineNumber]->address + 4 + j] = existingBytes[j];

        return 0;
    }

    return 1;
}

int empty_line(const char* line)
{
    for (int i = 0; i < (int) strlen(line); i++)
        if (line[i] != ' ')
            return 0;
    return 1;
}
