
thirteen:
data 13

onemillion:
data 1000000

pi:
data 3.14159

one_point_one:
data 1.1

temp:
data 0.0

addi x21, x0, 199
fcvt.s.w f3, x21   // fcvt.s.w, convert int to single precision
addi x20, x0, 1000
fsw f3, x20, 0     // fsw

flw f4, x20, 0     // flw

fcvt.w.s x22, f4   // fcvt.w.s, convert single precision to int

// FCVT.D.L
addi x10, x0, 33   // fcvt.d.l
fcvt.d.l f0, x10

// fld
auipc x20, pi
fld f0, x20, 0     // fld pi
auipc x20, one_point_one
fld f1, x20, 0     // fld one_point_one

// FCVT.L.D
fcvt.l.d x10, f0   // fcvt.l.d

fadd.d f2, f0, f1  // fadd.d

auipc x20, temp
fsd f2, x20, 0     // fsd

fsub.d f2, f0, f1  // fsub.d

fmul.d f2, f0, f1  // fmul.d

fdiv.d f2, f0, f1  // fdiv.d

fsqrt.d f2, f0  // fsqrt.d

// single precision
addi x22, x0, 31   // single precision
fcvt.s.w f10, x22
addi x23, x0, 2
fcvt.s.w f11, x23
addi x24, x0, 1000
fadd.s f12, f10, f11  // fadd.s
fsw f12, x24, 0
fsub.s f12, f10, f11  // fsub.s
fsw f12, x24, 0
fmul.s f12, f10, f11  // fmul.s
fsw f12, x24, 0
fdiv.s f12, f10, f11  // fdiv.s
fsw f12, x24, 0
fsqrt.s f12, f10      // fsqrt.s
fsw f12, x24, 0

