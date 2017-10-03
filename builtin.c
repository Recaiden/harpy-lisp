#include <stdlib.h>

#include "mpc.h"

#include "builtin.h"
#include "lval.h"
#include "lenv.h"

lval* builtin_load(lenv* e, lval* a)
{
  LASSERT_NUM("load", a, 1);
  LASSERT_TYPE("load", a, 0, LVAL_STR);

  /* Parse File given by string name */
  mpc_result_t r;
  if (mpc_parse_contents(a->cell[0]->str, Lispy, &r))
  {
    /* Read contents */
    lval* expr = lval_read(r.output);
    mpc_ast_delete(r.output);

    /* Evaluate each Expression */
    while (expr->count) {
      lval* x = lval_eval(e, lval_pop(expr, 0));
      /* If Evaluation leads to error print it */
      if (x->type == LVAL_ERR) { lval_println(x); }
      lval_del(x);
    }

    /* Delete expressions and arguments */
    lval_del(expr);
    lval_del(a);

    /* Return empty list */
    return lval_sexpr();

  } else {
    /* Get Parse Error as String */
    char* err_msg = mpc_err_string(r.error);
    mpc_err_delete(r.error);

    /* Create new error message using it */
    lval* err = lval_err("Could not load Library %s", err_msg);
    free(err_msg);
    lval_del(a);

    /* Cleanup and return error */
    return err;
  }
}

lval* builtin_read(lenv* e, lval* a)
{
  LASSERT_NUM("read", a, 1);
  LASSERT_TYPE("read", a, 0, LVAL_STR);

  /* Parse given string */
  mpc_result_t rrd;
  
  if(mpc_parse("<read>", a->cell[0]->str, Lispy, &rrd))
  {
    lval* x = lval_eval(e, lval_read(rrd.output));
    lval_println(x);
    lval_del(x);
    
    lval_del(a);
    
    mpc_ast_delete(rrd.output);
    
    /* Return empty list */
    return lval_sexpr();
  }
  else
  {
    /* Return error */
    char* err_msg = mpc_err_string(rrd.error);
    mpc_err_delete(rrd.error);
    mpc_err_print(rrd.error);

    lval* err = lval_err("Could not load Library %s", err_msg);
    free(err_msg);
    lval_del(a);
    
    return err;
  }
}

lval* builtin_lambda(lenv* e, lval* a) {
  /* Check Two arguments, each of which are Q-Expressions */
  LASSERT_NUM("\\", a, 2);
  LASSERT_TYPE("\\", a, 0, LVAL_QEXPR);
  LASSERT_TYPE("\\", a, 1, LVAL_QEXPR);

  /* Check first Q-Expression contains only Symbols */
  for (int i = 0; i < a->cell[0]->count; i++) {
    LASSERT(a, (a->cell[0]->cell[i]->type == LVAL_SYM),
      "Cannot define non-symbol. Got %s, Expected %s.",
      ltype_name(a->cell[0]->cell[i]->type),ltype_name(LVAL_SYM));
  }

  /* Pop first two arguments and pass them to lval_lambda */
  lval* formals = lval_pop(a, 0);
  lval* body = lval_pop(a, 0);
  lval_del(a);

  return lval_lambda(formals, body);
}

lval* builtin_list(lenv* e, lval* a)
{
  a->type = LVAL_QEXPR;
  return a;
}

lval* builtin_head(lenv* e, lval* a)
{
  LASSERT(a, a->count == 1,
	  "Function 'head' passed too many arguments. "
	  "Got %i, Expected %i.",
	  a->count, 1);
  LASSERT(a, a->cell[0]->type == LVAL_QEXPR || a->cell[0]->type == LVAL_STR,
	  "Function 'head' passed incorrect type for argument 0. "
	  "Got %s, Expected %s or %s.",
	  ltype_name(a->cell[0]->type), ltype_name(LVAL_QEXPR), ltype_name(LVAL_STR));

  lval* v;
  if(a->cell[0]->type == LVAL_STR)
  {
    if(strlen(a->cell[0]->str) == 0)
      v = lval_str("\0");
    else
    {
      a->cell[0]->str[1] = 0;
      v = lval_str(a->cell[0]->str);
    }
    lval_del(a);
  }
  else
  {
    LASSERT(a, a->cell[0]->count != 0, "Function 'head' passed {}.");
    v = lval_take(a, 0);  
    while (v->count > 1) { lval_del(lval_pop(v, 1)); }
  }
  return v;
}

