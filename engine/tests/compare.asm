
// SLT
addi x10, x0, 10   // SLT
addi x11, x0, 11
slt x20, x10, x11
addi x11, x0, 9
slt x20, x10, x11

// SLTI
addi x10, x0, 10   // SLTI
slti x20, x10, 11
slti x20, x10, 9

// SLTU
addi x10, x0, -10   // SLTU
addi x11, x0, -11
sltu x20, x10, x11
addi x11, x0, -9
sltu x20, x10, x11

// SLTIU
addi x10, x0, 10   // SLTIU
sltiu x20, x10, 11
sltiu x20, x10, 9

