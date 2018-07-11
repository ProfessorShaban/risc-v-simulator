
// labels
addi x20, x0, 4   // x20 = 4
addi x21, x0, 2   // x21 = 2
loop: addi x20, x20, -1 // x20 = x20 - 1
bgeu x20, x21, loop   // if (x20 == 0) goto loop

// BGEU
addi x20, x0, 4   // x20 = 4
addi x21, x0, 2   // x21 = 2
addi x20, x20, -1 // label1: x20 = x20 - 1
bgeu x20, x21, -2   // if (x20 == 0) goto label1

// BGE
addi x20, x0, 4   // x20 = 4
addi x20, x20, -1 // label1: x20 = x20 - 1
bge x20, x0, -2   // if (x20 == 0) goto label1

// backward branch
addi x20, x0, 1   // x20 = 1
addi x20, x20, -1 // label1: x20 = x20 - 1
beq x20, x0, -2   // if (x20 == 0) goto label1

addi x21, x0, 1

// forward branch
addi x9, x0, -19
addi x10, x0, 1000
add x11, x9, x10
beq x11, x0, 6

addi x12, x0, 1
beq x0, x0, 4

addi x12, x0, 2

add x13, x12, x0
