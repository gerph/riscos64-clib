#ifndef KERNEL_H
#define KERNEL_H

#include <stdint.h>

typedef struct _kernel_oserror_s {
    uint32_t errnum;
    char errmess[252];
} _kernel_oserror;

typedef struct _kernel_swi_regs {
    int32_t r[10];
} _kernel_swi_regs;

typedef struct {
   int32_t load, exec;       /* load, exec addresses */
   int32_t start, end;       /* start address/length, end address/attributes */
} _kernel_osfile_block;

typedef struct {
   void * dataptr;       /* memory address of data */
   int32_t nbytes, fileptr;
   int32_t buf_len;
   const char * wild_fld;      /* points to wildcarded filename to match */
} _kernel_osgbpb_block;


#define _KERNEL_NOXBIT (1lu<<31)    /* Always clear the X bit (otherwise always set) */
/*************************************************** Gerph *********
 Function:      _kernel_swi
 Description:   Call an arbitrary RISC OS SWI number.
 Parameters:    swinum = SWI number. May have _KERNEL_NOXBIT OR'd
                         into it to indicate that the SWI not set
                         the X bit but instead raise an error.
                in-> the registers to set before calling the SWI
                out-> the register values on exit
 Returns:       pointer to error, or NULL if no error reported
 ******************************************************************/
_kernel_oserror *_kernel_swi(int swinum, _kernel_swi_regs *in, _kernel_swi_regs *out);

/*************************************************** Gerph *********
 Function:      _kernel_fpavailable
 Description:   Determine whether floating point is available
 Parameters:    none
 Returns:       0 if no FP is available
                1 if standard FP is available
 ******************************************************************/
int _kernel_fpavailable(void);


/* If a function returned a negative error code, and it has also updated the
 * _kernel_last_oserror value, this will be returned:
 */
#define _kernel_ERROR (-2)

/*************************************************** Gerph *********
 Function:      _kernel_last_oserror
 Description:   Return the pointer to the last error reported by
                a _kernel function call (or a call that invokes one)
                Unless otherwise mentioned, all _kernel_ERROR returns
                should set this error.
 Parameters:    none
 Returns:       pointer to error, or NULL if no error reported yet
 ******************************************************************/
_kernel_oserror *_kernel_last_oserror(void);

/*************************************************** Gerph *********
 Function:      _kernel_raise_error
 Description:   Shut down the application and exit with an error
 Parameters:    err-> the error (in our buffer)
 Returns:       never
 ******************************************************************/
void __attribute__((noreturn)) _kernel_raise_error(_kernel_oserror *err);


/* Host values (as returned by OS_Byte 0, 1) */
#define _kernel_HOST_UNDEFINED    -1
#define _kernel_BBC_MOS1_0         0
#define _kernel_BBC_MOS1_2         1
#define _kernel_BBC_ACW            2
#define _kernel_BBC_MASTER         3
#define _kernel_BBC_MASTER_ET      4
#define _kernel_BBC_MASTER_COMPACT 5
#define _kernel_ARTHUR             6
#define _kernel_RISCOS             6
#define _kernel_SPRINGBOARD        7
#define _kernel_A_UNIX             8

/*************************************************** Gerph *********
 Function:      _kernel_hostos
 Description:   Report the host OS (OS_Byte 0, 1)
 Parameters:    none
 Returns:       6 for RISC OS (https://beebwiki.mdfs.net/OSBYTE_%2600)
 ******************************************************************/
int _kernel_hostos(void);

/*************************************************** Gerph *********
 Function:      _kernel_osrdch
 Description:   OS_ReadC
 Parameters:    none
 Returns:       character, or -1 if error, or -27 if escape
 ******************************************************************/
int _kernel_osrdch(void);

/*************************************************** Gerph *********
 Function:      _kernel_oswrch
 Description:   OS_WriteC
 Parameters:    character
 Returns:       -2 if failure, 0 if success
 ******************************************************************/
int _kernel_oswrch(int c);

/*************************************************** Gerph *********
 Function:      _kernel_osfind
 Description:   OS_Find interface
 Parameters:    op = OS_Find reason
                name = filename
 Returns:       Open: 0 if open failed (no error), or file handle, -2 failure
                Close: 0 for success -2 for failure
 ******************************************************************/
int _kernel_osfind(int op, const char *name);

/*************************************************** Gerph *********
 Function:      _kernel_osargs
 Description:   OS_Args interface
 Parameters:    op = OS_Args reason
                handle = file handle
                arg = additional argument
 Returns:       if op|handle == 0: returns r0
                else: returns r2
 ******************************************************************/
int _kernel_osargs(int op, unsigned handle, int arg);

