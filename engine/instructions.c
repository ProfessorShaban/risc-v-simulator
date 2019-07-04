
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <fenv.h>
#pragma STDC FENV_ACCESS ON

#include "sim.h"
#include "internal.h"
#include "instructions.h"
#include "utils.h"


void execute_load (simulator_internal *simi, assembly_instruction *instruction, delta *deltas, int num_deltas, int *deltas_used, int operation)
{
	unsigned long long imm = instruction -> imm31;
	unsigned long long address = simi -> reg [instruction -> rs1] + imm;

    if (operation == INSTRUCTION_LB || operation == INSTRUCTION_LBU) {
		unsigned long long num = simi -> memory[address];
		simi -> reg [instruction -> rd] = operation == INSTRUCTION_LB ? sign_extend(num, 8) : num;
	}
    if (operation == INSTRUCTION_LH || operation == INSTRUCTION_LHU) {
		unsigned long long num =
			simi -> memory[address] |
			(simi -> memory[address + 1] << 8);
		simi -> reg [instruction -> rd] = operation == INSTRUCTION_LH ? sign_extend(num, 16) : num;
	}
    if (operation == INSTRUCTION_LW || operation == INSTRUCTION_LWU) {
		unsigned long long num =
            (unsigned long long)(simi -> memory[address]) |
            ((unsigned long long)(simi -> memory[address + 1]) << 8) |
            ((unsigned long long)(simi -> memory[address + 2]) << 16) |
            ((unsigned long long)(simi -> memory[address + 3]) << 24);
		simi -> reg [instruction -> rd] = operation == INSTRUCTION_LW ? sign_extend(num, 32) : num;
	}
	if (operation == INSTRUCTION_LD) {
		unsigned long long num =
            (unsigned long long)(simi -> memory[address]) |
            ((unsigned long long)(simi -> memory[address + 1]) << 8) |
            ((unsigned long long)(simi -> memory[address + 2]) << 16) |
            ((unsigned long long)(simi -> memory[address + 3]) << 24) |
            (((unsigned long long)(simi -> memory[address + 4])) << 32) |
            (((unsigned long long)(simi -> memory[address + 5])) << 40) |
            (((unsigned long long)(simi -> memory[address + 6])) << 48) |
            (((unsigned long long)(simi -> memory[address + 7])) << 56);
		simi -> reg [instruction -> rd] = num;
	}

	if (*deltas_used < num_deltas - 1) {
		deltas[*deltas_used].type = register_delta;
		deltas[*deltas_used].register_number = instruction -> rd;
		(*deltas_used) ++;
	}
}

void execute_store (simulator_internal *simi, assembly_instruction *instruction, delta *deltas, int num_deltas, int *deltas_used, int operation)
{
	unsigned long long imm = instruction -> imm31 << 5 | instruction -> imm11;
	unsigned long long address = simi -> reg [instruction -> rs2] + imm;

	if (operation == INSTRUCTION_SB)
		simi -> memory [address] = simi -> reg [instruction -> rs1];
	if (operation == INSTRUCTION_SH) {
		simi -> memory [address] = simi -> reg [instruction -> rs1];
		simi -> memory [address+1] = simi -> reg [instruction -> rs1] >> 8;
	}
	if (operation == INSTRUCTION_SW) {
		simi -> memory [address] = simi -> reg [instruction -> rs1];
		simi -> memory [address+1] = simi -> reg [instruction -> rs1] >> 8;
		simi -> memory [address+2] = simi -> reg [instruction -> rs1] >> 16;
		simi -> memory [address+3] = simi -> reg [instruction -> rs1] >> 24;
	}
	if (operation == INSTRUCTION_SD) {
		simi -> memory [address] = simi -> reg [instruction -> rs1];
		simi -> memory [address+1] = simi -> reg [instruction -> rs1] >> 8;
		simi -> memory [address+2] = simi -> reg [instruction -> rs1] >> 16;
		simi -> memory [address+3] = simi -> reg [instruction -> rs1] >> 24;
		simi -> memory [address+4] = simi -> reg [instruction -> rs1] >> 32;
		simi -> memory [address+5] = simi -> reg [instruction -> rs1] >> 40;
		simi -> memory [address+6] = simi -> reg [instruction -> rs1] >> 48;
		simi -> memory [address+7] = simi -> reg [instruction -> rs1] >> 56;
	}

	if (*deltas_used < num_deltas - 1) {
		int bytes = 1;
		if (operation == INSTRUCTION_SH) bytes = 2;
		if (operation == INSTRUCTION_SW) bytes = 4;
		if (operation == INSTRUCTION_SD) bytes = 8;
		deltas[*deltas_used].type = memory_delta;
		deltas[*deltas_used].address = address;
		deltas[*deltas_used].memory_bytes = bytes;
		(*deltas_used) ++;
	}
}

