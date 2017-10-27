#ifndef HARPY_STDIO_H
#define HARPY_STDIO_H

#include "mpc.h"

#include "harpy.h"
#include "builtin.h"
#include "lval.h"
#include "lenv.h"

lval* builtin_fclose(lenv* e, lval* a);
lval* builtin_fopen(lenv* e, lval* a);
lval* builtin_fread(lenv* e, lval* a);
lval* builtin_fwrite(lenv* e, lval* a);
lval* builtin_fflush(lenv* e, lval* a);

void lenv_add_stdio(lenv* e);


#endif
