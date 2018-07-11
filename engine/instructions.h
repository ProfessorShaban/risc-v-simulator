

void execute_load (simulator_internal *simi, assembly_instruction *instruction, delta *deltas, int num_deltas, int *deltas_used, int operation);
void execute_store (simulator_internal *simi, assembly_instruction *instruction, delta *deltas, int num_deltas, int *deltas_used, int operation);
void execute_shift (simulator_internal *simi, assembly_instruction *instruction, delta *deltas, int num_deltas, int *deltas_used, int operation);
void execute_arithmetic (simulator_internal *simi, assembly_instruction *instruction, delta *deltas, int num_deltas, int *deltas_used, int operation);
void execute_arithmetic_immediate (simulator_internal *simi, assembly_instruction *instruction, delta *deltas, int num_deltas, int *deltas_used, int operation);
void execute_arithmetic_u (simulator_internal *simi, assembly_instruction *instruction, delta *deltas, int num_deltas, int *deltas_used, int operation);
void execute_compare (simulator_internal *simi, assembly_instruction *instruction, delta *deltas, int num_deltas, int *deltas_used, int operation);
void execute_jump_and_link (simulator_internal *simi, assembly_instruction *instruction, delta *deltas, int num_deltas, int *deltas_used, int operation);
void execute_branch (simulator_internal *simi, assembly_instruction *instruction, int operation);
void execute_mul (simulator_internal *simi, assembly_instruction *instruction, delta *deltas, int num_deltas, int *deltas_used, int operation);
void execute_floating (simulator_internal *simi, assembly_instruction *instruction, delta *deltas, int num_deltas, int *deltas_used, int operation);
void execute_floating_load (simulator_internal *simi, assembly_instruction *instruction, delta *deltas, int num_deltas, int *deltas_used, int operation);
void execute_floating_store (simulator_internal *simi, assembly_instruction *instruction, delta *deltas, int num_deltas, int *deltas_used, int operation);
void execute_meta (simulator_internal *simi, assembly_instruction *instruction, int operation);