void execute_shift (simulator_internal *simi, assembly_instruction *instruction, delta *deltas, int num_deltas, int *deltas_used, int operation)
{
	if (operation == INSTRUCTION_SLLI)
		simi -> reg[instruction -> rd] = simi -> reg[instruction -> rs1] << instruction -> rs2;
	if (operation == INSTRUCTION_SLLIW) {
		int word1 = ((int) simi -> reg [instruction -> rs1]) & 0xffffffff;
		int word2 = ((int) instruction -> rs2) & 0xffffffff;
		simi -> reg[instruction -> rd] = (word1 << word2) & 0xffffffff;
	}
	if (operation == INSTRUCTION_SRLI)
		simi -> reg[instruction -> rd] = simi -> reg[instruction -> rs1] >> instruction -> rs2;
	if (operation == INSTRUCTION_SRLIW) {
		int word1 = ((int) simi -> reg [instruction -> rs1]) & 0xffffffff;
		int word2 = ((int) instruction -> rs2) & 0xffffffff;
		simi -> reg[instruction -> rd] = (word1 >> word2) & 0xffffffff;
	}
	if (operation == INSTRUCTION_SRAI) {
		int shift_bits = instruction -> rs2;
		simi -> reg[instruction -> rd] = simi -> reg[instruction -> rs1] >> shift_bits;
		simi -> reg[instruction -> rd] = sign_extend(simi -> reg[instruction -> rd], 64 - shift_bits);
	}
	if (operation == INSTRUCTION_SRAIW) {
		int word1 = ((int) simi -> reg [instruction -> rs1]) & 0xffffffff;
		int shift_bits = ((int) instruction -> rs2) & 0xffffffff;
		simi -> reg[instruction -> rd] = (word1 >> shift_bits) & 0xffffffff;
		simi -> reg[instruction -> rd] = sign_extend(simi -> reg[instruction -> rd], 32 - shift_bits) & 0xffffffff;
	}
	if (operation == INSTRUCTION_SLL)
		simi -> reg[instruction -> rd] = simi -> reg[instruction -> rs1] << simi -> reg[instruction -> rs2];
	if (operation == INSTRUCTION_SLLW) {
		int word1 = ((int) simi -> reg [instruction -> rs1]) & 0xffffffff;
		int word2 = ((int) simi -> reg [instruction -> rs2]) & 0xffffffff;
		simi -> reg[instruction -> rd] = (word1 << word2) & 0xffffffff;
	}
	if (operation == INSTRUCTION_SRL)
		simi -> reg[instruction -> rd] = simi -> reg[instruction -> rs1] >> simi -> reg[instruction -> rs2];
	if (operation == INSTRUCTION_SRLW) {
		int word1 = ((int) simi -> reg [instruction -> rs1]) & 0xffffffff;
		int word2 = ((int) simi -> reg [instruction -> rs2]) & 0xffffffff;
		simi -> reg[instruction -> rd] = (word1 >> word2) & 0xffffffff;
	}
	if (operation == INSTRUCTION_SRA) {
		int shift_bits = (int) simi -> reg[instruction -> rs2];
		simi -> reg[instruction -> rd] = simi -> reg[instruction -> rs1] >> shift_bits;
		simi -> reg[instruction -> rd] = sign_extend(simi -> reg[instruction -> rd], 64 - shift_bits);
	}
	if (operation == INSTRUCTION_SRAW) {
		int word1 = ((int) simi -> reg [instruction -> rs1]) & 0xffffffff;
		int shift_bits = ((int) simi -> reg [instruction -> rs2]) & 0xffffffff;
		simi -> reg[instruction -> rd] = (word1 >> shift_bits) & 0xffffffff;
		simi -> reg[instruction -> rd] = sign_extend(simi -> reg[instruction -> rd], 32 - shift_bits) & 0xffffffff;
	}

	if (*deltas_used < num_deltas - 1) {
		deltas[*deltas_used].type = register_delta;
		deltas[*deltas_used].register_number = instruction -> rd;
		(*deltas_used) ++;
	}
}

