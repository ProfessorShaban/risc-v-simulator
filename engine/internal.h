

typedef struct label label;

struct label {
	char label[128];
	unsigned long long address;  // memory address
	label *next_label;
};

typedef struct {
	unsigned long long int *reg;		// registers
	double *freg;                       // floating-point registers
	unsigned char *memory;	            // memory
	unsigned long long pc;
	label *labels;
	OutputString *outputStringCallback;
	InputString *inputStringCallback;
	int breakpoints[100];
	int num_breakpoints;
} simulator_internal;


// load
#define INSTRUCTION_LB        10
#define INSTRUCTION_LH        11
#define INSTRUCTION_LW        12
#define INSTRUCTION_LD        13
#define INSTRUCTION_LBU       14
#define INSTRUCTION_LHU       15
#define INSTRUCTION_LWU       16
//#define INSTRUCTION_LDU       17 // can't find opcode

// store
#define INSTRUCTION_SB        20
#define INSTRUCTION_SH        21
#define INSTRUCTION_SW        22
#define INSTRUCTION_SD        23

// shift
#define INSTRUCTION_SLL       30
#define INSTRUCTION_SLLW      31
//#define INSTRUCTION_SLLD      32 // can't find opcode
#define INSTRUCTION_SLLI      33
#define INSTRUCTION_SLLIW     34
//#define INSTRUCTION_SLLID     35 // can't find opcode
#define INSTRUCTION_SRL       36
#define INSTRUCTION_SRLW      37
//#define INSTRUCTION_SRLD      38 // can't find opcode
#define INSTRUCTION_SRLI      39
#define INSTRUCTION_SRLIW     40
//#define INSTRUCTION_SRLID     41 // can't find opcode
#define INSTRUCTION_SRA       42
#define INSTRUCTION_SRAW      43
//#define INSTRUCTION_SRAD      44 // can't find opcode
#define INSTRUCTION_SRAI      45
#define INSTRUCTION_SRAIW     46
//#define INSTRUCTION_SRAID     47 // can't find opcode

// arithmetic
#define INSTRUCTION_ADD       50
#define INSTRUCTION_ADDW      51
//#define INSTRUCTION_ADDD      52 // can't find opcode
#define INSTRUCTION_ADDI      53
#define INSTRUCTION_ADDIW     54
//#define INSTRUCTION_ADDID     55 // can't find opcode
#define INSTRUCTION_SUB       56
#define INSTRUCTION_SUBW      57
//#define INSTRUCTION_SUBD      58 // can't find opcode
#define INSTRUCTION_LUI       59
#define INSTRUCTION_AUIPC     60

// logical
#define INSTRUCTION_XOR       70
#define INSTRUCTION_XORI      71
#define INSTRUCTION_OR        72
#define INSTRUCTION_ORI       73
#define INSTRUCTION_AND       74
#define INSTRUCTION_ANDI      75

// compare
#define INSTRUCTION_SLT       80
#define INSTRUCTION_SLTI      81
#define INSTRUCTION_SLTU      82
#define INSTRUCTION_SLTIU     83

// branch
#define INSTRUCTION_BEQ       90
#define INSTRUCTION_BNE       91
#define INSTRUCTION_BLT       92
#define INSTRUCTION_BGE       93
#define INSTRUCTION_BLTU      94
#define INSTRUCTION_BGEU      95

// jump and link
#define INSTRUCTION_JAL       100
#define INSTRUCTION_JALR      101

// multiply/divide
#define INSTRUCTION_MUL       110
#define INSTRUCTION_MULW      111
//#define INSTRUCTION_MULD      112 // can't find opcode
#define INSTRUCTION_MULH      113
#define INSTRUCTION_MULHSU    114
#define INSTRUCTION_MULHU     115
#define INSTRUCTION_DIV       116
#define INSTRUCTION_DIVW      117
//#define INSTRUCTION_DIVD      118 // can't find opcode
#define INSTRUCTION_DIVU      119
#define INSTRUCTION_REM       120
#define INSTRUCTION_REMW      121
//#define INSTRUCTION_REMD      122 // can't find opcode
#define INSTRUCTION_REMU      123
#define INSTRUCTION_REMUW     124
//#define INSTRUCTION_REMUD     125 // can't find opcode

// floating point move
#define INSTRUCTION_FMV_H_X   140
#define INSTRUCTION_FMV_S_X   141
#define INSTRUCTION_FMV_D_X   142
#define INSTRUCTION_FMV_X_H   143
#define INSTRUCTION_FMV_X_S   144
#define INSTRUCTION_FMV_X_D   145

// floating point convert
#define INSTRUCTION_FCVT_H_W  150
#define INSTRUCTION_FCVT_S_W  151
#define INSTRUCTION_FCVT_D_W  152
#define INSTRUCTION_FCVT_H_L  153
#define INSTRUCTION_FCVT_S_L  154
#define INSTRUCTION_FCVT_D_L  155
#define INSTRUCTION_FCVT_H_WU 156
#define INSTRUCTION_FCVT_S_WU 157
#define INSTRUCTION_FCVT_D_WU 158
#define INSTRUCTION_FCVT_H_LU 159
#define INSTRUCTION_FCVT_S_LU 160
#define INSTRUCTION_FCVT_D_LU 161
#define INSTRUCTION_FCVT_W_H  162
#define INSTRUCTION_FCVT_W_S  163
#define INSTRUCTION_FCVT_W_D  164
#define INSTRUCTION_FCVT_L_H  165
#define INSTRUCTION_FCVT_L_S  166
#define INSTRUCTION_FCVT_L_D  167
#define INSTRUCTION_FCVT_WU_H 168
#define INSTRUCTION_FCVT_WU_S 169
#define INSTRUCTION_FCVT_WU_D 170
#define INSTRUCTION_FCVT_LU_H 171
#define INSTRUCTION_FCVT_LU_S 172
#define INSTRUCTION_FCVT_LU_D 173

// floating point load
#define INSTRUCTION_FLW       180
#define INSTRUCTION_FLD       181

// floating point store
#define INSTRUCTION_FSW       190
#define INSTRUCTION_FSD       191

// floating point arithmetic
#define INSTRUCTION_FADD_S    200
#define INSTRUCTION_FADD_D    201
#define INSTRUCTION_FSUB_S    202
#define INSTRUCTION_FSUB_D    203
#define INSTRUCTION_FMUL_S    204
#define INSTRUCTION_FMUL_D    205
#define INSTRUCTION_FDIV_S    206
#define INSTRUCTION_FDIV_D    207
#define INSTRUCTION_FSQRT_S   208
#define INSTRUCTION_FSQRT_D   209

// floating point compare
#define INSTRUCTION_FEQ_S     210
#define INSTRUCTION_FEQ_D     211
#define INSTRUCTION_FLT_S     212
#define INSTRUCTION_FLT_D     213
#define INSTRUCTION_FLE_S     214
#define INSTRUCTION_FLE_D     215

// meta
#define INSTRUCTION_OUTSTR    221
#define INSTRUCTION_OUTLN     222
#define INSTRUCTION_OUTINT    223
#define INSTRUCTION_INSTR     224
#define INSTRUCTION_ININT     225

