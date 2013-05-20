CILLY=cilly
OBJS=fs.o
HEADERS=

all: $(OBJS)

%.o: %.c $(HEADERS)
	$(CILLY) -c --save-temps $^

clean:
	rm -f *.o *.i *.cil.c
