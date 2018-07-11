
#include <stdio.h>
#include <stdlib.h>
#include "sim.h"
#include "utils.h"


void show_instruction (assembly_instruction *instruction)
{
    if (instruction -> error == 0)
        printf("%llu: 0x%08x  // %d: %s\r\n", instruction -> address, instruction -> instruction,
               instruction -> source_line_number, instruction -> source_line);
    else
        printf("error: %s   // %d: %s\r\n", instruction -> error_message,
               instruction -> source_line_number, instruction -> source_line);
}

void show_instructions (assembly_instruction **instructions, int count)
{
    for (int i = 0; i < count; i++) {
        assembly_instruction *instruction = instructions[i];
        show_instruction (instruction);
    }
}

void open_file (simulator sim, char *command)
{
    int location = 0;
    char filename[255];
    sscanf (command, "%d %s", &location, filename);
    
    FILE *fp = fopen (filename, "r");
    if (fp == 0) {
        printf("Cannot find file %s\r\n", filename);
        return;
    }
    
    fseek(fp, 0L, SEEK_END);
    long file_size = ftell(fp);
    fseek(fp, 0L, SEEK_SET);
    
    char *file_contents = malloc (file_size + 2);
    
    int index = 0;
    while(1) {
        unsigned char c = fgetc(fp);
        if (feof(fp))
            break;
        file_contents[index++] = c;
    }
    file_contents[index++] = 0;
    
    fclose (fp);
    
    int number_of_instructions = 0;
    assembly_instruction **instructions = assemble(sim, file_contents, location, &number_of_instructions);
    show_instructions (instructions, number_of_instructions);
	deallocate_assemble_results(instructions, number_of_instructions);

	free(file_contents);
}

void get_bits (unsigned int num, char *str, int bits)
{
    int index = 0;
    while (num > 0) {
        str[index++] = '0' + num % 2;
        num = num >> 1;
    }
    while (index < bits)
        str[index++] = '0';
    str[index] = 0;
    
    // reverse string
    strrev(str);
}

void instruction_details (simulator sim, char *command)
{
    int address = 0;
    int index_of_space = 0;
    while (command[index_of_space] != ' ' && command[index_of_space] != 0)
        index_of_space ++;
    sscanf (command, "%d", &address);
    
    assembly_instruction *instruction = get_instruction(sim, address);
    
    if(instruction == 0)
        printf ("Unrecognized instruction\r\n");
    else {
        if (instruction -> format == 'J')
            printf("%llu: format:UJ, ", instruction -> address);
        else if (instruction -> format == 'B')
            printf("%llu: format:SB, ", instruction -> address);
        else
            printf("%llu: format:%c, ", instruction -> address, instruction -> format);
        char opcodeBits[32];
        get_bits (instruction -> opcode, opcodeBits, 7);
        char funct3Bits[32];
        get_bits (instruction -> funct3, funct3Bits, 3);
        switch (instruction -> format) {
            case 'R':
                printf("opcode:%s, funct3:%s, rs1:%d, rs2:%d, rd:%d\r\n",
                       opcodeBits,
                       funct3Bits,
                       instruction -> rs1,
                       instruction -> rs2,
                       instruction -> rd);
                break;
            case 'I':
                printf("opcode:%s, funct3:%s, rs1:%d, imm:%d, rd:%d\r\n",
                       opcodeBits,
                       funct3Bits,
                       instruction -> rs1,
                       instruction -> imm31,
                       instruction -> rd);
                break;
            case 'S':
                printf("opcode:%s, funct3:%s, rs1:%d, rs2:%d, imm[11:5]:%d, imm[4:0]:%d\r\n",
                       opcodeBits,
                       funct3Bits,
                       instruction -> rs1,
                       instruction -> rs2,
                       instruction -> funct7,
                       instruction -> rd);
                break;
            case 'B': // SB
                printf("opcode:%s, funct3:%s, rs1:%d, rs2:%d, imm[12|10:5]:%d, imm[4:1|11]:%d\r\n",
                       opcodeBits,
                       funct3Bits,
                       instruction -> rs1,
                       instruction -> rs2,
                       instruction -> funct7,
                       instruction -> rd);
                break;
            case 'U':
                printf("opcode:%s, rd:%d, imm:%d\r\n",
                       opcodeBits,
                       instruction -> rd,
                       instruction -> imm31);
                break;
            case 'J': // UJ
                printf("opcode:%s, rd:%d, imm:%d\r\n",
                       opcodeBits,
                       instruction -> rd,
                       instruction -> imm31);
                break;
            default:
                printf("unrecognized format\r\n");
                break;
        }
    }
}

