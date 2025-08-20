# Makefile for building bin for aarch64
#
# Code compiles with:
#
#	__riscos
#	__riscos64
#   __aarch64__
#
# Norcroft defines not set:
#	__acorn
#	__arm

TARGET ?= hello_world

CROSS_ROOT = ${shell echo $$CROSS_ROOT}

ALL_TARGETS = hello_world \
			  hello_world_macros


targetted:
	make ${TARGET},ffc TARGET=${TARGET}

all:
	for i in ${ALL_TARGETS} ; do make $$i,ffc TARGET=$$i || exit $$? ; done

shell: dockcross-linux-arm64
	./dockcross-linux-arm64 bash

dockcross-linux-arm64:
	docker run --rm dockcross/linux-arm64:latest > dockcross-linux-arm64
	chmod +x dockcross-linux-arm64

clean:
	-rm -f *.o *.a *.bin *,ffc *.map

ifeq (${CROSS_ROOT},)
# If we're outside the docker container, re-run ourselves inside the container

CLIBDIR = $(shell realpath ../clib/export)

ifneq ($(filter-out all shell dockcross-linux-arm64 clean,${MAKECMDGOALS}),)
# The command wasn't one of our invocation commands above
.PHONY: ${MAKECMDGOALS}
${MAKECMDGOALS}: dockcross-linux-arm64 ${CRT_OBJS}
	./dockcross-linux-arm64 --args "-v ${CLIBDIR}:/ro64/clib" -- bash -c "cd . && make ${MAKECMDGOALS} TARGET=${TARGET}"
else
.PHONY: ${DEFAULT_GOAL}
${DEFAULT_GOAL}: dockcross-linux-arm64 ${CRT_OBJS}
	./dockcross-linux-arm64 --args "-v ${CLIBDIR}:/ro64/clib" -- bash -c "cd . && make ${MAKECMDGOALS} TARGET=${TARGET}"
endif

${CLIBDIR}/libcrt.a:
	@echo C library has not been exported >&2
	@echo Use 'make export' in the clib directory >&2
	@false

else
# We are within the docker container

CLIBDIR = /ro64/clib

CRT_OBJS = 	${CLIBDIR}/libcrt.a


USE_FUNC_SIGNATURE ?= 1

# Remove the flags that might make code think it's compiling for linux system.
CFLAGS = -U__linux -U__linux__ -U__unix__ -U__unix -Ulinux -Uunix -U__gnu_linux__

# Add the definitions to indicate that we're compiling for RISC OS
CFLAGS += -D__riscos -D__riscos64

# Allow us to build without assuming the standard library is present
CFLAGS += -nostdlib -ffreestanding -march=armv8-a
#CFLAGS += -nostdlib -ffreestanding -march=armv8-a+nofp

# Add the exports directory to those things we'll build with
CFLAGS += -I${CLIBDIR}/C -I${CLIBDIR}/Lib/ -I${CLIBDIR}

# CLib libraries for the headers
AFLAGS += -I${CLIBDIR}

# Options to allow function signatures to appear RISC OS-like
ifeq (${USE_FUNC_SIGNATURE},1)
CFLAGS += -fpatchable-function-entry=10,10
endif

# Position independant code allows relocation
CFLAGS += -fPIC

# Optimisation options
CFLAGS += -O1

# Options for this build
CFLAGS += 

# Assembler flags
AFLAGS = -march=armv8-a

# Flags for the linker
LDFLAGS = -T ${CLIBDIR}/linker/utility.lnk

CC = aarch64-unknown-linux-gnu-gcc
AS = aarch64-unknown-linux-gnu-as
LD = aarch64-unknown-linux-gnu-ld
AR = aarch64-unknown-linux-gnu-ar
OBJCOPY = aarch64-unknown-linux-gnu-objcopy
OBJDUMP = aarch64-unknown-linux-gnu-objdump
ROSYMBOLS = ${CLIBDIR}/bin/riscos64-addsignatures
RORELOC = ${CLIBDIR}/bin/riscos64-mkreloc

OBJS =	${TARGET}.o

%.o: %.c
	${CC} ${CFLAGS} -c -o $@ $?

%.o: %.s
	${AS} ${AFLAGS} -o $@ $?

${TARGET}.bin: ${OBJS} ${CRT_OBJS}
	${LD} -r ${OBJS} ${CRT_OBJS} ${LDFLAGS} -o $@.tmp
	${OBJDUMP} --reloc --syms --wide $@.tmp > $@.syms
	${RORELOC} $@.syms $@.reloc
	${LD} ${OBJS} $@.reloc ${CRT_OBJS} ${LDFLAGS} -o $@
	${RM} $@.tmp $@.syms $@.reloc

${TARGET}.syms: ${TARGET}.bin
	${OBJDUMP} -t $? > $@

ifeq (${USE_FUNC_SIGNATURE},1)
${TARGET},ffc: ${TARGET}.bin ${TARGET}.syms
	${OBJCOPY} -O binary -j .text -j .rodata -j .data.rel.ro -j .data -j .data.rel -j .bss ${TARGET}.bin $@
	python ${ROSYMBOLS} ${TARGET}.syms $@
else
${TARGET},ffc: ${TARGET}.bin
	${OBJCOPY} -O binary -j .text -j .rodata -j .data.rel.ro -j .data -j .data.rel -j .bss ${TARGET}.bin $@
endif

endif
