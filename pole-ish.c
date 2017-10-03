
#include <stdio.h>
#include <stdlib.h>

#include "mpc.h"

#include "harpy.h"
#include "builtin.h"
#include "lenv.h"
#include "lval.h"

#ifdef _WIN32

static char buffer[2048];

// NOTE lval substructures for functions and expressions.
// TODO seperate int and floating-point types
// NOTE boolean types?
// TODO builtin logical operators

char* readline(char* prompt) {
  fputs(prompt, stdout);
  fgets(buffer, 2048, stdin);
  char* cpy = malloc(strlen(buffer)+1);
  strcpy(cpy, buffer);
  cpy[strlen(cpy)-1] = '\0';
  return cpy;
}

void add_history(char* unused) {}

#else
#include <editline/readline.h>
#include <editline/history.h>
#endif

int main(int argc, char** argv)
{
  mpc_parser_t* Comment  = mpc_new("comment");
  mpc_parser_t* String  = mpc_new("string");
  mpc_parser_t* Number = mpc_new("number");
  mpc_parser_t* Symbol = mpc_new("symbol");
  mpc_parser_t* Sexpr  = mpc_new("sexpr");
  mpc_parser_t* Qexpr  = mpc_new("qexpr");
  mpc_parser_t* Expr   = mpc_new("expr");
  Lispy  = mpc_new("lispy");
  
  mpca_lang(MPCA_LANG_DEFAULT,
    "    \
      comment : /;[^\\r\\n]*/ ;				 \
      number  : /-?([0-9]+\\.)?[0-9]+/ ;		 \
      string  : /\"(\\\\.|[^\"])*\"/ ;                   \
      symbol  : /[a-zA-Z0-9_+\%\\-*\\/\\\\=<>!&]+/ ;	 \
      sexpr   : '(' <expr>* ')' ;                        \
      qexpr   : '{' <expr>* '}' ;                        \
      expr    : <string> | <number> | <comment> | <symbol> | <sexpr> | <qexpr> ;	\
      lispy   : /^/ <expr>* /$/ ;                        \
    ",
	    Comment, Number, String, Symbol, Sexpr, Qexpr, Expr, Lispy);

  FILE* handleVer = fopen("VERSION", "rb");
  char  bufferVer[128];
  int sizeVersion;

  if(handleVer)
  {
    fseek (handleVer, 0, SEEK_END);
    sizeVersion = ftell (handleVer);
    rewind (handleVer);
    fread(bufferVer, 1, sizeVersion, handleVer);
  }

  printf("Harpy-Lispy Version %s", bufferVer);
  puts("Press Ctrl+c to Exit\n");

  lenv* e = lenv_new();
  lenv_add_builtins(e);
  
  // builtin macros
  mpc_result_t rbmac;
  
  mpc_parse("<library>",
	    "load \"stdlib.ark\"",
	    Lispy, &rbmac);
  lval_eval(e, lval_read(rbmac.output));

  /* Supplied with list of files */
  if (argc >= 2)
  {
    /* loop over each supplied filename (starting from 1) */
    for (int i = 1; i < argc; i++) {

      /* Argument list with a single argument, the filename */
      lval* args = lval_add(lval_sexpr(), lval_str(argv[i]));

      /* Pass to builtin load and get the result */
      lval* x = builtin_load(e, args);

      /* If the result is an error be sure to print it */
      if (x->type == LVAL_ERR) { lval_println(x); }
      lval_del(x);
    }
  }
  
  while (1)
  {
    char* input = readline("==> ");
    add_history(input);
    
    mpc_result_t r;
    if (mpc_parse("<stdin>", input, Lispy, &r))
    {
      lval* x = lval_eval(e, lval_read(r.output));
      
      lval_println(x);
      lval_del(x);
      
      mpc_ast_delete(r.output);
    } else
    {
      mpc_err_print(r.error);
      mpc_err_delete(r.error);
    }
    
    free(input);
  }

  lenv_del(e);
  mpc_cleanup(8, Comment, String, Number, Symbol, Sexpr, Qexpr, Expr, Lispy);
  
  return 0;
}

