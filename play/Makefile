CILLY=cilly
KLEEPATH=/home/user/prog/s2e/klee-cde-package
LLVMGCC=$(KLEEPATH)/bin/llvm-gcc.cde
CFLAGS=-I$(KLEEPATH)/cde-root/home/pgbovine/klee/include/ --emit-llvm -c -g
OBJS=array.o
HEADERS=

all: $(OBJS)

%.o: %.c $(HEADERS)
	$(LLVMGCC) $(CFLAGS) $^

clean:
	rm -f *.o *.i *.cil.c
