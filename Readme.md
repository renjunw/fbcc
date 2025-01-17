# FBCC: Fabrice Bellard's C Compiler

FBCC can running on qemu-riscv32

This is a modified copy of Fabrice Bellard's C compiler which is available
at [https://bellard.org/fbcc/](https://bellard.org/fbcc/). 

All I've done is fix it up so that it runs on Ubuntu 18.04 LTS.

The compiler no longer compiles itself because of the interesting
type definitions in the headers of current `flex` and/or `bison`.

I've also translated the French documentation into [English](doc/fbcc.md).


## Original README

```
This is the distribution of FBCC which stands for the "Fabrice Bellard's
C Compiler" :-)

I have programmed this C compiler as a little project the 'Majeure
Informatique' at the Ecole Polytechnique (in France). It is fairly limited
but is able to completly compile itself, which was my main goal.

It may be used as a didactic example to show how should be (or should not be
!) programmed a complete compiler. It is given 'as is'. If you want to
improve it, please send me the patches you made.

Overview :
--------

 * 'fbcc' takes a preprocessed Ansi C source as input and produces an
 assembler source for a stack based virtual machine.

 * 'fbas' assembles and links the assembler sources. The linking is done by
 simply concatening all the sources. It generates a binary in pseudo code
 for the virtual machine interpreter.

 * 'fbvm' executes the pseudo code. It contains a few predefined functions
 for the standard C libraries.
 
Files :
-----

 * fbcc/lib contains the source code for the standard C librairies. Some
 sources comes from the Linux kernel, and other from the GNU C library.

 * fbcc/doc contains the slides (in French) I made for the oral exam where I
 had to present the project.

 * fbcc/test2 contains some examples for testing fbcc.

 * 'fbMakefile's are used to bootstrap (i.e. to compile fbcc/fbas with
 itself). Then you can try to compile the compiled-by-itself FBCC and see
 that the two pseudo binaries are the same ! (You can use "make -f
 fbMakefile" to use directly this alternate Makefile - be careful to build
 before all libraries).


Portability :
-----------

* Although fbcc and fbas are in ANSI C, the pseudo code they generate is
 machine dependant. In particular, the pseudo code is endianess dependant.
 The main limitation up to now is that the compiler supposes that pointers
 and integers have the same size, which is usually false on the new 64 bits
 processors (e.g. the Digital Alpha).

 * I have given two examples of configuration files for Intel and Sparc
 platforms. You must make the right symbolic link to fbcc/fbvm/fbvmspec.h

 * FBCC does not implement all the ANSI C standard - It is only a student
 project ! You can check in fbcc/doc to see what is not implemented.

Changes :
-------

(26/10/2008) small fixes to make it compile on Recent Linux distributions.
 
Notes :
-----

 * Many comments in the C sources are in French... It was easier for me :)

 * I have used bison and flex - Maybe yacc and lex can work too.
 
 * Many optimisations should be done - the more obvious and easier should
 be constant propagation.

 * Be careful : when an error occurs, the incomplete .s file is not deleted.
 You should delete it before lauching make another time.

 * The examples in fbcc/test2 can be launched with the command 'vm
 file_name'.

 * Good luck !

Fabrice Bellard (http://bellard.org)
```