void do_set_pc (simulator sim, char *command)
{
    int address = 0;
    int index_of_space = 0;
    while (command[index_of_space] != ' ' && command[index_of_space] != 0)
        index_of_space ++;
    sscanf (command, "%d", &address);

    set_pc(sim, address);
    unsigned long long pc = get_pc (sim);
    printf ("PC: %16llu\r\n", pc);
}

void show_registers (simulator sim, char *command)
{
    int hex = 0;
    int index_of_nonspace = 0;
    while (command[index_of_nonspace] == ' ')
        index_of_nonspace ++;
    if (command[index_of_nonspace] == 'x')
        hex = 1;

    unsigned long long *reg = get_registers (sim);
    double *freg = get_fregisters (sim);
    unsigned long long pc = get_pc (sim);
    printf ("PC: %llu\r\n", pc);
    for (int i = 0; i < 32; i++) {
        char padded [25];
        int max_width = 18;
        if (hex)
            sprintf (padded, "%16llx", reg[i]);
        else
            sprintf (padded, "%*lld", max_width, reg[i]);

        // move spaces to the back
        int nonspaceIndex = 0;
        while (nonspaceIndex < strlen(padded) && padded[nonspaceIndex] == ' ')
            nonspaceIndex++;
        copy_string(padded, &padded[nonspaceIndex]);
        int len = (int) strlen(padded);
        for (int i = 0; i < nonspaceIndex; i++)
            padded[len + i] = ' ';
        padded[len + nonspaceIndex] = 0;

        if (hex)
            printf ("x%02d: 0x%s ", i, padded);
        else
            printf ("x%02d: %s ", i, padded);
        if (i % 4 == 3)
            printf("\r\n");
    }
    printf("\r\n");
    for (int i = 0; i < 32; i++) {
        char padded [25];
        sprintf (padded, "%18.8g", freg[i]);

        // move spaces to the back
        int nonspaceIndex = 0;
        while (nonspaceIndex < strlen(padded) && padded[nonspaceIndex] == ' ')
            nonspaceIndex++;
        copy_string(padded, &padded[nonspaceIndex]);
        int len = (int) strlen(padded);
        for (int i = 0; i < nonspaceIndex; i++)
            padded[len + i] = ' ';
        padded[len + nonspaceIndex] = 0;

        printf ("f%02d: %s ", i, padded);
        if (i % 4 == 3)
            printf("\r\n");
    }
    printf("\r\n");
}

void show_memory (simulator sim, char *command)
{
    int address = 0;
    int size = 0;
    int index_of_space = 0;
    while (command[index_of_space] != ' ' && command[index_of_space] != 0)
        index_of_space ++;
    sscanf (command, "%d %d", &address, &size);
    
    unsigned char *mem = get_memory (sim, address);
    
    for (int i = 0; i < size; i += 4 * 4)
        printf ("%08x: %02x %02x %02x %02x   %02x %02x %02x %02x   %02x %02x %02x %02x   %02x %02x %02x %02x\r\n", address + i,
                mem[i + 0], mem[i + 1], mem[i + 2], mem[i + 3],
                mem[i + 4], mem[i + 5], mem[i + 6], mem[i + 7],
                mem[i + 8], mem[i + 9], mem[i +10], mem[i +11],
                mem[i +12], mem[i +13], mem[i +14], mem[i +15]);
}

