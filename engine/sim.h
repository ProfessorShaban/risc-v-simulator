
#ifndef SIM_H
#define SIM_H


#define MEMSIZE 1000000		// memory is this many bytes

typedef void OutputString (const char *);
typedef const char * InputString (void);

typedef enum { register_delta, fregister_delta, memory_delta } delta_type;

typedef void *simulator;

typedef struct {
	delta_type type;
	int register_number;
	unsigned long long address;
	int memory_bytes;	// length of address range
} delta;

typedef struct {
	int source_line_number;
	int error;			// 1 in case of error, 0 otherwise
	char *error_message;
	unsigned long long address;
	char *source_line;
	char *mnemonic;
	char format;
	unsigned int instruction;
	unsigned int opcode;
	unsigned int funct7;
	unsigned int funct3;
	unsigned int rs1, rs2, rd;
    int imm31;
    int imm11;
} assembly_instruction;

#ifdef FROM_CPP

extern "C" simulator create_simulator(OutputString outputStringCallback, InputString inputStringCallback);
extern "C" void delete_simulator(simulator sim);
extern "C" void deallocate_assemble_results(assembly_instruction **instructions, int number_of_instructions);
extern "C" assembly_instruction** assemble (simulator sim, const char *program, int address, int *number_of_instructions);
extern "C" assembly_instruction* get_instruction (simulator sim, unsigned long long address);
extern "C" unsigned char* get_memory (simulator sim, unsigned long long address);
extern "C" unsigned long long int* get_registers (simulator sim);
extern "C" double* get_fregisters (simulator sim);
extern "C" void set_pc (simulator sim, unsigned long long address);
extern "C" unsigned long long get_pc (simulator sim);
extern "C" void do_step(simulator sim, char **error_message, delta *deltas, int num_deltas, int *deltas_used);

extern "C" void add_breakpoint (simulator sim, int address);
extern "C" void delete_breakpoint (simulator sim, int address);
extern "C" void get_breakpoints (simulator sim, int *num_breakpoints, int **breakpoints);
extern "C" void clear_breakpoints (simulator sim);
extern "C" void do_run (simulator sim, char **error_message, delta *deltas, int num_deltas, int *deltas_used);

#endif

#ifndef FROM_CPP

simulator create_simulator(OutputString outputStringCallback, InputString inputStringCallback);
void delete_simulator(simulator sim);
void deallocate_assemble_results(assembly_instruction **instructions, int number_of_instructions);
assembly_instruction** assemble (simulator sim, const char *program, int address, int *number_of_instructions);
assembly_instruction* get_instruction (simulator sim, unsigned long long address);
unsigned char* get_memory (simulator sim, unsigned long long address);
unsigned long long int* get_registers (simulator sim);
double* get_fregisters (simulator sim);
void set_pc (simulator sim, unsigned long long address);
unsigned long long get_pc (simulator sim);
void do_step(simulator sim, char **error_message, delta *deltas, int num_deltas, int *deltas_used);

void add_breakpoint (simulator sim, int address);
void delete_breakpoint (simulator sim, int address);
void get_breakpoints (simulator sim, int *num_breakpoints, int **breakpoints);
void clear_breakpoints (simulator sim);
void do_run (simulator sim, char **error_message, delta *deltas, int num_deltas, int *deltas_used);

#endif

#endif
