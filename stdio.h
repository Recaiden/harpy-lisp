#ifndef STDIO_H
#define STDIO_H

#include "mpc.h"

#include "harpy.h"
#include "builtin.h"
#include "lval.h"

lval* builtin_fclose(lenv* e, lval* a);
lval* builtin_fopen(lenv* e, lval* a);
lval* builtin_fread(lenv* e, lval* a);
lval* builtin_fwrite(lenv* e, lval* a);
lval* builtin_fflush(lenv* e, lval* a);

void lenv_add_stdio(lenv* e);


#endif
