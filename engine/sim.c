
#include <stdio.h>
#include <stdlib.h>
#include "sim.h"
#include "internal.h"
#include "utils.h"
#include "instructions.h"


typedef struct {
    char format;
    char *mnemonic;
    int opcode;
    int funct7;
    int funct3;
    int check_funct7;
	int check_funct3;
	int operation;
} instruction_format;

typedef struct {
    char *mnemonic;
    int index;  // index into instruction format array
} instruction_index_entry;

typedef struct {
    int opcode;
    int funct3;
    int funct7;
    int check_funct3;
    int check_funct7;
    int index;  // index into instruction format array
} instruction_index_opcode_entry;


#define NUM_INSTRUCTIONS 200
instruction_format instruction_formats[NUM_INSTRUCTIONS];
instruction_index_entry instruction_index[NUM_INSTRUCTIONS];
instruction_index_opcode_entry instruction_opcode_index[NUM_INSTRUCTIONS];

int instruction_count = 0;


void initialize_format (int *array_index, instruction_format *format, char* mnemonic, int operation, char format_code, int opcode,
	int funct3, int check_funct3, int funct7, int check_funct7)
{
    format -> format = format_code;
    format -> mnemonic = mnemonic;
    format -> opcode = opcode;
    format -> funct3 = funct3;
    format -> funct7 = funct7;
    format -> check_funct7 = check_funct7;
	format -> check_funct3 = check_funct3;
	format -> operation = operation;

    // also add to the index
    instruction_index[*array_index].index = *array_index;
    instruction_index[*array_index].mnemonic = mnemonic;

    // add to the opcode index
    instruction_opcode_index[*array_index].index = *array_index;
    instruction_opcode_index[*array_index].opcode = format -> opcode;
    instruction_opcode_index[*array_index].funct3 = format -> funct3;
    instruction_opcode_index[*array_index].check_funct3 = format -> check_funct3;
    instruction_opcode_index[*array_index].funct7 = format -> funct7;
    instruction_opcode_index[*array_index].check_funct7 = format -> check_funct7;

    (*array_index)++;
}

int compare_index_entries(const void* a, const void* b)
{
     return stricmp(((instruction_index_entry*) a)->mnemonic, ((instruction_index_entry*) b)->mnemonic);
}

int compare_opcode_index_entries(const void* a, const void* b)
{
    instruction_index_opcode_entry* a_entry = (instruction_index_opcode_entry *) a;
    instruction_index_opcode_entry* b_entry = (instruction_index_opcode_entry *) b;

    if (a_entry->opcode > b_entry->opcode)
        return 1;
    if (a_entry->opcode < b_entry->opcode)
        return -1;

    if (a_entry->funct3 > b_entry->funct3)
        return 1;
    if (a_entry->funct3 < b_entry->funct3)
        return -1;

    if (a_entry->funct7 > b_entry->funct7)
        return 1;
    if (a_entry->funct7 < b_entry->funct7)
        return -1;

    return 0;
}

