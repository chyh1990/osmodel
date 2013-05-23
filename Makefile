CILLY=cilly
#KLEEPATH=/home/user/prog/s2e/klee-cde-package
KLEEPATH=/home/chenyh/os/klee-cde-package
LLVMGCC=llvm-gcc.cde
KLEE=klee.cde
CFLAGS=-I../../include/ --emit-llvm -c -g
OBJS=fs.o counter.o
HEADERS=

all: $(OBJS)

%.o: %.c $(HEADERS)
	$(LLVMGCC) $(CFLAGS) $^

run:
	$(KLEE) fs.o

clean:
	rm -f *.o *.i *.cil.c
	rm -rf klee-out-*
