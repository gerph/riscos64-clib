# RISC OS 64 C library and binaries

This repository contains a C library for AArch64 RISC OS (RISC OS 64). The library is in the process of being developed to allow modules and absolutes to be used within a 64-bit RISC OS environment.

## Usage

There are multiple directories within this repositories that contain binaries:

* `clib` contains the C library. This must be built first.
* `absolutes` contains absolute files (AIF).
* `modules` contains module files.
* `utilities` contains utilities.

To build these on a POSIX system you will require Docker for the cross compiler.

### C library

The C library can be built with:

    make

The C library is limited in functionality, but provides sufficient implementation
for simple tools to be ported. Additional functions can be added as necessary.
The library is usable with both absolutes and modules.

### Absolutes

The absolute test files can be built with:

    make all

Individual files can be built with:

    make TARGET=<filename>

The binaries will be produced as `,ff8` suffixed files. There is also a `.map`
suffixed file, which contains a description of where the content lives within
the file.

### Modules

The module test files can be built with:

    make all

Individual files can be built with:

    make TARGET=<filename>

The binaries will be produced as `,ffa` suffixed files. Again a `.map` file is
present.

### Utilities

Sample utility files can be built with:

    make all

Individual files can be built with:

    make TARGET=<filename>

The binaries will be produced as `,ffc` suffixed files. Again a `.map` file is
present.

### Exported libraries

The exported libraries and tools are built automatically by the GitHub build system.
The exported directory can be downloaded from the [Releases](https://github.com/gerph/riscos64-clib/releases)
and used on POSIX systems (Linux, macOS, WSL2). To install the tooling, use:

    bin/robuild64 install <target-dir>

This will install the tools:

* `riscos64-cc` - compiler
* `riscos64-link` - linker
* `riscos64-libfile` - library tool
* `riscos64-as` - assembler

all of which will work with RISC OS 64-bit binaries.

For example:

To build a single source into an executable, you might use:

    riscos64-cc main.c

Which would build 64-bit executable called `main,ff8`.

Alternatively, the compile and link process can be done separately, for example:

    riscos64-cc -c main.c -o main.o
    riscos64-link -aif main.o -o main,ff8


## AIF Format

The 64-bit AIF format has been defined as follows:

| Offset | Value       | Instructions | Meaning |
| ------ | ----------- | ------------ | ------- |
| &000   | `&e1a00000` | ARM32: `NOP`             | ARM32 decompression |
| &004   | `&e1a00000` | ARM32: `NOP`             | ARM32 reloc |
| &008   | `&e1a00000` | ARM32: `NOP`             | ARM32 zero init |
| &00C   | `&eb00000b` | ARM32: `BL      entry`   | ARM32 image entry |
| &010   | `&ef000011` | ARM32: `SWI     OS_Exit` | explicit OS_Exit |
| &014   | variable    | code + static data size  | read only size |
| &018   | variable    | R/W data size| read write size |
| &01C   | `&0`        | - | debug size |
| &020   | variable    | zero init size | zero init size |
| &024   | `&0`        | - | debug type |
| &028   | `&8000`     | - | linked base address |
| &02C   | `&0`        | - | workspace size (obsolete) |
| &030   | `&40`       | - | address mode |
| &034   | `&0`        | - | data base address |
| &038   | `&0`        | - | reserved |
| &03c   | `&0`        | - | reserved |
| &040   | `&e28f0000` | ARM32: `entry: ADR     r0, error_block`    | ARM32 error report |
| &044   | `&ef00002b` | ARM32: `SWI     OS_GenerateError`          |  |
| &048   | `&0`        | ARM32: `error_block: DCD     0` |  |
| &04c   | string      | ARM32: `= "AArch64 binaries cannot be run on 32-bit RISC OS", 0` |  |
| &07c   | `&0`        | ARM32: `DCD     0` |  |
| ... |
| &100   | `&D503201F` | ARM64: `NOP` | AArch64 decompression |
| &104   | `&94000004` | ARM64: `BL zeroinit` | AArch64 zero init |
| &108   | `&9400001F` | ARM64: `BL entry` | AArch64 entry point |

This follows the pattern defined in https://riscos.com/support/developers/riscos6/programmer/codeformats.html for Absolute files, with 64 (&40) in place of the bitness.

## Module Format

The 64-bit module format is only slightly varied from that defined previously, following the definition in https://pyromaniac.riscos.online/pyromaniac/prm/kernel/modules/modules-supplement.html.

From the standard 32-bit module, there are the following differences:

* The initialisation offset has bit 30 set. This indicates that the module is not ARM. The architecture is defined within the feature flags.
* The feature flags bits 4-7 contains the architecture type:
    * 0 = AArch32
    * 1 = AArch64
    * 2 = x86 64-bit
    * 15 = Python
* The feature flags bit 2 indicates that a zero-initialisation size is present following the feature flags.
* The module will have its zero-initialisation area initialised to 0.
* The module will always be allocated on a page boundary plus 4. That is, the base address of the module in hex will always end in `004`. This allows the standard mechanism for referencing addresses using the `ADRP` instruction to be used.
* Modules will never be multiply instantiated.
* All module entry points follow the pattern of the CMHG entry points, rather than the original register assignments, with the exception of `r12` which remains in `x12`.
    * The initialisation entry point:
        * `x0` -> start string
        * `x1`  = instance number (always 0)
        * `x12` = private word pointer
    * The finalisation entry point:
        * `x12` = private word pointer
    * The SWI entry point:
        * `x0`  = SWI offset
        * `x1` -> register block (x0-x9 on entry)
        * `x12` = private word pointer
    * The command entry point:
        * `x0` -> argument string
        * `x1`  = count of arguments
        * `x2`  = command number
        * `x12` = private word pointer
* All module entry points which can return an error pointer, must return the error pointer in `r0`, or 0. The `V` flag need not be set. This follows the pattern of CMHG entry points.
* All interfaces are assumed to use the frame pointer for a call chain, and should not 0 this value on entry.


## Utility format

Utilities, like AIF files, have an ARM 32-bit header. This follows the standard utility file format defined at https://riscos.com/support/developers/riscos6/programmer/codeformats.html

The format has been extended, thus:

| Offset | Value       | Instructions             | Meaning |
| ------ | ----------- | ------------------------ | ------- |
| &000   | `&ea000005` | ARM32: `B       &1C`     | ARM32 entry |
| &004   | `&79766748` | -                        | magic 1 |
| &008   | `&216c6776` | -                        | magic 2 |
| &00c   | variable    | code + static size       | Read-only size |
| &010   | variable    | R/W data size            | Read-write size |
| &014   | `&00000040` | -                        | Bitness + flags |
| &018   | variable    | -                        | offset of ARM64 entry |
| &01c   | `&e28f0004` | ARM32: `ADR     r0, &24` | ARM32 error report |
| &020   | `&e3500102` | ARM32: `CMP     r0, #&80000000` | |
| &024   | `&e1a0f00e` | ARM32: `MOV     pc, lr`  | |
| &028   | `&00000000` | ARM32: error number      | |
| &02c   | `&63724141` | ARM32: `= "AArch64 binaries cannot be run on 32-bit RISC OS" ,0` | |

The utility file will be entered at the offset given at offset &18.

Future versions of this specification may require that the utility be loaded at a page boundary plus 4 bytes, as modules are.

## Change history

See [Changes](https://github.com/gerph/riscos64-clib/wiki/Changes) for summary of the changes in each release.
