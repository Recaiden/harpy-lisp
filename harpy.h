#ifndef HARPY_H
#define HARPY_H

#include "mpc.h"

mpc_parser_t* Lispy;

/* LVAL_INT is an integer type, LVAL_NUM is a flaoting point type */
enum type_lval { LVAL_ERR, LVAL_INT, LVAL_NUM, LVAL_SYM, LVAL_STR, LVAL_FUN, LVAL_SEXPR, LVAL_QEXPR };

struct lval;
struct lenv;
typedef struct lval lval;
typedef struct lenv lenv;

#define LASSERT(args, cond, fmt, ...)		\
  if (!(cond))					\
  {						\
    lval* err = lval_err(fmt, ##__VA_ARGS__);	\
    lval_del(args);				\
    return err;					\
  }

#define LASSERT_TYPE(func, args, index, expect) \
  LASSERT(args, args->cell[index]->type == expect, \
    "Function '%s' passed incorrect type for argument %i. " \
    "Got %s, Expected %s.", \
    func, index, ltype_name(args->cell[index]->type), ltype_name(expect))

#define LASSERT_NUM(func, args, num) \
  LASSERT(args, args->count == num, \
    "Function '%s' passed incorrect number of arguments. " \
    "Got %i, Expected %i.", \
    func, args->count, num)

#define LASSERT_NOT_EMPTY(func, args, index) \
  LASSERT(args, args->cell[index]->count != 0, \
    "Function '%s' passed {} for argument %i.", func, index);

#endif
