.include "asm/macros.hdr"

.global __os_writec
.global __os_readc
.global __os_word
.global __os_inkey
.global __os_write0
.global __os_write0end
.global __os_writen
.global __os_newline
.global __os_readescapestate
.global __os_readmonotonictime
.global __os_generateerror
.global __os_file2
.global __os_file3
.global __os_fscontrol2
.global __os_fscontrol3
.global _kernel_osfile
.global __os_changeenvironment
.global __os_byte_out1

.section .text.os_writec

    FUNC    "__os_writec"
    STP     x29, x30, [sp, #-16]!
    MOV     x29, sp
    MOV     x10, #0                     // OS_WriteC
    ORR     x10, x10, #0x20000
    SVC     #0
    CSEL    x0, x0, xzr, VS
    LDP     x29, x30, [sp], #16
    RET

.section .text.os_readc
    FUNC    "__os_readc"
    STP     x29, x30, [sp, #-16]!
    MOV     x29, sp
    MOV     x1, #0
    MOV     x10, #4                     // OS_ReadC
    ORR     x10, x10, #0x20000
    SVC     #0
    CSINV   x0, x0, xzr, VC
    CMP     x1, #1
    MOV     x1, #-27
    CSEL    x0, x0, x1, NE
    LDP     x29, x30, [sp], #16
    RET

.section .text.os_word
    FUNC    "__os_word"
    STP     x29, x30, [sp, #-16]!
    MOV     x29, sp
    STRB    w1, [x2]
    MOV     x1, x2
    MOV     x10, #7                     // OS_Word
    ORR     x10, x10, #0x20000
    SVC     #0
    CSEL    x0, x0, xzr, VS
    LDP     x29, x30, [sp], #16
    RET

.section .text.os_inkey
    FUNC    "__os_inkey"
    STP     x29, x30, [sp, #-16]!
    MOV     x29, sp
    AND     x1, x0, #255
    LSR     x2, x0, #8
    MOV     x0, #0x81                   // INKEY
    MOV     x10, #0x6                   // OS_Byte
    ORR     x10, x10, #0x20000
    SVC     #0                          // Returns R1 = character read
                                        //         R2 = 0 =>char was read, 27=>escape, 255=>nothing read
// We are going to return -1 for nothing read, and -2 for escape
    CMP     x2, #27
    BEQ     __os_inkey_escape
    CMP     x2, #255
    CSINV   x0, x1, xzr, NE             // if x2!=255 x0=x1 else x0=-1
__os_inkey_exit:
    LDP     x29, x30, [sp], #16
    RET

__os_inkey_escape:
    MOV     x0, #-2
    B       __os_inkey_exit

.section .text.os_write0
    FUNC    "__os_write0"
    STP     x29, x30, [sp, #-16]!
    MOV     x29, sp
    MOV     x10, #0x2                   // OS_Write0
    ORR     x10, x10, #0x20000
    SVC     #0
    CSEL    x0, x0, xzr, VS
    LDP     x29, x30, [sp], #16
    RET

.section .text.os_write0end
    FUNC    "__os_write0end"
    STP     x29, x30, [sp, #-16]!
    MOV     x29, sp
    MOV     x10, #0x2                   // OS_Write0
    ORR     x10, x10, #0x20000
    SVC     #0
    STR     x0, [x1]
    CSEL    x0, x0, xzr, VS
    LDP     x29, x30, [sp], #16
    RET

.section .text.os_writen
    FUNC    "__os_writen"
    STP     x29, x30, [sp, #-16]!
    MOV     x29, sp
    MOV     x10, #0x46                  // OS_WriteN
    ORR     x10, x10, #0x20000
    SVC     #0
    CSEL    x0, x0, xzr, VS
    LDP     x29, x30, [sp], #16
    RET

.section .text.os_newline
    FUNC    "__os_newline"
    STP     x29, x30, [sp, #-16]!
    MOV     x29, sp
    MOV     x10, #0x3                   // OS_NewLine
    ORR     x10, x10, #0x20000
    SVC     #0
    CSEL    x0, x0, xzr, VS
    LDP     x29, x30, [sp], #16
    RET

.section .text.os_readescapestate
    FUNC    "__os_readescapestate"
    STP     x29, x30, [sp, #-16]!
    MOV     x29, sp
    MOV     x10, #0x2C                  // OS_ReadEscapeState
    ORR     x10, x10, #0x20000
    SVC     #0
// We avoid flag returns from SWIs on RISC OS 64, so this returns the state in x0
    LDP     x29, x30, [sp], #16
    RET

.section .text.os_readmonotonictime
    FUNC    "__os_readmonotonictime"
    STP     x29, x30, [sp, #-16]!
    MOV     x29, sp
    MOV     x10, #0x42                  // OS_ReadMonotonicTime
    ORR     x10, x10, #0x20000
    SVC     #0
    CSEL    x0, x0, xzr, VC             // if VC return time; otherwise return 0
    LDP     x29, x30, [sp], #16
    RET

.section .text.os_changeenvironment
// r0 = envnumber, r1 = handler, r2 = workspace, r3 = buffer, r4-> old values (r1,r2,r3)
// returns r0-> error
    FUNC    "__os_changeenvironment"
    STP     x29, x30, [sp, #-16]!
    MOV     x29, sp
    MOV     x10, #0x40                  // OS_ChangeEnvironment
    ORR     x10, x10, #0x20000
    SVC     #0
    BVS     __os_changeenvironment_error

    CMP     x4, #0
    BEQ     __os_changeenvironment_exit
    STP     x1, x2, [x4]
    STR     x3, [x4, #16]
__os_changeenvironment_exit:
    MOV     x0, #0                      // no error

__os_changeenvironment_error:
    LDP     x29, x30, [sp], #16
    RET

.section .text.os_generateerror
    FUNC    "__os_generateerror"
    STP     x29, x30, [sp, #-16]!
    MOV     x29, sp
    MOV     x10, #0x2B                  // OS_GenerateError
    SVC     #0
    LDP     x29, x30, [sp], #16
    MOV     x0, #1                      // If OS_GenerateError returns, we jump to the exit code
    B       exit

.section .text.os_file2
    FUNC    "__os_file2"
__os_file3:
.type __os_file3, @function
    STP     x29, x30, [sp, #-16]!
    MOV     x29, sp
    MOV     x10, #0x9                   // OS_File
    ORR     x10, x10, #0x20000
    SVC     #0
    LDP     x29, x30, [sp], #16
    RET

.section .text.kernel_osfile
/* => x0 = op
      x1-> filename
      x2-> buffer for x2-x5 (in and out)
   <= r0 = r0 on return; or -1 if error.
 */
    FUNC    "_kernel_osfile"
    STP     x29, x30, [sp, #-16]!
    MOV     x29, sp

    MOV     x6, x2

    CMP     x6, #0
    BEQ     _kosfile_skip_load

    LDP     w2, w3, [x6]
    LDP     w4, w5, [x6, #8]

_kosfile_skip_load:
    MOV     x10, #0x9                   // OS_File
    ORR     x10, x10, #0x20000
    SVC     #0

    CMP     x6, #0
    BEQ     _kosfile_skip_store

    STP     w2, w3, [x6]
    STP     w4, w5, [x6, #8]

_kosfile_skip_store:
    MOV     x6, #-2
    CSEL    x0, x0, x6, VS              // return -2 if error, or x0 if not

    LDP     x29, x30, [sp], #16
    RET

.section .text.os_fscontrol2
    FUNC    "__os_fscontrol2"
__os_fscontrol3:
.type __os_fscontrol3, @function
    STP     x29, x30, [sp, #-16]!
    MOV     x29, sp
    MOV     x10, #0x29                  // OS_FSControl
    ORR     x10, x10, #0x20000
    SVC     #0
    LDP     x29, x30, [sp], #16
    RET

// OS_Byte with simple semantics
// int __os_byte_out1(r0, r1, r2) => r1 value on return
.section .text.os_byte_out1
    FUNC    "__os_byte_out1"
    STP     x29, x30, [sp, #-16]!
    MOV     x29, sp
    MOV     x10, #0x6                   // OS_Byte
    SVC     #0
    MOV     x0, x1
    LDP     x29, x30, [sp], #16
    RET

