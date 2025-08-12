.section .init.aif, "a"

.global _start
.global _aif64_entry

.include "asm/macros.hdr"
.include "asm/swis.hdr"


.balign 4
_aif64_entry:
.type _aif64_entry, @function
        BL      __RelocCode     // Relocation code
        BL      _zeroinit       // Zero initialisation
        BL      _start
        MOV     x10, #0x11      // OS_Exit
        SVC     #0

_zeroinit:
.type _zeroinit, @function
        SUB     x0, lr, #0x100 + 0x8    // Start of the application space
        LDR     w3, [x0, #0x14] // code size
        LDR     w4, [x0, #0x18] // data size
        ADD     x1, x0, x3
        ADD     x1, x1, x4
        LDR     w4, [x0, #0x20] // zero init size
        MOV     x5, #0
        MOV     x6, #0

        CMP     x4, #128
        BLO     _zeroinit_fastdone
_zeroinit_fastloop:
        STP     x5, x6, [x1], #16
        STP     x5, x6, [x1], #16
        STP     x5, x6, [x1], #16
        STP     x5, x6, [x1], #16
        STP     x5, x6, [x1], #16
        STP     x5, x6, [x1], #16
        STP     x5, x6, [x1], #16
        STP     x5, x6, [x1], #16
        SUB     x4, x4, #128
        CMP     x4, #128
        BHS     _zeroinit_fastloop
_zeroinit_fastdone:
        CMP     x4, #4
        BLO     _zeroinit_slowdone
_zeroinit_slowloop:
        STR     w5, [x1], #4
        SUB     x4, x4, #4
        CMP     x4, #4
        BHS     _zeroinit_slowloop

_zeroinit_slowdone:
        RET

_start:
.type _start, @function
// Set up the stack pointer
        MOV     x10, #OS_GetEnv
        SVC     #0

        MOV     fp, #0          // Clear the frame pointer
        MOV     sp, x1          // Set the stack pointer to our memory limit (maybe this should be implicit now?)

        MOV     x3, x1          // memory limit
        SUB     x1, lr, #0x100 + 0xC    // Start of the application space

// x0-> start string
// x1-> start of application
// x2-> end of application
// x3-> end of memory
        LDR     w2, [x1, #0x14] // code size
        LDR     w4, [x1, #0x18] // data size
        ADD     x2, x1, x2
        ADD     x2, x2, x4
        LDR     w4, [x1, #0x20] // zero init size
        ADD     x2, x2, x4

        BL      __main
        B       _Exit
