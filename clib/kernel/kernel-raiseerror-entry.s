.include "asm/macros.hdr"

.text

.global _kernel_raise_error


// _kernel_raise_error will:
//
// * clear the stack pointer to the top of the stack
// * reset the frame pointer
// * copy the error supplied into a buffer.
// * call _kernel_raise_error_int

_kernel_raise_error:
    LDR     x1, in_module_p
    LDR     x1, [x1]
    CMP     x1, #1
    BNE     in_application
// FIXME: This is another place where we assume that being a module we don't
//        execute in application space.

// If we're in a module and they called this function then... not much
// we can do, as they don't expect to return. We will call OS_GenerateError
// and hope.
    B       __os_generateerror

in_application:
    LDR     x1, root_stack_p
    LDR     x1, [x1]
    MOV     sp, x1
    MOV     fp, #0

    CMP     x0, #0
    BEQ     no_error_copy
// If the error block were invalid, we would call the abort handler, and
// we have no way of determining the place we were called from. That's tough,
// as by calling the _kernel_raise_error you're acknowledging that you're
// done now.

// copy the error block
    LDR     x1, error_buffer_p
    MOV     x4, x1
    ADD     x3, x4, #255
    LDR     w2, [x0], #4                // copy the error word
    STR     w2, [x4], #4
error_copy_loop:
    MOV     w2, #0
    CMP     x4, x3
    BEQ     error_copy_big
    LDRB    w2, [x0], #1
error_copy_big:
    STRB    w2, [x4], #1
    CMP     w2, #0
    BNE     error_copy_loop

    MOV     x0, x1                      // error is now in our buffer
no_error_copy:
    B       _kernel_raise_error_int

root_stack_p:
.dword      __root_stack
error_buffer_p:
.dword      _kernel_error_buffer + 4    // +4 to skip PC ; FIXME: only 32bit.
in_module_p:
.dword      _kernel_inmodule