void initialize_formats()
{
    if (instruction_formats[0].opcode != 0)
        return;

    int i = 0;

	// load
    initialize_format (&i, &instruction_formats[i], "lb", INSTRUCTION_LB, 'I', 0x03, 0, 1, 0, 0);
    initialize_format (&i, &instruction_formats[i], "lh", INSTRUCTION_LH, 'I', 0x03, 1, 1, 0, 0);
    initialize_format (&i, &instruction_formats[i], "lw", INSTRUCTION_LW, 'I', 0x03, 2, 1, 0, 0);
    initialize_format (&i, &instruction_formats[i], "ld", INSTRUCTION_LD, 'I', 0x03, 3, 1, 0, 0);
    initialize_format (&i, &instruction_formats[i], "lbu", INSTRUCTION_LBU, 'I', 0x03, 4, 1, 0, 0);
    initialize_format (&i, &instruction_formats[i], "lhu", INSTRUCTION_LHU, 'I', 0x03, 5, 1, 0, 0);
    initialize_format (&i, &instruction_formats[i], "lwu", INSTRUCTION_LWU, 'I', 0x03, 6, 1, 0, 0);
    //initialize_format (&i, &instruction_formats[i], "ldu", INSTRUCTION_LDU, 'I', 0x03, 0, 1, 0, 0);

	// store
    initialize_format (&i, &instruction_formats[i], "sb", INSTRUCTION_SB, 'S', 0x23, 0, 1, 0, 0);
    initialize_format (&i, &instruction_formats[i], "sh", INSTRUCTION_SH, 'S', 0x23, 1, 1, 0, 0);
    initialize_format (&i, &instruction_formats[i], "sw", INSTRUCTION_SW, 'S', 0x23, 2, 1, 0, 0);
    initialize_format (&i, &instruction_formats[i], "sd", INSTRUCTION_SD, 'S', 0x23, 3, 1, 0, 0);

	// shift
    initialize_format (&i, &instruction_formats[i], "sll", INSTRUCTION_SLL, 'R', 0x33, 1, 1, 0, 1);
    initialize_format (&i, &instruction_formats[i], "sllw", INSTRUCTION_SLLW, 'R', 0x3B, 1, 1, 0, 1);
    //initialize_format (&i, &instruction_formats[i], "slld", INSTRUCTION_SLLD, 'R', 0x3B, 1, 1, 0, 1);
    initialize_format (&i, &instruction_formats[i], "slli", INSTRUCTION_SLLI, 'X', 0x13, 1, 1, 0, 1);
    initialize_format (&i, &instruction_formats[i], "slliw", INSTRUCTION_SLLIW, 'X', 0x1B, 1, 1, 0, 1);
    //initialize_format (&i, &instruction_formats[i], "sllid", INSTRUCTION_SLLID, 'X', 0x3B, 1, 1, 0, 1);
    initialize_format (&i, &instruction_formats[i], "srl", INSTRUCTION_SRL, 'R', 0x33, 5, 1, 0, 1);
    initialize_format (&i, &instruction_formats[i], "srlw", INSTRUCTION_SRLW, 'R', 0x3B, 5, 1, 0, 1);
    //initialize_format (&i, &instruction_formats[i], "srld", INSTRUCTION_SRLD, 'R', 0x3B, 1, 1, 0, 1);
    initialize_format (&i, &instruction_formats[i], "srli", INSTRUCTION_SRLI, 'X', 0x13, 5, 1, 0, 1);
    initialize_format (&i, &instruction_formats[i], "srliw", INSTRUCTION_SRLIW, 'X', 0x1B, 5, 1, 0, 1);
    //initialize_format (&i, &instruction_formats[i], "srlid", INSTRUCTION_SRLID, 'X', 0x3B, 1, 1, 0, 1);
    initialize_format (&i, &instruction_formats[i], "sra", INSTRUCTION_SRA, 'R', 0x33, 5, 1, 20, 1);
    initialize_format (&i, &instruction_formats[i], "sraw", INSTRUCTION_SRAW, 'R', 0x3B, 5, 1, 20, 1);
    //initialize_format (&i, &instruction_formats[i], "srad", INSTRUCTION_SRAD, 'R', 0x3B, 1, 1, 0, 1);
    initialize_format (&i, &instruction_formats[i], "srai", INSTRUCTION_SRAI, 'X', 0x13, 5, 1, 20, 1);
    initialize_format (&i, &instruction_formats[i], "sraiw", INSTRUCTION_SRAIW, 'X', 0x1B, 5, 1, 20, 1);
    //initialize_format (&i, &instruction_formats[i], "sraid", INSTRUCTION_SRAID, 'X', 0x3B, 1, 1, 0, 1);

	// arithmetic
    initialize_format (&i, &instruction_formats[i], "add", INSTRUCTION_ADD, 'R', 0x33, 0, 1, 0, 1);
    initialize_format (&i, &instruction_formats[i], "addw", INSTRUCTION_ADDW, 'R', 0x3B, 0, 1, 0, 1);
    //initialize_format (&i, &instruction_formats[i], "addd", INSTRUCTION_ADDD, 'R', 0x33, 0, 1, 0, 1);
    initialize_format (&i, &instruction_formats[i], "addi", INSTRUCTION_ADDI, 'I', 0x13, 0, 1, 0, 0);
    initialize_format (&i, &instruction_formats[i], "addiw", INSTRUCTION_ADDIW, 'I', 0x1B, 0, 1, 0, 0);
    //initialize_format (&i, &instruction_formats[i], "addid", INSTRUCTION_ADDID, 'I', 0x33, 0, 1, 0, 1);
    initialize_format (&i, &instruction_formats[i], "sub", INSTRUCTION_SUB, 'R', 0x33, 0, 1, 0x20, 1);
    initialize_format (&i, &instruction_formats[i], "subw", INSTRUCTION_SUBW, 'R', 0x3B, 0, 1, 20, 1);
    //initialize_format (&i, &instruction_formats[i], "subd", INSTRUCTION_SUBD, 'R', 0x33, 0, 1, 0, 1);
    initialize_format (&i, &instruction_formats[i], "lui", INSTRUCTION_LUI, 'U', 0x37, 0, 0, 0, 0);
    initialize_format (&i, &instruction_formats[i], "auipc", INSTRUCTION_AUIPC, 'U', 0x17, 0, 0, 0, 0);

	// logical
    initialize_format (&i, &instruction_formats[i], "xor", INSTRUCTION_XOR, 'R', 0x33, 4, 1, 0, 1);
    initialize_format (&i, &instruction_formats[i], "xori", INSTRUCTION_XORI, 'I', 0x13, 4, 1, 0, 0);
    initialize_format (&i, &instruction_formats[i], "or", INSTRUCTION_OR, 'R', 0x33, 6, 1, 0, 1);
    initialize_format (&i, &instruction_formats[i], "ori", INSTRUCTION_ORI, 'I', 0x13, 6, 1, 0, 0);
    initialize_format (&i, &instruction_formats[i], "and", INSTRUCTION_AND, 'R', 0x33, 7, 1, 0, 1);
    initialize_format (&i, &instruction_formats[i], "andi", INSTRUCTION_ANDI, 'I', 0x13, 7, 1, 0, 0);

	// compare
    initialize_format (&i, &instruction_formats[i], "slt", INSTRUCTION_SLT, 'R', 0x33, 2, 1, 0, 0);
    initialize_format (&i, &instruction_formats[i], "slti", INSTRUCTION_SLTI, 'I', 0x13, 2, 1, 0, 0);
    initialize_format (&i, &instruction_formats[i], "sltu", INSTRUCTION_SLTU, 'R', 0x33, 3, 1, 0, 0);
    initialize_format (&i, &instruction_formats[i], "sltiu", INSTRUCTION_SLTIU, 'I', 0x13, 3, 1, 0, 0);

	// branch
    initialize_format (&i, &instruction_formats[i], "beq", INSTRUCTION_BEQ, 'B', 0x63, 0, 1, 0, 0);
    initialize_format (&i, &instruction_formats[i], "bne", INSTRUCTION_BNE, 'B', 0x63, 1, 1, 0, 0);
    initialize_format (&i, &instruction_formats[i], "blt", INSTRUCTION_BLT, 'B', 0x63, 4, 1, 0, 0);
    initialize_format (&i, &instruction_formats[i], "bge", INSTRUCTION_BGE, 'B', 0x63, 5, 1, 0, 0);
    initialize_format (&i, &instruction_formats[i], "bltu", INSTRUCTION_BLTU, 'B', 0x63, 6, 1, 0, 0);
    initialize_format (&i, &instruction_formats[i], "bgeu", INSTRUCTION_BGEU, 'B', 0x63, 7, 1, 0, 0);

	// jump and link
    initialize_format (&i, &instruction_formats[i], "jal", INSTRUCTION_JAL, 'J', 0x6F, 0, 0, 0, 0);
    initialize_format (&i, &instruction_formats[i], "jalr", INSTRUCTION_JALR, 'I', 0x67, 0, 1, 0, 0);

	// multiply/divide
    initialize_format (&i, &instruction_formats[i], "mul", INSTRUCTION_MUL, 'R', 0x33, 0, 1, 1, 1);
    initialize_format (&i, &instruction_formats[i], "mulw", INSTRUCTION_MULW, 'R', 0x3B, 0, 1, 1, 1);
    //initialize_format (&i, &instruction_formats[i], "muld", INSTRUCTION_MULD, 'R', 0x33, 0, 1, 1, 1);
    initialize_format (&i, &instruction_formats[i], "mulh", INSTRUCTION_MULH, 'R', 0x33, 1, 1, 1, 1);
    initialize_format (&i, &instruction_formats[i], "mulHSU", INSTRUCTION_MULHSU, 'R', 0x33, 2, 1, 1, 1);
    initialize_format (&i, &instruction_formats[i], "mulhu", INSTRUCTION_MULHU, 'R', 0x33, 3, 1, 1, 1);
    initialize_format (&i, &instruction_formats[i], "div", INSTRUCTION_DIV, 'R', 0x33, 4, 1, 1, 1);
    initialize_format (&i, &instruction_formats[i], "divw", INSTRUCTION_DIVW, 'R', 0x3B, 4, 1, 1, 1);
    //initialize_format (&i, &instruction_formats[i], "divd", INSTRUCTION_DIVD, 'R', 0x33, 0, 1, 1, 1);
    initialize_format (&i, &instruction_formats[i], "divu", INSTRUCTION_DIVU, 'R', 0x33, 5, 1, 1, 1);
    initialize_format (&i, &instruction_formats[i], "rem", INSTRUCTION_REM, 'R', 0x33, 6, 1, 1, 1);
    initialize_format (&i, &instruction_formats[i], "remw", INSTRUCTION_REMW, 'R', 0x3B, 6, 1, 1, 1);
    //initialize_format (&i, &instruction_formats[i], "remd", INSTRUCTION_REMD, 'R', 0x33, 0, 1, 1, 1);
    initialize_format (&i, &instruction_formats[i], "remu", INSTRUCTION_REMU, 'R', 0x33, 7, 1, 1, 1);
    initialize_format (&i, &instruction_formats[i], "remuw", INSTRUCTION_REMUW, 'R', 0x3B, 7, 1, 1, 1);
    //initialize_format (&i, &instruction_formats[i], "remud", INSTRUCTION_REMUD, 'R', 0x33, 0, 1, 1, 1);

	// floating point convert
    initialize_format (&i, &instruction_formats[i], "fcvt.d.l", INSTRUCTION_FCVT_D_L, 'R', 0x53, 0, 0, 0x69, 1);
    initialize_format (&i, &instruction_formats[i], "fcvt.l.d", INSTRUCTION_FCVT_L_D, 'R', 0x53, 0, 0, 0x61, 1);
    initialize_format (&i, &instruction_formats[i], "fcvt.w.s", INSTRUCTION_FCVT_W_S, 'R', 0x53, 0, 0, 0x60, 1);
    initialize_format (&i, &instruction_formats[i], "fcvt.s.w", INSTRUCTION_FCVT_S_W, 'R', 0x53, 0, 0, 0x68, 1);

	// floating point load
    initialize_format (&i, &instruction_formats[i], "fld", INSTRUCTION_FLD, 'I', 0x07, 3, 1, 0, 1);
    initialize_format (&i, &instruction_formats[i], "flw", INSTRUCTION_FLW, 'I', 0x07, 2, 1, 0, 1);

	// floating point store
    initialize_format (&i, &instruction_formats[i], "fsd", INSTRUCTION_FSD, 'S', 0x27, 3, 1, 0, 0);
    initialize_format (&i, &instruction_formats[i], "fsw", INSTRUCTION_FSW, 'S', 0x27, 2, 1, 0, 0);

	// floating point arithmetic
    initialize_format (&i, &instruction_formats[i], "fadd.d", INSTRUCTION_FADD_D, 'R', 0x53, 0, 0, 1, 1);
    initialize_format (&i, &instruction_formats[i], "fsub.d", INSTRUCTION_FSUB_D, 'R', 0x53, 0, 0, 5, 1);
    initialize_format (&i, &instruction_formats[i], "fmul.d", INSTRUCTION_FMUL_D, 'R', 0x53, 0, 0, 9, 1);
    initialize_format (&i, &instruction_formats[i], "fdiv.d", INSTRUCTION_FDIV_D, 'R', 0x53, 0, 0, 13, 1);
    initialize_format (&i, &instruction_formats[i], "fsqrt.d", INSTRUCTION_FSQRT_D, 'R', 0x53, 0, 0, 0x2d, 1);
    initialize_format (&i, &instruction_formats[i], "fadd.s", INSTRUCTION_FADD_S, 'R', 0x53, 0, 0, 0, 1);
    initialize_format (&i, &instruction_formats[i], "fsub.s", INSTRUCTION_FSUB_S, 'R', 0x53, 0, 0, 4, 1);
    initialize_format (&i, &instruction_formats[i], "fmul.s", INSTRUCTION_FMUL_S, 'R', 0x53, 0, 0, 8, 1);
    initialize_format (&i, &instruction_formats[i], "fdiv.s", INSTRUCTION_FDIV_S, 'R', 0x53, 0, 0, 12, 1);
    initialize_format (&i, &instruction_formats[i], "fsqrt.s", INSTRUCTION_FSQRT_S, 'R', 0x53, 0, 0, 0x2c, 1);

	// floating point compare
    initialize_format (&i, &instruction_formats[i], "feq.d", INSTRUCTION_FEQ_D, 'R', 0x53, 2, 1, 0x51, 1);
    initialize_format (&i, &instruction_formats[i], "flt.d", INSTRUCTION_FLT_D, 'R', 0x53, 1, 1, 0x51, 1);
    initialize_format (&i, &instruction_formats[i], "fle.d", INSTRUCTION_FLE_D, 'R', 0x53, 0, 1, 0x51, 1);
    initialize_format (&i, &instruction_formats[i], "feq.s", INSTRUCTION_FEQ_S, 'R', 0x53, 2, 1, 0x50, 1);
    initialize_format (&i, &instruction_formats[i], "flt.s", INSTRUCTION_FLT_S, 'R', 0x53, 1, 1, 0x50, 1);
    initialize_format (&i, &instruction_formats[i], "fle.s", INSTRUCTION_FLE_S, 'R', 0x53, 0, 1, 0x50, 1);

	// meta
    initialize_format (&i, &instruction_formats[i], "outstr", INSTRUCTION_OUTSTR, 'M', 0x5a, 0, 1, 0, 1);
    initialize_format (&i, &instruction_formats[i], "outln", INSTRUCTION_OUTLN, 'M', 0x5a, 0, 1, 1, 1);
    initialize_format (&i, &instruction_formats[i], "outint", INSTRUCTION_OUTINT, 'M', 0x5a, 0, 1, 2, 1);
    initialize_format (&i, &instruction_formats[i], "instr", INSTRUCTION_INSTR, 'M', 0x5a, 0, 1, 3, 1);
    initialize_format (&i, &instruction_formats[i], "inint", INSTRUCTION_ININT, 'M', 0x5a, 0, 1, 4, 1);

    instruction_count = i;

    // sort indexes
    qsort (instruction_index, instruction_count, sizeof(instruction_index_entry), compare_index_entries);
    qsort (instruction_opcode_index, instruction_count, sizeof(instruction_index_opcode_entry), compare_opcode_index_entries);
}

