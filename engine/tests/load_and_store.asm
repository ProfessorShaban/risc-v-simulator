
Start:

// Store byte, halfword, word, doubleword
addi x20, x0, 254  // 0xfe
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

addi x7, x0, 20
repeat:
addi x21, x0, 2018
sd x20, x21, 0     // store doubleword
addi x7, x7, -1
addi x21, x21, 8
bne x7, x0, repeat

add x1, x0, x7
add x2, x0, x20
add x3, x1, x2
add x4, x1, x3
add x5, x1, x4
add x6, x1, x5
add x7, x1, x6
add x8, x1, x7
add x9, x1, x8
add x10, x1, x9
add x11, x1, x10
add x12, x1, x11
add x13, x1, x12
add x14, x1, x13
add x15, x1, x14
add x16, x1, x15
add x17, x1, x16
add x18, x1, x17
add x19, x1, x18
add x20, x1, x19
add x21, x1, x20
add x22, x1, x21
add x23, x1, x22
add x24, x1, x23
add x25, x1, x24
add x26, x1, x25
add x27, x1, x26
add x28, x1, x27
add x29, x1, x28
add x30, x1, x29
add x31, x1, x30
add x32, x1, x31

addi x21, x0, 2018
sb x20, 0(x21)     // store byte at 2000
sd x0, x21, 0      // clear memory at 2000
sh x20, x21, 0     // store halfword at 2000
sd x0, x21, 0      // clear memory at 2000
sw x20, x21, 0     // store word at 2000
sd x0, x21, 0      // clear memory at 2000
sd x20, x21, 0     // store doubleword at 2000

// load
addi x21, x0, 2024 // load
lb x20, x21, 4     // lb
lh x20, x21, 4     // lh
lw x20, x21, 4     // lw
ld x20, x21, 0     // ld
lbu x20, x21, 4     // lbu
lhu x20, x21, 4     // lhu
lwu x20, x21, 4     // lwu

// load byte
addi x9, x0, -19
addi x10, x0, 2024
sb x9, x10, 0
lb x12, x10, 0

j start
