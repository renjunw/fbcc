FBCC = ../fbcc/fbcc
FBAS = ../fbas/fbas
FBINCLUDE = .
FBLIBS = startup.ss fblib.s

all: fblib.s

#
# stdlib
#
fblib.s: fblib.c


clean:
	rm -f *.s *.i *~

.c.s:
	riscv32-unknown-linux-gnu-gcc -E -I$(FBINCLUDE) $*.c >$*.i
	$(FBCC) < $*.i > $*.s
