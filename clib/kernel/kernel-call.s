.include "asm/macros.hdr"

.text

.global _kernel_call

// _kernel_call(void *func, void *funcpw, inregs, outregs)
    FUNC    "_kernel_call"
    STP     x29, x30, [sp, #-16]!
    MOV     x29, sp
    MOV     x11, x0                         // x11-> function
    MOV     x12, x1                         // x12 = private word
    MOV     x10, x3                         // x10-> output registers

// Initialise registers
    CMP     x2, #0
    BNE     _kernel_call_in_regs

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
    B       _kernel_call_in_done

_kernel_call_in_regs:
    MOV     x13, x2
    LDP     w0, w1, [x13], #8
    LDP     w2, w3, [x13], #8
    LDP     w4, w5, [x13], #8
    LDP     w6, w7, [x13], #8
    LDP     w8, w9, [x13], #8

_kernel_call_in_done:
    STR     x10, [sp, #-16]!
    BLR     x11
    LDR     x10, [sp], #16

    MOV     x12, #0
    BVC     _kernel_call_no_error
    MOV     x12, x0

_kernel_call_no_error:

// now store register results
    CMP     x10, #0
    BEQ     _kernel_call_out_regs_done

    STP     w0, w1, [x10], #8
    STP     w2, w3, [x10], #8
    STP     w4, w5, [x10], #8
    STP     w6, w7, [x10], #8
    STP     w8, w9, [x10], #8

_kernel_call_out_regs_done:

    MOV     x0, x12

    LDP     x29, x30, [sp], #16
    RET