void execute_arithmetic (simulator_internal *simi, assembly_instruction *instruction,
	delta *deltas, int num_deltas, int *deltas_used, int operation)
{
    if (instruction -> rd == 0)
        return;

    if (operation == INSTRUCTION_ADD)
	    simi -> reg[instruction -> rd] = simi -> reg [instruction -> rs1] + simi -> reg [instruction -> rs2];
	if (operation == INSTRUCTION_ADDW) {
		int word1 = ((int) simi -> reg [instruction -> rs1]) & 0xffffffff;
		int word2 = ((int) simi -> reg [instruction -> rs2]) & 0xffffffff;
		simi -> reg[instruction -> rd] = (word1 + word2) & 0xffffffff;
	}
    if (operation == INSTRUCTION_SUB)
		simi -> reg[instruction -> rd] = simi -> reg [instruction -> rs1] - simi -> reg [instruction -> rs2];
	if (operation == INSTRUCTION_SUBW) {
		int word1 = ((int) simi -> reg [instruction -> rs1]) & 0xffffffff;
		int word2 = ((int) simi -> reg [instruction -> rs2]) & 0xffffffff;
		simi -> reg[instruction -> rd] = (word1 - word2) & 0xffffffff;
	}
    if (operation == INSTRUCTION_XOR)
		simi -> reg[instruction -> rd] = simi -> reg [instruction -> rs1] ^ simi -> reg [instruction -> rs2];
    if (operation == INSTRUCTION_OR)
		simi -> reg[instruction -> rd] = simi -> reg [instruction -> rs1] | simi -> reg [instruction -> rs2];
    if (operation == INSTRUCTION_AND)
		simi -> reg[instruction -> rd] = simi -> reg [instruction -> rs1] & simi -> reg [instruction -> rs2];

	if (*deltas_used < num_deltas - 1) {
		deltas[*deltas_used].type = register_delta;
		deltas[*deltas_used].register_number = instruction -> rd;
		(*deltas_used) ++;
	}
}

void execute_arithmetic_immediate (simulator_internal *simi, assembly_instruction *instruction,
								   delta *deltas, int num_deltas, int *deltas_used, int operation)
{
	if (instruction -> rd == 0)
		return;

	int immediate = (signed int) instruction -> imm31;
	long long immediate_64 = immediate;
	unsigned long long unsigned_immediate_64 = (unsigned long long) immediate_64;
	unsigned long long result = 0;

	if (operation == INSTRUCTION_ADDI)
		result = simi -> reg [instruction -> rs1] + unsigned_immediate_64;
	if (operation == INSTRUCTION_ADDIW) {
		int word1 = ((int) simi -> reg [instruction -> rs1]) & 0xffffffff;
		int word2 = ((int) unsigned_immediate_64) & 0xffffffff;
		result = (word1 + word2) & 0xffffffff;
	}
	if (operation == INSTRUCTION_XORI)
		result = simi -> reg [instruction -> rs1] ^ unsigned_immediate_64;
	if (operation == INSTRUCTION_ORI)
		result = simi -> reg [instruction -> rs1] | unsigned_immediate_64;
	if (operation == INSTRUCTION_ANDI)
		result = simi -> reg [instruction -> rs1] & unsigned_immediate_64;

	simi -> reg[instruction -> rd] = result;

	if (*deltas_used < num_deltas - 1) {
		deltas[*deltas_used].type = register_delta;
		deltas[*deltas_used].register_number = instruction -> rd;
		(*deltas_used) ++;
	}
}