lval* builtin_tail(lenv* e, lval* a)
{
  LASSERT(a, a->count == 1,
	  "Function 'tail' passed too many arguments. "
	  "Got %i, Expected %i.",
	  a->count, 1);
  LASSERT(a, a->cell[0]->type == LVAL_QEXPR || a->cell[0]->type == LVAL_STR,
	  "Function 'tail' passed incorrect type for argument 0. "
	  "Got %s, Expected %s or %s.",
	  ltype_name(a->cell[0]->type), ltype_name(LVAL_QEXPR), ltype_name(LVAL_STR));

  lval* v;
  if(a->cell[0]->type == LVAL_STR)
  {
    if(strlen(a->cell[0]->str) == 0)
      v = lval_str("\0");
    else
      v = lval_str(a->cell[0]->str+1);
    lval_del(a);
  }
  else
  {
    LASSERT(a, a->cell[0]->count != 0, "Function 'tail' passed {}.");
    
    v = lval_take(a, 0);  
    lval_del(lval_pop(v, 0));
  }
  return v;
}

lval* builtin_eval(lenv* e, lval* a)
{
  LASSERT(a, a->count == 1,
    "Function 'eval' passed too many arguments.");
  LASSERT(a, a->cell[0]->type == LVAL_QEXPR,
    "Function 'eval' passed incorrect type.");
  
  lval* x = lval_take(a, 0);
  x->type = LVAL_SEXPR;
  return lval_eval(e, x);
}

lval* builtin_join(lenv* e, lval* a)
{
  for (int i = 0; i < a->count; i++)
  {
    LASSERT(a, a->cell[i]->type == LVAL_QEXPR || a->cell[i]->type == LVAL_STR,
	    "Function 'join' passed incorrect type: %s", ltype_name(a->cell[i]->type));
    LASSERT(a, a->cell[i]->type == a->cell[0]->type,
	    "Function 'join' passed conflicting types: %s & %s",
	    ltype_name(a->cell[0]->type), ltype_name(a->cell[i]->type) );
  }

  lval* x;
  if(a->cell[0]->type == LVAL_STR)
  {
    int bufferSize = 0;
    for (int i = 0; i < a->count; i++)
    {
      bufferSize += strlen(a->cell[i]->str);
    }
    char* buffer = malloc(bufferSize+1);
    memset(buffer, 0, bufferSize+1);
    for (int i = 0; i < a->count; i++)
    {
      strcat(buffer,  a->cell[i]->str);
    }
    x = lval_str(buffer);
    free(buffer);
  }
  else
  {
    x = lval_pop(a, 0);
    while (a->count)
    {
      x = lval_join(x, lval_pop(a, 0));
    }
  }
  
  lval_del(a);
  return x;
}

lval* builtin_int_op(lenv* e, lval* a, char* op)
{
  lval* x = lval_pop(a, 0);
  if ((strcmp(op, "-") == 0) && a->count == 0) { x->integer = -x->integer; }
  
  while (a->count > 0)
  {
    lval* y = lval_pop(a, 0);
    
    if (strcmp(op, "+") == 0) { x->integer += y->integer; }
    if (strcmp(op, "-") == 0) { x->integer -= y->integer; }
    if (strcmp(op, "*") == 0) { x->integer *= y->integer; }
    if (strcmp(op, "/") == 0)
    {
      if (y->integer == 0)
      {
        lval_del(x); lval_del(y);
        x = lval_err("Division By Zero.");
        break;
      }
      x->integer /= y->integer;
    }
     if (strcmp(op, "%") == 0)
     {
      if (y->integer == 0)
      {
        lval_del(x); lval_del(y);
        x = lval_err("Division By Zero.");
        break;
      }
      x->integer = x->integer % y->integer;
    }
    
    lval_del(y);
  }
  
  lval_del(a);
  return x;
}

