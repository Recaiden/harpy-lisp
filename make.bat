gcc -o lenv.o -c lenv.c
gcc -o builtin.o -c builtin.c
gcc -o lval.o -c lval.c
gcc -o stdio.o -c stdio.c
gcc -g -std=c99 -Wall -o harpy pole-ish.c mpc.c builtin.o lval.o lenv.o stdio.o harpy.h