void execute_arithmetic_u (simulator_internal *simi, assembly_instruction *instruction,
	delta *deltas, int num_deltas, int *deltas_used, int operation)
{
	if (instruction -> rd == 0)
		return;

	if (operation == INSTRUCTION_AUIPC)
		simi -> reg[instruction -> rd] = simi -> pc + ((signed) instruction -> imm31) * 2;
	if (operation == INSTRUCTION_LUI)
		simi -> reg[instruction -> rd] = instruction -> imm31 << 12;

	if (*deltas_used < num_deltas - 1) {
		deltas[*deltas_used].type = register_delta;
		deltas[*deltas_used].register_number = instruction -> rd;
		(*deltas_used) ++;
	}
}

void execute_compare (simulator_internal *simi, assembly_instruction *instruction,
	delta *deltas, int num_deltas, int *deltas_used, int operation)
{
	if (instruction -> rd == 0)
		return;

	if (operation == INSTRUCTION_SLT)
		simi -> reg[instruction -> rd] = (signed long long) simi -> reg[instruction -> rs1] < (signed long long) simi -> reg[instruction -> rs2] ? 1 : 0;
	if (operation == INSTRUCTION_SLTI)
		simi -> reg[instruction -> rd] = (signed long long) simi -> reg[instruction -> rs1] < (signed long long) instruction -> imm31 ? 1 : 0;
	if (operation == INSTRUCTION_SLTU)
		simi -> reg[instruction -> rd] = simi -> reg[instruction -> rs1] < simi -> reg[instruction -> rs2] ? 1 : 0;
	if (operation == INSTRUCTION_SLTIU)
		simi -> reg[instruction -> rd] = simi -> reg[instruction -> rs1] < instruction -> imm31 ? 1 : 0;

	if (*deltas_used < num_deltas - 1) {
		deltas[*deltas_used].type = register_delta;
		deltas[*deltas_used].register_number = instruction -> rd;
		(*deltas_used) ++;
	}
}

void execute_branch (simulator_internal *simi, assembly_instruction *instruction, int operation)
{
    signed long long signed_rs1 = (signed long long) simi -> reg[instruction -> rs1];
    signed long long signed_rs2 = (signed long long) simi -> reg[instruction -> rs2];

	if (operation == INSTRUCTION_BEQ && signed_rs1 != signed_rs2)
		return;
    if (operation == INSTRUCTION_BNE && signed_rs1 == signed_rs2)
        return;
    if (operation == INSTRUCTION_BLT && signed_rs1 >= signed_rs2)
        return;
    if (operation == INSTRUCTION_BGE && signed_rs1 < signed_rs2)
        return;
    if (operation == INSTRUCTION_BLTU && simi -> reg[instruction -> rs1] >= simi -> reg[instruction -> rs2])
        return;
    if (operation == INSTRUCTION_BGEU && simi -> reg[instruction -> rs1] < simi -> reg[instruction -> rs2])
        return;

	unsigned int bit11 = instruction -> imm11 & 0x1;
	unsigned int bit12 = instruction -> imm31 >> 6;
	unsigned int bits1to4 = instruction -> imm11 >> 1;
	unsigned int bits5to10 = instruction -> imm31 & 0x3f;
	unsigned int offset_unsigned = (bit12 << 11) | (bit11 << 10) | (bits5to10 << 4) | bits1to4;
	int offset = (int) (signed long long) sign_extend (offset_unsigned, 12);
	offset = offset << 1;

	simi -> pc = simi -> pc - 4 + offset;
}