lval* builtin_op(lenv* e, lval* a, char* op)
{
  int fp = 0;
  /* Make sure all types are good, set floating point conversion flag if applicable */
  for (int i = 0; i < a->count; i++)
  {
    if(a->cell[i]->type == LVAL_NUM)
    {
      fp = 1;
      continue;
    }
    if(a->cell[i]->type == LVAL_INT)
    {
      continue;
    }
      
    lval_del(a);
    return lval_err("Cannot operate on non-number!");
  }

  /* If only int params, operate on the int field, otherwise it becomes float */
  if(!fp)
    return builtin_int_op(e, a, op);
  
  lval* x = lval_pop(a, 0);
  if(x->type == LVAL_INT)
  {
    x->num = (double)(x->integer);
    x->type = LVAL_NUM;
  }
  if ((strcmp(op, "-") == 0) && a->count == 0) { x->num = -x->num; }
  
  while (a->count > 0)
  {
    lval* y = lval_pop(a, 0);
    if(y->type == LVAL_INT)
      y->num = (double)(y->integer);
    
    if (strcmp(op, "+") == 0) { x->num += y->num; }
    if (strcmp(op, "-") == 0) { x->num -= y->num; }
    if (strcmp(op, "*") == 0) { x->num *= y->num; }
    if (strcmp(op, "/") == 0)
    {  
      if (y->num == 0)
      {
        lval_del(x); lval_del(y);
        x = lval_err("Division By Zero.");
        break;
      }
      x->num /= y->num;
    }
     if (strcmp(op, "%") == 0)
     {
      if (y->num == 0)
      {
        lval_del(x); lval_del(y);
        x = lval_err("Division By Zero.");
        break;
      }
      x->num = fmod(x->num, y->num);
    }
    
    lval_del(y);
  }   
  
  lval_del(a);
  return x;
}

lval* builtin_add(lenv* e, lval* a)
{ return builtin_op(e, a, "+"); }

lval* builtin_sub(lenv* e, lval* a)
{ return builtin_op(e, a, "-"); }

lval* builtin_mul(lenv* e, lval* a)
{ return builtin_op(e, a, "*"); }

lval* builtin_div(lenv* e, lval* a)
{ return builtin_op(e, a, "/"); }

lval* builtin_mod(lenv* e, lval* a)
{ return builtin_op(e, a, "%"); }

lval* builtin_comp(lenv* e, lval* a, char* op)
{
  LASSERT_NUM(op, a, 2);
  LASSERT_TYPE(op, a, 0, LVAL_NUM);
  LASSERT_TYPE(op, a, 1, LVAL_NUM);

  int r;

  if (strcmp(op, "<=") == 0) { r = (a->cell[0]->num <= a->cell[1]->num); }
  if (strcmp(op, ">=") == 0) { r = (a->cell[0]->num >= a->cell[1]->num); }
  if (strcmp(op, "<") == 0)  { r = (a->cell[0]->num <  a->cell[1]->num); }
  if (strcmp(op, ">") == 0)  { r = (a->cell[0]->num >  a->cell[1]->num); }
  if (strcmp(op, "==") == 0) { r = (a->cell[0]->num == a->cell[1]->num); }
  if (strcmp(op, "!=") == 0) { r = (a->cell[0]->num != a->cell[1]->num); }
  
  lval_del(a);
  return lval_num(r);
}

lval* builtin_lt(lenv* e, lval* a)
{ return builtin_comp(e, a, "<"); }

lval* builtin_gt(lenv* e, lval* a)
{ return builtin_comp(e, a, ">"); }

lval* builtin_le(lenv* e, lval* a)
{ return builtin_comp(e, a, "<="); }

lval* builtin_ge(lenv* e, lval* a)
{ return builtin_comp(e, a, ">="); }

