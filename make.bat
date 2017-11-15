gcc -g -o lenv.o -c -g lenv.c
gcc -g -o builtin.o -c -g builtin.c
gcc -g -o lval.o -c -g lval.c
gcc -g -o stdio.o -c -g stdio.c
gcc -g -std=c99 -Wall -o harpy pole-ish.c mpc.c builtin.o lval.o lenv.o stdio.o harpy.h