simulator create_simulator(OutputString *outputStringCallback, InputString inputStringCallback)
{
    initialize_formats ();

	simulator_internal *sim = malloc(sizeof (simulator_internal));
	sim -> memory = malloc(MEMSIZE);
	sim -> reg = malloc (sizeof (long long int) * 32);
    sim -> freg = malloc (sizeof (double) * 32);
	sim -> outputStringCallback = outputStringCallback;
	sim -> inputStringCallback = inputStringCallback;

	// initialize registers
	for (int i = 0; i < 32; i++) {
		sim -> reg[i] = 0;
        sim -> freg[i] = 0;
    }

	// initialize memory
	for (int i = 0; i < MEMSIZE; i++)
		sim -> memory[i] = 0;

	sim -> pc = 0;

    sim -> labels = 0;

	sim -> num_breakpoints = 0;

	return (simulator) sim;
}

void deallocate_labels (label *label)
{
	if (label == 0)
		return;
	deallocate_labels (label -> next_label);
	free(label);
}

void delete_simulator(simulator sim)
{
	simulator_internal *simi = (simulator_internal *) sim;
	deallocate_labels(simi -> labels);
	free(simi -> memory);
	free(simi -> reg);
	free(simi -> freg);
	free(simi);
}

unsigned char* get_memory (simulator sim, unsigned long long address)
{
	simulator_internal *simi = (simulator_internal *) sim;
	return & (simi -> memory[address]);
}

unsigned long long int* get_registers (simulator sim)
{
    simulator_internal *simi = (simulator_internal *) sim;
    return simi -> reg;
}

double* get_fregisters (simulator sim)
{
	simulator_internal *simi = (simulator_internal *) sim;
	return simi -> freg;
}

void set_pc (simulator sim, unsigned long long address)
{
	simulator_internal *simi = (simulator_internal *) sim;
	simi -> pc = address;
}

unsigned long long get_pc (simulator sim)
{
	simulator_internal *simi = (simulator_internal *) sim;
	return simi -> pc;
}

void get_line (char *line, const char *program, int *index, int line_size)
{
	int line_index = 0;
    while (program[*index] != 0 && program[*index] != '\n' && line_index < line_size - 2) {
		line[line_index] = program[*index];
		line_index ++;
		(*index) ++;
	}
	if (program[*index] == '\n')
		(*index) ++;

	line[line_index] = 0;
}

void get_token (char *line, int *index, char *token)
{
	token[0] = 0;

	// ignore empty space
	while (line[*index] == ' ' || line[*index] == '\t')
		(*index)++;

	// return empty token if we see a comment
	if (line[*index] == '/' && line[*index+1] == '/') {
		token[0] = 0;
		while (line[*index] != 0)
			(*index)++;
		return;
	}

    // each of these is its own token
	if (line[*index] == ',') {
		(*index)++;
		token[0] = ',';
		token[1] = 0;
		return;
	}
    if (line[*index] == '(') {
        (*index)++;
        token[0] = '(';
        token[1] = 0;
        return;
    }
    if (line[*index] == ')') {
        (*index)++;
        token[0] = ')';
        token[1] = 0;
        return;
    }

	// copy characters until end of line or end of token
	int token_index = 0;
	while (line[*index] != ' ' && line[*index] != '\t' && line[*index] != 0 &&
            line[*index] != ',' &&
            line[*index] != '(' &&
            line[*index] != ')' &&
            line[*index] != '/' && line[*index] != ':') {
		token[token_index++] = line[(*index)++];
	}
	token[token_index] = 0;
}

int get_int(char *token, int *success)
{
	*success = 1;

	int result = 0;
	int index = 0;
	int negative = 0;
	if (token[0] == '-') {
		index = 1;
		negative = 1;
	}
    int found_a_digit = 0;

    while (token[index] >= '0' && token[index] <= '9') {
		result = result * 10 + token[index] - '0';
		index++;
        found_a_digit = 1;
	}
    if (!found_a_digit) {
        *success = 0;
        return 0;
    }
	return negative ? -result : result;
}