lval* builtin_cmp(lenv* e, lval* a, char* op) {
  LASSERT_NUM(op, a, 2);
  int r;
  if (strcmp(op, "==") == 0) {
    r =  lval_eq(a->cell[0], a->cell[1]);
  }
  if (strcmp(op, "!=") == 0) {
    r = !lval_eq(a->cell[0], a->cell[1]);
  }
  lval_del(a);
  return lval_num(r);
}

lval* builtin_eq(lenv* e, lval* a) {
  return builtin_cmp(e, a, "==");
}

lval* builtin_ne(lenv* e, lval* a) {
  return builtin_cmp(e, a, "!=");
}

lval* builtin_if(lenv* e, lval* a)
{
  LASSERT_NUM("if", a, 3);
  LASSERT_TYPE("if", a, 0, LVAL_NUM);
  LASSERT_TYPE("if", a, 1, LVAL_QEXPR);
  LASSERT_TYPE("if", a, 2, LVAL_QEXPR);

  /* Mark Both Expressions as evaluable */
  lval* x;
  a->cell[1]->type = LVAL_SEXPR;
  a->cell[2]->type = LVAL_SEXPR;

  x = lval_eval(e, lval_pop(a, a->cell[0]->num ? 1 : 2));

  /* Delete argument list and return */
  lval_del(a);
  return x;
}

lval* builtin_var(lenv* e, lval* a, char* func)
{
  LASSERT(a, a->cell[0]->type == LVAL_QEXPR,
	  "Function 'def' passed incorrect type!");
  
  /* First argument is symbol list */
  lval* syms = a->cell[0];
  
  /* Ensure all elements of first list are symbols */
  for (int i = 0; i < syms->count; i++)
  {
    LASSERT(a, syms->cell[i]->type == LVAL_SYM,
	    "'%s' cannot define non-symbol", func);

  /* Disallow redefinition of builtins */
    LASSERT(a, strstr("+-*/%<>!", syms->cell[i]->sym) == NULL,
	    "'%s' cannot redefine arithmetic operator %s", func, syms->cell[i]->sym);
    
    LASSERT(a, strcmp(syms->cell[i]->sym, "def") != 0,
	    "'%s' cannot redefine itself", func);
    LASSERT(a, strcmp(syms->cell[i]->sym, "=") != 0,
	    "'%s' cannot redefine itself", func);

    LASSERT(a, strcmp(syms->cell[i]->sym, "tail") != 0,
	    "'%s' cannot redefine builtin functions", func);
    LASSERT(a, strcmp(syms->cell[i]->sym, "head") != 0,
	    "'%s' cannot redefine builtin functions", func);
    LASSERT(a, strcmp(syms->cell[i]->sym, "list") != 0,
	    "'%s' cannot redefine builtin functions", func);
    LASSERT(a, strcmp(syms->cell[i]->sym, "eval") != 0,
	    "'%s' cannot redefine builtin functions", func);
    LASSERT(a, strcmp(syms->cell[i]->sym, "join") != 0,
	    "'%s' cannot redefine builtin functions", func);
    LASSERT(a, strcmp(syms->cell[i]->sym, "if") != 0,
	    "'%s' cannot redefine builtin functions", func);
    LASSERT(a, strcmp(syms->cell[i]->sym, "\\") != 0,
	    "'%s' cannot redefine builtin functions", func);
    LASSERT(a, strcmp(syms->cell[i]->sym, "lambda") != 0,
	    "'%s' cannot redefine builtin functions", func);
    LASSERT(a, strcmp(syms->cell[i]->sym, "load") != 0,
	    "'%s' cannot redefine builtin functions", func);
    LASSERT(a, strcmp(syms->cell[i]->sym, "read") != 0,
	    "'%s' cannot redefine builtin functions", func);
    LASSERT(a, strcmp(syms->cell[i]->sym, "error") != 0,
	    "'%s' cannot redefine builtin functions", func);
    LASSERT(a, strcmp(syms->cell[i]->sym, "print") != 0,
	    "'%s' cannot redefine builtin functions", func);
    LASSERT(a, strcmp(syms->cell[i]->sym, "exit") != 0,
	    "'%s' cannot redefine builtin functions", func);
    LASSERT(a, strcmp(syms->cell[i]->sym, "quit") != 0,
	    "'%s' cannot redefine builtin functions", func);
    
  }

   LASSERT(a, (syms->count == a->count-1),
    "Function '%s' passed too many arguments for symbols. "
    "Got %i, Expected %i.", func, syms->count, a->count-1);

  /* Assign copies of values to symbols */
  for (int i = 0; i < syms->count; i++)
  {
    /* If 'def' define in globally. If 'put' define in locally */
    if (strcmp(func, "def") == 0)
    {
      lenv_def(e, syms->cell[i], a->cell[i+1]);
    }

    if (strcmp(func, "=")   == 0)
    {
      lenv_put(e, syms->cell[i], a->cell[i+1]);
    }
  }

  lval_del(a);
  return lval_sexpr();
}