/*************************************************** Gerph *********
 Function:      _kernel_osgbpb
 Description:   OS_GBPB interface to read bytes from the FS
 Parameters:    op = OS_GBPB operation
                handle = file handle to operate on
                inout = the parameters to pass through to the OS_GBPB interface
 Returns:       _kernel_ERROR for failure, or 0 for success (-1 was C set)
 ******************************************************************/
int _kernel_osgbpb(int op, unsigned handle, _kernel_osgbpb_block *inout);

/*************************************************** Gerph *********
 Function:      _kernel_osbget
 Description:   OS_BGet interface to read a single byte from a file
 Parameters:    handle = the file handle to read
 Returns:       byte read, or -1 if nothing read (eg EOF)
 ******************************************************************/
int _kernel_osbget(unsigned handle);

/*************************************************** Gerph *********
 Function:      _kernel_osfile
 Description:   Call OS_File for file based operations
 Parameters:    op = the operation reason
                name-> the filename to operate on
                inout-> the block for input/output parameters:
                            load = load address (r2)
                            exec = exec address (r3)
                            start = start address for file operations (r4)
                            end = end address for file operations (r5)
 Returns:       r0 on return, or _kernel_ERROR for failure
 ******************************************************************/
int _kernel_osfile(int op, const char *name, _kernel_osfile_block *inout);

/*************************************************** Gerph *********
 Function:      _kernel_osbyte
 Description:   OS_Byte interface
 Parameters:    op = OS_Byte reason
                x = BBC X register
                y = BBC Y register
 Returns:       X | (Y<<8) | (C<<16) ; C is never used in 64bit
                _kernel_ERROR for failure
 ******************************************************************/
int _kernel_osbyte(int op, int x, int y);

/*************************************************** Gerph *********
 Function:      _kernel_osword
 Description:   OS_Word interface
 Parameters:    op = OS_Word reason
                data-> the data for the OS_Word call.
 Returns:       return r1, or _kernel_ERROR for failure
 ******************************************************************/
int _kernel_osword(int op, int *data);

/*************************************************** Gerph *********
 Function:      _kernel_escape_seen
 Description:   Check whether the Escape key has been pressed
 Parameters:    none
 Returns:       1 if escape has been pressed, or 0 if not
 ******************************************************************/
int _kernel_escape_seen(void);

/*************************************************** Gerph *********
 Function:      _kernel_setenv
 Description:   Set an environment variable
 Parameters:    var-> the variable name to set
                val-> the value to set to, or NULL to delete
 Returns:       pointer to error if failed, or NULL if successful
 ******************************************************************/
_kernel_oserror *_kernel_setenv(const char *var, const char *val);

/*************************************************** Gerph *********
 Function:      _kernel_getenv
 Description:   Read the environment variable into a supplied buffer,
                as a 0-terminated string
 Parameters:    var-> the variable name to read
                buf-> the buffer to use to read the string into
                size = the size of the buffer
 Returns:       pointer to error if failed, or NULL if successful
 ******************************************************************/
_kernel_oserror *_kernel_getenv(const char *var, char *buf, unsigned size);


/*************************************************** Gerph *********
 Function:      _kernel_oscli
 Description:   Run a CLI command and report whether it errored - this
                may replace the current application.
 Parameters:    cmd-> the command to run
 Returns:       1 if ok, 0 if failed
 NOTE:          This is implemented according to the actual implementation.
                The PRM implies that the return indicates that there is an error.
 ******************************************************************/
int _kernel_oscli(const char *cli);


/*************************************************** Gerph *********
 Function:      _kernel_backtrace
 Description:   Report the backtrace to VDU
 Parameters:    none
 Returns:       none
 ******************************************************************/
void _kernel_backtrace(void);

typedef struct {
   uint64_t fp; /* current FP */
   uint64_t sp; /* current SP */
   uint64_t lr; /* Where we were */
   uint64_t pc; /* Where we are */
} _kernel_unwindblock;

/*************************************************** Gerph *********
  Function:      _kernel_unwind
  Description:   Unwind the stack; of return the current state
  Parameters:    inout-> the unwind block to step up from
                 language-> where to store the language name pointer
  Returns:       >0 if got a stack entry
                 0 if we reached the end of the backtrace
                 <0 if something went wrong
  ******************************************************************/
int _kernel_unwind(_kernel_unwindblock *inout, char **language);

/*************************************************** Gerph *********
 Function:      _kernel_procname
 Description:   For a given PC, return the procedure name
 Parameters:    pc = the address to search
 Returns:       Pointer to the function name, or NULL if none known
 ******************************************************************/
const char *_kernel_procname(uint64_t pc);

/*************************************************** Gerph *********
 Function:      _kernel_system
 Description:   Call the CLI command as a subprocess/replacement
 Parameters:    cmd-> the command to run
                chain = 0 to run as a sub-program
                        1 to replace the current program
 Returns:       1 if failed, 0 if successful?
 ******************************************************************/
int _kernel_system(const char *cmd, int chain);

#endif