double get_double(char *token, int *success)
{
	*success = 1;

	double result = 0;
	sscanf(token, "%lf", &result);
	return result;
}

instruction_format* get_instruction_format (char* mnemonic)
{
    // binary search for the mnemonic in the index
    int high = instruction_count;
    int low = 0;
    while (low <= high) {
        int mid = (low + high - 1) / 2;

        // special case
        if (high == low)
            mid = high;

        int compare = stricmp(mnemonic, instruction_index[mid].mnemonic);
        if (compare == 0) {
            int index = instruction_index[mid].index;
            return &instruction_formats[index];
        }

        // special case continued
        if (high == low)
            break;

        if (compare > 0)
            low = mid + 1;
        else
            high = mid - 1;
    }

    return 0;
}

// 0 for success, 1 for failure
int get_comma (char *line, int *index, assembly_instruction *instruction)
{
    char token[128];
    get_token(line, index, token);
    if (stricmp(token, ",") != 0) {
        instruction -> error = 1;
        instruction -> error_message = "Unrecognized input (expecting comma)";
        return 1;
    }
    return 0;
}

// 0 for success, 1 for failure
int get_certain_token (char *line, int *index, assembly_instruction *instruction, const char *expected_token)
{
    char token[128];
    get_token(line, index, token);
    if (stricmp((char *) token, (char *) expected_token) != 0) {
        instruction -> error = 1;
        instruction -> error_message = "Unrecognized input";
        return 1;
    }
    return 0;
}

// 0 for success, 1 for failure
int get_register (char *line, int *index, assembly_instruction *instruction, unsigned int *reg, char *error_message)
{
    char token[128];
    get_token(line, index, token);
    if (token[0] != 'x' && token[0] != 'X' && token[0] != 'f' && token[0] == 'F') {
        instruction -> error = 1;
        instruction -> error_message = error_message;
        return 1;
    }

	int success = 0;
    *reg = get_int(&token[1], &success);
    if (success != 1) {
        instruction -> error = 1;
        instruction -> error_message = error_message;
        return 1;
    }
    return 0;
}

// 0 for success, 1 for failure
int get_immediate (char *line, int *index, assembly_instruction *instruction, unsigned int *imm, char *error_message)
{
    char token[128];
    get_token(line, index, token);

    int success = 0;
    *imm = get_int(token, &success);
    if (success != 1) {
        instruction -> error = 1;
        instruction -> error_message = error_message;
        return 1;
    }
    return 0;
}

// 0 for success, 1 for failure
int get_immediate_or_offset (char *line, int *index, assembly_instruction *instruction, unsigned int *imm,
    char *error_message, int address, simulator_internal *simi)
{
    char token[128];
    get_token(line, index, token);

    // check for label reference
    if ((token[0] >= 'a' && token[0] <= 'z') || (token[0] >= 'A' && token[0] <= 'Z')) {
        // look up label
        label *labels = simi -> labels;
        while (labels != 0) {
            if (!stricmp(labels -> label, token)) {
                *imm = (int) ((labels -> address - address)/2);
                return 0;
            }
            labels = labels -> next_label;
        }

        instruction -> error = 1;
        instruction -> error_message = error_message;
        return 1;
    }

	int success = 0;
	*imm = get_int(token, &success);
    if (success != 1) {
        instruction -> error = 1;
        instruction -> error_message = error_message;
        return 1;
    }
    return 0;
}

// 0 = success, 1 = error
int build_instruction_m (assembly_instruction *instruction, char *line, int index)
{
	if (instruction -> funct7 != 1)	// everything except outln requires one register
    	if (get_register (line, &index, instruction, &(instruction -> rd), "unrecognized rd")) return 1;

    instruction -> instruction =
        instruction -> funct7 << 25 |
        instruction -> rs2 << 20 |
        instruction -> rs1 << 15 |
        instruction -> funct3 << 12 |
        instruction -> rd << 7 |
        instruction -> opcode;

    return 0;
}

// 0 = success, 1 = error
int build_instruction_r (instruction_format *format, assembly_instruction *instruction, char *line, int index)
{
	if (get_register (line, &index, instruction, &(instruction -> rd), "unrecognized rd")) return 1;
	if (get_comma (line, &index, instruction)) return 1;
	if (get_register (line, &index, instruction, &(instruction -> rs1), "unrecognized rs1")) return 1;

	// FMV, FCVT, etc. don't have rs2
	if ((format -> operation >= INSTRUCTION_FMV_H_X && format -> operation <= INSTRUCTION_FCVT_LU_D) ||
		format -> operation == INSTRUCTION_FSQRT_S ||
		format -> operation == INSTRUCTION_FSQRT_D) {
	}
	else {
		if (get_comma (line, &index, instruction)) return 1;
		if (get_register (line, &index, instruction, &(instruction -> rs2), "unrecognized rs2")) return 1;
	}

	// special cases
	if (format -> operation == INSTRUCTION_FCVT_D_L) instruction -> rs2 = 2;
	if (format -> operation == INSTRUCTION_FCVT_D_LU) instruction -> rs2 = 3;
	if (format -> operation == INSTRUCTION_FCVT_L_D) instruction -> rs2 = 2;
	if (format -> operation == INSTRUCTION_FCVT_LU_D) instruction -> rs2 = 3;

	instruction -> instruction =
		instruction -> funct7 << 25 |
		instruction -> rs2 << 20 |
		instruction -> rs1 << 15 |
		instruction -> funct3 << 12 |
		instruction -> rd << 7 |
		instruction -> opcode;

	return 0;
}

// 0 = success, 1 = error
int build_instruction_i (assembly_instruction *instruction, char *line, int index)
{
	if (get_register (line, &index, instruction, &(instruction -> rd), "unrecognized rd")) return 1;
	if (get_comma (line, &index, instruction)) return 1;
	if (get_register (line, &index, instruction, &(instruction -> rs1), "unrecognized rs1")) return 1;
	if (get_comma (line, &index, instruction)) return 1;
	if (get_immediate (line, &index, instruction, &(instruction -> imm31), "unrecognized immediate")) return 1;

    if (instruction -> imm31 > 2047 || instruction -> imm31 < -2048) {
        instruction -> error_message = "immediate value out of range";
        instruction -> error = 1;
        return 1;
    }

	instruction -> instruction =
	instruction -> imm31 << 20 |
	instruction -> rs1 << 15 |
	instruction -> funct3 << 12 |
	instruction -> rd << 7 |
	instruction -> opcode;

	return 0;
}

// 0 = success, 1 = error
int build_instruction_x (assembly_instruction *instruction, char *line, int index)
{
    if (get_register (line, &index, instruction, &(instruction -> rd), "unrecognized rd")) return 1;
    if (get_comma (line, &index, instruction)) return 1;
    if (get_register (line, &index, instruction, &(instruction -> rs1), "unrecognized rs1")) return 1;
    if (get_comma (line, &index, instruction)) return 1;
    if (get_immediate (line, &index, instruction, &(instruction -> rs2), "unrecognized immediate")) return 1;

    instruction -> instruction =
		instruction -> funct7 << 25 |
        instruction -> rs2 << 20 |
        instruction -> rs1 << 15 |
        instruction -> funct3 << 12 |
        instruction -> rd << 7 |
        instruction -> opcode;
    
    return 0;
}