void execute_jump_and_link (simulator_internal *simi, assembly_instruction *instruction,
	delta *deltas, int num_deltas, int *deltas_used, int operation)
{
	if (operation == INSTRUCTION_JAL) {
		if (instruction -> rd != 0)
			simi -> reg[instruction -> rd] = simi -> pc + 4;

		unsigned int bit11 = (instruction -> imm31 & 0x100) >> 8;
		unsigned int bit20 = instruction -> imm31 >> 19;
		unsigned int bits1to10 = (instruction -> imm31 & 0x3fe00) >> 9;
		unsigned int bits12to19 = instruction -> imm31 & 0xff;
		unsigned int offset_unsigned = (bit20 << 19) | (bits12to19 << 11) | (bit11 << 10) | bits1to10;

		simi -> pc = simi -> pc + offset_unsigned * 2;

		// compensate for pc=pc+4
		simi -> pc -= 4;
	}
	if (operation == INSTRUCTION_JALR) {
		if (instruction -> rd != 0)
			simi -> reg[instruction -> rd] = simi -> pc + 4;
		simi -> pc = simi -> reg[instruction -> rs1] + instruction -> imm31;

		// compensate for pc=pc+4
		simi -> pc -= 4;
	}

	if (instruction -> rd != 0)
		if (*deltas_used < num_deltas - 1) {
			deltas[*deltas_used].type = register_delta;
			deltas[*deltas_used].register_number = instruction -> rd;
			(*deltas_used) ++;
		}
}

void execute_mul (simulator_internal *simi, assembly_instruction *instruction, delta *deltas, int num_deltas, int *deltas_used, int operation)
{
	if (instruction -> rd == 0)
		return;

	if (operation == INSTRUCTION_MUL)
		simi -> reg[instruction -> rd] = (signed long long) simi -> reg [instruction -> rs1] * (signed long long) simi -> reg [instruction -> rs2];
	if (operation == INSTRUCTION_MULW) {
		int word1 = ((int) simi -> reg [instruction -> rs1]) & 0xffffffff;
		int word2 = ((int) simi -> reg [instruction -> rs2]) & 0xffffffff;
		simi -> reg[instruction -> rd] = (word1 * word2) & 0xffffffff;
	}
	if (operation == INSTRUCTION_DIV)
		simi -> reg[instruction -> rd] = (signed long long) simi -> reg [instruction -> rs1] / (signed long long) simi -> reg [instruction -> rs2];
	if (operation == INSTRUCTION_DIVW) {
		int word1 = ((int) simi -> reg [instruction -> rs1]) & 0xffffffff;
		int word2 = ((int) simi -> reg [instruction -> rs2]) & 0xffffffff;
		simi -> reg[instruction -> rd] = (word1 / word2) & 0xffffffff;
	}
	if (operation == INSTRUCTION_DIVU)
		simi -> reg[instruction -> rd] = (unsigned long long) simi -> reg [instruction -> rs1] / (unsigned long long) simi -> reg [instruction -> rs2];
	if (operation == INSTRUCTION_REM)
		simi -> reg[instruction -> rd] = (signed long long) simi -> reg [instruction -> rs1] % (signed long long) simi -> reg [instruction -> rs2];
	if (operation == INSTRUCTION_REMW) {
		int word1 = ((int) simi -> reg [instruction -> rs1]) & 0xffffffff;
		int word2 = ((int) simi -> reg [instruction -> rs2]) & 0xffffffff;
		simi -> reg[instruction -> rd] = (word1 % word2) & 0xffffffff;
	}
	if (operation == INSTRUCTION_REMU)
		simi -> reg[instruction -> rd] = (unsigned long long) simi -> reg [instruction -> rs1] % (unsigned long long) simi -> reg [instruction -> rs2];
	if (operation == INSTRUCTION_REMUW) {
		unsigned int word1 = ((unsigned int) simi -> reg [instruction -> rs1]) & 0xffffffff;
		unsigned int word2 = ((unsigned int) simi -> reg [instruction -> rs2]) & 0xffffffff;
		simi -> reg[instruction -> rd] = (word1 % word2) & 0xffffffff;
	}

	 if (*deltas_used < num_deltas - 1) {
		 deltas[*deltas_used].type = register_delta;
		 deltas[*deltas_used].register_number = instruction -> rd;
		 (*deltas_used) ++;
	 }
}