lval* builtin_def(lenv* e, lval* a)
{
  return builtin_var(e, a, "def");
}

lval* builtin_put(lenv* e, lval* a)
{
  return builtin_var(e, a, "=");
}

lval* builtin_print(lenv* e, lval* a) {

  /* Print each argument followed by a space */
  for (int i = 0; i < a->count; i++) {
    lval_print(a->cell[i]); putchar(' ');
  }

  /* Print a newline and delete arguments */
  putchar('\n');
  lval_del(a);

  return lval_sexpr();
}

lval* builtin_error(lenv* e, lval* a) {
  LASSERT_NUM("error", a, 1);
  LASSERT_TYPE("error", a, 0, LVAL_STR);

  /* Construct Error from first argument */
  lval* err = lval_err(a->cell[0]->str);

  /* Delete arguments and return */
  lval_del(a);
  return err;
}

lval* builtin_exit(lenv* e, lval* a)
{
  lenv_del(e);
  lval_del(a);

  exit(0);
}

void lenv_add_builtin(lenv* e, char* name, lbuiltin func)
{
  lval* k = lval_sym(name);
  lval* v = lval_fun(func, name);
  lenv_put(e, k, v);
  lval_del(k); lval_del(v);
}

void lenv_add_builtins(lenv* e)
{
  /* List Functions */
  lenv_add_builtin(e, "list", builtin_list);
  lenv_add_builtin(e, "head", builtin_head);
  lenv_add_builtin(e, "tail", builtin_tail);
  lenv_add_builtin(e, "eval", builtin_eval);
  lenv_add_builtin(e, "join", builtin_join);

  /* Mathematical Functions */
  lenv_add_builtin(e, "+", builtin_add);
  lenv_add_builtin(e, "-", builtin_sub);
  lenv_add_builtin(e, "*", builtin_mul);
  lenv_add_builtin(e, "/", builtin_div);
  lenv_add_builtin(e, "%", builtin_mod);

  /* Comparator Functions */
  lenv_add_builtin(e, "<=", builtin_le);
  lenv_add_builtin(e, ">=", builtin_ge);
  lenv_add_builtin(e, "==", builtin_eq);
  lenv_add_builtin(e, "<", builtin_lt);
  lenv_add_builtin(e, ">", builtin_gt);
  lenv_add_builtin(e, "!=", builtin_ne);

  lenv_add_builtin(e, "if", builtin_if);

  /* Variable Functions */
  lenv_add_builtin(e, "\\",  builtin_lambda);
  lenv_add_builtin(e, "lambda",  builtin_lambda);
  lenv_add_builtin(e, "def",  builtin_def);
  lenv_add_builtin(e, "=",  builtin_put);

  /* String Functions */
  lenv_add_builtin(e, "load",  builtin_load);
  lenv_add_builtin(e, "read",  builtin_read);
  lenv_add_builtin(e, "error", builtin_error);
  lenv_add_builtin(e, "print", builtin_print);

  lenv_add_builtin(e, "exit", builtin_exit);
  lenv_add_builtin(e, "quit", builtin_exit);
}