// 0 = success, 1 = error
int build_instruction_s (assembly_instruction *instruction, char *line, int index)
{
    if (get_register (line, &index, instruction, &(instruction -> rs1), "unrecognized rs1")) return 1;
    if (get_comma (line, &index, instruction)) return 1;
    if (get_register (line, &index, instruction, &(instruction -> rs2), "unrecognized rs2")) return 1;
    if (get_comma (line, &index, instruction)) return 1;
    unsigned int imm = 0;
    if (get_immediate (line, &index, instruction, &imm, "unrecognized immediate")) return 1;
    instruction -> imm31 = imm >> 5;
    instruction -> imm11 = imm & 0x1f;

    instruction -> instruction =
        instruction -> imm31 << 25 |
        instruction -> rs2 << 20 |
        instruction -> rs1 << 15 |
        instruction -> funct3 << 12 |
        instruction -> imm11 << 7 |
        instruction -> opcode;

    return 0;
}

// 0 = success, 1 = error
int build_instruction_sb (assembly_instruction *instruction, char *line, int index, int address, simulator_internal *simi)
{
    if (get_register (line, &index, instruction, &(instruction -> rs1), "unrecognized rs1")) return 1;
    if (get_comma (line, &index, instruction)) return 1;
    if (get_register (line, &index, instruction, &(instruction -> rs2), "unrecognized rs2")) return 1;
    if (get_comma (line, &index, instruction)) return 1;
    unsigned int imm = 0;
    if (get_immediate_or_offset (line, &index, instruction, &imm, "unrecognized immediate", address, simi)) return 1;

	unsigned int bit11 = (imm & 0x400) >> 10;
	unsigned int bit12 = (imm & 0X800) >> 11;
	unsigned int bits1to4 = (imm & 0xf);
	unsigned int bits5to10 = (imm & 0x3f0) >> 4;
    instruction -> imm31 = (bit12 << 6) | bits5to10;
    instruction -> imm11 = (bits1to4 << 1) | bit11;

    instruction -> instruction =
        instruction -> imm31 << 25 |
        instruction -> rs2 << 20 |
        instruction -> rs1 << 15 |
        instruction -> funct3 << 12 |
        instruction -> imm11 << 7 |
        instruction -> opcode;
    
    return 0;
}

// 0 = success, 1 = error
int build_instruction_u (assembly_instruction *instruction, char *line, int index, int address, simulator_internal *simi)
{
    if (get_register (line, &index, instruction, &(instruction -> rd), "unrecognized rd")) return 1;
    if (get_comma (line, &index, instruction)) return 1;
	if (get_immediate_or_offset (line, &index, instruction, &(instruction -> imm31), "unrecognized immediate", address, simi)) return 1;

    instruction -> instruction =
        instruction -> imm31 << 12 |
        instruction -> rd << 7 |
        instruction -> opcode;
    
    return 0;
}

// 0 = success, 1 = error
int build_instruction_uj (assembly_instruction *instruction, char *line, int index, int address, simulator_internal *simi)
{
    if (get_register (line, &index, instruction, &(instruction -> rd), "unrecognized rd")) return 1;
    if (get_comma (line, &index, instruction)) return 1;
	unsigned int imm = 0;
	if (get_immediate_or_offset (line, &index, instruction, &imm, "unrecognized immediate", address, simi)) return 1;

	unsigned int bits1to10 = (imm & 0x3ff);
	unsigned int bit11 = (imm & 0x400) >> 10;
	unsigned int bits12to19 = (imm & 0x3f800) >> 11;
	unsigned int bit20 = (imm & 0X80000) >> 19;
	instruction -> imm31 = (bit20 << 19) | (bits1to10 << 9) | (bit11 << 8) | bits12to19;

    instruction -> instruction =
        instruction -> imm31 << 12 |
        instruction -> rd << 7 |
        instruction -> opcode;

    return 0;
}

// if it's a pseudoinstruction, expand the pseudoinstruction into its long form, changing the token, line and index appropriately
void translate_pseudoinstruction(char *token, char *line, int *index)
{
	// nop
	if (stricmp(token, "nop") == 0) {
		// nop = addi x0, x0, 0
		char new_line [128];
		copy_string (new_line, line);
		*index = *index - 3;	// back up to 'nop' start
		new_line [*index] = 0;
		append_string (new_line, (const char *) "addi x0, x0, 0");
		append_string (new_line, line + *index + 3);
		copy_string (token, "addi");
		*index += 4;	// 4 = strlen("addi")
		copy_string (line, new_line);
	}

	// j
	if (stricmp(token, "j") == 0) {
		// j <label> = beq x0, x0, label
		char new_line [128];
		copy_string (new_line, line);
		*index = *index - 1;	// back up to 'j' start
		new_line [*index] = 0;
		append_string (new_line, (const char *) "beq x0, x0, ");
		append_string (new_line, line + *index + 1);
		copy_string (token, "beq");
		*index += 3;	// 3 = strlen("beq")
		copy_string (line, new_line);
	}

	// mv
	if (stricmp(token, "mv") == 0) {
		// mv xd, xs1 = addi xd, xs1, 0
		char new_line [128];
		copy_string (new_line, line);
		*index = *index - 2;	// back up to 'mv' start
		new_line [*index] = 0;
		append_string (new_line, (const char *) "addi ");
		append_string (new_line, line + *index + 2);
		copy_string (token, "addi");
		*index += 4;	// 4 = strlen("addi")

		// now add ", 0"
        int index_of_comment = -1;
        for (size_t i = 0; i < strlen(new_line); i++) {
			if (new_line[i] == '/') {
				index_of_comment = i;
				break;
			}
		}
		if (index_of_comment == -1)
			append_string (new_line, ", 0");
		else {
			char comment[128];
			copy_string (comment, &new_line[index_of_comment]);
			new_line[index_of_comment] = 0;
			append_string(new_line, ", 0 ");
			append_string (new_line, comment);
		}

		copy_string (line, new_line);
	}

    // translate the immediate memory addressing
    if (!stricmp(token, "lb") ||
        !stricmp(token, "lh") ||
        !stricmp(token, "lw") ||
        !stricmp(token, "ld") ||
        !stricmp(token, "lbu") ||
        !stricmp(token, "lhu") ||
        !stricmp(token, "lwu") ||
        !stricmp(token, "sb") ||
        !stricmp(token, "sh") ||
        !stricmp(token, "sw") ||
        !stricmp(token, "sd")) {

        int temp_index = *index;
        assembly_instruction *instruction = malloc (sizeof (assembly_instruction));

        if (get_register (line, &temp_index, instruction, &(instruction -> rd), "unrecognized rd")) return;
        if (get_comma (line, &temp_index, instruction)) return;
        unsigned int imm = 0;
        if (get_immediate (line, &temp_index, instruction, &imm, "unrecognized immediate")) return;
        if (get_certain_token(line, &temp_index, instruction, "(")) return;
        if (get_register (line, &temp_index, instruction, &(instruction -> rs1), "unrecognized rs1")) return;
        if (get_certain_token(line, &temp_index, instruction, ")")) return;

        char new_line [128];
        sprintf(new_line, "%s x%d, x%d, %d", token, instruction->rd, instruction->rs1, imm);
        copy_string(line, new_line);
        }
}

