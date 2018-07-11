
addi x10, x0, 33
fcvt.d.l f0, x10
addi x10, x0, 31
fcvt.d.l f1, x10
fcvt.d.l f2, x10
feq.d x11, f0, f1    // feq.d
feq.d x11, f1, f2    // feq.d

flt.d x11, f0, f1    // flt.d
flt.d x11, f1, f2    // flt.d

fle.d x11, f0, f1    // fle.d
fle.d x11, f1, f2    // fle.d

// single-precision
addi x10, x0, 33     // single precision
fcvt.s.w f10, x10
addi x10, x0, 31
fcvt.s.w f11, x10
fcvt.s.w f12, x10
feq.s x11, f10, f11  // feq.s
feq.s x11, f11, f12  // feq.s

flt.s x11, f10, f11  // flt.s
flt.s x11, f11, f12  // flt.s

fle.s x11, f10, f11  // fle.s
fle.s x11, f11, f12  // fle.s