void execute_floating_load (simulator_internal *simi, assembly_instruction *instruction, delta *deltas, int num_deltas, int *deltas_used, int operation)
{
	int imm = instruction -> imm31;
	int address = (int) simi -> reg[instruction -> rs1] + imm;
	if (operation == INSTRUCTION_FLD) {
		simi -> freg[instruction -> rd] = *(double *) &simi -> memory [address];
	}
	if (operation == INSTRUCTION_FLW) {
		*(float *) &simi -> freg[instruction -> rd] = *(float *) &simi -> memory [address];
	}

	if (*deltas_used < num_deltas - 1) {
		deltas[*deltas_used].type = fregister_delta;
		deltas[*deltas_used].register_number = instruction -> rd;
		(*deltas_used) ++;
	}
}

void execute_floating_store (simulator_internal *simi, assembly_instruction *instruction, delta *deltas, int num_deltas, int *deltas_used, int operation)
{
	int imm = instruction -> imm31 << 5 | instruction -> imm11;
	int address = (int) simi -> reg[instruction -> rs2] + imm;
	int bytes = 0;
	if (operation == INSTRUCTION_FSD) {
		*(double *) &simi -> memory [address] = simi -> freg[instruction -> rs1];
		bytes = 8;
	}
	if (operation == INSTRUCTION_FSW) {
		*(float *) &simi -> memory [address] = *(float *) &simi -> freg[instruction -> rs1];
		bytes = 4;
	}

	if (*deltas_used < num_deltas - 1) {
		deltas[*deltas_used].type = memory_delta;
		deltas[*deltas_used].address = address;
		deltas[*deltas_used].memory_bytes = bytes;
		(*deltas_used) ++;
	}
}

int get_rounding_mode ()
{
	return fegetround();
}

int translate_rounding_mode (int risc_v_mode)
{
	/*
	 rounding modes
	 0 (RNE) - round to nearest, ties to even
	 1 (RTZ) - round toward zero
	 2 (RDN) - rounds down
	 3 (RUP) - rounds up
	 4 (RMM) - round to nearest, ties to Max Magnitude. We implement it like RNE
	 7 - In instruction's rm field, selects dynamic rounding mode. We implement it like RNE

	 rounding mode    C name
	 -----------------------
	 to nearest       FE_TONEAREST
	 toward zero      FE_TOWARDZERO
	 to +infinity     FE_UPWARD
	 to -infinity     FE_DOWNWARD
	 */

	int mode = FE_TONEAREST;
	if (risc_v_mode == 0) mode = FE_TONEAREST;
	if (risc_v_mode == 1) mode = FE_TOWARDZERO;
	if (risc_v_mode == 2) mode = FE_DOWNWARD;
	if (risc_v_mode == 3) mode = FE_UPWARD;
	if (risc_v_mode == 4) mode = FE_TONEAREST;
	if (risc_v_mode == 7) mode = FE_TONEAREST;

	return mode;
}

void set_rounding_mode (int mode)
{
	fesetround(mode);
}