assembly_instruction* assemble_line (simulator *sim, int address, char *line, int line_number)
{
	simulator_internal *simi = (simulator_internal *) sim;

	char token[128];
	int index = 0;
	get_token (line, &index, token);
	if (token[0] == 0)	// empty line
		return 0;

    // check for label
	if (line[index] == ':') {

        // label already mapped to address, no need to map it here
        index++;

        // continue scanning line
        get_token (line, &index, token);
        if (token[0] == 0)  // empty rest of line
            return 0;
    }

	translate_pseudoinstruction (token, line, &index);

    assembly_instruction *instruction = malloc (sizeof (assembly_instruction));

	instruction -> source_line = malloc (strlen(line) * sizeof(char) + 2);
	copy_string(instruction -> source_line, line);

    // find instruction format
    instruction_format *format = get_instruction_format (token);
    if (format == 0) {
        instruction -> error = 1;
        instruction -> error_message = "Unrecognized instruction";
        instruction -> source_line_number = line_number;
        instruction -> source_line = line;
        return instruction;
    }

	instruction -> source_line_number = line_number;
	instruction -> error = 0;
	instruction -> error_message = 0;
	instruction -> address = address;

    instruction -> format = format -> format;
    instruction -> opcode = format -> opcode;
    instruction -> funct3 = format -> funct3;
	instruction -> funct7 = format -> funct7;

	instruction -> rd = 0;
	instruction -> rs1 = 0;
	instruction -> rs2 = 0;
	instruction -> imm11 = 0;
	instruction -> imm31 = 0;

    int error_occurred = 0;
    switch (format -> format) {
		case 'M':
            error_occurred = build_instruction_m (instruction, line, index);
			break;
        case 'R':
            error_occurred = build_instruction_r (format, instruction, line, index);
            break;
        case 'I':
            error_occurred = build_instruction_i (instruction, line, index);
            break;
		case 'X':
            error_occurred = build_instruction_x (instruction, line, index);
			break;
        case 'S':
            error_occurred = build_instruction_s (instruction, line, index);
            break;
        case 'B':
            error_occurred = build_instruction_sb (instruction, line, index, address, simi);
            break;
        case 'U':
            error_occurred = build_instruction_u (instruction, line, index, address, simi);
            break;
        case 'J':
            error_occurred = build_instruction_uj (instruction, line, index, address, simi);
            break;
        default:
            printf ("Unknown instruction format");
            instruction -> error = 1;
            instruction -> error_message = "Unrecognized instruction";
            return instruction;
    }
    
    if (error_occurred)
        return instruction; // instruction contains error message

	// put instruction in memory (little endian)
	unsigned int inst = instruction -> instruction;
	simi -> memory[address+0] = (inst >>  0) & 0xff;
	simi -> memory[address+1] = (inst >>  8) & 0xff;
	simi -> memory[address+2] = (inst >> 16) & 0xff;
	simi -> memory[address+3] = (inst >> 24) & 0xff;

	return instruction;
}

int process_data_statement (simulator *sim, int *address, char *line)
{
	simulator_internal *simi = (simulator_internal *) sim;

	char token[128];
	int index = 0;
	get_token (line, &index, token);
	if (stricmp ("data", token))
		return 0;

	int index_after_data = index;

	get_token (line, &index, token);
	if (strlen(token) == 0)
		return 0;
	if (token[0] == '"') {

		// put string in memory

		int first_double_quote = 0;
        for (size_t i = 0; i < strlen(line); i++)
			if (line[i] == '"') {
				first_double_quote = i;
				break;
			}
		if (first_double_quote == 0)
			return 0;

		int second_double_quote = 0;
        for (size_t i = first_double_quote + 1; i < strlen(line); i++)
			if (line[i] == '"') {
				second_double_quote = i;
				break;
			}
		if (second_double_quote == 0)
			return 0;

		int string_length = second_double_quote - first_double_quote - 1;
		for (int i = 0; i < string_length; i++)
			simi -> memory[*address + i] = line[first_double_quote+1+i];
		simi -> memory [*address + string_length] = 0;
		string_length ++;

		// keep string length at word boundary
		if (string_length % 4 != 0)
			string_length += 4 - (string_length % 4);
		*address += string_length;

		return 1;
	}

	// read numbers
	index = index_after_data;
	get_token(line, &index, token);
	while (token[0] != 0) {

		int found_decimal = 0;
        for (size_t i = 0; i < strlen(token); i++)
			if (token[i] == '.')
				found_decimal = 1;

		int success = 0;
		if (!found_decimal) {
			int number = get_int(token, &success);
			if (success != 1)
				return 0;

			int *int_address = (int *) &(simi -> memory [*address]);
			*int_address = number;
			*address += sizeof(int);
		}
		else {
			double number = get_double(token, &success);
			if (success != 1)
				return 0;

			double *double_address = (double *) &(simi -> memory [*address]);
			*double_address = number;
			*address += sizeof(double);
		}

		// read possible comma
		get_token(line, &index, token);
		if (token[0] == ',')
			// read next integer if comma was found
			get_token(line, &index, token);
	}

	return 1;
}

int get_data_length (char *line, int index)
{
	char token[128];
	get_token (line, &index, token);
	if (strlen(token) == 0)
		return 0;
	if (token[0] == '"') {

		// return length of the string

		int first_double_quote = 0;
        for (size_t i = 0; i < strlen(line); i++)
			if (line[i] == '"') {
				first_double_quote = i;
				break;
			}
		if (first_double_quote == 0)
			return 0;

		int second_double_quote = 0;
        for (size_t i = first_double_quote + 1; i < strlen(line); i++)
			if (line[i] == '"') {
				second_double_quote = i;
				break;
			}
		if (second_double_quote == 0)
			return 0;

		int string_length = second_double_quote - first_double_quote - 1;
		string_length ++;	// for the null string terminator

		// keep string length at word boundary
		if (string_length % 4 != 0)
			string_length += 4 - (string_length % 4);

		return string_length;
	}

	// read number of integers and return the total number of bytes
	int num_of_commas = 0, num_of_floats = 0;
    for (size_t i = 0; i < strlen(line); i++) {
		if (line[i] == '/')
			break;	// stop at comment
		if (line[i] == ',')
			num_of_commas ++;
		if (line[i] == '.')
			num_of_floats ++;
	}
	int size = (num_of_commas + 1) * 4;
	size += num_of_floats * 4;
	return size;
}

void deallocate_assemble_results(assembly_instruction **instructions, int number_of_instructions)
{
	for (int i = 0; i < number_of_instructions; i++)
		free (instructions[i]);
	free (instructions);
}

assembly_instruction** assemble (simulator sim, const char *program, int address, int *number_of_instructions)
{
	simulator_internal *simi = (simulator_internal *) sim;

	// preprocess

	deallocate_labels (simi -> labels);
	simi -> labels = 0;
	clear_breakpoints(sim);

	int line_number = 1;
	int index = 0;
	char line[256];
	int preprocess_address = address;
	while (1) {
		if (program[index] == 0)
			break;

        get_line (line, program, &index, 255);

		char token[128];
		int index = 0;
		get_token (line, &index, token);

		// ignore empty lines and comment lines
		if (token[0] != 0 && token[0] != '/') {

			// check for label
			if (line[index] == ':') {

				// map label to address
				index++;

				label *new_label = malloc (sizeof(label));
				new_label -> address = preprocess_address;
				new_label -> next_label = simi -> labels;
				copy_string (new_label -> label, token);
				simi -> labels = new_label;

				// continue scanning line
				get_token (line, &index, token);
			}

			if (token[0] != 0 && token[0] != '/') {

				if (!stricmp(token, "data")) {
					// skip appropriate number of bytes
					preprocess_address += get_data_length (line, index);
				}
				else
					preprocess_address += 4;
			}
		}

		line_number ++;
	}

	// assemble

	*number_of_instructions = 0;

	assembly_instruction **result = malloc (sizeof (assembly_instruction *) * (*number_of_instructions + 10));

	int pc_initialized = 0;
	line_number = 1;
	index = 0;
	while (1) {
		if (program[index] == 0)
			break;

        get_line (line, program, &index, 255);

		int success = process_data_statement (sim, &address, line);

		if (!success) {

			assembly_instruction *instruction = assemble_line (sim, address, line, line_number);

			// result of 0 indicates empty line (that's ok)
			if (instruction != 0) {
				result[*number_of_instructions] = instruction;
				(*number_of_instructions) ++;
				if (instruction -> error == 0) {
					if (!pc_initialized) {
						pc_initialized = 1;
						simi -> pc = address;
					}
					address += 4;
				}

				// expand result periodically
				if ((*number_of_instructions) % 10 == 0) {
					result = realloc (result, sizeof (assembly_instruction) * (*number_of_instructions + 10));
				}
			}
		}

		line_number ++;
	}

	return result;
}

