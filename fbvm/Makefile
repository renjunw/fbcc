OBJS = fbvm.o
CFLAGS = -g -Wall -O2
LIBS =
LFLAGS	= 

all: fbvm

clean:
	rm -f *.o *~ fbvm fbvmspec.h

fbvm: $(OBJS)
	riscv32-unknown-linux-gnu-gcc -static -g $(OBJS) $(LFLAGS) -o fbvm $(LIBS)

.c.o:
	riscv32-unknown-linux-gnu-gcc -static $(CFLAGS) -c $*.c

