CILLY=cilly
KLEEPATH=/home/user/prog/s2e/klee-cde-package
#KLEEPATH=/home/chenyh/os/klee-cde-package
LLVMGCC=llvm-gcc.cde
KLEE=klee.cde
CFLAGS=-I$(KLEEPATH)/cde-root/home/pgbovine/klee/include/ --emit-llvm -c -g
OBJS=fs.o counter.o testout/test_sys_mmap___sys_munmap.o
HEADERS=

all: $(OBJS)

%.o: %.c $(HEADERS)
	$(LLVMGCC) $(CFLAGS) $^

run:
	$(KLEE) fs.o

clean:
	rm -f *.o *.i *.cil.c
	rm -rf klee-out-*
