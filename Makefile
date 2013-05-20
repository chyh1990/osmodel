CILLY=cilly
OBJS=fs.o

all: $(OBJS)

%.o: %.c
	$(CILLY) -c --save-temps $^

clean:
	rm -f *.o *.i *.cil.c
