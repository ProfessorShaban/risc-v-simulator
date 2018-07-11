
prompt:
data "Enter a number: "

answer_str:
data "The number doubled is: "

user_input_int:
data 1234

double_data:
data 1234

// JAL
auipc x20, prompt
outstr x20
auipc x20, user_input_int
inint x20
lw x10, x20, 0
jal x30, double_x10
auipc x20, double_data
sw x10, x20, 0
auipc x20, answer_str
outstr x20
auipc x10, double_data
outint x10
outln
j end

double_x10:
slli x10, x10, 1
jalr x0, x30, 0

end:
addi x0, x0, 1
