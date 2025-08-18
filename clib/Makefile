# Makefile for building CLib library for aarch64
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

CROSS_ROOT = ${shell echo $$CROSS_ROOT}

ALL_TARGETS = libcrt.a

DEFAULT_GOAL = libcrt.a


targetted: export_headers VersionNum
	make ${DEFAULT_GOAL}

.PHONY: export

export: targetted
	rm -rf export
	mkdir export
	cp -R libcrt.a export/
	cp -R riscos_headers/* export/
	mv export/Lib/CLib/* export/
	rmdir export/Lib/CLib
	rm export/_marker
	cp -R bin export/
	cp -R asm export/Asm
	cp -R linker export/linker

# NOTE: Not used yet
all: export_headers
	for i in ${ALL_TARGETS} ; do make $$i,ff8 TARGET=$$i || exit $$? ; done

export_headers: local_headers \
				riscos_headers/Lib/CLib/swis.h \
				riscos_headers/Lib/CLib/kernel.h \
				riscos_headers/Lib/CLib/swi_numbers.h \
				riscos_headers/Lib/CLib/swis_os.h \
				riscos_headers/Lib/CLib/_heap_implementation.h

riscos_headers/Lib/CLib/%.h: %.h | local_headers
	cp $? $@
riscos_headers/Lib/CLib/%.h: kernel/%.h | local_headers
	cp $? $@
riscos_headers/Lib/CLib/%.h: heap/%.h | local_headers
	cp $? $@

local_headers: riscos_headers/_marker
riscos_headers/_marker:
	mkdir -p riscos_headers/C/riscos
	mkdir -p riscos_headers/Lib/CLib
ifeq (${CEXPORT_DIR},)
	echo No local headers defined in CEXPORT_DIR
else
	for hdr in ${CEXPORT_DIR}/h/* ; do cp "$$hdr" "riscos_headers/C/$$(basename "$$hdr").h" ; done
	for hdr in ${CEXPORT_DIR}/riscos/h/* ; do cp "$$hdr" "riscos_headers/C/riscos/$$(basename "$$hdr").h" ; done
	for hdrdir in ${LIB_DIR}/* ; do mkdir -p "riscos_headers/Lib/$$(basename "$$hdrdir")" ; for hdr in "$$hdrdir/h"/* ; do cp "$$hdr" "riscos_headers/Lib/$$(basename "$$hdrdir")/$$(basename "$$hdr").h" ; done ; done
	# For now, we don't want to export all the old RISC OS header files from Norcroft
	-rmdir riscos_headers/Lib/*
	rm riscos_headers/Lib/CLib/*
endif
	touch riscos_headers/_marker


shell: dockcross-linux-arm64
	./dockcross-linux-arm64 bash

dockcross-linux-arm64:
	docker run --rm dockcross/linux-arm64:latest > dockcross-linux-arm64
	chmod +x dockcross-linux-arm64

clean:
	-rm -rf *.o *.a *.bin *,ff8 *.map riscos_headers/ export/
	-rm -rf builtins/*.o \
			heap/*.o \
			io/*.o \
			fs/*.o \
			errno/*.o \
			kernel/*.o \
			conversion/*.o \
			string/*.o \
			time/*.o \
			sysvars/*.o \
			ctypes/*.o \
			maths/*.o \
			random/*.o \
			VersionNum

ifeq (${CROSS_ROOT},)
# If we're outside the docker container, re-run ourselves inside the container
ifneq ($(filter-out all shell dockcross-linux-arm64 clean export,${MAKECMDGOALS}),)
# The command wasn't one of our invocation commands above
.PHONY: ${MAKECMDGOALS}
${MAKECMDGOALS}: dockcross-linux-arm64 VersionNum
	./dockcross-linux-arm64 make ${MAKECMDGOALS}
else
.PHONY: ${DEFAULT_GOAL}
${DEFAULT_GOAL}: dockcross-linux-arm64 VersionNum
	./dockcross-linux-arm64 make ${DEFAULT_GOAL}

VersionNum: ../VersionNum
	cp $? $@

endif

else
# We are inside the docker container

USE_FUNC_SIGNATURE ?= 1

# Remove the flags that might make code think it's compiling for linux system.
CFLAGS = -U__linux -U__linux__ -U__unix__ -U__unix -Ulinux -Uunix -U__gnu_linux__

# Add the definitions to indicate that we're compiling for RISC OS
CFLAGS += -D__riscos -D__riscos64

# Allow us to build without assuming the standard library is present
CFLAGS += -nostdlib -ffreestanding -march=armv8-a
#CFLAGS += -nostdlib -ffreestanding -march=armv8-a+nofp

# Add the exports directory to those things we'll build with
CFLAGS += -Iriscos_headers/C/ -Iriscos_headers/Lib/ -Iriscos_headers/Lib/CLib/ -I.

# Options to allow function signatures to appear RISC OS-like
ifeq (${USE_FUNC_SIGNATURE},1)
CFLAGS += -fpatchable-function-entry=8,8
endif

# Position independant code allows relocation
CFLAGS += -fPIC

# Optimisation options
CFLAGS += -O1

# Options for this build
CFLAGS += -DARCH_HAS_DIVISION
CFLAGS += -DCONSTRUCTORS

# Assembler flags
AFLAGS = -march=armv8-a

# Compiler builtins
BUILTIN_OBJS = \
		builtins/fp_mode.o \
		builtins/addtf3.o \
		builtins/subtf3.o \
		builtins/multf3.o \
		builtins/divtf3.o \
		builtins/fixtfdi.o \
		builtins/fixunsitf.o \
		builtins/fixunstfdi.o \
		builtins/fixunditf.o \
		builtins/comparetf2.o \
		builtins/extenddftf2.o \
		builtins/extendsftf2.o \
		builtins/floatditf.o \
		builtins/trunctfdf2.o \
		builtins/trunctfsf2.o
CFLAGS += -Ibuiltins/

# Maths libraries
MATHS_OBJS = \
		maths/e_acos.o \
		maths/e_acosf.o \
		maths/e_asin.o \
		maths/e_asinf.o \
		maths/e_atan2.o \
		maths/e_atan2f.o \
		maths/e_atan2l.o \
		maths/e_exp.o \
		maths/e_expf.o \
		maths/e_log.o \
		maths/e_logf.o \
		maths/e_fmod.o \
		maths/e_fmodf.o \
		maths/e_fmodl.o \
		maths/e_pow.o \
		maths/e_powf.o \
		maths/e_rem_pio2.o \
		maths/e_sqrt.o \
		maths/e_sqrtf.o \
		\
		maths/k_rem_pio2.o \
		maths/k_cos.o \
		maths/k_cosf.o \
		maths/k_sin.o \
		maths/k_sinf.o \
		maths/k_tan.o \
		maths/k_tanf.o \
		\
		maths/s_atan.o \
		maths/s_atanf.o \
		maths/s_ceil.o \
		maths/s_ceill.o \
		maths/s_ceilf.o \
		maths/s_copysign.o \
		maths/s_copysignl.o \
		maths/s_copysignf.o \
		maths/s_cos.o \
		maths/s_cosf.o \
		maths/s_fabs.o \
		maths/s_fabsf.o \
		maths/s_floor.o \
		maths/s_floorf.o \
		maths/s_floorl.o \
		maths/s_modf.o \
		maths/s_modff.o \
		maths/s_modfl.o \
		maths/s_nextafter.o \
		maths/s_nextafterl.o \
		maths/s_nextafterf.o \
		maths/s_sin.o \
		maths/s_sinf.o \
		maths/s_tan.o \
		maths/s_tanf.o \
		maths/s_trunc.o \
		maths/s_truncf.o \
		maths/s_truncl.o \
		maths/s_scalbn.o \
		maths/s_scalbnl.o \

CFLAGS += -Imaths/include/


CC = aarch64-unknown-linux-gnu-gcc
AS = aarch64-unknown-linux-gnu-as
LD = aarch64-unknown-linux-gnu-ld
AR = aarch64-unknown-linux-gnu-ar
OBJCOPY = aarch64-unknown-linux-gnu-objcopy

OBJS = 	\
		assert.o \
		abort.o \
		\
		ctypes/flags.o \
		ctypes/lower.o \
		ctypes/upper.o \
		ctypes/is.o \
		\
		errno/errno.o \
		errno/strerror.o \
		errno/perror.o \
		\
		heap/accumulator.o \
		heap/osheap.o \
		heap/o1heap.o \
		heap/o1heap-shim.o \
		heap/init.o \
		\
		io/io-constants.o \
		io/io-console.o \
		io/io-consoleio.o \
		io/io-init.o \
		io/io-file-misc.o \
		io/io-file-fopen.o \
		io/io-file-fopen-lesser.o \
		io/io-file-read.o \
		io/io-file-write.o \
		io/io-file-seek.o \
		io/io-file-seek-lesser.o \
		io/io-fgets.o \
		io/io-fprintf.o \
		io/io-printf.o \
		io/io-sprintf.o \
		io/io-vprintf.o \
		io/sscanf.o \
		io/stackcheck.o \
		io/io-memstream.o \
		\
		fs/fs-errors.o \
		fs/fs-remove.o \
		fs/fs-rename.o \
		fs/fs-tmpfile.o \
		fs/fs-tmpnam.o \
		fs/fs-io.o \
		\
		kernel/kernel-backtrace.o \
		kernel/kernel-escape.o \
		kernel/kernel-state.o \
		kernel/kernel-cli.o \
		kernel/kernel-system.o \
		kernel/kernel-unwind-current.o \
		kernel/kernel-unwind.o \
		kernel/kernel-swi.o \
		kernel/kernel-swix.o \
		kernel/kernel-hostos.o \
		kernel/kernel-fpavailable.o \
		kernel/kernel-osbyte.o \
		kernel/kernel-osrdch.o \
		kernel/kernel-oswrch.o \
		kernel/kernel-osgbpb.o \
		kernel/kernel-osargs.o \
		kernel/kernel-osword.o \
		kernel/kernel-osfind.o \
		\
		atexit.o \
		clib.o \
		constructors.o \
		longjmp.o \
		env.o \
		signal.o \
		main.o \
		module.o \
		malloc.o \
		\
		qsort.o \
		bsearch.o \
		\
		string/memchr.o \
		string/memcmp.o \
		string/memcpy.o \
		string/memmove.o \
		string/memset.o \
		string/strcat.o \
		string/strchr.o \
		string/strcmp.o \
		string/strcpy.o \
		string/strcspn.o \
		string/strdup.o \
		string/strlen.o \
		string/strncat.o \
		string/strncmp.o \
		string/strncpy.o \
		string/strrchr.o \
		string/strpbrk.o \
		string/strspn.o \
		string/strstr.o \
		string/strtok.o \
		string/strcase.o \
		\
		conversion/atoi.o \
		conversion/atol.o \
		conversion/strtod.o \
		conversion/strtold.o \
		conversion/strtol.o \
		conversion/strtoll.o \
		conversion/strtoul.o \
		conversion/strtoull.o \
		\
		conversion/cvt_decimal.o \
		conversion/cvt_hex.o \
		\
		time/time-clock.o \
		time/time-time.o \
		time/time-gmtime.o \
		time/time-localtime.o \
		time/strftime.o \
		time/asctime.o \
		\
		sysvars/sysvars.o \
		sysvars/sysvars-kernel.o \
		sysvars/sysvars-init.o \
		\
		version.o \
		\
		abs.o \
		labs.o \
		\
		${MATHS_OBJS} \
		\
		random/rand.o \
		random/random.o \
		\
		swis-extras.o \
		swis-kernel.o \
		swis-module.o \
		swis-heap.o \
		swis.o \
		start.o \
		start-exit.o \
		\
		${BUILTIN_OBJS}

#		trunc.o \
#		fabs.o \


libcrt.a: ${OBJS}
	${AR} -rc $@ ${OBJS}

%.o: %.c
	${CC} ${CFLAGS} -c -o $@ $?

%.o: %.s
	${AS} ${AFLAGS} -o $@ $?

version.o: | VersionNum

endif
