
greeting:
data "Hello"

name_prompt:
data "Enter your name: "

name_prefix:
data "Your name is: "

user_input:
data "                                                                                "

user_input_int:
data 1234

powers_of_2:
data 1, 2, 4, 8, 16


// get string input
auipc x20, name_prompt
outstr x20
auipc x20, user_input
instr x20
auipc x21, name_prefix
outstr x21
outstr x20
outln

// get integer input
auipc x20, user_input_int
inint x20
outint x20
outln

// print greeting
auipc x20, greeting
outstr x20	// output the string "Hello"
outln		// output a new line

// print powers of 2
auipc x20, powers_of_2
addi x11, x0, 5
loop: outint x20
outln
addi x20, x20, 4
addi x11, x11, -1
beq x11, x0, done
j loop
done:

