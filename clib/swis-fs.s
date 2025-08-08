.include "asm/macros.hdr"

.global __os_bget
.global _kernel_osbget

.section .text.os_bget

    FUNC    "__os_bget"
_kernel_osbget:
    STP     x29, x30, [sp, #-16]!
    MOV     x29, sp
    MOV     x1, x0
    MOV     x10, #0xa                  // OS_BGet
    ORR     x10, x10, #0x20000
    SVC     #0
    CSEL    x0, x0, xzr, VS            // error, and EOF return -1
    LDP     x29, x30, [sp], #16
    RET
