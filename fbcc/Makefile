CFLAGS = -static -g -Wall -O2 
LIBS =
LFLAGS	= 

all: fbcc

clean:
	rm -f *.o *.i *.s *~ fbcc fbcc_lex.yy.c fbparse.c fbas.o

FBCC_OBJS = fblist.o fbgen.o fbtype.o fbconst.o fberror.o fbsym.o fbparse.o fileops.o fbas.o fbcc.o

fbcc: $(FBCC_OBJS)
	riscv32-unknown-linux-gnu-gcc -static $(FBCC_OBJS) $(LFLAGS) -o fbcc $(LIBS)

fbsym.o: fbsym.c fbcc.h

fblist.o: fblist.c fbcc.h

fbtype.o: fbtype.c fbcc.h

fbgen.o: fbgen.c fbcc.h

fbconst.o: fbconst.c fbcc.h

fbcc_lex.yy.c: fbcc_lex.l fbcc.h
	flex -o fbcc_lex.yy.c fbcc_lex.l

fbparse.c: fbcc_lex.yy.c fbparse.y fbcc.h
	bison -o fbparse.c fbparse.y

fileops.o: fileops.c compiler.h config.h 
	riscv32-unknown-linux-gnu-gcc -c $(CFLAGS) fileops.c -o fileops.o

.c.o:
	riscv32-unknown-linux-gnu-gcc $(CFLAGS) -c $*.c

fbas.o: fbas.c
	riscv32-unknown-linux-gnu-gcc -c $(CFLAGS) fbas.c -o fbas.o

fbas: fbas.c
	riscv32-unknown-linux-gnu-gcc $(CFLAGS) fbas.c -o fbas
