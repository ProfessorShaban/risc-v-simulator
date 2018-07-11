
// ADDIW
addi x11, x0, 11
addiw x12, x11, -1

// Add
addi x10, x0, 10
addi x11, x0, 11
add x12, x10, x11

// Sub
addi x10, x0, 101
addi x11, x0, 5
sub x12, x10, x11

// ADDW
addi x10, x0, 10  // addw
addi x11, x0, 11
addw x12, x10, x11

// SUBW
addi x10, x0, 10  // subw
addi x11, x0, 12
subw x12, x10, x11

// Negative numbers
addi x10, x0, -33
addi x9, x0, 11
add x11, x9, x10

addi x10, x0, -33
addi x9, x0, 11
sub x11, x10, x9

// XOR
addi x10, x0, 183  // x10 = 0xb7
addi x11, x0, 255  // x11 = 0xff
xor x12, x10, x11  // x12 = 0x48 (72)

// Or
addi x10, x0, 183  // x10 = 0xb7
addi x11, x0, 15   // x11 = 0x0f
or x12, x10, x11   // x12 = 0xbf (191)

// And
addi x10, x0, 183  // x10 = 0xb7
addi x11, x0, 240  // x11 = 0xf0
and x12, x10, x11  // x12 = 0xb0 (176)

// Addi, Xori
addi x10, x0, 183  // x10 = 0xb7
xori x12, x10, 255 // immediate = 0xff, x12 = 0x48 (72)

// Ori
addi x10, x0, 183  // x10 = 0xb7
ori x12, x10, 15   // immediate = 0x0f, x12 = 0xbf (191)

// Andi
addi x10, x0, 183  // x10 = 0xb7
andi x12, x10, 240 // immediate = 0xf0, x12 = 0xb0 (176)

// LUI
lui x20, 600   // LUI

// AUIPC
auipc x20, 10  // AUIPC

