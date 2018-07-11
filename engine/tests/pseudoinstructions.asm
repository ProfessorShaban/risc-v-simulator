
// mv (move)
mv x22, x0
addi x20, x0, 1   // x20 = 1
mv x21, x20       // x21 = x20
mv x22, x20

// nop (no operation)
addi x20, x0, 4   // x20 = 4
nop               // do nothing
nop
addi x20, x20, 1  // x20 = x20 + 1

// j (jump)
addi x20, x0, 10  // x20 = 10
j target
addi x20, x20, 1  // x20 = x20 + 1
target:
addi x20, x20, 1  // x20 = x20 + 1

