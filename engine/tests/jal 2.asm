
addi x10, x0, 10 //g​
addi x11, x0, 11 //h​
addi x12, x0, 12 //i​
addi x13, x0, 13 //j​

addi x5, x0, 5​
addi x6, x0, 6​
addi x20, x0, 20​

addi x2, x0, 2040  // sp is stored in register x2

jal x1, leaf_example

addi x8, x0, 8​

j end

leaf_example:
addi x2, x2, -24​
sd   x5, 16(x2)​
sd   x6, 8(x2)​
sd   x20, 0(x2)​
add  x5, x10, x11​
add  x6, x12, x13​
sub  x20, x5,x6​
addi x10, x20,0​
ld   x20, 0(x2)​
ld   x6, 8(x2)​
ld   x5, 16(x2)​
addi x2, x2, 24​
jalr x0, x1, 0​

end:

add x1, x0, x0
