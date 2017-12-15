#ifndef HARPY_H
#define HARPY_H

#include "mpc.h"

mpc_parser_t* Lispy;

/* LVAL_INT is an integer type, LVAL_NUM is a floating point type */
enum type_lval { LVAL_ERR, // Error
		 LVAL_INT, // Integer
		 LVAL_NUM, // Double-precision floating point
		 LVAL_SYM, // Symbol, holds variables
		 LVAL_STR, // String
		 LVAL_FUN, // Function
		 LVAL_SEXPR, // Code-snippet
		 LVAL_QEXPR, // Data-bundle
		 LVAL_OBJ // contains raw memory or system objects
};

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
