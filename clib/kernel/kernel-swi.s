.include "asm/macros.hdr"

.text

.global _kernel_swi

// _kernel_swi(int swinum, inregs, outregs)
// If b31 of swinum is set, we clear the X bit.
    FUNC    "_kernel_swi"
    STP     x29, x30, [sp, #-16]!
    MOV     x29, sp
    MOV     x3, x0, LSR #31-17
    AND     x3, x3, #0x20000
    ORR     x10, x0, #0x20000               // SWI number with X bit set
    BIC     x10, x10, x3
    MOV     x11, x2

// Initialise registers
    CMP     x1, #0
    BNE     _kernel_swi_in_regs

    MOV     x0, #0
    MOV     x1, #0
    MOV     x2, #0
    MOV     x3, #0
    MOV     x4, #0
    MOV     x5, #0
    MOV     x6, #0
    MOV     x7, #0
    MOV     x8, #0
    MOV     x9, #0
    B       _kernel_swi_in_done

_kernel_swi_in_regs:
    MOV     x12, x1
    LDP     w0, w1, [x12], #8
    LDP     w2, w3, [x12], #8
    LDP     w4, w5, [x12], #8
    LDP     w6, w7, [x12], #8
    LDP     w8, w9, [x12], #8

_kernel_swi_in_done:
    SVC     #0

    MOV     x12, #0
    BVC     _kernel_swi_no_error
    MOV     x12, x0

_kernel_swi_no_error:

// now store register results
    CMP     x11, #0
    BEQ     _kernel_swi_out_regs_done

    STP     w0, w1, [x11], #8
    STP     w2, w3, [x11], #8
    STP     w4, w5, [x11], #8
    STP     w6, w7, [x11], #8
    STP     w8, w9, [x11], #8

_kernel_swi_out_regs_done:

    MOV     x0, x12

    LDP     x29, x30, [sp], #16
    RET