void step (simulator sim, int run)
{
    char *error_message = 0;
	delta deltas[100];
    int num_deltas = 100;
	int deltas_used = 0;
	if (run)
		do_run (sim, &error_message, deltas, num_deltas, &deltas_used);
	else
		do_step (sim, &error_message, deltas, num_deltas, &deltas_used);

    if (error_message != 0)
        printf("%s\r\n", error_message);

    int reg_used[32];
    int freg_used[32];
    for (int i = 0; i < 32; i++)
        reg_used[i] = freg_used[i] = 0;
	for (int i = 0; i < deltas_used; i++) {
		delta delta = deltas[i];
		if (delta.type == register_delta) {
            if (!reg_used [delta.register_number]) {
                reg_used [delta.register_number] = 1;
                unsigned long long *reg = get_registers(sim);
                printf ("x%llu = 0x%llx (%lld)\r\n", (long long) delta.register_number, reg[delta.register_number], reg[delta.register_number]);
            }
		}
		else if (delta.type == fregister_delta) {
            if (!freg_used [delta.register_number]) {
                freg_used [delta.register_number] = 1;
                double *freg = get_fregisters(sim);
                printf ("f%llu = %.8g\r\n", (long long) delta.register_number, freg[delta.register_number]);
            }
		}
		else if (delta.type == memory_delta) {
			unsigned char *mem = get_memory(sim, delta.address);
			printf ("mem[%llu] = %02x %02x %02x %02x", delta.address, mem[0], mem[1], mem[2], mem[3]);
			if (delta.memory_bytes > 4)
				printf(" (+ %d bytes)", delta.memory_bytes - 4);
			printf ("\n");
		}
		else
			printf ("Unknown delta type\r\n");
	}

    // print PC
    unsigned long long pc = get_pc (sim);
    printf ("PC: %llu\r\n", pc);
}

void do_list_breakpoints (simulator sim)
{
	int *breakpoints;
	int num_of_breakpoints;
	get_breakpoints(sim, &num_of_breakpoints, &breakpoints);
	printf("Breakpoints: ");
	if (num_of_breakpoints == 0)
		printf ("(none)");
	for (int i = 0; i < num_of_breakpoints; i++) {
		printf ("%d", breakpoints[i]);
		if (i < num_of_breakpoints - 1)
			printf (", ");
	}
	printf ("\n");
}

void do_breakpoints (simulator sim, char *command)
{
	int address = 0;
	int index_of_space = 1;

	switch (command[0]) {
		case 'a':
			command++;
			while (command[index_of_space] != ' ' && command[index_of_space] != 0)
				index_of_space ++;
			sscanf (command, "%d", &address);
			add_breakpoint(sim, address);
			break;
		case 'd':
			command++;
			while (command[index_of_space] != ' ' && command[index_of_space] != 0)
				index_of_space ++;
			sscanf (command, "%d", &address);
			delete_breakpoint(sim, address);
			break;
		case 'l':
			break;
		default:
			printf("Unknown command\n");
			return;
	}

	do_list_breakpoints(sim);
}

void output_string (const char *str)
{
	printf("%s", str);
}

char line [1024];
const char * input_string ()
{
	if (fgets(line, sizeof(line), stdin)) {
		size_t len = strlen(line);
		if (len > 0 && line[len - 1] == '\n')
			line[--len] = '\0';
	}
	return line;
}

int main(int argc, const char * argv[])
{
    simulator sim = create_simulator(output_string, input_string);

    while (1) {
        char command[256];

        printf("> ");
        fgets(command, 255, stdin);

        switch (command[0]) {
            case 0:
                break;
            case '?':
            case 'h':
                printf("h                  : print this help\r\n");
                printf("g                  : show register contents\r\n");
                printf("g x                : show register contents (hex)\r\n");
                printf("m <address> <size> : show memory contents\r\n");
                printf("l <address> <file> : load file into location\r\n");
                printf("d <address>        : get instruction details\r\n");
                printf("p <address>        : set pc to location\r\n");
				printf("b a <address>      : add breakpoint at address\r\n");
				printf("b d <address>      : delete breakpoint at address\r\n");
				printf("b l                : list breakpoints\r\n");
                printf("s                  : step\r\n");
				printf("r                  : run\r\n");
                printf("q                  : quit\r\n");
                break;
			case 'r':
				step (sim, 1 /* run */);
				break;
			case 'b':
				do_breakpoints(sim, &command[2]);
				break;
            case 'g':
                show_registers (sim, &command[2]);
                break;
            case 'm':
                show_memory (sim, &command[2]);
                break;
            case 'q':
                break;
            case 'l':
                open_file (sim, &command[2]);
                break;
            case 'd':
                instruction_details (sim, &command[2]);
                break;
            case 'p':
                do_set_pc (sim, &command[2]);
                break;
            case 's':
                step (sim, 0 /* run */);
                break;
            case '\n':
                // empty line
                break;
            default:
                printf("Unknown command...\r\n");
                break;
        }
        
        if (command[0] == 'q')
            break;
    }
    
    delete_simulator(sim);
}