instruction_format* find_instruction_format_by_opcode(int opcode, int funct3, int funct7) {

    int high = instruction_count;
    int low = 0;
    while (low <= high) {
        int mid = (low + high - 1) / 2;

        // special case
        if (high == low)
            mid = high;

        int compare = 0;
        if (opcode > instruction_opcode_index[mid].opcode)
            compare = 1;
        else if (opcode < instruction_opcode_index[mid].opcode)
            compare = -1;
        else {
            if (instruction_opcode_index[mid].check_funct3) {
                if (funct3 > instruction_opcode_index[mid].funct3)
                    compare = 1;
                else if (funct3 < instruction_opcode_index[mid].funct3)
                    compare = -1;
                else {
                    if (instruction_opcode_index[mid].check_funct7) {
                        if (funct7 > instruction_opcode_index[mid].funct7)
                            compare = 1;
                        else if (funct7 < instruction_opcode_index[mid].funct7)
                            compare = -1;
                    }
                }
            }
        }

        if (compare == 0) {
            int index = instruction_opcode_index[mid].index;
            return &instruction_formats[index];
        }

        // special case continued
        if (high == low)
            break;

        if (compare > 0)
            low = mid + 1;
        else
            high = mid - 1;
    }

    return 0;
}

instruction_format* get_instruction_format_from_instruction (unsigned int mem, int opcode, int funct3,
    int funct7, int *imm31, int *imm11, int *rd, int *rs1, int *rs2)
{
    *imm31 = 0;
    *imm11 = 0;
    *rd = 0;
    *rs1 = 0;
    *rs2 = 0;

    instruction_format *format = 0;

    // if it's a floating point instruction, we can't use binary search, since in some cases, funct3 is ignored
    // and funct7 is checked, so we can't sort by opcode then funct3 then funct7
    if (opcode >= INSTRUCTION_FMV_H_X && opcode <= INSTRUCTION_FLE_D) {
        for (int i = 0; i < NUM_INSTRUCTIONS; i++)
            if (instruction_formats[i].opcode == opcode &&
                    (!instruction_formats[i].check_funct3 || (instruction_formats[i].check_funct3 && instruction_formats[i].funct3 == funct3)) &&
                    (!instruction_formats[i].check_funct7 || (instruction_formats[i].check_funct7 && instruction_formats[i].funct7 == funct7))) {
                format = &instruction_formats[i];
                break;
            }
    }
    else
        // binary search of instructions, sorted by opcode/funct3/funct7
        format = find_instruction_format_by_opcode(opcode, funct3, funct7);

    if (!format)
        return 0;

    *rs1 = mem >> 15 & 0x1f;
    *rs2 = mem >> 20 & 0x1f;
    *rd = mem >> 7 & 0x1f;
    if (format -> format == 'R') {
    }
	else if (format -> format == 'X') {
	}
    else if (format -> format == 'I') {
        *imm31 = (unsigned int) sign_extend(mem >> 20, 12);
    }
	else if (format -> format == 'M') {
	}
    else if (format -> format == 'S') {
        *imm31 = mem >> 25;
        *imm11 = mem >> 7 & 0x1f;
    }
    else if (format -> format == 'B') {
        *imm31 = mem >> 25;
        *imm11 = mem >> 7 & 0x1f;
    }
    else if (format -> format == 'U') {
		*imm31 = (unsigned int) sign_extend(mem >> 12, 20);
    }
    else if (format -> format == 'J') {
		*imm31 = (unsigned int) sign_extend(mem >> 12, 20);
    }
    else
        return 0;   // unknown format

    return format;
}

assembly_instruction* get_instruction (simulator sim, unsigned long long address)
{
	simulator_internal *simi = (simulator_internal *) sim;

	// get instruction from memory (little endian)
	unsigned int mem =
		simi -> memory[address+0] <<  0 |
		simi -> memory[address+1] <<  8 |
		simi -> memory[address+2] << 16 |
		simi -> memory[address+3] << 24;

	int opcode = mem & 0x7f;
	int funct3 = (mem >> 12) & 0x7;
    int funct7 = (mem >> 25) & 0x7f;
	char *mnemonic = 0;
	char format = 0;
	int rd, rs1, imm31, rs2, imm11;

    instruction_format *instruction_format = get_instruction_format_from_instruction(mem, opcode, funct3, funct7, &imm31, &imm11, &rd, &rs1, &rs2);
    if (instruction_format == 0)
        return 0;   // unrecognized
    
    mnemonic = instruction_format -> mnemonic;
    format = instruction_format -> format;

	assembly_instruction *instruction = malloc (sizeof (assembly_instruction));
	instruction -> opcode = opcode;
	instruction -> rd = rd;
	instruction -> rs1 = rs1;
	instruction -> imm31 = imm31;
	instruction -> imm11 = imm11;
	instruction -> funct3 = funct3;
	instruction -> funct7 = funct7;
	instruction -> rs2 = rs2;
	instruction -> address = address;
	instruction -> format = format;
    instruction -> mnemonic = mnemonic;
	instruction -> instruction = instruction_format -> operation;

	return instruction;
}

/* unused
void deallocate_get_assembly_results (assembly_instruction **instructions, int size)
{
	for (int i = 0; i < size; i++)
		if (instructions[i] != 0)
			free (instructions[i]);
	free (instructions);
}

assembly_instruction** get_assembly (simulator sim, unsigned long long address, int size)
{
	assembly_instruction **result = malloc (sizeof (assembly_instruction) * size);
	for (int i = 0; i < size; i++)
		result[i] = get_instruction(sim, address+i);

	return result;
}
*/