void execute_floating (simulator_internal *simi, assembly_instruction *instruction, delta *deltas, int num_deltas, int *deltas_used, int operation)
{
	int original_mode = -1;

	if (operation == INSTRUCTION_FCVT_L_D ||
		operation == INSTRUCTION_FCVT_W_S ||
		operation == INSTRUCTION_FADD_D ||
		operation == INSTRUCTION_FSUB_D ||
		operation == INSTRUCTION_FMUL_D ||
		operation == INSTRUCTION_FDIV_D ||
		operation == INSTRUCTION_FSQRT_D ||
		operation == INSTRUCTION_FADD_S ||
		operation == INSTRUCTION_FSUB_S ||
		operation == INSTRUCTION_FMUL_S ||
		operation == INSTRUCTION_FDIV_S ||
		operation == INSTRUCTION_FSQRT_S) {
		// handle rounding mode
		int rounding_mode = instruction -> funct3;
		original_mode = get_rounding_mode();
		set_rounding_mode (translate_rounding_mode (rounding_mode));
	}

	// floating point convert

	// convert from floating point to integer
	if (operation == INSTRUCTION_FCVT_L_D) {
		// convert from double-precision floating point to 64-bit integer
		if (instruction -> rd == 0)
			return;

		int code = instruction -> rs2;

		if (code == 2)	// FCVT.L.D
			simi -> reg[instruction -> rd] = (signed long long) simi -> freg [instruction -> rs1];
		if (code == 3)	// FCVT.LU.D
			simi -> reg[instruction -> rd] = (unsigned long long) simi -> freg [instruction -> rs1];
	}
	if (operation == INSTRUCTION_FCVT_W_S) {
		// convert from single-precision floating point to 32-bit integer
		if (instruction -> rd == 0)
			return;
		simi -> reg[instruction -> rd] = (signed long) (*(float *) &simi -> freg [instruction -> rs1]);
	}

	// convert from integer to floating point
	if (operation == INSTRUCTION_FCVT_D_L) {
		// convert from 64-bit integer to double precision
		int code = instruction -> rs2;

		if (code == 2)	// FCVT.D.L
			simi -> freg[instruction -> rd] = (signed long long) simi -> reg [instruction -> rs1];
		if (code == 3)	// FCVT.D.LU
			simi -> freg[instruction -> rd] = (unsigned long long) simi -> reg [instruction -> rs1];
	}
	if (operation == INSTRUCTION_FCVT_S_W) {
		// convert from 32-bit integer to single precision
		* (float *) &simi -> freg[instruction -> rd] = (float) (signed long) simi -> reg [instruction -> rs1];
	}

	// floating point arithmetic
	if (operation == INSTRUCTION_FADD_D)
		simi -> freg[instruction -> rd] = simi -> freg [instruction -> rs1] + simi -> freg [instruction -> rs2];
	if (operation == INSTRUCTION_FADD_S)
		*(float *) &simi -> freg[instruction -> rd] = *(float *) &simi -> freg [instruction -> rs1] + *(float *) &simi -> freg [instruction -> rs2];

	if (operation == INSTRUCTION_FSUB_D)
		simi -> freg[instruction -> rd] = simi -> freg [instruction -> rs1] - simi -> freg [instruction -> rs2];
	if (operation == INSTRUCTION_FSUB_S)
		*(float *) &simi -> freg[instruction -> rd] = *(float *) &simi -> freg [instruction -> rs1] - *(float *) &simi -> freg [instruction -> rs2];

	if (operation == INSTRUCTION_FMUL_D)
		simi -> freg[instruction -> rd] = simi -> freg [instruction -> rs1] * simi -> freg [instruction -> rs2];
	if (operation == INSTRUCTION_FMUL_S)
		*(float *) &simi -> freg[instruction -> rd] = *(float *) &simi -> freg [instruction -> rs1] * *(float *) &simi -> freg [instruction -> rs2];

	if (operation == INSTRUCTION_FDIV_D)
		simi -> freg[instruction -> rd] = simi -> freg [instruction -> rs1] / simi -> freg [instruction -> rs2];
	if (operation == INSTRUCTION_FDIV_S)
		*(float *) &simi -> freg[instruction -> rd] = *(float *) &simi -> freg [instruction -> rs1] / *(float *) &simi -> freg [instruction -> rs2];

	if (operation == INSTRUCTION_FSQRT_D)
		simi -> freg[instruction -> rd] = sqrt(simi -> freg [instruction -> rs1]);
	if (operation == INSTRUCTION_FSQRT_S)
		*(float *) &simi -> freg[instruction -> rd] = sqrt(*(float *) &simi -> freg [instruction -> rs1]);

	// floating point compare
	if (operation == INSTRUCTION_FEQ_D) {
		if (instruction -> rd == 0)
			return;
		simi -> reg[instruction -> rd] = simi -> freg [instruction -> rs1] == simi -> freg [instruction -> rs2] ? 1 : 0;
	}
	if (operation == INSTRUCTION_FEQ_S) {
		if (instruction -> rd == 0)
			return;
		simi -> reg[instruction -> rd] = (*(float *) &simi -> freg [instruction -> rs1]) == (*(float *) &simi -> freg [instruction -> rs2]) ? 1 : 0;
	}
	if (operation == INSTRUCTION_FLT_D) {
		if (instruction -> rd == 0)
			return;
		simi -> reg[instruction -> rd] = simi -> freg [instruction -> rs1] < simi -> freg [instruction -> rs2] ? 1 : 0;
	}
	if (operation == INSTRUCTION_FLT_S) {
		if (instruction -> rd == 0)
			return;
		simi -> reg[instruction -> rd] = (*(float *) &simi -> freg [instruction -> rs1]) < (*(float *) &simi -> freg [instruction -> rs2]) ? 1 : 0;
	}
	if (operation == INSTRUCTION_FLE_D) {
		if (instruction -> rd == 0)
			return;
		simi -> reg[instruction -> rd] = simi -> freg [instruction -> rs1] <= simi -> freg [instruction -> rs2] ? 1 : 0;
	}
	if (operation == INSTRUCTION_FLE_S) {
		if (instruction -> rd == 0)
			return;
		simi -> reg[instruction -> rd] = (*(float *) &simi -> freg [instruction -> rs1]) <= (*(float *) &simi -> freg [instruction -> rs2]) ? 1 : 0;
	}

	if (original_mode != -1)
		set_rounding_mode (original_mode);

	if (*deltas_used < num_deltas - 1) {
		if ((operation >= INSTRUCTION_FEQ_S && operation <= INSTRUCTION_FLE_D) ||
			operation == INSTRUCTION_FCVT_L_D ||
			operation == INSTRUCTION_FCVT_W_S)
			deltas[*deltas_used].type = register_delta;
		else
			deltas[*deltas_used].type = fregister_delta;
		deltas[*deltas_used].register_number = instruction -> rd;
		(*deltas_used) ++;
	}
}

void execute_meta (simulator_internal *simi, assembly_instruction *instruction, int operation)
{
	if (operation == INSTRUCTION_OUTSTR) {
		const char *str = (const char *) & (simi -> memory [simi -> reg[instruction -> rd]]);
		simi -> outputStringCallback (str);
	}
	if (operation == INSTRUCTION_OUTLN) {
		simi -> outputStringCallback ("\n");
	}
	if (operation == INSTRUCTION_OUTINT) {
        int *num_ptr = (int *) &(simi -> memory [simi -> reg[instruction -> rd]]);
		int num = *num_ptr;
		char num_str[128];
		sprintf(num_str, "%d", num);
		simi -> outputStringCallback(num_str);
	}
	if (operation == INSTRUCTION_INSTR) {
		const char *str = simi -> inputStringCallback();
		copy_string ((char *) &simi -> memory[simi -> reg[instruction -> rd]], str);
	}
	if (operation == INSTRUCTION_ININT) {
		const char *str = simi -> inputStringCallback();
		int num = 0;
		sscanf (str, "%d", &num);
        * ((int *) &simi -> memory[simi -> reg[instruction -> rd]]) = num;
	}
}
