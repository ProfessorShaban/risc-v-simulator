
// sraw
addi x20, x0, -19  // sraw
addi x21, x0, 4
sraw x20, x20, x21

// sraiw
addi x20, x0, -19  // sraiw
sraiw x20, x20, 4

// slliw
addi x20, x0, 254
slliw x20, x20, 8  // slliw

// sra
addi x20, x0, 254  // sra
addi x21, x0, 4
sra x20, x20, x21

// srai
addi x20, x0, -19  // srai
srai x20, x20, 4

addi x20, x0, -19  // sra
addi x21, x0, 4
sra x20, x20, x21

// srliw
addi x20, x0, 254
srliw x20, x20, 4  // srliw

// sllw
addi x20, x0, 254
addi x21, x0, 8
sllw x20, x20, x21 // sllw

// srlw
addi x20, x0, 254
addi x21, x0, 4
srlw x20, x20, x21 // srlw

// slli
addi x20, x0, 254  // slli, load x20 = 0xfefdfcfbfaf9f8f7
slli x20, x20, 8   // shift left
addi x20, x20, 253 // 0xfd
slli x20, x20, 8   // shift left
addi x20, x20, 252 // 0xfc
slli x20, x20, 8   // shift left
addi x20, x20, 251 // 0xfb
slli x20, x20, 8   // shift left
addi x20, x20, 250 // 0xfa
slli x20, x20, 8   // shift left
addi x20, x20, 249 // 0xf9
slli x20, x20, 8   // shift left
addi x20, x20, 248 // 0xf8
slli x20, x20, 8   // shift left
addi x20, x20, 247 // 0xf7
// now x20 contains 0xfefdfcfbfaf9f8f7

// sll
addi x20, x0, 254  // sll
addi x21, x0, 16
sll x20, x20, x21

// srli
addi x20, x0, 254  // srli
srli x20, x20, 4

// srl
addi x20, x0, 254  // srl
addi x21, x0, 4
srl x20, x20, x21

addi x20, x0, -19  // srl
addi x21, x0, 4
srl x20, x20, x21
