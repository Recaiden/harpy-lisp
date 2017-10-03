#ifndef LVAL_H
#define LVAL_H

#include "mpc.h"

#include "harpy.h"
#include "builtin.h"
#include "lval.h"

struct lval
{
  int type;

  // Basic Types
  double num;
  char* err;
  char* sym;
  char* str;

  // Functions
  lbuiltin builtin;
  lenv* env;
  lval* formals;
  lval* body;

  // Expression
  int count;
  lval** cell;
};


int lval_eq(lval* x, lval* y);

//============================================================================//
//                  S U B T Y P E   C O N S T R U C T O R S                   //
//============================================================================//
lval* lval_fun(lbuiltin func, char* name);
lval* lval_str(char* st);
lval* lval_num(double x);
lval* lval_err(char* fmt, ...);
lval* lval_sym(char* s);
lval* lval_sexpr(void);
lval* lval_lambda(lval* formals, lval* body);
lval* lval_qexpr(void);

//============================================================================//
//                             L V A L   M E T H O D S                        //
//============================================================================//

void lval_del(lval* v);
lval* lval_add(lval* v, lval* x);
lval* lval_pop(lval* v, int i) ;
lval* lval_join(lval* x, lval* y);
lval* lval_take(lval* v, int i);
void lval_expr_print(lval* v, char open, char close);
void lval_print_str(lval* v);
void lval_print(lval* v);
lval* lval_copy(lval* v);
void lval_println(lval* v);
lval* lval_call(lenv* e, lval* f, lval* a);
char* ltype_name(int t);
lval* lval_eval_sexpr(lenv* e, lval* v) ;
lval* lval_eval(lenv* e, lval* v);
lval* lval_read_num(mpc_ast_t* t);
lval* lval_read_str(mpc_ast_t* t);
lval* lval_read(mpc_ast_t* t);

#endif
