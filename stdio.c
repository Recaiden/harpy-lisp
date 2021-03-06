#include "stdio.h"

/* fclose filehandle(obj) */
lval* builtin_fclose(lenv* e, lval* a)
{
  LASSERT_NUM("fclose", a, 1);
  LASSERT_TYPE("fclose", a, 0, LVAL_OBJ);

  LASSERT(a, (strncmp(a->cell[0]->sym, "FILE", 4) == 0),
	  "fclose requires FILE, got %s", a->cell[0]->sym);
  
}

/* fopen filename(str) mode(str) */
lval* builtin_fopen(lenv* e, lval* a)
{
  LASSERT_NUM("fopen", a, 2);
  LASSERT_TYPE("fopen", a, 0, LVAL_STR);
  LASSERT_TYPE("fopen", a, 1, LVAL_STR);
  
  lval* filename = lval_pop(a, 0);
  lval* strMode = lval_pop(a, 0);

  lval* x = lval_obj("FILE", sizeof(FILE*));
  x->mem = (void*)fopen(filename->str, strMode->str);
  if(x->mem == NULL)
  {
    lval_del(x);
    x = lval_err(strerror(errno));
    perror("The following error occurred opening");
  }

  lval_del(filename);
  lval_del(strMode);
  return x;
}


/* fread filehandle(obj) size(int) */
lval* builtin_fread(lenv* e, lval* a)
{
  LASSERT_TYPE("fread", a, 0, LVAL_OBJ);
  LASSERT_TYPE("fread", a, 1, LVAL_INT);
  
  LASSERT(a, (strncmp(a->cell[0]->sym, "FILE", 4) == 0),
	  "fread requires FILE, got %s", a->cell[0]->sym);
  
  lval* valFile = lval_pop(a, 0);
  lval* valSize = lval_pop(a, 0);
  int size = valSize->integer;
  int cRead;

  char* buffer = malloc(size);

  cRead = fread(buffer, size, 1, (FILE*)valFile->mem);

  lval* x;
  if(cRead)
  {
    x = lval_obj("MEM", size);
    memcpy( x->mem, buffer, size );
  }
  else
    x = lval_err("fread failed");
  free(buffer);

  lval_del(valFile);
  lval_del(valSize);

  return x;
}

/* fwrite filehandle(obj) mem(???) size(int) */
/* TODO writing only strings is really limiting, should be able to write binary.*/
lval* builtin_fwrite(lenv* e, lval* a)
{
  LASSERT_TYPE("fwrite", a, 0, LVAL_OBJ);
  // TODO add type for raw objects?
  LASSERT_TYPE("fwrite", a, 2, LVAL_INT);

  LASSERT(a, (strncmp(a->cell[0]->sym, "FILE", 4) == 0),
	  "fwrite requires FILE, got %s", a->cell[0]->sym);

  lval* valFile = lval_pop(a, 0);
  lval* valSrc = lval_pop(a, 0);
  lval* valSize = lval_pop(a, 0);
  int size = valSize->integer;
  int cWrote;
  
  cWrote = fwrite(valSrc->str, size, 1, (FILE*)valFile->mem);

  lval* x;
  if(cWrote == 1)
    x = lval_int(size);
  else
    x = lval_err("fwrite failed");

  lval_del(valFile);
  lval_del(valSrc);
  lval_del(valSize);

  return x;
}

/* fgets src(obj) maxsize(int) */
lval* builtin_fread_linen(lenv* e, lval* a)
{
  LASSERT_NUM("fgets", a, 2);
  LASSERT_TYPE("fgets", a, 0, LVAL_OBJ);
  LASSERT_TYPE("fgets", a, 1, LVAL_INT);
  LASSERT(a, (strncmp(a->cell[0]->sym, "FILE", 4) == 0),
	  "fgets requires FILE, got %s", a->cell[0]->sym);
  
  lval* valFile = lval_pop(a, 0);
  lval* valSize = lval_pop(a, 0);
  int size = valSize->integer;
  char* statusBuffer;

  char* buffer = malloc(size);

  statusBuffer = fgets(buffer, size, (FILE*)valFile->mem);

  lval* x;
  if(statusBuffer)
    x = lval_str(buffer);
  else
    x = lval_err("fgets failed");
  free(buffer);

  lval_del(valFile);
  lval_del(valSize);

  return x;
}

/* fwrite filehandle(obj) text(str) */
lval* builtin_fwrite_string(lenv* e, lval* a)
{
  LASSERT_NUM("printf", a, 2);
  LASSERT_TYPE("fprint", a, 0, LVAL_OBJ);
  LASSERT_TYPE("fprint", a, 1, LVAL_STR);

  LASSERT(a, (strncmp(a->cell[0]->sym, "FILE", 4) == 0),
	  "fprint requires FILE, got %s", a->cell[0]->sym);

  lval* valFile = lval_pop(a, 0);
  lval* valText = lval_pop(a, 0);
  int size = strlen(valText->str);
  int cWrote;
  
  cWrote = fwrite(valText->str, size, 1, (FILE*)valFile->mem);

  lval* x;
  if(cWrote == 1)
    x = lval_int(size);
  else
    x = lval_err("fprint failed");

  lval_del(valFile);
  lval_del(valText);

  return x;
}

lval* builtin_fflush(lenv* e, lval* a)
{
}

/* 
 getpos
 seek
 setpos
 rewind

 end-of-file

 character manipulation
 string manipulation

 tempfiles
*/

/* Implemented at a higher level */
/*
  freopen ; close then open, or set permissions
  printf ; print to a specific stream

scan/print
get/put

 */

/* fread filehandle(obj) size(int) */



void lenv_add_stdio(lenv* e)
{
  // Print and get for strings, reand write for raw data
  lenv_add_builtin(e, "fopen", builtin_fopen);
  lenv_add_builtin(e, "fread", builtin_fread);
  lenv_add_builtin(e, "fwrite", builtin_fwrite);
  lenv_add_builtin(e, "fget", builtin_fread_linen); //wraps fgets
  lenv_add_builtin(e, "fprint", builtin_fwrite_string); //functions like fputs

  //TODO add stdio, stdout, stderr to the global environment as handles

  lval* x = lval_obj("FILE", sizeof(FILE*));
  lval* y = lval_sym("stdin");
  x->mem = (void*)stdin;
  lenv_def(e, y, x);
  lval_del(x);
  lval_del(y);

  x = lval_obj("FILE", sizeof(FILE*));
  y = lval_sym("stdout");
  x->mem = (void*)stdout;
  lenv_def(e, y, x);
  lval_del(x);
  lval_del(y);

  x = lval_obj("FILE", sizeof(FILE*));
  y = lval_sym("stderr");
  x->mem = (void*)stderr;
  lenv_def(e, y, x);
  lval_del(x);
  lval_del(y);
}