void do_step(simulator sim, char **error_message, delta *deltas, int num_deltas, int *deltas_used)
{
	simulator_internal *simi = (simulator_internal *) sim;

	assembly_instruction *instruction = get_instruction(sim, simi -> pc);

	if (instruction == 0) {
		*error_message = "Unrecognized instruction";
		return;
	}

	// load
	if (instruction -> instruction == INSTRUCTION_LB ||
		instruction -> instruction == INSTRUCTION_LH ||
		instruction -> instruction == INSTRUCTION_LW ||
		instruction -> instruction == INSTRUCTION_LD ||
		instruction -> instruction == INSTRUCTION_LBU ||
		instruction -> instruction == INSTRUCTION_LHU ||
		instruction -> instruction == INSTRUCTION_LWU
		//instruction -> instruction == INSTRUCTION_LDU
		)
		execute_load(simi, instruction, deltas, num_deltas, deltas_used, instruction -> instruction);

	// store
	else if (instruction -> instruction == INSTRUCTION_SB ||
			 instruction -> instruction == INSTRUCTION_SH ||
			 instruction -> instruction == INSTRUCTION_SW ||
			 instruction -> instruction == INSTRUCTION_SD)
		execute_store(simi, instruction, deltas, num_deltas, deltas_used, instruction -> instruction);

	// shift
	else if (instruction -> instruction == INSTRUCTION_SLL ||
			 instruction -> instruction == INSTRUCTION_SLLW ||
			 //instruction -> instruction == INSTRUCTION_SLLD ||
			 instruction -> instruction == INSTRUCTION_SLLI ||
			 instruction -> instruction == INSTRUCTION_SLLIW ||
			 //instruction -> instruction == INSTRUCTION_SLLID ||
			 instruction -> instruction == INSTRUCTION_SRL ||
			 instruction -> instruction == INSTRUCTION_SRLW ||
			 //instruction -> instruction == INSTRUCTION_SRLD ||
			 instruction -> instruction == INSTRUCTION_SRLI ||
			 instruction -> instruction == INSTRUCTION_SRLIW ||
			 //instruction -> instruction == INSTRUCTION_SRLID ||
			 instruction -> instruction == INSTRUCTION_SRA ||
			 instruction -> instruction == INSTRUCTION_SRAW ||
			 //instruction -> instruction == INSTRUCTION_SRAD ||
			 instruction -> instruction == INSTRUCTION_SRAI ||
			 instruction -> instruction == INSTRUCTION_SRAIW
			 //instruction -> instruction == INSTRUCTION_SRAID
			 )
		execute_shift(simi, instruction, deltas, num_deltas, deltas_used, instruction -> instruction);

	// arithmetic
	else if (instruction -> instruction == INSTRUCTION_ADD ||
			 instruction -> instruction == INSTRUCTION_ADDW ||
			 //instruction -> instruction == INSTRUCTION_ADDD ||
			 instruction -> instruction == INSTRUCTION_SUB ||
			 instruction -> instruction == INSTRUCTION_SUBW
			 //instruction -> instruction == INSTRUCTION_SUBD
			 )
		execute_arithmetic(simi, instruction, deltas, num_deltas, deltas_used, instruction -> instruction);
	else if (instruction -> instruction == INSTRUCTION_ADDI ||
			 instruction -> instruction == INSTRUCTION_ADDIW
			 //instruction -> instruction == INSTRUCTION_ADDID
			 )
		execute_arithmetic_immediate(simi, instruction, deltas, num_deltas, deltas_used, instruction -> instruction);
	else if (instruction -> instruction == INSTRUCTION_AUIPC ||
			 instruction -> instruction == INSTRUCTION_LUI)
		execute_arithmetic_u(simi, instruction, deltas, num_deltas, deltas_used, instruction -> instruction);

	// logical
	else if (instruction -> instruction == INSTRUCTION_XOR ||
			 instruction -> instruction == INSTRUCTION_OR ||
			 instruction -> instruction == INSTRUCTION_AND)
		execute_arithmetic(simi, instruction, deltas, num_deltas, deltas_used, instruction -> instruction);
	else if (instruction -> instruction == INSTRUCTION_XORI ||
			 instruction -> instruction == INSTRUCTION_ORI ||
			 instruction -> instruction == INSTRUCTION_ANDI)
		execute_arithmetic_immediate(simi, instruction, deltas, num_deltas, deltas_used, instruction -> instruction);

	// compare
	else if (instruction -> instruction == INSTRUCTION_SLT ||
			 instruction -> instruction == INSTRUCTION_SLTI ||
			 instruction -> instruction == INSTRUCTION_SLTU ||
			 instruction -> instruction == INSTRUCTION_SLTIU)
		execute_compare(simi, instruction, deltas, num_deltas, deltas_used, instruction -> instruction);

	// branch
	else if (instruction -> instruction == INSTRUCTION_BEQ ||
			 instruction -> instruction == INSTRUCTION_BNE ||
			 instruction -> instruction == INSTRUCTION_BLT ||
			 instruction -> instruction == INSTRUCTION_BGE ||
			 instruction -> instruction == INSTRUCTION_BLTU ||
			 instruction -> instruction == INSTRUCTION_BGEU)
        execute_branch(simi, instruction, instruction -> instruction);

	// jump and link
	else if (instruction -> instruction == INSTRUCTION_JAL ||
			 instruction -> instruction == INSTRUCTION_JALR)
		execute_jump_and_link(simi, instruction, deltas, num_deltas, deltas_used, instruction -> instruction);

	// multiply/divide
	else if (instruction -> instruction == INSTRUCTION_MUL ||
			 instruction -> instruction == INSTRUCTION_MULW ||
			 //instruction -> instruction == INSTRUCTION_MULD ||
			 instruction -> instruction == INSTRUCTION_MULH ||
			 instruction -> instruction == INSTRUCTION_MULHSU ||
			 instruction -> instruction == INSTRUCTION_MULHU ||
			 instruction -> instruction == INSTRUCTION_DIV ||
			 instruction -> instruction == INSTRUCTION_DIVW ||
			 //instruction -> instruction == INSTRUCTION_DIVD ||
			 instruction -> instruction == INSTRUCTION_DIVU ||
			 instruction -> instruction == INSTRUCTION_REM ||
			 instruction -> instruction == INSTRUCTION_REMW ||
			 //instruction -> instruction == INSTRUCTION_REMD ||
			 instruction -> instruction == INSTRUCTION_REMU ||
			 instruction -> instruction == INSTRUCTION_REMUW
			 //instruction -> instruction == INSTRUCTION_REMUD
			 )
		execute_mul(simi, instruction, deltas, num_deltas, deltas_used, instruction -> instruction);

	// floating point load
	else if (instruction -> instruction == INSTRUCTION_FLW ||
			 instruction -> instruction == INSTRUCTION_FLD)
		execute_floating_load(simi, instruction, deltas, num_deltas, deltas_used, instruction -> instruction);

	// floating point store
	else if (instruction -> instruction == INSTRUCTION_FSW ||
			 instruction -> instruction == INSTRUCTION_FSD)
		execute_floating_store(simi, instruction, deltas, num_deltas, deltas_used, instruction -> instruction);

	// floating point
	else if (instruction -> instruction >= INSTRUCTION_FMV_H_X &&
			 instruction -> instruction <= INSTRUCTION_FLE_D)
		execute_floating(simi, instruction, deltas, num_deltas, deltas_used, instruction -> instruction);

	// meta
	else if (instruction -> instruction == INSTRUCTION_OUTSTR ||
			 instruction -> instruction == INSTRUCTION_OUTLN ||
			 instruction -> instruction == INSTRUCTION_OUTINT ||
			 instruction -> instruction == INSTRUCTION_INSTR ||
			 instruction -> instruction == INSTRUCTION_ININT)
        execute_meta(simi, instruction, instruction -> instruction);

	else {
		*error_message = "Unrecognized instruction";
		free(instruction);
		return;
	}

	simi -> pc += 4;

	*error_message = 0;
	free(instruction);
}

void add_breakpoint (simulator sim, int address)
{
	simulator_internal *simi = (simulator_internal *) sim;
	if (simi -> num_breakpoints >= 99)
		return;
	simi -> breakpoints[simi -> num_breakpoints++] = address;
}

void delete_breakpoint (simulator sim, int address)
{
	simulator_internal *simi = (simulator_internal *) sim;

	for (int i = simi -> num_breakpoints - 1; i >= 0; i--)
		if (simi -> breakpoints[i] == address) {
			for (int j = i; j < simi -> num_breakpoints - 1; j++)
				simi -> breakpoints[j] = simi -> breakpoints[j+1];
			simi -> num_breakpoints --;
		}
}

void get_breakpoints (simulator sim, int *num_breakpoints, int **breakpoints)
{
	simulator_internal *simi = (simulator_internal *) sim;
	*num_breakpoints = simi -> num_breakpoints;
	*breakpoints = simi -> breakpoints;
}

void clear_breakpoints (simulator sim)
{
	simulator_internal *simi = (simulator_internal *) sim;
	simi -> num_breakpoints = 0;
}

// run until error or breakpoint
void do_run(simulator sim, char **error_message, delta *deltas, int num_deltas, int *deltas_used)
{
	simulator_internal *simi = (simulator_internal *) sim;
	while (1) {
		do_step(sim, error_message, deltas, num_deltas, deltas_used);

		// check for breakpoint
		for (int i = 0; i < simi -> num_breakpoints; i++)
            if (simi -> pc == (unsigned long long) simi -> breakpoints[i]) {
				*error_message = "Breakpoint";
				break;
			}

		// if error, stop
		if (*error_message != 0)
			break;
	}
}
